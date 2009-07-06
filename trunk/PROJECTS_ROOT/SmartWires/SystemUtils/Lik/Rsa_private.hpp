// RSA public key encryption
#if !defined(AFX_RSAPRIV__INCLUDED_)
#define AFX_RSAPRIV__INCLUDED_

#include "vlong.hpp"
#include "rsa_public.hpp"

class private_key : public public_key
{
public:
	vlong p,q;
	vlong decrypt( const vlong& cipher );
	
	void create( const char * r1, const char * r2 );
	// r1 and r2 should be null terminated random strings
	// each of length around 35 characters (for a 500 bit modulus)
};

// Перевод числа в строку
CString to_strRB( vlong x );
CString to_strBB( vlong x );
CString to_str( vlong x );
#endif