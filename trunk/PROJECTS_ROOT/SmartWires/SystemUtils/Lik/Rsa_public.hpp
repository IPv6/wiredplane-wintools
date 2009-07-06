// RSA public key encryption
#if !defined(AFX_RSAPUB__INCLUDED_)
#define AFX_RSAPUB__INCLUDED_

#include "vlong.hpp"

class public_key
{
public:
	public_key()
	{
		m=0;
		e=0;
	};
	vlong m,e;
	vlong encrypt( const vlong& plain ); // Requires 0 <= plain < m
};

#endif