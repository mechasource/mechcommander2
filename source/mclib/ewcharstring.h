#ifndef EWCHARSTRING_H
#define EWCHARSTRING_H

//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
// Notes				essentially EString, except that it uses wchars (not chars) even when UNICODE is not defined
//				
//***************************************************************

//***************************************************************

/* just for this file, we want UNICODE and K_UNICODE defined and MBCS undefined */
#ifdef UNICODE
#define _UNICODE_WAS_PREVIOUSLY_DEFINED___EWCHARSTRING_H
#else
#define UNICODE
#endif /* def UNICODE */

#ifdef K_UNICODE
#define _K_UNICODE_WAS_PREVIOUSLY_DEFINED___EWCHARSTRING_H
#else
#define K_UNICODE
#endif /* def K_UNICODE */

#ifdef MBCS
#define _MBCS_WAS_PREVIOUSLY_DEFINED___EWCHARSTRING_H
#define _PREVIOUS_MBCS___EWCHARSTRING_H		MBCS
#undef MBCS
#endif /* def MBCS */

//***************************************************************



#ifdef UNICODE
#define EWCSChar uint16_t
#else
#define EWCSChar char
#endif

#include <gameos.hpp>

class EWCharString 
{

public:

	// ENUMERATIONS & CONSTANTS
	static cint32_t INVALID_INDEX;

	// CONSTRUCTORS/DESTRUCTORS
	/*explicit*/ EWCharString( const EWCSChar* newString );
	EWCharString( const EWCharString& );
	/*explicit*/ EWCharString( const EWCSChar );
	EWCharString(void);
	~EWCharString(void);

	// MANIPULATORS

	void		Replace( int32_t Start_Index, const EWCharString& String );
	void		Replace( int32_t Start_Index, const EWCSChar* );

	bool		Remove( int32_t Start_Index, int32_t End_Index );
	bool Remove( EWCharString& Sub_String );

	// puts this string in the middle of another string
	inline void	Insert( int32_t Start_Index, const EWCharString& String );
	void	Insert( int32_t Start_Index, const EWCSChar* String ); 

	void		Swap( EWCharString& );
	inline void	Empty(void);
	void		MakeUpper(void);
	void 		MakeLower(void);
	void 		Reverse(void);

	// works like sprintf with the destination buffer being
	// the this pointer
	void	Format( const EWCSChar*, ... );

	inline const EWCharString& operator=( const EWCSChar* );
	inline const EWCharString& operator=( const EWCharString& );
	inline const EWCharString& operator=( EWCSChar );

	inline const EWCharString& operator+=( const EWCSChar* );
	inline const EWCharString& operator+=( const EWCharString& );
	inline const EWCharString& operator+=( EWCSChar );

	friend EWCharString operator+( const EWCharString&, const EWCharString& );

	friend EWCharString operator+( const EWCSChar*, const EWCharString& );
	friend EWCharString operator+( const EWCharString& , const EWCSChar* );

	friend EWCharString operator+( const EWCharString&,  const  EWCSChar );
	friend EWCharString operator+( const EWCSChar,  const  EWCharString& );

 	// ACCESSORS

	// these are case sensitive, use Compare
	// if you don't want case senstitivity
	inline bool operator==( const EWCharString& ) const;
	inline bool operator==( const EWCSChar* ) const;
	inline friend bool operator==( const EWCSChar*, const EWCharString& );

	// these functions return -1 if "this" is less than the passed in string
	int32_t Compare( const EWCharString&, bool Case_Sensitive = false ) const;
	int32_t Compare( const EWCSChar*, bool Case_Sensitive = false ) const;

	bool operator!=( const EWCharString& ) const;
	bool operator!=( const EWCSChar*) const;
	friend bool operator!=( const EWCSChar*, const EWCharString& );

	inline EWCSChar& operator[]( int32_t Index );
	inline const EWCSChar& operator[](int32_t Index) const;

	inline bool operator<( const EWCharString& ) const; 
	inline bool operator<( const EWCSChar* ) const;
	inline friend bool operator<( const EWCSChar*, const EWCharString& );

	inline bool operator<=( const EWCharString& ) const;
	inline bool operator<=( const EWCSChar*) const;
	inline friend bool operator<=( const EWCSChar*, const EWCharString&);

	inline bool operator>( const EWCharString& ) const;
	inline bool operator>(const EWCSChar*) const;
	inline friend bool operator>( const EWCSChar*, const EWCharString&);

