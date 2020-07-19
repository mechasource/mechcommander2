//--------------------------------------------------------------------------------------
// File: WaveBank.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=248929
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------

#include "stdinc.h"
#include "Audio.h"
#include "WaveBankReader.h"
#include "SoundCommon.h"
#include "platformhelpers.h"

#include <list>

using namespace directxtk;


//======================================================================================
// WaveBank
//======================================================================================

// Internal object implementation class.
class WaveBank::Impl : public IVoiceNotify
{
public:
    explicit Impl(_In_ AudioEngine* engine) :
        mEngine(engine),
        mOneShots(0),
        mPrepared(false),
        mStreaming(false)
    {
        assert(mEngine != nullptr);
        mEngine->RegisterNotify(this, false);
    }

    Impl(Impl&&) = default;
    Impl& operator= (Impl&&) = default;

    Impl(Impl const&) = delete;
    Impl& operator= (Impl const&) = delete;

    ~Impl() override
    {
        if (!mInstances.empty())
        {
            DebugTrace("WARNING: Destroying WaveBank \"%hs\" with %zu outstanding instances\n",
                mReader.BankName(), mInstances.size());

            for (auto it = mInstances.begin(); it != mInstances.end(); ++it)
            {
                assert(*it != nullptr);
                (*it)->OnDestroyParent();
            }

            mInstances.clear();
        }

        if (mOneShots > 0)
        {
            DebugTrace("WARNING: Destroying WaveBank \"%hs\" with %u outstanding one shot effects\n",
                mReader.BankName(), mOneShots);
        }

        if (mEngine)
        {
            mEngine->UnregisterNotify(this, true, false);
            mEngine = nullptr;
        }
    }

    HRESULT Initialize(_In_ AudioEngine* engine, _In_ const std::wstring_view& wbfilename) noexcept;

    void Play(uint32_t index, float volume, float pitch, float pan);

    // IVoiceNotify
    void __cdecl OnBufferEnd() override
    {
        InterlockedDecrement(&mOneShots);
    }

    void __cdecl OnCriticalError() override
    {
        mOneShots = 0;
    }

    void __cdecl OnReset() override
    {
        // No action required
    }

    void __cdecl OnUpdate() override
    {
        // We do not register for update notification
        assert(false);
    }

    void __cdecl OnDestroyEngine() noexcept override
    {
        mEngine = nullptr;
        mOneShots = 0;
    }

    void __cdecl OnTrim() override
    {
        // No action required
    }

    void __cdecl GatherStatistics(AudioStatistics& stats) const noexcept override
    {
        stats.playingOneShots += mOneShots;

        if (!mStreaming)
        {
            stats.audioBytes += mReader.BankAudioSize();

        #ifdef DIRECTX_ENABLE_XMA2
            if (mReader.HasXMA())
                stats.xmaAudioBytes += mReader.BankAudioSize();
        #endif
        }
    }

    void __cdecl OnDestroyParent() noexcept override
    {
    }

    AudioEngine*                        mEngine;
    std::list<IVoiceNotify*>            mInstances;
    WaveBankReader                      mReader;
    uint32_t                            mOneShots;
    bool                                mPrepared;
    bool                                mStreaming;
};


_Use_decl_annotations_
HRESULT WaveBank::Impl::Initialize(AudioEngine* engine, const std::wstring_view& wbfilename) noexcept
{
    if (!engine || wbfilename.empty())
        return E_INVALIDARG;

    HRESULT hr = mReader.Open(wbfilename);
    if (FAILED(hr))
        return hr;

    mStreaming = mReader.IsStreamingBank();

    return S_OK;
}


