//===========================================================================//
// File:	memstrm.hpp                                                      //
// Contents: Implementation Details of resource management                   //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#include <stuff/stuff.hpp>

namespace Stuff {class MemoryStream;}

namespace MemoryStreamIO
{
	Stuff::MemoryStream&
		Write(
		Stuff::MemoryStream *stream,
		const Stuff::MemoryStream *input_stream
		);
}

namespace Stuff {

	class MemoryStream;
	class DynamicMemoryStream;

	//##########################################################################
	//#######################    MemoryStream    ###############################
	//##########################################################################

	cint32_t End_Of_Stream=-1;

	cint32_t Empty_Bit_Buffer=-20;


	class MemoryStream :
		public RegisteredClass
	{
		friend MemoryStream&
			MemoryStreamIO::Write(
			MemoryStream *stream,
			const MemoryStream *input_stream
			);

	public:
		static void
			InitializeClass(void);
		static void
			TerminateClass(void);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Construction and testing
		//
	public:
		MemoryStream(
			PVOID stream_start,
			size_t stream_size,
			size_t initial_offset=0)
			: RegisteredClass(DefaultData)
		{
			Check_Pointer(this);

			streamStart = static_cast<PUCHAR>(stream_start);
			streamSize = stream_size;
			Verify(initial_offset <= stream_size);
			currentPosition = streamStart + initial_offset;
			currentBit = Empty_Bit_Buffer;
		}

		virtual ~MemoryStream(void)
		{
		}

		void
			TestInstance(void) const;

		static bool
			TestClass(void);

	protected:
		MemoryStream(
			ClassData* class_data,
			PVOID stream_start,
			size_t stream_size,
			size_t initial_offset=0)
			: RegisteredClass(class_data)
		{
			Check_Pointer(this);

			streamStart = static_cast<PUCHAR>(stream_start);
			streamSize = stream_size;
			Verify(initial_offset <= stream_size);
			currentPosition = streamStart + initial_offset;
			currentBit = 0;
		}

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Class Data Support
		//
	public:
		static ClassData
			*DefaultData;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Accessors
		//
	public:
		virtual PVOID
			GetPointer(void) const
		{
			Check_Object(this); return currentPosition;
		}

		size_t
			GetIndex(void) const
		{
			Check_Object(this); return size_t(currentPosition - streamStart);
		}

		size_t
			GetSize(void) const
		{
			Check_Object(this); return streamSize;
		}
		
		size_t
			GetBytesUsed(void) const
		{
			Check_Object(this); return size_t(currentPosition - streamStart);
		}
		
		virtual size_t
			GetBytesRemaining(void) const
		{
			Check_Object(this); return size_t(streamSize - GetBytesUsed());
		}

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Assignment methods
		//
	public:
		virtual void
			SetPointer(PVOID new_pointer)
		{
			Check_Pointer(this);
			currentPosition = Cast_Pointer(PUCHAR, new_pointer);
			Check_Object(this);
		}
		void
			operator=(PVOID new_pointer)
		{
			SetPointer(new_pointer);
		}

		virtual void
			SetPointer(size_t index)
		{
			Check_Pointer(this);
			currentPosition = streamStart + index;
			Check_Object(this);
		}
		void
			operator=(size_t index)
		{
			SetPointer(index);
		}

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Stream methods
		//
	public:
		void
			Rewind(void)
		{
			SetPointer((size_t) 0U);currentBit = Empty_Bit_Buffer;
		}

		virtual bool
			AllocateBytes(size_t count)
		{
			return GetBytesRemaining() <= count;
		}
		
		virtual MemoryStream&
			AdvancePointer(size_t count)
		{
			Check_Object(this);
			currentPosition += count;

			Verify(currentPosition >= streamStart);
			Verify(currentPosition <= streamStart + streamSize);

			Check_Object(this);
			return *this;
		}
		
		MemoryStream&
			operator+=(size_t count)
		{
			return AdvancePointer(count);
		}

		virtual MemoryStream&
			RewindPointer(size_t count)
		{
			Check_Object(this);
			currentPosition -= count;

			Verify(currentPosition >= streamStart);
			Verify(currentPosition <= streamStart + streamSize);

			Check_Object(this);
			return *this;
		}
		MemoryStream&
			operator-=(size_t count)
		{return RewindPointer(count);}

		virtual MemoryStream&
			ReadBytes(
			PVOID ptr,
			size_t number_of_bytes
			);

		MemoryStream&
			WriteBytes(
			PCVOID ptr,
			size_t number_of_bytes
			)
		{
			Check_Object(this);
			Check_Pointer(static_cast<PCVOID>(ptr));
			Verify(number_of_bytes > 0);

			Mem_Copy(GetPointer(), static_cast<PCVOID>(ptr), 
				number_of_bytes, GetBytesRemaining());
			AdvancePointer(number_of_bytes);
			return *this;
		}

		MemoryStream&
			ReadSwappedBytes(
			PVOID ptr,
			size_t number_of_bytes
			);
		MemoryStream&
			WriteSwappedBytes(
			PCVOID ptr,
			size_t number_of_bytes
			);

		virtual int
			ReadChar(void);

		virtual bool
			ReadLine(
			PSTR buffer,
			size_t size_of_buffer,
			char continuator = '\0'
			);

		MString
			ReadString(
			size_t size_of_buffer = 512,
			char continuator = '\0'
			);

		void
			WriteLine(PSTR buffer);


		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Bit operators  
		//
		// WARNING - DO NOT MIX AND MATCH WITH BYTE METHODS!!!!!
		//

	private:
		int
			currentBit;

		uint8_t
			workingBitBuffer;

		MemoryStream&
			ReadUnsafeBits(PVOID ptr, ULONG number_of_bits);

	public:
		MemoryStream&
			ReadBit(bool &bit_value);

		MemoryStream&
			WriteBit(const bool &bit_value);


		template <typename T>
		MemoryStream& ReadBits(T *ptr, ULONG number_of_bits)
		{
			Verify(number_of_bits <= 32);
			Check_Object(this); Check_Pointer(ptr);
			if(ptr)*ptr = 0; return ReadUnsafeBits(ptr, number_of_bits);
		}

#if _CONSIDERED_OBSOLETE
		MemoryStream&
			ReadBits(int *ptr, ULONG number_of_bits)
		{
			Verify(number_of_bits <= 32);
			Check_Object(this); Check_Pointer(ptr);
			if(ptr)*ptr = 0; return ReadUnsafeBits(ptr, number_of_bits);
		}

		MemoryStream&
			ReadBits(uint8_t *ptr, ULONG number_of_bits)
		{
			Verify(number_of_bits <= 8);
			Check_Object(this); Check_Pointer(ptr);
			if(ptr)*ptr = 0; return ReadUnsafeBits(ptr, number_of_bits);
		}


		MemoryStream&
			ReadBits(USHORT *ptr, ULONG number_of_bits)
		{
			Verify(number_of_bits <= 16);
			Check_Object(this); Check_Pointer(ptr);
			if(ptr)*ptr = 0; return ReadUnsafeBits(ptr, number_of_bits);
		}

		MemoryStream&
			ReadBits(ULONG *ptr, ULONG number_of_bits)
		{
			Verify(number_of_bits <= 32);
			Check_Object(this); Check_Pointer(ptr);
			if(ptr)*ptr = 0; return ReadUnsafeBits(ptr, number_of_bits);
		}

		MemoryStream&
			ReadBits(size_t *ptr, ULONG number_of_bits)
		{
			Verify(number_of_bits <= 32);
			Check_Object(this); Check_Pointer(ptr);
			*ptr = 0; return ReadUnsafeBits(ptr, number_of_bits);
		}

		MemoryStream&
			ReadBits(float *ptr, ULONG number_of_bits)
		{
			Verify(number_of_bits == 32);
			Check_Object(this); Check_Pointer(ptr);
			*ptr = 0.0; return ReadUnsafeBits(ptr, number_of_bits);
		}

		MemoryStream&
			ReadBits(double *ptr, ULONG number_of_bits)
		{
			Verify(number_of_bits == 64);
			Check_Object(this); Check_Pointer(ptr);
			*ptr = 0.0; return ReadUnsafeBits(ptr, number_of_bits);
		}

#endif

		MemoryStream&
			WriteBits(PCVOID ptr, ULONG number_of_bits);

		MemoryStream&
			ReadBitsToScaledInt(int &number, int min, int max,  ULONG number_of_bits);

		MemoryStream&
			WriteScaledIntToBits(cint32_t &number, int min, int max,  ULONG number_of_bits);

		MemoryStream&
			ReadBitsToScaledFloat(float &number, float min, float max,  ULONG number_of_bits);

		MemoryStream&
			WriteScaledFloatToBits(const float &number, float min, float max,  ULONG number_of_bits);

		void
			ByteAlign(void);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Protected data
		//
	protected:
		uint8_t
			*streamStart,
			*currentPosition;
		size_t
			streamSize;
	};