	inline bool operator>=( const EWCharString& ) const;
	inline bool operator>=( const EWCSChar*) const;
	inline friend bool operator>=( const EWCSChar*, const EWCharString&);
	
	int32_t Size(void) const; 	// number of bytes
	int32_t Length(void) const;	// number of characters

	// search functions
	int32_t Find( EWCSChar, int32_t Start_Index = EWCharString::INVALID_INDEX) const;
	int32_t Find( const EWCharString&, int32_t Start_Index = EWCharString::INVALID_INDEX) const;
	int32_t Find( const EWCSChar*, int32_t Start_Index = EWCharString::INVALID_INDEX) const;

	int32_t ReverseFind ( EWCSChar, int32_t End_Index = EWCharString::INVALID_INDEX) const;

	
	// we are going to treat this object as a TCHAR array, so we 
	// don't have to worry about #of chars versus #of bytes
	EWCharString SubString( int32_t Start_Index, int32_t End_Index ) const;
	inline EWCharString Left( int32_t Num_Chars) const;	 
	inline EWCharString Right( int32_t Num_Chars) const;	

	inline bool 		IsEmpty(void) const;

	PWSTR	CreateUNICODE(void) const;
	PSTR 	CreateMBCS(void) const; 

	inline	const EWCSChar* Data(void) const;

	// ALL UNICODE SPECIFIC AND

#ifndef UNICODE

	int32_t Find( uint16_t, int32_t Start_Index = -1 ) const;

#else // K_UNICODE

	// we'll convert string literals for you 
	
	EWCharString( PCSTR );
			
	friend EWCharString operator+( const EWCharString&, PSTR );
	friend EWCharString operator+( PSTR, const EWCharString& );
	
	friend EWCharString operator+( char, const EWCharString& );
	friend EWCharString operator+( const EWCharString&, char );
	
	const EWCharString& operator+=( PCSTR );
	const EWCharString& operator+=( char );

	const EWCharString& operator=( char );

	bool operator==( PCSTR );
	friend bool operator==( PCSTR, const EWCharString& );

	bool operator!=( PCSTR) const;
	friend bool operator!=( PCSTR, const EWCharString& );

	bool operator<( PCSTR ) const;
	friend bool operator<( PCSTR, const EWCharString& );
	
	bool operator>( PCSTR ) const;
	friend bool operator>( PCSTR, const EWCharString& );

	bool operator<=( PCSTR) const;
	friend bool operator<=( PCSTR, const EWCharString&);

	bool operator>=( PCSTR) const;
	friend bool operator>=( PCSTR, const EWCharString&);

	void	Format( PCSTR, ... );

	int32_t Find( char, int32_t Start_Index = EWCharString::INVALID_INDEX) const;


#endif // Unicode 

private:


	// helper functions

	// Allocates a specific amount
	void	Alloc( int32_t Min_Amount );
	
	// Reallocates if you want to make a change to a shared buffer
	inline	void	ChecEBuffer(void);
	void	ChecEBufferDoRealloc(void);

	// sets the buffer, reallocs if necessary
	void	Assign( const EWCSChar* p_Str );


	static  inline  PWSTR	ToUnicode( puint16_t Buffer, pcuint8_t p_Str, int32_t Num_Chars  );
	static	inline	int32_t	StrSize( const EWCSChar* p_Str );
	
	struct EBuffer
	{
		int32_t m_Ref_Count;		// reference count
		int32_t m_Data_Length;		// Length of String
		int32_t m_Alloc_Length;	// Length of the Buffer
	
		inline EWCSChar* Data(void);
		inline void Release(void);

		static EBuffer	s_Empty_Buffer;
		static EBuffer* s_p_Empty_Buffer;
	};

	EBuffer*		m_pBuffer;
	static cint32_t s_Alloc_Allign;
	static cint32_t s_Force_Ansi;
	static cint32_t s_Force_Unicode;

};

//***************************************************************

// inlines



/////////////////////////////////////////////////////////////////
inline void	EWCharString::ChecEBuffer()
{
	if ( m_pBuffer->m_Ref_Count > 0 )
	{
		m_pBuffer->m_Ref_Count --;
		m_pBuffer = EBuffer::s_p_Empty_Buffer;
	}
}

/////////////////////////////////////////////////////////////////
inline void EWCharString::EBuffer::Release()
{
	if ( this != s_p_Empty_Buffer )
	{
		m_Ref_Count --;
		if ( m_Ref_Count < 0 )
		{
			delete [] (PSTR)this;
		}
	}
}