void WaveBank::Impl::Play(uint32_t index, float volume, float pitch, float pan)
{
    assert(volume >= -XAUDIO2_MAX_VOLUME_LEVEL && volume <= XAUDIO2_MAX_VOLUME_LEVEL);
    assert(pitch >= -1.f && pitch <= 1.f);
    assert(pan >= -1.f && pan <= 1.f);

    if (mStreaming)
    {
        DebugTrace("ERROR: One-shots can only be created from an in-memory wave bank\n");
        throw std::exception("WaveBank::Play");
    }

    if (index >= mReader.Count())
    {
        DebugTrace("WARNING: Index %u not found in wave bank with only %u entries, one-shot not triggered\n",
            index, mReader.Count());
        return;
    }

    if (!mPrepared)
    {
        mReader.WaitOnPrepare();
        mPrepared = true;
    }

    char wfxbuff[64] = {};
    auto wfx = reinterpret_cast<WAVEFORMATEX*>(wfxbuff);
    HRESULT hr = mReader.GetFormat(index, wfx, sizeof(wfxbuff));
    ThrowIfFailed(hr);

    IXAudio2SourceVoice* voice = nullptr;
    mEngine->AllocateVoice(wfx, SoundEffectInstance_Default, true, &voice);

    if (!voice)
        return;

    if (volume != 1.f)
    {
        hr = voice->SetVolume(volume);
        ThrowIfFailed(hr);
    }

    if (pitch != 0.f)
    {
        float fr = XAudio2SemitonesToFrequencyRatio(pitch * 12.f);

        hr = voice->SetFrequencyRatio(fr);
        ThrowIfFailed(hr);
    }

    if (pan != 0.f)
    {
        float matrix[16];
        if (ComputePan(pan, wfx->nChannels, matrix))
        {
            hr = voice->SetOutputMatrix(nullptr, wfx->nChannels, mEngine->GetOutputChannels(), matrix);
            ThrowIfFailed(hr);
        }
    }

    hr = voice->Start(0);
    ThrowIfFailed(hr);

    XAUDIO2_BUFFER buffer = {};
    hr = mReader.GetWaveData(index, &buffer.pAudioData, buffer.AudioBytes);
    ThrowIfFailed(hr);

    WaveBankReader::Metadata metadata;
    hr = mReader.GetMetadata(index, metadata);
    ThrowIfFailed(hr);

    buffer.Flags = XAUDIO2_END_OF_STREAM;
    buffer.pContext = this;

    #ifdef DIRECTX_ENABLE_XWMA

    XAUDIO2_BUFFER_WMA wmaBuffer = {};

    uint32_t tag;
    hr = mReader.GetSeekTable(index, &wmaBuffer.pDecodedPacketCumulativeBytes, wmaBuffer.PacketCount, tag);
    ThrowIfFailed(hr);

    if (tag == WAVE_FORMAT_WMAUDIO2 || tag == WAVE_FORMAT_WMAUDIO3)
    {
        hr = voice->SubmitSourceBuffer(&buffer, &wmaBuffer);
    }
    else
    #endif // xWMA
    {
        hr = voice->SubmitSourceBuffer(&buffer, nullptr);
    }
    if (FAILED(hr))
    {
        DebugTrace("ERROR: WaveBank failed (%08X) when submitting buffer:\n", static_cast<uint32_t>(hr));
        DebugTrace("\tFormat Tag %u, %u channels, %u-bit, %u Hz, %u bytes\n",
            wfx->wFormatTag, wfx->nChannels, wfx->wBitsPerSample, wfx->nSamplesPerSec, metadata.lengthBytes);
        throw std::exception("SubmitSourceBuffer");
    }

    InterlockedIncrement(&mOneShots);
}


//--------------------------------------------------------------------------------------
// WaveBank
//--------------------------------------------------------------------------------------

// Public constructors.
_Use_decl_annotations_
WaveBank::WaveBank(AudioEngine* engine, const std::wstring_view& wbfilename)
    : pimpl(std::make_unique<Impl>(engine))
{
    HRESULT hr = pimpl->Initialize(engine, wbfilename);
    if (FAILED(hr))
    {
        DebugTrace("ERROR: WaveBank failed (%08X) to intialize from .xwb file \"%ls\"\n",
            static_cast<uint32_t>(hr), wbfilename);
        throw std::exception("WaveBank");
    }

    DebugTrace("INFO: WaveBank \"%hs\" with %u entries loaded from .xwb file \"%ls\"\n",
               pimpl->mReader.BankName(), pimpl->mReader.Count(), wbfilename);
}


// Move constructor.
WaveBank::WaveBank(WaveBank&& moveFrom) noexcept
    : pimpl(std::move(moveFrom.pimpl))
{
}


// Move assignment.
WaveBank& WaveBank::operator= (WaveBank&& moveFrom) noexcept
{
    pimpl = std::move(moveFrom.pimpl);
    return *this;
}


// Public destructor.
WaveBank::~WaveBank()
{
}


// Public methods (one-shots)
void WaveBank::Play(uint32_t index)
{
    pimpl->Play(index, 1.f, 0.f, 0.f);
}


void WaveBank::Play(uint32_t index, float volume, float pitch, float pan)
{
    pimpl->Play(index, volume, pitch, pan);
}


void WaveBank::Play(_In_z_ const std::string_view& name)
{
    uint32_t index = pimpl->mReader.Find(name);
    if (index == uint32_t(-1))
    {
        DebugTrace("WARNING: Name '%hs' not found in wave bank, one-shot not triggered\n", name);
        return;
    }

    pimpl->Play(index, 1.f, 0.f, 0.f);
}


