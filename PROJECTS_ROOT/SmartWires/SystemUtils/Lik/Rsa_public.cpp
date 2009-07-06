#include "rsa_public.hpp"

vlong public_key::encrypt( const vlong& plain )
{
	return modexp( plain, e, m );
}
