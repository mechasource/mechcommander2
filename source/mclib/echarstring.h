#ifndef ECHARSTRING_H
#define ECHARSTRING_H

//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
// Notes				essentially EString, except that it uses chars (not wchars) even when UNICODE is defined
//				
//***************************************************************

//***************************************************************

/* just for this file, we want UNICODE, K_UNICODE and MBCS undefined */
#ifdef UNICODE
#define _UNICODE_WAS_PREVIOUSLY_DEFINED___ECHARSTRING_H
#define _PREVIOUS_UNICODE___ECHARSTRING_H	UNICODE
#undef UNICODE
#endif /* def UNICODE */

#ifdef K_UNICODE
#define _K_UNICODE_WAS_PREVIOUSLY_DEFINED___ECHARSTRING_H
#define _PREVIOUS_K_UNICODE___ECHARSTRING_H		UNICODE
#undef K_UNICODE
#endif /* def K_UNICODE */

#ifdef MBCS
#define _MBCS_WAS_PREVIOUSLY_DEFINED___ECHARSTRING_H
#define _PREVIOUS_MBCS___ECHARSTRING_H		MBCS
#undef MBCS
#endif /* def MBCS */

//***************************************************************



#ifdef UNICODE
#define ECSChar uint16_t
#else
#define ECSChar char
#endif

#include <gameos.hpp>

class ECharString 
{

public:

	// ENUMERATIONS & CONSTANTS
	static cint32_t INVALID_INDEX;

	// CONSTRUCTORS/DESTRUCTORS
	/*explicit*/ ECharString( const ECSChar* newString );
	ECharString( const ECharString& );
	/*explicit*/ ECharString( const ECSChar );
	ECharString(void);
	~ECharString(void);

	// MANIPULATORS

	void		Replace( int32_t Start_Index, const ECharString& String );
	void		Replace( int32_t Start_Index, const ECSChar* );

	bool		Remove( int32_t Start_Index, int32_t End_Index );
	bool Remove( ECharString& Sub_String );

	// puts this string in the middle of another string
	inline void	Insert( int32_t Start_Index, const ECharString& String );
	void	Insert( int32_t Start_Index, const ECSChar* String ); 

	void		Swap( ECharString& );
	inline void	Empty(void);
	void		MakeUpper(void);
	void 		MakeLower(void);
	void 		Reverse(void);

	// works like sprintf with the destination buffer being
	// the this pointer
	void	Format( const ECSChar*, ... );

	inline const ECharString& operator=( const ECSChar* );
	inline const ECharString& operator=( const ECharString& );
	inline const ECharString& operator=( ECSChar );

	inline const ECharString& operator+=( const ECSChar* );
	inline const ECharString& operator+=( const ECharString& );
	inline const ECharString& operator+=( ECSChar );

	friend ECharString operator+( const ECharString&, const ECharString& );

	friend ECharString operator+( const ECSChar*, const ECharString& );
	friend ECharString operator+( const ECharString& , const ECSChar* );

	friend ECharString operator+( const ECharString&,  const  ECSChar );
	friend ECharString operator+( const ECSChar,  const  ECharString& );

 	// ACCESSORS

	// these are case sensitive, use Compare
	// if you don't want case senstitivity
	inline bool operator==( const ECharString& ) const;
	inline bool operator==( const ECSChar* ) const;
	inline friend bool operator==( const ECSChar*, const ECharString& );

	// these functions return -1 if "this" is less than the passed in string
	int32_t Compare( const ECharString&, bool Case_Sensitive = false ) const;
	int32_t Compare( const ECSChar*, bool Case_Sensitive = false ) const;

	bool operator!=( const ECharString& ) const;
	bool operator!=( const ECSChar*) const;
	friend bool operator!=( const ECSChar*, const ECharString& );

	inline ECSChar& operator[]( int32_t Index );
	inline const ECSChar& operator[](int32_t Index) const;

	inline bool operator<( const ECharString& ) const; 
	inline bool operator<( const ECSChar* ) const;
	inline friend bool operator<( const ECSChar*, const ECharString& );

	inline bool operator<=( const ECharString& ) const;
	inline bool operator<=( const ECSChar*) const;
	inline friend bool operator<=( const ECSChar*, const ECharString&);