void WaveBank::Play(_In_z_ const std::string_view& name, float volume, float pitch, float pan)
{
    uint32_t index = pimpl->mReader.Find(name);
    if (index == uint32_t(-1))
    {
        DebugTrace("WARNING: Name '%hs' not found in wave bank, one-shot not triggered\n", name);
        return;
    }

    pimpl->Play(index, volume, pitch, pan);
}


// Public methods (sound effect instance)
std::unique_ptr<SoundEffectInstance> WaveBank::CreateInstance(uint32_t index, SOUND_EFFECT_INSTANCE_FLAGS flags)
{
    auto& wb = pimpl->mReader;

    if (pimpl->mStreaming)
    {
        DebugTrace("ERROR: SoundEffectInstances can only be created from an in-memory wave bank\n");
        throw std::exception("WaveBank::CreateInstance");
    }

    if (index >= wb.Count())
    {
        // We don't throw an exception here as titles often simply ignore missing assets rather than fail
        return std::unique_ptr<SoundEffectInstance>();
    }

    if (!pimpl->mPrepared)
    {
        wb.WaitOnPrepare();
        pimpl->mPrepared = true;
    }

    auto effect = new SoundEffectInstance(pimpl->mEngine, this, index, flags);
    assert(effect != nullptr);
    pimpl->mInstances.emplace_back(effect->GetVoiceNotify());
    return std::unique_ptr<SoundEffectInstance>(effect);
}


std::unique_ptr<SoundEffectInstance> WaveBank::CreateInstance(_In_z_ const std::string_view& name, SOUND_EFFECT_INSTANCE_FLAGS flags)
{
    uint32_t index = pimpl->mReader.Find(name);
    if (index == uint32_t(-1))
    {
        // We don't throw an exception here as titles often simply ignore missing assets rather than fail
        return std::unique_ptr<SoundEffectInstance>();
    }

    return CreateInstance(index, flags);
}


// Public methods (sound stream instance)
std::unique_ptr<SoundStreamInstance> WaveBank::CreateStreamInstance(uint32_t index, SOUND_EFFECT_INSTANCE_FLAGS flags)
{
    auto& wb = pimpl->mReader;

    if (!pimpl->mStreaming)
    {
        DebugTrace("ERROR: SoundStreamInstances can only be created from a streaming wave bank\n");
        throw std::exception("WaveBank::CreateStreamInstance");
    }

    if (index >= wb.Count())
    {
        // We don't throw an exception here as titles often simply ignore missing assets rather than fail
        return std::unique_ptr<SoundStreamInstance>();
    }

    if (!pimpl->mPrepared)
    {
        wb.WaitOnPrepare();
        pimpl->mPrepared = true;
    }

    auto effect = new SoundStreamInstance(pimpl->mEngine, this, index, flags);
    assert(effect != nullptr);
    pimpl->mInstances.emplace_back(effect->GetVoiceNotify());
    return std::unique_ptr<SoundStreamInstance>(effect);
}


std::unique_ptr<SoundStreamInstance> WaveBank::CreateStreamInstance(_In_z_ const std::string_view& name, SOUND_EFFECT_INSTANCE_FLAGS flags)
{
    uint32_t index = pimpl->mReader.Find(name);
    if (index == uint32_t(-1))
    {
        // We don't throw an exception here as titles often simply ignore missing assets rather than fail
        return std::unique_ptr<SoundStreamInstance>();
    }

    return CreateStreamInstance(index, flags);
}


void WaveBank::UnregisterInstance(_In_ IVoiceNotify* instance)
{
    auto it = std::find(pimpl->mInstances.begin(), pimpl->mInstances.end(), instance);
    if (it == pimpl->mInstances.end())
        return;

    pimpl->mInstances.erase(it);
}


// Public accessors.
bool WaveBank::IsPrepared() const noexcept
{
    if (pimpl->mPrepared)
        return true;

    if (!pimpl->mReader.IsPrepared())
        return false;

    pimpl->mPrepared = true;
    return true;
}


bool WaveBank::IsInUse() const noexcept
{
    return (pimpl->mOneShots > 0) || !pimpl->mInstances.empty();
}


bool WaveBank::IsStreamingBank() const noexcept
{
    return pimpl->mReader.IsStreamingBank();
}


size_t WaveBank::GetSampleSizeInBytes(uint32_t index) const noexcept
{
    if (index >= pimpl->mReader.Count())
        return 0;

    WaveBankReader::Metadata metadata;
    HRESULT hr = pimpl->mReader.GetMetadata(index, metadata);
    if (FAILED(hr))
        return 0;

    return metadata.lengthBytes;
}