/////////////////////////////////////////////////////////////////
inline void	EWCharString::Empty()
{
	m_pBuffer->Release(void);
	m_pBuffer =  EBuffer::s_p_Empty_Buffer;
}


/////////////////////////////////////////////////////////////////
inline void EWCharString::Insert( int32_t Start_Index, const EWCharString& String )
{
	Insert( Start_Index, String.m_pBuffer->Data() );
}

/////////////////////////////////////////////////////////////////
inline const EWCharString& EWCharString::operator=( const EWCSChar* p_String )
{
	ChecEBuffer(void);
	Assign( p_String );
	return *this;
}

/////////////////////////////////////////////////////////////////
inline const EWCharString& EWCharString::operator=( const EWCharString& Src )
{
	if ( &Src != this )
	{
		m_pBuffer->Release(void);
		m_pBuffer = Src.m_pBuffer;
		Src.m_pBuffer->m_Ref_Count ++;
	}

	return *this; 
}

/////////////////////////////////////////////////////////////////
inline const EWCharString& EWCharString::operator=( EWCSChar Char )
{
	ChecEBuffer(void);

	EWCSChar Tmp[2];
	Tmp[0] = Char;
	Tmp[1] = 0;
	Assign( Tmp );

	return *this;
}

/////////////////////////////////////////////////////////////////
inline const EWCharString& EWCharString::operator+=( const EWCSChar* p_String )
{
	Insert( m_pBuffer->m_Data_Length, p_String );
	return *this;
}

/////////////////////////////////////////////////////////////////
inline const EWCharString& EWCharString::operator+=( const EWCharString& String )
{
	Insert( m_pBuffer->m_Data_Length, String );
	return *this;
}

/////////////////////////////////////////////////////////////////
inline const EWCharString& EWCharString::operator+=( EWCSChar Char )
{
	EWCSChar Tmp[2];
	Tmp[0] = Char;
	Tmp[1] = 0;
	Insert(  m_pBuffer->m_Data_Length, Tmp );

	return *this;	
}

/////////////////////////////////////////////////////////////////
inline bool EWCharString::operator!=( const EWCharString& Str_To_Compare ) const
{
	return !( operator==(Str_To_Compare) ); 
}

/////////////////////////////////////////////////////////////////
inline bool EWCharString::operator!=( const EWCSChar* p_Str_To_Compare ) const
{
	return !( operator==( p_Str_To_Compare ) );
}

/////////////////////////////////////////////////////////////////
inline EWCharString EWCharString::Left( int32_t Num_Chars) const
{
	// Bill changed to Num_Chars - 1, this was always returning one character too many
	return SubString( 0, Num_Chars - 1 ); 
}

/////////////////////////////////////////////////////////////////
inline EWCharString EWCharString::Right( int32_t Num_Chars) const
{
	return SubString( m_pBuffer->m_Data_Length - Num_Chars, 
		m_pBuffer->m_Data_Length - 1 );
}

/////////////////////////////////////////////////////////////////
inline bool EWCharString::IsEmpty(void) const
{
	return (m_pBuffer->m_Data_Length <= 0);
}

/////////////////////////////////////////////////////////////////
inline	const EWCSChar* EWCharString::Data(void) const
{
	return ( m_pBuffer->Data() );
}

/////////////////////////////////////////////////////////////////
inline EWCSChar* EWCharString::EBuffer::Data()
{
	if ( !m_Alloc_Length )
	{
		return 0;
	}
	return (EWCSChar*)(this + 1);
}

/////////////////////////////////////////////////////////////////
inline bool EWCharString::operator==( const EWCharString& Str_To_Compare) const
{
	return ( 0 == Compare( Str_To_Compare, true ) );
}

/////////////////////////////////////////////////////////////////
inline bool EWCharString::operator==( const EWCSChar* p_String ) const
{

	return ( 0 == Compare( p_String, true ) );
}

/////////////////////////////////////////////////////////////////
inline bool operator==( const EWCSChar* p_String, const EWCharString& Str )
{
	return ( 0 == Str.Compare( p_String ) );
}

/////////////////////////////////////////////////////////////////
inline bool operator!=( const EWCSChar* p_String, const EWCharString& Str )
{
	return !(Str == p_String );
}
/////////////////////////////////////////////////////////////////
inline EWCSChar& EWCharString::operator[]( int32_t Index )
{
	ChecEBufferDoRealloc(void);

	gosASSERT( Index < m_pBuffer->m_Data_Length );

	return *(m_pBuffer->Data() + Index);
}

