//--------------------------------------------------------------------------------------
// File: DynamicSoundEffectInstance.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=248929
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------

#include "stdinc.h"
#include "SoundCommon.h"

using namespace directxtk;


//======================================================================================
// DynamicSoundEffectInstance
//======================================================================================

// Internal object implementation class.
class DynamicSoundEffectInstance::Impl : public IVoiceNotify
{
public:
    Impl(_In_ AudioEngine* engine,
         _In_ DynamicSoundEffectInstance* object,
        std::function<void(DynamicSoundEffectInstance*)>& bufferNeeded,
         int32_t sampleRate, int32_t channels, int32_t sampleBits,
        SOUND_EFFECT_INSTANCE_FLAGS flags) :
        mBase(),
        mBufferNeeded(nullptr),
        mObject(object)
    {
        if ((sampleRate < XAUDIO2_MIN_SAMPLE_RATE)
            || (sampleRate > XAUDIO2_MAX_SAMPLE_RATE))
        {
            DebugTrace("DynamicSoundEffectInstance sampleRate must be in range %u...%u\n", XAUDIO2_MIN_SAMPLE_RATE, XAUDIO2_MAX_SAMPLE_RATE);
            throw std::invalid_argument("DynamicSoundEffectInstance");
        }

        if (!channels || (channels > 8))
        {
            DebugTrace("DynamicSoundEffectInstance channels must be in range 1...8\n");
            throw std::invalid_argument("DynamicSoundEffectInstance");
        }

        switch (sampleBits)
        {
            case 8:
            case 16:
                break;

            default:
                DebugTrace("DynamicSoundEffectInstance sampleBits must be 8-bit or 16-bit\n");
                throw std::invalid_argument("DynamicSoundEffectInstance");
        }

        m_bufferevent.reset(::CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE));
        if (!m_bufferevent)
        {
            throw std::exception("CreateEvent");
        }

        CreateIntegerPCM(&mWaveFormat, sampleRate, channels, sampleBits);

        assert(engine != nullptr);
        engine->RegisterNotify(this, true);

        mBase.Initialize(engine, &mWaveFormat, flags);

        mBufferNeeded = bufferNeeded;
    }

    Impl(Impl&&) = default;
    Impl& operator= (Impl&&) = default;

    Impl(Impl const&) = delete;
    Impl& operator= (Impl const&) = delete;

    ~Impl() override
    {
        mBase.DestroyVoice();

        if (mBase.engine)
        {
            mBase.engine->UnregisterNotify(this, false, true);
            mBase.engine = nullptr;
        }
    }

    void Play();

    void Resume();

    void SubmitBuffer(_In_reads_bytes_(audioBytes) const uint8_t* pAudioData, uint32_t offset, size_t audioBytes);

    const WAVEFORMATEX* GetFormat() const noexcept { return &mWaveFormat; }

    // IVoiceNotify
    void __cdecl OnBufferEnd() override
    {
        ::SetEvent(m_bufferevent.get());
    }

    void __cdecl OnCriticalError() override
    {
        mBase.OnCriticalError();
    }

    void __cdecl OnReset() override
    {
        mBase.OnReset();
    }

    void __cdecl OnUpdate() override;

    void __cdecl OnDestroyEngine() noexcept override
    {
        mBase.OnDestroy();
    }

    void __cdecl OnTrim() override
    {
        mBase.OnTrim();
    }

    void __cdecl GatherStatistics(AudioStatistics& stats) const noexcept override
    {
        mBase.GatherStatistics(stats);
    }

    void __cdecl OnDestroyParent() noexcept override
    {
    }

    SoundEffectInstanceBase                             mBase;

private:
    wil::unique_handle                                        m_bufferevent;
    std::function<void(DynamicSoundEffectInstance*)>    mBufferNeeded;
    DynamicSoundEffectInstance*                         mObject;
    WAVEFORMATEX                                        mWaveFormat;
};


void DynamicSoundEffectInstance::Impl::Play()
{
    if (!mBase.voice)
    {
        mBase.AllocateVoice(&mWaveFormat);
    }

    (void)mBase.Play();

    if (mBase.voice && (mBase.state == PLAYING) && (mBase.GetPendingBufferCount() <= 2))
    {
        ::SetEvent(m_bufferevent.get());
    }
}


void DynamicSoundEffectInstance::Impl::Resume()
{
    if (mBase.voice && (mBase.state == PAUSED))
    {
        mBase.Resume();

        if ((mBase.state == PLAYING) && (mBase.GetPendingBufferCount() <= 2))
        {
            ::SetEvent(m_bufferevent.get());
        }
    }
}


_Use_decl_annotations_
void DynamicSoundEffectInstance::Impl::SubmitBuffer(const uint8_t* pAudioData, uint32_t offset, size_t audioBytes)
{
    if (!pAudioData || !audioBytes)
        throw std::exception("Invalid audio data buffer");

    if (audioBytes > UINT32_MAX)
        throw std::out_of_range("SubmitBuffer");

    XAUDIO2_BUFFER buffer = {};
    buffer.AudioBytes = static_cast<uint32_t>(audioBytes);
    buffer.pAudioData = pAudioData;

    if (offset)
    {
        assert(mWaveFormat.wFormatTag == WAVE_FORMAT_PCM);
        buffer.PlayBegin = offset / mWaveFormat.nBlockAlign;
        buffer.PlayLength = static_cast<uint32_t>((audioBytes - offset) / mWaveFormat.nBlockAlign);
    }

    buffer.pContext = this;

    HRESULT hr = mBase.voice->SubmitSourceBuffer(&buffer, nullptr);
    if (FAILED(hr))
    {
    #ifdef _DEBUG
        DebugTrace("ERROR: DynamicSoundEffectInstance failed (%08X) when submitting buffer:\n", static_cast<uint32_t>(hr));

        DebugTrace("\tFormat Tag %u, %u channels, %u-bit, %u Hz, %zu bytes [%u offset)\n",
            mWaveFormat.wFormatTag, mWaveFormat.nChannels, mWaveFormat.wBitsPerSample, mWaveFormat.nSamplesPerSec, audioBytes, offset);
    #endif
        throw std::exception("SubmitSourceBuffer");
    }
}