size_t WaveBank::GetSampleDuration(uint32_t index) const noexcept
{
    if (index >= pimpl->mReader.Count())
        return 0;

    WaveBankReader::Metadata metadata;
    HRESULT hr = pimpl->mReader.GetMetadata(index, metadata);
    if (FAILED(hr))
        return 0;

    return metadata.duration;
}


size_t WaveBank::GetSampleDurationMS(uint32_t index) const noexcept
{
    if (index >= pimpl->mReader.Count())
        return 0;

    char buff[64] = {};
    auto wfx = reinterpret_cast<WAVEFORMATEX*>(buff);
    HRESULT hr = pimpl->mReader.GetFormat(index, wfx, sizeof(buff));
    if (FAILED(hr))
        return 0;

    WaveBankReader::Metadata metadata;
    hr = pimpl->mReader.GetMetadata(index, metadata);
    if (FAILED(hr))
        return 0;

    return static_cast<size_t>((uint64_t(metadata.duration) * 1000) / wfx->nSamplesPerSec);
}


_Use_decl_annotations_
const WAVEFORMATEX* WaveBank::GetFormat(uint32_t index, WAVEFORMATEX* wfx, size_t maxsize) const noexcept
{
    if (index >= pimpl->mReader.Count())
        return nullptr;

    HRESULT hr = pimpl->mReader.GetFormat(index, wfx, maxsize);
    if (FAILED(hr))
        return nullptr;

    return wfx;
}


_Use_decl_annotations_
int32_t WaveBank::Find(const std::string_view& name) const
{
    return static_cast<int32_t>(pimpl->mReader.Find(name));
}


#ifdef DIRECTX_ENABLE_XWMA

_Use_decl_annotations_
bool WaveBank::FillSubmitBuffer(uint32_t index, XAUDIO2_BUFFER& buffer, XAUDIO2_BUFFER_WMA& wmaBuffer) const
{
    memset(&buffer, 0, sizeof(buffer));
    memset(&wmaBuffer, 0, sizeof(wmaBuffer));

    HRESULT hr = pimpl->mReader.GetWaveData(index, &buffer.pAudioData, buffer.AudioBytes);
    ThrowIfFailed(hr);

    WaveBankReader::Metadata metadata;
    hr = pimpl->mReader.GetMetadata(index, metadata);
    ThrowIfFailed(hr);

    buffer.LoopBegin = metadata.loopStart;
    buffer.LoopLength = metadata.loopLength;

    uint32_t tag;
    hr = pimpl->mReader.GetSeekTable(index, &wmaBuffer.pDecodedPacketCumulativeBytes, wmaBuffer.PacketCount, tag);
    ThrowIfFailed(hr);

    return (tag == WAVE_FORMAT_WMAUDIO2 || tag == WAVE_FORMAT_WMAUDIO3);
}

#else // !xWMA

_Use_decl_annotations_
void WaveBank::FillSubmitBuffer(uint32_t index, XAUDIO2_BUFFER& buffer) const
{
    memset(&buffer, 0, sizeof(buffer));

    HRESULT hr = pimpl->mReader.GetWaveData(index, &buffer.pAudioData, buffer.AudioBytes);
    ThrowIfFailed(hr);

    WaveBankReader::Metadata metadata;
    hr = pimpl->mReader.GetMetadata(index, metadata);
    ThrowIfFailed(hr);

    buffer.LoopBegin = metadata.loopStart;
    buffer.LoopLength = metadata.loopLength;
}

#endif


HANDLE WaveBank::GetAsyncHandle() const noexcept
{
    if (pimpl)
    {
        return pimpl->mReader.GetAsyncHandle();
    }

    return nullptr;
}


_Use_decl_annotations_
bool WaveBank::GetPrivateData(uint32_t index, void* data, size_t datasize)
{
    if (index >= pimpl->mReader.Count())
        return false;

    if (!data)
        return false;

    switch (datasize)
    {
        case sizeof(WaveBankReader::Metadata):
        {
            auto ptr = reinterpret_cast<WaveBankReader::Metadata*>(data);
            return SUCCEEDED(pimpl->mReader.GetMetadata(index, *ptr));
        }

        case sizeof(WaveBankSeekData):
        {
            auto ptr = reinterpret_cast<WaveBankSeekData*>(data);
            return SUCCEEDED(pimpl->mReader.GetSeekTable(index, &ptr->seekTable, ptr->seekCount, ptr->tag));
        }

        default:
            return false;
    }
}
