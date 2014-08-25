#ifndef ESTRING_H
#define ESTRING_H

//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
// Notes				essentiall CString with an E.
//				
//***************************************************************


//***************************************************************
#ifdef UNICODE
#define EChar uint16_t
#else
#define EChar char
#endif

typedef const EChar*	ECHARPTR;

#include <stdlib.h>
#include <gameos.hpp>

class EString 
{

public:

	// ENUMERATIONS & CONSTANTS
	static cint32_t INVALID_INDEX;

	// CONSTRUCTORS/DESTRUCTORS
	/*explicit*/ EString( const EChar* newString );
	EString( const EString& );
	/*explicit*/ EString( const EChar );
	EString(void);
	~EString(void);

	operator ECHARPTR(void) const { return Data(void); }
	// MANIPULATORS

	void		Replace( int32_t Start_Index, const EString& String );
	void		Replace( int32_t Start_Index, const EChar* );

	bool		Remove( int32_t Start_Index, int32_t End_Index );
	bool Remove( EString& Sub_String );

	// puts this string in the middle of another string
	inline void	Insert( int32_t Start_Index, const EString& String );
	void	Insert( int32_t Start_Index, const EChar* String ); 

	void		Swap( EString& );
	inline void	Empty(void);
	void		MakeUpper(void);
	void 		MakeLower(void);
	void 		Reverse(void);

	// works like sprintf with the destination buffer being
	// the this pointer
	void	Format( const EChar*, ... );

	inline const EString& operator=( const EChar* );
	inline const EString& operator=( const EString& );
	inline const EString& operator=( EChar );

	inline const EString& operator+=( const EChar* );
	inline const EString& operator+=( const EString& );
	inline const EString& operator+=( EChar );

	friend EString operator+( const EString&, const EString& );

	friend EString operator+( const EChar*, const EString& );
	friend EString operator+( const EString& , const EChar* );

	friend EString operator+( const EString&,  const  EChar );
	friend EString operator+( const EChar,  const  EString& );

 	// ACCESSORS

	// these are case sensitive, use Compare
	// if you don't want case senstitivity
	inline bool operator==( const EString& ) const;
	inline bool operator==( const EChar* ) const;
	inline friend bool operator==( const EChar*, const EString& );

	// these functions return -1 if "this" is less than the passed in string
	int32_t Compare( const EString&, bool Case_Sensitive = false ) const;
	int32_t Compare( const EChar*, bool Case_Sensitive = false ) const;

	bool operator!=( const EString& ) const;
	bool operator!=( const EChar*) const;
	friend bool operator!=( const EChar*, const EString& );

	inline EChar& operator[]( int32_t Index );
	inline const EChar& operator[](int32_t Index) const;

	inline bool operator<( const EString& ) const; 
	inline bool operator<( const EChar* ) const;
	inline friend bool operator<( const EChar*, const EString& );

	inline bool operator<=( const EString& ) const;
	inline bool operator<=( const EChar*) const;
	inline friend bool operator<=( const EChar*, const EString&);

	inline bool operator>( const EString& ) const;
	inline bool operator>(const EChar*) const;
	inline friend bool operator>( const EChar*, const EString&);

	inline bool operator>=( const EString& ) const;
	inline bool operator>=( const EChar*) const;
	inline friend bool operator>=( const EChar*, const EString&);
	
	int32_t Size(void) const; 	// number of bytes
	int32_t Length(void) const;	// number of characters

	// search functions
	int32_t Find( EChar, int32_t Start_Index = EString::INVALID_INDEX) const;
	int32_t Find( const EString&, int32_t Start_Index = EString::INVALID_INDEX) const;
	int32_t Find( const EChar*, int32_t Start_Index = EString::INVALID_INDEX) const;

	int32_t ReverseFind ( EChar, int32_t End_Index = EString::INVALID_INDEX) const;

	
	// we are going to treat this object as a TCHAR array, so we 
	// don't have to worry about #of chars versus #of bytes
	EString SubString( int32_t Start_Index, int32_t End_Index ) const;
	inline EString Left( int32_t Num_Chars) const;	 
	inline EString Right( int32_t Num_Chars) const;	

	inline bool 		IsEmpty(void) const;

	PWSTR	CreateUNICODE(void) const;
	PSTR 	CreateMBCS(void) const; 

	inline	const EChar* Data(void) const;
	void	SetBufferSize( int32_t );

	// ALL UNICODE SPECIFIC AND

#ifndef UNICODE

	int32_t Find( uint16_t, int32_t Start_Index = -1 ) const;

#else // K_UNICODE