	template <class T> inline MemoryStream&
		Read_Swapped(
		MemoryStream *stream,
		T data
		)
	{
		return stream->ReadSwappedBytes(data, sizeof(*data));
	}

	template <class T> inline MemoryStream&
		Write_Swapped(
		MemoryStream *stream,
		const T data
		)
	{
		return stream->WriteSwappedBytes(&data, sizeof(data));
	}

	//--------------------------------------------------------------------------
	// Extraction operators
	//
	template <class T> inline MemoryStream&
		operator>>(
		MemoryStream &stream,
		T &output
		)
	{
		return MemoryStreamIO::Read(&stream, &output);
	}

	//--------------------------------------------------------------------------
	// Insertion operators
	//
	template <class T> inline MemoryStream&
		operator<<(
		MemoryStream &stream,
		const T &input
		)
	{
		return MemoryStreamIO::Write(&stream, &input);
	}

	inline MemoryStream&
		operator<<(
		MemoryStream& stream,
		PCSTR input
		)
	{
		Check_Pointer(input);
		return stream.WriteBytes(static_cast<PCVOID>(input), strlen(input));
	}

	//##########################################################################
	//###################    DynamicMemoryStream    ############################
	//##########################################################################

	class DynamicMemoryStream:
		public MemoryStream
	{
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Construction, destruction, and testing
		//
	public:
		DynamicMemoryStream(size_t stream_size=0);
		DynamicMemoryStream(
			PVOID stream_start,
			size_t stream_size,
			size_t initial_offset=0
			);
		DynamicMemoryStream(const DynamicMemoryStream& otherStream);
		~DynamicMemoryStream(void);

		void
			TestInstance(void) const
		{
			Verify((size_t)(currentPosition - streamStart) <= streamSize);
			Verify(streamSize <= bufferSize);
		}

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Accessors
		//
	public:
		size_t
			GetBufferSize(void)
		{
			Check_Object(this);
			return bufferSize;
		}

		void
			SetSize(size_t stream_length)
		{
			Check_Object(this);
			Verify(stream_length <= bufferSize);
			streamSize = stream_length;
		}

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Stream methods
		//
	public:

		MemoryStream&
			WriteBytes(
			PCVOID ptr,
			size_t number_of_bytes
			);

		bool	AllocateBytes(size_t count);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Private data
		//
	private:
		size_t
			bufferSize;
		bool
			ownsStream;
	};

}

