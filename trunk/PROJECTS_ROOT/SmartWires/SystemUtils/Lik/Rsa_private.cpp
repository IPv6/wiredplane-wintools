#include "rsa_private.hpp"

void private_key::create( const char * r1, const char * r2 )
{
	// Choose primes
	{
		prime_factory pf;
		p = pf.find_prime( from_str(r1) );
		q = pf.find_prime( from_str(r2) );
		if ( p > q ) { vlong tmp = p; p = q; q = tmp; }
	}
	// Calculate public key
	{
		m = p*q;
		e = 50001; // must be odd since p-1 and q-1 are even
		while ( gcd(p-vlong(1),e) != vlong(1) || gcd(q-vlong(1),e) != vlong(1) ) e += 2;
	}
}

vlong private_key::decrypt( const vlong& cipher )
{
	// Calculate values for performing decryption
	// These could be cached, but the calculation is quite fast
	vlong d = modinv( e, (p-vlong(1))*(q-vlong(1)) );
	vlong u = modinv( p, q );
	vlong dp = d % (p-vlong(1));
	vlong dq = d % (q-vlong(1));
	
	// Apply chinese remainder theorem
	vlong a = modexp( cipher % p, dp, p );
	vlong b = modexp( cipher % q, dq, q );
	if ( b < a ) b += q;
	return a + p * ( ((b-a)*u) % q );
}

extern vlong vCharBaseRb;
extern vlong vCharBaseBb;
CString to_strBB( vlong x )
{
	CString sRes="";
	while (x >= vCharBaseBb)
	{
		char c=(unsigned char)((unsigned char)(x % vCharBaseBb)+(unsigned char)FIRSTCHAR_BB);
		sRes=sRes+c;
		x=(x - (x % vCharBaseBb))/vCharBaseBb;
	}
	sRes=sRes+(CString)(char((unsigned char)(x)+(unsigned char)FIRSTCHAR_BB));
	sRes.MakeReverse();
	return sRes;
}


CString to_strRB( vlong x )
{
	CString sRes="";
	CString sRawRes="";
	while (x >= vCharBaseRb)
	{
		if(sRawRes!=""){
			if(sRawRes.GetLength()%30==0){
				sRes+='\n';
			}else if(sRawRes.GetLength()%10==0){
				sRes+='-';
			}
		}
		//================================
		char c=(unsigned char)((unsigned char)(x % vCharBaseRb)+(unsigned char)FIRSTCHAR_RB);
		sRes=sRes+c;
		sRawRes=sRawRes+c;
		x=(x - (x % vCharBaseRb))/vCharBaseRb;
	}
	sRes=sRes+(CString)(char((unsigned char)(x)+(unsigned char)FIRSTCHAR_RB));
	sRes.MakeReverse();
	return sRes;
}

CString to_str( vlong x)
{
	CString sRes="";
	while (x > vlong(1))
	{
		CString c=char(x % vlong(256));
		sRes=c+sRes;
		x=(x - x % vlong(256))/vlong(256);
	}
	return sRes;
}