void DynamicSoundEffectInstance::Impl::OnUpdate()
{
    uint32_t result = WaitForSingleObjectEx(m_bufferevent.get(), 0, FALSE);
    switch (result)
    {
        case WAIT_TIMEOUT:
            break;

        case WAIT_OBJECT_0:
            if (mBufferNeeded)
            {
                // This callback happens on the same thread that called AudioEngine::Update()
                mBufferNeeded(mObject);
            }
            break;

        case WAIT_FAILED:
            throw std::exception("WaitForSingleObjectEx");
    }
}



//--------------------------------------------------------------------------------------
// DynamicSoundEffectInstance
//--------------------------------------------------------------------------------------

#pragma warning( disable : 4355 )

// Public constructors
_Use_decl_annotations_
DynamicSoundEffectInstance::DynamicSoundEffectInstance(
    AudioEngine* engine,
    std::function<void(DynamicSoundEffectInstance*)> bufferNeeded,
    int32_t sampleRate,
    int32_t channels,
    int32_t sampleBits,
    SOUND_EFFECT_INSTANCE_FLAGS flags) :
    pimpl(std::make_unique<Impl>(engine, this, bufferNeeded, sampleRate, channels, sampleBits, flags))
{
}


// Move constructor.
DynamicSoundEffectInstance::DynamicSoundEffectInstance(DynamicSoundEffectInstance&& moveFrom) noexcept
    : pimpl(std::move(moveFrom.pimpl))
{
}


// Move assignment.
DynamicSoundEffectInstance& DynamicSoundEffectInstance::operator= (DynamicSoundEffectInstance&& moveFrom) noexcept
{
    pimpl = std::move(moveFrom.pimpl);
    return *this;
}


// Public destructor.
DynamicSoundEffectInstance::~DynamicSoundEffectInstance()
{
}


// Public methods.
void DynamicSoundEffectInstance::Play()
{
    pimpl->Play();
}


void DynamicSoundEffectInstance::Stop(bool immediate) noexcept
{
    bool looped = false;
    pimpl->mBase.Stop(immediate, looped);
}


void DynamicSoundEffectInstance::Pause() noexcept
{
    pimpl->mBase.Pause();
}


void DynamicSoundEffectInstance::Resume()
{
    pimpl->Resume();
}


void DynamicSoundEffectInstance::SetVolume(float volume)
{
    pimpl->mBase.SetVolume(volume);
}


void DynamicSoundEffectInstance::SetPitch(float pitch)
{
    pimpl->mBase.SetPitch(pitch);
}


void DynamicSoundEffectInstance::SetPan(float pan)
{
    pimpl->mBase.SetPan(pan);
}


void DynamicSoundEffectInstance::Apply3D(const AudioListener& listener, const AudioEmitter& emitter, bool rhcoords)
{
    pimpl->mBase.Apply3D(listener, emitter, rhcoords);
}


_Use_decl_annotations_
void DynamicSoundEffectInstance::SubmitBuffer(const uint8_t* pAudioData, size_t audioBytes)
{
    pimpl->SubmitBuffer(pAudioData, 0, audioBytes);
}


_Use_decl_annotations_
void DynamicSoundEffectInstance::SubmitBuffer(const uint8_t* pAudioData, uint32_t offset, size_t audioBytes)
{
    pimpl->SubmitBuffer(pAudioData, offset, audioBytes);
}


// Public accessors.
SoundState DynamicSoundEffectInstance::GetState() noexcept
{
    return pimpl->mBase.GetState(false);
}


size_t DynamicSoundEffectInstance::GetSampleDuration(size_t bytes) const noexcept
{
    auto wfx = pimpl->GetFormat();
    if (!wfx || !wfx->wBitsPerSample || !wfx->nChannels)
        return 0;

    return static_cast<size_t>((uint64_t(bytes) * 8)
                               / (uint64_t(wfx->wBitsPerSample) * uint64_t(wfx->nChannels)));
}


size_t DynamicSoundEffectInstance::GetSampleDurationMS(size_t bytes) const noexcept
{
    auto wfx = pimpl->GetFormat();
    if (!wfx || !wfx->nAvgBytesPerSec)
        return 0;

    return static_cast<size_t>((uint64_t(bytes) * 1000) / wfx->nAvgBytesPerSec);
}


size_t DynamicSoundEffectInstance::GetSampleSizeInBytes(uint64_t duration) const noexcept
{
    auto wfx = pimpl->GetFormat();
    if (!wfx || !wfx->nSamplesPerSec)
        return 0;

    return static_cast<size_t>(((duration * wfx->nSamplesPerSec) / 1000) * wfx->nBlockAlign);
}


int32_t DynamicSoundEffectInstance::GetPendingBufferCount() const noexcept
{
    return pimpl->mBase.GetPendingBufferCount();
}


const WAVEFORMATEX* DynamicSoundEffectInstance::GetFormat() const noexcept
{
    return pimpl->GetFormat();
}