namespace MemoryStreamIO {

	template<typename T> 
	inline Stuff::MemoryStream&
		Read(
		Stuff::MemoryStream* stream,
		T *output
		)
	{
		return stream->ReadBytes(output, sizeof(*output));
	}

	template<typename T> 
	inline Stuff::MemoryStream&
		Write(
		Stuff::MemoryStream* stream,
		const T *input
		)
	{
		return stream->WriteBytes(input, sizeof(*input));
	}

#if  _CONSIDERED_OBSOLETE
	inline Stuff::MemoryStream&
		Read(
		Stuff::MemoryStream* stream,
		bool *output
		)
	{
		return stream->ReadBytes(output, sizeof(*output));
	}

	inline Stuff::MemoryStream&
		Read(
		Stuff::MemoryStream* stream,
		CHAR *output
		)
	{
		return stream->ReadBytes(output, sizeof(*output));
	}

	inline Stuff::MemoryStream&
		Read(
		Stuff::MemoryStream* stream,
		uint8_t *output
		)
	{
		return stream->ReadBytes(output, sizeof(*output));
	}

	inline Stuff::MemoryStream&
		Read(
		Stuff::MemoryStream* stream,
		SHORT *output
		)
	{
		return stream->ReadBytes(output, sizeof(*output));
	}