	// we'll convert string literals for you 
	
	EString( PCSTR );
			
	friend EString operator+( const EString&, PSTR );
	friend EString operator+( PSTR, const EString& );
	
	friend EString operator+( char, const EString& );
	friend EString operator+( const EString&, char );
	
	const EString& operator+=( PCSTR );
	const EString& operator+=( char );

	const EString& operator=( char );

	bool operator==( PCSTR );
	friend bool operator==( PCSTR, const EString& );

	bool operator!=( PCSTR) const;
	friend bool operator!=( PCSTR, const EString& );

	bool operator<( PCSTR ) const;
	friend bool operator<( PCSTR, const EString& );
	
	bool operator>( PCSTR ) const;
	friend bool operator>( PCSTR, const EString& );

	bool operator<=( PCSTR) const;
	friend bool operator<=( PCSTR, const EString&);

	bool operator>=( PCSTR) const;
	friend bool operator>=( PCSTR, const EString&);

	void	Format( PCSTR, ... );

	int32_t Find( char, int32_t Start_Index = EString::INVALID_INDEX) const;


#endif // Unicode 

private:


	// helper functions

	// Allocates a specific amount
	void	Alloc( int32_t Min_Amount );
	
	// Reallocates if you want to make a change to a shared buffer
	inline	void	ChecEBuffer(void);
	void	ChecEBufferDoRealloc(void);

	// sets the buffer, reallocs if necessary
	void	Assign( const EChar* p_Str );


	static  inline  PWSTR	ToUnicode( puint16_t Buffer, pcuint8_t p_Str, int32_t Num_Chars  );
	static	inline	int32_t	StrSize( const EChar* p_Str );
	
	struct EBuffer
	{
		int32_t m_Ref_Count;		// reference count
		int32_t m_Data_Length;		// Length of String
		int32_t m_Alloc_Length;	// Length of the Buffer
	