	inline bool operator>( const ECharString& ) const;
	inline bool operator>(const ECSChar*) const;
	inline friend bool operator>( const ECSChar*, const ECharString&);

	inline bool operator>=( const ECharString& ) const;
	inline bool operator>=( const ECSChar*) const;
	inline friend bool operator>=( const ECSChar*, const ECharString&);
	
	int32_t Size(void) const; 	// number of bytes
	int32_t Length(void) const;	// number of characters

	// search functions
	int32_t Find( ECSChar, int32_t Start_Index = ECharString::INVALID_INDEX) const;
	int32_t Find( const ECharString&, int32_t Start_Index = ECharString::INVALID_INDEX) const;
	int32_t Find( const ECSChar*, int32_t Start_Index = ECharString::INVALID_INDEX) const;

	int32_t ReverseFind ( ECSChar, int32_t End_Index = ECharString::INVALID_INDEX) const;

	
	// we are going to treat this object as a TCHAR array, so we 
	// don't have to worry about #of chars versus #of bytes
	ECharString SubString( int32_t Start_Index, int32_t End_Index ) const;
	inline ECharString Left( int32_t Num_Chars) const;	 
	inline ECharString Right( int32_t Num_Chars) const;	

	inline bool 		IsEmpty(void) const;

	PWSTR	CreateUNICODE(void) const;
	PSTR 	CreateMBCS(void) const; 

	inline	const ECSChar* Data(void) const;

	// ALL UNICODE SPECIFIC AND

#ifndef UNICODE

	int32_t Find( uint16_t, int32_t Start_Index = -1 ) const;

#else // K_UNICODE

	// we'll convert string literals for you 
	
	ECharString( PCSTR );
			
	friend ECharString operator+( const ECharString&, PSTR );
	friend ECharString operator+( PSTR, const ECharString& );
	
	friend ECharString operator+( char, const ECharString& );
	friend ECharString operator+( const ECharString&, char );
	
	const ECharString& operator+=( PCSTR );
	const ECharString& operator+=( char );

	const ECharString& operator=( char );

	bool operator==( PCSTR );
	friend bool operator==( PCSTR, const ECharString& );

	bool operator!=( PCSTR) const;
	friend bool operator!=( PCSTR, const ECharString& );

	bool operator<( PCSTR ) const;
	friend bool operator<( PCSTR, const ECharString& );
	
	bool operator>( PCSTR ) const;
	friend bool operator>( PCSTR, const ECharString& );

	bool operator<=( PCSTR) const;
	friend bool operator<=( PCSTR, const ECharString&);

	bool operator>=( PCSTR) const;
	friend bool operator>=( PCSTR, const ECharString&);

	void	Format( PCSTR, ... );

	int32_t Find( char, int32_t Start_Index = ECharString::INVALID_INDEX) const;


#endif // Unicode 

private:


	// helper functions

	// Allocates a specific amount
	void	Alloc( int32_t Min_Amount );
	
	// Reallocates if you want to make a change to a shared buffer
	inline	void	ChecEBuffer(void);
	void	ChecEBufferDoRealloc(void);

	// sets the buffer, reallocs if necessary
	void	Assign( const ECSChar* p_Str );


	static  inline  PWSTR	ToUnicode( puint16_t Buffer, pcuint8_t p_Str, int32_t Num_Chars  );
	static	inline	int32_t	StrSize( const ECSChar* p_Str );
	
	struct EBuffer
	{
		int32_t m_Ref_Count;		// reference count
		int32_t m_Data_Length;		// Length of String
		int32_t m_Alloc_Length;	// Length of the Buffer
	