	inline Stuff::MemoryStream&
		Read(
		Stuff::MemoryStream* stream,
		USHORT *output
		)
	{
		return stream->ReadBytes(output, sizeof(*output));
	}

	inline Stuff::MemoryStream&
		Read(
		Stuff::MemoryStream* stream,
		int *output
		)
	{
		return stream->ReadBytes(output, sizeof(*output));
	}

	inline Stuff::MemoryStream&
		Read(
		Stuff::MemoryStream* stream,
		unsigned *output
		)
	{
		return stream->ReadBytes(output, sizeof(*output));
	}

	inline Stuff::MemoryStream&
		Read(
		Stuff::MemoryStream* stream,
		LONG *output
		)
	{
		return stream->ReadBytes(output, sizeof(*output));
	}

	inline Stuff::MemoryStream&
		Read(
		Stuff::MemoryStream* stream,
		size_t *output
		)
	{
		return stream->ReadBytes(output, sizeof(*output));
	}

	inline Stuff::MemoryStream&
		Write(
		Stuff::MemoryStream* stream,
		const bool *input
		)
	{
		return stream->WriteBytes(input, sizeof(*input));
	}

	inline Stuff::MemoryStream&
		Write(
		Stuff::MemoryStream* stream,
		PCSTR input
		)
	{
		return stream->WriteBytes(input, sizeof(*input));
	}

	inline Stuff::MemoryStream&
		Write(
		Stuff::MemoryStream* stream,
		PCUCHAR input
		)
	{
		return stream->WriteBytes(input, sizeof(*input));
	}

	inline Stuff::MemoryStream&
		Write(
		Stuff::MemoryStream* stream,
		const SHORT *input
		)
	{
		return stream->WriteBytes(input, sizeof(*input));
	}

	inline Stuff::MemoryStream&
		Write(
		Stuff::MemoryStream* stream,
		const USHORT *input
		)
	{
		return stream->WriteBytes(input, sizeof(*input));
	}

	inline Stuff::MemoryStream&
		Write(
		Stuff::MemoryStream* stream,
		pcint32_t input
		)
	{
		return stream->WriteBytes(input, sizeof(*input));
	}

	
	inline Stuff::MemoryStream&
		Write(
		Stuff::MemoryStream* stream,
		const unsigned *input
		)
	{
		return stream->WriteBytes(input, sizeof(*input));
	}

	inline Stuff::MemoryStream&
		Write(
		Stuff::MemoryStream* stream,
		const LONG *input
		)
	{
		return stream->WriteBytes(input, sizeof(*input));
	}

	inline Stuff::MemoryStream&
		Write(
		Stuff::MemoryStream* stream,
		const size_t *input
		)
	{
		return stream->WriteBytes(input, sizeof(*input));
	}

#endif

#if _CONSIDERED_OBSOLETE
	inline Stuff::MemoryStream&
		Write(
		Stuff::MemoryStream* stream,
		PCSTR  const *input
		)
	{
		Check_Pointer(*input);
		return stream->WriteBytes(*input, strlen(*input));
	}
#endif

	Stuff::MemoryStream&
		Write(
		Stuff::MemoryStream* stream,
		const Stuff::MemoryStream* input_stream
		);

}