/////////////////////////////////////////////////////////////////
inline const EWCSChar& EWCharString::operator[](int32_t Index) const
{
	gosASSERT( Index < m_pBuffer->m_Data_Length );
	
	return *(m_pBuffer->Data() + Index);
}

/////////////////////////////////////////////////////////////////
inline bool EWCharString::operator<( const EWCharString& Greater_String ) const
{
	return ( 0 > Compare( Greater_String, true));
	
}

/////////////////////////////////////////////////////////////////
inline bool operator<( const EWCSChar* p_Lesser_String, 
						const EWCharString& Greater_String )
{
	return ( 0 < Greater_String.Compare( p_Lesser_String, true ) ); 
}
	
/////////////////////////////////////////////////////////////////
inline bool EWCharString::operator<( const EWCSChar* p_Greater_String ) const
{
	return ( 0 > Compare( p_Greater_String, true ) ); 
}

/////////////////////////////////////////////////////////////////
inline bool EWCharString::operator<=( const EWCharString& Greater_String ) const
{
	return ( 1 > Compare( Greater_String, true ) );
}

/////////////////////////////////////////////////////////////////
inline bool operator<=( const EWCSChar* p_Lesser_String, 
						const EWCharString& Greater_String )
{
	return ( 0 < Greater_String.Compare( p_Lesser_String, true ) ); 
					 
}

/////////////////////////////////////////////////////////////////
inline bool EWCharString::operator<=( const EWCSChar* p_Greater_String ) const
{
	return ( 1 > Compare( p_Greater_String, true ) ); 
}


/////////////////////////////////////////////////////////////////
inline bool EWCharString::operator>( const EWCharString& Lesser_String ) const
{
	return ( 0 < Compare( Lesser_String, true ) );
}

/////////////////////////////////////////////////////////////////
inline bool operator>( const EWCSChar* p_Greater_String, 
					    const EWCharString& Lesser_String )
{
	return ( 0 > Lesser_String.Compare( p_Greater_String, true ) );
}

/////////////////////////////////////////////////////////////////
inline bool EWCharString::operator>( const EWCSChar* p_Lesser_String ) const
{
	return ( 0 < Compare( p_Lesser_String, true ) );
}

/////////////////////////////////////////////////////////////////
inline bool EWCharString::operator>=( const EWCharString& Lesser_String ) const
{
	return ( -1 < Compare( Lesser_String, true ) );
}

/////////////////////////////////////////////////////////////////
inline bool operator>=( const EWCSChar* p_Greater_String, 
					    const EWCharString& Lesser_String )
{
	return ( 1 > Lesser_String.Compare( p_Greater_String, true ) );
}

/////////////////////////////////////////////////////////////////
inline bool EWCharString::operator>=( const EWCSChar* p_Lesser_String ) const
{
	return ( -1 < Compare( p_Lesser_String, true ) );
}



//***************************************************************

/* restore UNICODE, K_UNICODE and MBCS to their previous state */
#ifndef _UNICODE_WAS_PREVIOUSLY_DEFINED___EWCHARSTRING_H
#undef UNICODE
#else
#undef _UNICODE_WAS_PREVIOUSLY_DEFINED___EWCHARSTRING_H
#endif /* ndef _UNICODE_WAS_PREVIOUSLY_DEFINED___EWCHARSTRING_H */

#ifndef _K_UNICODE_WAS_PREVIOUSLY_DEFINED___EWCHARSTRING_H
#undef K_UNICODE
#else
#undef _K_UNICODE_WAS_PREVIOUSLY_DEFINED___EWCHARSTRING_H
#endif /* ndef _K_UNICODE_WAS_PREVIOUSLY_DEFINED___EWCHARSTRING_H */

#ifdef _MBCS_WAS_PREVIOUSLY_DEFINED___EWCHARSTRING_H
#undef _MBCS_WAS_PREVIOUSLY_DEFINED___EWCHARSTRING_H
#define MBCS	_PREVIOUS_MBCS___EWCHARSTRING_H
#undef _PREVIOUS_MBCS___EWCHARSTRING_H
#endif /* def _MBCS_WAS_PREVIOUSLY_DEFINED___EWCHARSTRING_H */

//***************************************************************


#endif //EWCharString_H_