		inline ECSChar* Data(void);
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
inline void	ECharString::ChecEBuffer()
{
	if ( m_pBuffer->m_Ref_Count > 0 )
	{
		m_pBuffer->m_Ref_Count --;
		m_pBuffer = EBuffer::s_p_Empty_Buffer;
	}
}

/////////////////////////////////////////////////////////////////
inline void ECharString::EBuffer::Release()
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
inline void	ECharString::Empty()
{
	m_pBuffer->Release(void);
	m_pBuffer =  EBuffer::s_p_Empty_Buffer;
}


/////////////////////////////////////////////////////////////////
inline void ECharString::Insert( int32_t Start_Index, const ECharString& String )
{
	Insert( Start_Index, String.m_pBuffer->Data() );
}

/////////////////////////////////////////////////////////////////
inline const ECharString& ECharString::operator=( const ECSChar* p_String )
{
	ChecEBuffer(void);
	Assign( p_String );
	return *this;
}

/////////////////////////////////////////////////////////////////
inline const ECharString& ECharString::operator=( const ECharString& Src )
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
inline const ECharString& ECharString::operator=( ECSChar Char )
{
	ChecEBuffer(void);

	ECSChar Tmp[2];
	Tmp[0] = Char;
	Tmp[1] = 0;
	Assign( Tmp );

	return *this;
}

/////////////////////////////////////////////////////////////////
inline const ECharString& ECharString::operator+=( const ECSChar* p_String )
{
	Insert( m_pBuffer->m_Data_Length, p_String );
	return *this;
}

/////////////////////////////////////////////////////////////////
inline const ECharString& ECharString::operator+=( const ECharString& String )
{
	Insert( m_pBuffer->m_Data_Length, String );
	return *this;
}

/////////////////////////////////////////////////////////////////
inline const ECharString& ECharString::operator+=( ECSChar Char )
{
	ECSChar Tmp[2];
	Tmp[0] = Char;
	Tmp[1] = 0;
	Insert(  m_pBuffer->m_Data_Length, Tmp );

	return *this;	
}

/////////////////////////////////////////////////////////////////
inline bool ECharString::operator!=( const ECharString& Str_To_Compare ) const
{
	return !( operator==(Str_To_Compare) ); 
}

/////////////////////////////////////////////////////////////////
inline bool ECharString::operator!=( const ECSChar* p_Str_To_Compare ) const
{
	return !( operator==( p_Str_To_Compare ) );
}

/////////////////////////////////////////////////////////////////
inline ECharString ECharString::Left( int32_t Num_Chars) const
{
	// Bill changed to Num_Chars - 1, this was always returning one character too many
	return SubString( 0, Num_Chars - 1 ); 
}

/////////////////////////////////////////////////////////////////
inline ECharString ECharString::Right( int32_t Num_Chars) const
{
	return SubString( m_pBuffer->m_Data_Length - Num_Chars, 
		m_pBuffer->m_Data_Length - 1 );
}

/////////////////////////////////////////////////////////////////
inline bool ECharString::IsEmpty(void) const
{
	return (m_pBuffer->m_Data_Length <= 0);
}

/////////////////////////////////////////////////////////////////
inline	const ECSChar* ECharString::Data(void) const
{
	return ( m_pBuffer->Data() );
}

/////////////////////////////////////////////////////////////////
inline ECSChar* ECharString::EBuffer::Data()
{
	if ( !m_Alloc_Length )
	{
		return 0;
	}
	return (ECSChar*)(this + 1);
}

/////////////////////////////////////////////////////////////////
inline bool ECharString::operator==( const ECharString& Str_To_Compare) const
{
	return ( 0 == Compare( Str_To_Compare, true ) );
}

/////////////////////////////////////////////////////////////////
inline bool ECharString::operator==( const ECSChar* p_String ) const
{

	return ( 0 == Compare( p_String, true ) );
}

/////////////////////////////////////////////////////////////////
inline bool operator==( const ECSChar* p_String, const ECharString& Str )
{
	return ( 0 == Str.Compare( p_String ) );
}

/////////////////////////////////////////////////////////////////
inline bool operator!=( const ECSChar* p_String, const ECharString& Str )
{
	return !(Str == p_String );
}
/////////////////////////////////////////////////////////////////
inline ECSChar& ECharString::operator[]( int32_t Index )
{
	ChecEBufferDoRealloc(void);

	gosASSERT( Index < m_pBuffer->m_Data_Length );

	return *(m_pBuffer->Data() + Index);
}

/////////////////////////////////////////////////////////////////
inline const ECSChar& ECharString::operator[](int32_t Index) const
{
	gosASSERT( Index < m_pBuffer->m_Data_Length );
	
	return *(m_pBuffer->Data() + Index);
}

/////////////////////////////////////////////////////////////////
inline bool ECharString::operator<( const ECharString& Greater_String ) const
{
	return ( 0 > Compare( Greater_String, true));
	
}

/////////////////////////////////////////////////////////////////
inline bool operator<( const ECSChar* p_Lesser_String, 
						const ECharString& Greater_String )
{
	return ( 0 < Greater_String.Compare( p_Lesser_String, true ) ); 
}
	
/////////////////////////////////////////////////////////////////
inline bool ECharString::operator<( const ECSChar* p_Greater_String ) const
{
	return ( 0 > Compare( p_Greater_String, true ) ); 
}

/////////////////////////////////////////////////////////////////
inline bool ECharString::operator<=( const ECharString& Greater_String ) const
{
	return ( 1 > Compare( Greater_String, true ) );
}

/////////////////////////////////////////////////////////////////
inline bool operator<=( const ECSChar* p_Lesser_String, 
						const ECharString& Greater_String )
{
	return ( 0 < Greater_String.Compare( p_Lesser_String, true ) ); 
					 
}

/////////////////////////////////////////////////////////////////
inline bool ECharString::operator<=( const ECSChar* p_Greater_String ) const
{
	return ( 1 > Compare( p_Greater_String, true ) ); 
}


/////////////////////////////////////////////////////////////////
inline bool ECharString::operator>( const ECharString& Lesser_String ) const
{
	return ( 0 < Compare( Lesser_String, true ) );
}

/////////////////////////////////////////////////////////////////
inline bool operator>( const ECSChar* p_Greater_String, 
					    const ECharString& Lesser_String )
{
	return ( 0 > Lesser_String.Compare( p_Greater_String, true ) );
}

/////////////////////////////////////////////////////////////////
inline bool ECharString::operator>( const ECSChar* p_Lesser_String ) const
{
	return ( 0 < Compare( p_Lesser_String, true ) );
}

/////////////////////////////////////////////////////////////////
inline bool ECharString::operator>=( const ECharString& Lesser_String ) const
{
	return ( -1 < Compare( Lesser_String, true ) );
}

/////////////////////////////////////////////////////////////////
inline bool operator>=( const ECSChar* p_Greater_String, 
					    const ECharString& Lesser_String )
{
	return ( 1 > Lesser_String.Compare( p_Greater_String, true ) );
}

/////////////////////////////////////////////////////////////////
inline bool ECharString::operator>=( const ECSChar* p_Lesser_String ) const
{
	return ( -1 < Compare( p_Lesser_String, true ) );
}



//***************************************************************

/* restore UNICODE, K_UNICODE and MBCS to their previous state */
#ifdef _UNICODE_WAS_PREVIOUSLY_DEFINED___ECHARSTRING_H
#undef _UNICODE_WAS_PREVIOUSLY_DEFINED___ECHARSTRING_H
#define UNICODE		_PREVIOUS_UNICODE___ECHARSTRING_H
#undef _PREVIOUS_UNICODE___ECHARSTRING_H
#endif /* def _UNICODE_WAS_PREVIOUSLY_DEFINED___ECHARSTRING_H */

#ifdef _K_UNICODE_WAS_PREVIOUSLY_DEFINED___ECHARSTRING_H
#undef _K_UNICODE_WAS_PREVIOUSLY_DEFINED___ECHARSTRING_H
#define K_UNICODE	_PREVIOUS_K_UNICODE___ECHARSTRING_H
#undef _PREVIOUS_K_UNICODE___ECHARSTRING_H
#endif /* def _K_UNICODE_WAS_PREVIOUSLY_DEFINED___ECHARSTRING_H */

#ifdef _MBCS_WAS_PREVIOUSLY_DEFINED___ECHARSTRING_H
#undef _MBCS_WAS_PREVIOUSLY_DEFINED___ECHARSTRING_H
#define MBCS	_PREVIOUS_MBCS___ECHARSTRING_H
#undef _PREVIOUS_MBCS___ECHARSTRING_H
#endif /* def _MBCS_WAS_PREVIOUSLY_DEFINED___ECHARSTRING_H */

//***************************************************************


#endif //ECharString_H_