		inline EChar* Data(void);
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
inline void	EString::ChecEBuffer()
{
	if ( m_pBuffer &&  m_pBuffer->m_Ref_Count > 0 )
	{
		m_pBuffer->m_Ref_Count --;
		m_pBuffer = EBuffer::s_p_Empty_Buffer;
	}
}

/////////////////////////////////////////////////////////////////
inline void EString::EBuffer::Release()
{
	if ( this != s_p_Empty_Buffer )
	{
		m_Ref_Count --;
		if ( m_Ref_Count < 0 )
		{
			free( (PSTR)this ); 
		}
	}
}

/////////////////////////////////////////////////////////////////
inline void	EString::Empty()
{
	m_pBuffer->Release(void);
	m_pBuffer =  EBuffer::s_p_Empty_Buffer;
}


/////////////////////////////////////////////////////////////////
inline void EString::Insert( int32_t Start_Index, const EString& String )
{
	Insert( Start_Index, String.m_pBuffer->Data() );
}

/////////////////////////////////////////////////////////////////
inline const EString& EString::operator=( const EChar* p_String )
{
	ChecEBuffer(void);
	Assign( p_String );
	return *this;
}

/////////////////////////////////////////////////////////////////
inline const EString& EString::operator=( const EString& Src )
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
inline const EString& EString::operator=( EChar Char )
{
	ChecEBuffer(void);

	EChar Tmp[2];
	Tmp[0] = Char;
	Tmp[1] = 0;
	Assign( Tmp );

	return *this;
}

/////////////////////////////////////////////////////////////////
inline const EString& EString::operator+=( const EChar* p_String )
{
	Insert( m_pBuffer->m_Data_Length, p_String );
	return *this;
}

/////////////////////////////////////////////////////////////////
inline const EString& EString::operator+=( const EString& String )
{
	Insert( m_pBuffer->m_Data_Length, String );
	return *this;
}

/////////////////////////////////////////////////////////////////
inline const EString& EString::operator+=( EChar Char )
{
	EChar Tmp[2];
	Tmp[0] = Char;
	Tmp[1] = 0;
	Insert(  m_pBuffer->m_Data_Length, Tmp );

	return *this;	
}

/////////////////////////////////////////////////////////////////
inline bool EString::operator!=( const EString& Str_To_Compare ) const
{
	return !( operator==(Str_To_Compare) ); 
}

/////////////////////////////////////////////////////////////////
inline bool EString::operator!=( const EChar* p_Str_To_Compare ) const
{
	return !( operator==( p_Str_To_Compare ) );
}

/////////////////////////////////////////////////////////////////
inline EString EString::Left( int32_t Num_Chars) const
{
	// Bill changed to Num_Chars - 1, this was always returning one character too many
	return SubString( 0, Num_Chars - 1 ); 
}

/////////////////////////////////////////////////////////////////
inline EString EString::Right( int32_t Num_Chars) const
{
	return SubString( m_pBuffer->m_Data_Length - Num_Chars, 
		m_pBuffer->m_Data_Length - 1 );
}

/////////////////////////////////////////////////////////////////
inline bool EString::IsEmpty(void) const
{
	return (m_pBuffer->m_Data_Length <= 0);
}

/////////////////////////////////////////////////////////////////
inline	const EChar* EString::Data(void) const
{
	return ( m_pBuffer->Data() );
}

/////////////////////////////////////////////////////////////////
inline EChar* EString::EBuffer::Data()
{
	if ( !m_Alloc_Length )
	{
		return 0;
	}
	return (EChar*)(this + 1);
}

/////////////////////////////////////////////////////////////////
inline bool EString::operator==( const EString& Str_To_Compare) const
{
	return ( 0 == Compare( Str_To_Compare, true ) );
}

/////////////////////////////////////////////////////////////////
inline bool EString::operator==( const EChar* p_String ) const
{

	return ( 0 == Compare( p_String, true ) );
}

/////////////////////////////////////////////////////////////////
inline bool operator==( const EChar* p_String, const EString& Str )
{
	return ( 0 == Str.Compare( p_String ) );
}

/////////////////////////////////////////////////////////////////
inline bool operator!=( const EChar* p_String, const EString& Str )
{
	return !(Str == p_String );
}
/////////////////////////////////////////////////////////////////
inline EChar& EString::operator[]( int32_t Index )
{
	ChecEBufferDoRealloc(void);

	gosASSERT( Index < m_pBuffer->m_Data_Length + 1 ); // we'll give you the null

	return *(m_pBuffer->Data() + Index);
}

/////////////////////////////////////////////////////////////////
inline const EChar& EString::operator[](int32_t Index) const
{
	gosASSERT( Index < m_pBuffer->m_Data_Length );
	
	return *(m_pBuffer->Data() + Index);
}

/////////////////////////////////////////////////////////////////
inline bool EString::operator<( const EString& Greater_String ) const
{
	return ( 0 > Compare( Greater_String, true));
	
}

/////////////////////////////////////////////////////////////////
inline bool operator<( const EChar* p_Lesser_String, 
						const EString& Greater_String )
{
	return ( 0 < Greater_String.Compare( p_Lesser_String, true ) ); 
}
	
/////////////////////////////////////////////////////////////////
inline bool EString::operator<( const EChar* p_Greater_String ) const
{
	return ( 0 > Compare( p_Greater_String, true ) ); 
}

/////////////////////////////////////////////////////////////////
inline bool EString::operator<=( const EString& Greater_String ) const
{
	return ( 1 > Compare( Greater_String, true ) );
}

/////////////////////////////////////////////////////////////////
inline bool operator<=( const EChar* p_Lesser_String, 
						const EString& Greater_String )
{
	return ( 0 < Greater_String.Compare( p_Lesser_String, true ) ); 
					 
}

/////////////////////////////////////////////////////////////////
inline bool EString::operator<=( const EChar* p_Greater_String ) const
{
	return ( 1 > Compare( p_Greater_String, true ) ); 
}


/////////////////////////////////////////////////////////////////
inline bool EString::operator>( const EString& Lesser_String ) const
{
	return ( 0 < Compare( Lesser_String, true ) );
}

/////////////////////////////////////////////////////////////////
inline bool operator>( const EChar* p_Greater_String, 
					    const EString& Lesser_String )
{
	return ( 0 > Lesser_String.Compare( p_Greater_String, true ) );
}

/////////////////////////////////////////////////////////////////
inline bool EString::operator>( const EChar* p_Lesser_String ) const
{
	return ( 0 < Compare( p_Lesser_String, true ) );
}

/////////////////////////////////////////////////////////////////
inline bool EString::operator>=( const EString& Lesser_String ) const
{
	return ( -1 < Compare( Lesser_String, true ) );
}

/////////////////////////////////////////////////////////////////
inline bool operator>=( const EChar* p_Greater_String, 
					    const EString& Lesser_String )
{
	return ( 1 > Lesser_String.Compare( p_Greater_String, true ) );
}

/////////////////////////////////////////////////////////////////
inline bool EString::operator>=( const EChar* p_Lesser_String ) const
{
	return ( -1 < Compare( p_Lesser_String, true ) );
}




#endif //EString_H_

