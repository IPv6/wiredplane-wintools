#include "stdafx.h"
#include "Stdlik.h"
#include <wchar.h>
#include "../SupportClasses.h"
#include "checksum.h"

CString GetHash(const char* p)
{
	checksum current;
    current.add(p);
	return Format("%x",current.get());
}

CString B2H(const unsigned char* digest, int iLen)
{
	if(digest==0){
		return "";
	}
	unsigned char const* pc = digest;
	if(iLen<0){
		iLen=strlen((const char *)digest);
	}
	CString result;
	for(int i=0;i<iLen;i++) 
    { // convert md to hex-represented string (hex-letters in upper case!)
		CString sHex;
		sHex.Format("%02x",(*pc));
		result += sHex;
		pc++;
	}
	return result;
}

CString MD5_HexBytes(BYTE* SourceString, int iSize)
{
	unsigned char digest[16] = {0};    // message digest
	MD5_CTX context;
	MD5Init(&context);
	MD5Update(&context, (unsigned char*)SourceString, iSize);
	MD5Final(digest, &context);
	return B2H((const unsigned char*)digest,16);
}

CString MD5_HexString(const char* SourceString)
{
	unsigned char digest[16] = {0};    // message digest
	CString tmp = SourceString;
	MD5_CTX context;
	MD5Init(&context);
	MD5Update(&context, (unsigned char*)(const char*)tmp, tmp.GetLength());
	MD5Final(digest, &context);
	return B2H((const unsigned char*)digest,16);
}

/////////////////////////////////////////////////////////////////////////////
// ARACrypt operations

void ARACrypt::SetKey(LPCTSTR csKey)
{
#ifndef ART_VERSION
	CString csSeed;
	
	m_csKey = csKey;
	
	if (m_csKey.IsEmpty())
	{
		//Put some really outrageous characters in seed just in case.
		csSeed = "\x43\xC8\x21\xD3\xF4\xB3\x10\x27\x09\xAA\x18\x56";
		
		//TO DO: Add Message to Event Log and/or window when there is one.
		//		AfxMessageBox("WARNING: Missing Pass Phrase. Default in effect!");
	}
	else
	{
		csSeed = m_csKey;
	}
	
	// Make sure seed is at least 12 bytes long . 
	
	int nIdx = 0;
	
	for (nIdx = 0; csSeed.GetLength() < 12; nIdx++)
	{
		csSeed += csSeed[nIdx];
	}
	
	CRYPT_START
		// LFSR A, B, and C get the first, second, and
		// third four bytes of the seed, respectively.
		
		for (nIdx = 0; nIdx < 4; nIdx++)
		{
			m_LFSR_A = ((m_LFSR_A <<= 8) | 
				((unsigned long int) csSeed[nIdx + 0]));
			
			m_LFSR_B = (( m_LFSR_B <<= 8) |
				((unsigned long int) csSeed[nIdx + 4]));
			
			m_LFSR_C = (( m_LFSR_C <<= 8) | 
				((unsigned long int) csSeed[nIdx + 8]));
		}
		
		
		// If any LFSR contains 0x00000000, load a 
		// non-zero default value instead. There is
		// no particular "good" value. The ones here
		// were selected to be distinctive during
		// testing and debugging.
		
		if (0x00000000 == m_LFSR_A)
			m_LFSR_A = 0x13579BDF;
		
		if (0x00000000 == m_LFSR_B)
			m_LFSR_B = 0x2468ACE0;
		
		if (0x00000000 == m_LFSR_C)
			m_LFSR_C = 0xFDB97531;
		CRYPT_END
		return;
	#endif	
}

void ARACrypt::GetKey(CString& csKey)
{
	csKey = m_csKey;
}

/////////////////////////////////////////////////////////////////////////////
// ARACrypt

ARACrypt::ARACrypt()
:

// Initialize the shift registers to non-zero 
// values. If the shift register contains all 
// 0's when the generator starts, it will not 
// produce a usable sequence of bits. The 
// numbers used here have no special features 
// except for being non-zero.

m_LFSR_A( 0x13579ADF ),
m_LFSR_B( 0x2438ACE0 ),
m_LFSR_C( 0xFDB97511 ),


// Initialize the masks to magic numbers. 
// These values are primitive polynomials mod 
// 2, described in Applied Cryptography, 
// second edition, by Bruce Schneier (New York: 
// John Wiley and Sons, 1994). See Chapter 15: 
// Random Sequence Generators and Stream 
// Ciphers, particularly the discussion on 
// Linear Feedback Shift Registers.
//
// The primitive polynomials used here are:
// Register A:	( 32, 7, 6, 2, 0 )
// Register B:	( 31, 6, 0 )
// Register C:	( 29, 2, 0 )
//
// The bits that must be set to "1" in the 
// XOR masks are:
// Register A:	( 31, 6, 5, 1 )
// Register B:	( 30, 5 )
// Register C:	( 28, 1 )
//
// Developer's Note
// DO NOT CHANGE THESE NUMBERS WITHOUT 
// REFERRING TO THE DISCUSSION IN SCHNEIER'S 
// BOOK. They are some of very few 
// near-32-bit values that will act as 
// maximal-length random generators.

m_Mask_A( 0x80000062 ), 
m_Mask_B( 0x40000020 ), 
m_Mask_C( 0x10000002 ), 


// Set up LFSR "rotate" masks.
// These masks limit the number of bits 
// used in the shift registers. Each one 
// provides the most-significant bit (MSB) 
// when performing a "rotate" operation. Here 
// are the shift register sizes and the byte 
// mask needed to place a "1" bit in the MSB 
// for Rotate-1, and a zero in the MSB for 
// Rotate-0. All the shift registers are stored
// in an unsigned 32-bit integer, but these 
// masks effectively make the registers 32 
// bits (A), 31 bits (B), and 29 bits (C).
//
//	Bit	  |  3            2             1            0
//	Pos'n | 1098 7654  3210 9876  5432 1098  7654 3210
//	===== | ==========================================
//	Value | 8421-8421  8421-8421  8421-8421  8421-8421
//	===== | ==========================================
//		  | 
// A-Rot0 | 0111 1111  1111 1111  1111 1111  1111 1111  
// A-Rot1 | 1000 0000  0000 0000  0000 0000  0000 0000 
//		  | 
// B-Rot0 | 0011 1111  1111 1111  1111 1111  1111 1111  
// B-Rot1 | 1100 0000  0000 0000  0000 0000  0000 0000  
//		  | 
// C-Rot0 | 0000 1111  1111 1111  1111 1111  1111 1111  
// C-Rot1 | 1111 0000  0000 0000  0000 0000  0000 0000  
//
//	
// Reg Size	MSB Position	Rotate-0 Mask	Rotate-1 Mask
//	A	32		31			0x7FFFFFFF		0x80000000
//	B	31		30			0x3FFFFFFF		0xC0000000
//	C	29		28			0x0FFFFFFF		0xF0000000
//

m_Rot0_A( 0x7FFFFFFF ), 
m_Rot0_B( 0x3FFFFFFF ), 
m_Rot0_C( 0x0FFFFFFF ),
m_Rot1_A( 0x80000000 ), 
m_Rot1_B( 0xC0000000 ), 
m_Rot1_C( 0xF0000000 )
{
	m_csKey = _T("");
}


// Everything is on the frame.
ARACrypt::~ARACrypt()
{
}



//***********************************************
// ARACrypt::TransformChar
//***********************************************
// Transform a single character. If it is 
// plaintext, it will be encrypted. If it is
// encrypted, and if the LFSRs are in the same
// state as when it was encrypted (that is, the
// same keys loaded into them and the same number
// of calls to TransformChar after the keys
// were loaded), the character will be decrypted
// to its original value.
//
// DEVELOPER'S NOTE
// This code contains corrections to the LFSR
// operations that supercede the code examples
// in Applied Cryptography (first edition, up to
// at least the 4th printing, and second edition,
// up to at least the 6th printing). More recent
// errata sheets may show the corrections.
//***********************************************

void ARACrypt::TransformChar(unsigned char& cTarget)
{
#ifndef ART_VERSION
	CRYPT_START
	int					Counter	= 0;
	unsigned char		Crypto	= '\0';
	unsigned long int	Out_B	= (m_LFSR_B & 0x00000001);
	unsigned long int	Out_C	= (m_LFSR_C & 0x00000001);
	
	// Cycle the LFSRs eight times to get eight 
	// pseudo-random bits. Assemble these into 
	// a single random character (Crypto).
	
	for (Counter = 0; Counter < 8; Counter++)
	{
		if (m_LFSR_A & 0x00000001)
		{
			// The least-significant bit of LFSR 
			// A is "1". XOR LFSR A with its 
			// feedback mask.
			
			m_LFSR_A = (((m_LFSR_A ^ m_Mask_A) >> 1) | m_Rot1_A);
			
			// Clock shift register B once.
			if ( m_LFSR_B & 0x00000001 )
			{
				// The LSB of LFSR B is "1". XOR 
				// LFSR B with its feedback mask.
				
				m_LFSR_B = (((m_LFSR_B ^ m_Mask_B) >> 1) | m_Rot1_B);
				
				Out_B = 0x00000001;
			}
			else
			{
				// The LSB of LFSR B is "0". Rotate 
				// the LFSR contents once.
				
				m_LFSR_B = (( m_LFSR_B >> 1) & m_Rot0_B);
				
				Out_B = 0x00000000;
			}
		}
		else
		{
			// The LSB of LFSR A is "0". 
			// Rotate the LFSR contents once.
			
			m_LFSR_A = (( m_LFSR_A >> 1) & m_Rot0_A);
			
			
			// Clock shift register C once.
			
			if ( m_LFSR_C & 0x00000001 )
			{
				// The LSB of LFSR C is "1". 
				// XOR LFSR C with its feedback mask.
				
				m_LFSR_C = ((( m_LFSR_C ^ m_Mask_C) >> 1) | m_Rot1_C);
				Out_C = 0x00000001;
			}
			else
			{
				// The LSB of LFSR C is "0". Rotate 
				// the LFSR contents once.
				
				m_LFSR_C = ((m_LFSR_C >> 1) & m_Rot0_C);
				
				Out_C = 0x00000000;
			}
			
		}
		
		// XOR the output from LFSRs B and C and 
		// rotate it into the right bit of Crypto.
		
		//The follwing conversion warning is unecessary here as 
		//'loss of data' is a side effect and harmless.
#pragma warning(disable : 4244)
		
		Crypto = ((Crypto << 1) | (Out_B ^ Out_C));
		
#pragma warning(default : 4244)
		
	}
	
	// XOR the resulting character with the 
	// input character to encrypt/decrypt it.
	
	//The follwing conversion warning not necessary here either.
	//The 'loss of data', so to speak is a side effect and harmless.
#pragma warning(disable : 4244)
	
	cTarget = ( cTarget ^ Crypto );
	
	if (cTarget == NULL){
		cTarget = ( cTarget ^ Crypto );	
	}
#pragma warning( default : 4244 )
	CRYPT_END
#endif
	return;
}


//***********************************************
// ARACrypt::TransformString
//***********************************************
// Encrypt or decrypt a standard string in place.
// The string to transform is passed in as 
// Target.
//***********************************************

void ARACrypt::TransformString(LPCTSTR csKey, CString& csTarget)
{
#ifndef ART_VERSION
	// Reset the shift registers.
	
	SetKey(csKey);
	
	
	// Transform each character in the string.
	// 
	// DEVELOPER'S NOTE
	// ==========================================
	// DO NOT TREAT THE OUTPUT STRING AS A NULL-
	// TERMINATED STRING. 
	// ==========================================
	// The transformation process can create '\0' 
	// characters in the output string. Always 
	// use the length() method to retrieve the full 
	// string when it has been transformed.
	
	// bek NOTE: The above note does not apply to this
	// implementation of Warren Ward's method.
	// ARACrypt knows about NULLs and transforms them
	// into XOR NULLs so the entire result can be
	// treated as a 'normal' NULL terminated string.
	CRYPT_START
	int nLen = csTarget.GetLength();
	
	for (int nPos = 0; nPos < nLen; nPos++)
	{
		//The follwing conversion warning not necessary here either.
		//The 'loss of data', so to speak is a side effect and harmless.
#pragma warning(disable : 4244)
		
		unsigned char cBuff = csTarget.GetAt(nPos);
		TransformChar((unsigned char&) cBuff);
		csTarget.SetAt(nPos, cBuff);
	}
	CRYPT_END
#endif
	return;
}

  /****************************************************************************
  *                                 checksum::add
  * Inputs:
  *        BYTE value:
  * Result: void
  * 
  * Effect: 
  *        Adds the byte to the checksum
****************************************************************************/

void checksum::add(BYTE value)
{
	#ifndef ART_VERSION
	BYTE cipher = (value ^ (r >> 8));
	r = (cipher + r) * c1 + c2;
	sum += cipher;
	#endif
} // checksum::add(BYTE)

  /****************************************************************************
  *                                 checksum::add
  * Inputs:
  *        const CString & s: String to add
  * Result: void
  * 
  * Effect: 
  *        Adds each character of the string to the checksum
****************************************************************************/

void checksum::add(const CString & s)
{
	for(int i = 0; i < s.GetLength(); i++)
		add((BYTE)s.GetAt(i));
} // checksum::add(CString)

CString Recrypt(CString sRes)
{
#ifndef ART_VERSION
	CString sAppName=GetApplicationName();
	sAppName.MakeUpper();
	ARACrypt ac;
	ac.TransformString(sAppName,sRes);
#ifdef _DEBUG
	CString result;
	for(int i=0;i<sRes.GetLength();i++) 
    { // convert md to hex-represented string (hex-letters in upper case!)
		CString sHex;
		sHex.Format("\\x%02x",BYTE(sRes[i]));
		result += sHex;
	}
	TRACE("Crypting result:");
	TRACE(result);
	TRACE("\nOriginal result:");
	TRACE(sRes);
	TRACE("\n");
#endif
#endif
	return sRes;
}


/****************************************************************************
*                                  checksum::add
* Inputs:
*        DWORD d: word to add
* Result: void
* 
* Effect: 
*        Adds the bytes of the DWORD to the checksum
****************************************************************************/

void checksum::add(DWORD value)
{
	union { DWORD value; BYTE bytes[4]; } data;
	data.value = value;
	for(UINT i = 0; i < sizeof(data.bytes); i++)
		add(data.bytes[i]);
} // checksum::add(DWORD)

  /****************************************************************************
  *                                 checksum::add
  * Inputs:
  *        WORD value:
  * Result: void
  * 
  * Effect: 
  *        Adds the bytes of the WORD value to the checksum
****************************************************************************/

void checksum::add(WORD value)
{
	#ifndef ART_VERSION
	union { DWORD value; BYTE bytes[2]; } data;
	data.value = value;
	for(UINT i = 0; i < sizeof(data.bytes); i++)
		add(data.bytes[i]);
	#endif
} // checksum::add(WORD)


  /****************************************************************************
  *                                 checksum::add
  * Inputs:
  *        LPBYTE b: pointer to byte array
  *        UINT length: count
  * Result: void
  * 
  * Effect: 
  *        Adds the bytes to the checksum
****************************************************************************/

void checksum::add(LPBYTE b, UINT length)
{
	for(UINT i = 0; i < length; i++){
		add(b[i]);
	}
} // checksum::add(LPBYTE, UINT)


void GetRegInfoFromText(CString &sText,CString& sLikUser,CString& sLikKey, const char* szSalt)
{
	if(szSalt==0){
		szSalt=EXECRYPTOR_SALT;
	}
	CRYPT_START
	sText+="\n";
	if(sText.Find("WPK")!=-1){
		sLikUser=CDataXMLSaver::GetInstringPart("Registration name","\n",sText);
		sLikUser.TrimLeft(" \n\r><\\/,;:\t");
		sLikUser.TrimRight(" \n\r><\\/,;:\t");
		sLikKey=CDataXMLSaver::GetInstringPart("Registration key","\n",sText);
		if(sLikKey==""){
			sLikKey=CDataXMLSaver::GetInstringPart("Serial number","\n",sText);
		}
		sLikKey.TrimLeft(" \n\r><\\/,;:\t");
		sLikKey.TrimRight(" \n\r><\\/,;:\t");
	}else{
		sLikUser=CDataXMLSaver::GetInstringPart("Your registration name is:","\n",sText);
		if(sLikUser==""){
			sLikUser=CDataXMLSaver::GetInstringPart("Registration name:","\n",sText);
		}
		sLikUser.TrimLeft(" \n\r><\\/,;:\t");
		sLikUser.TrimRight(" \n\r><\\/,;:\t");
		sLikUser.Replace(szSalt,"");
		sLikKey=CDataXMLSaver::GetInstringPart("begin","end",sText);
		if(sLikKey==""){
			sLikKey=CDataXMLSaver::GetInstringPart("Serial number","\n",sText);
		}
		sLikKey.Replace("\n","");
		sLikKey.Replace("\r","");
		sLikKey.Replace("\t","");
		sLikKey.Replace("-","");
		sLikKey.Replace(" ","");
		sLikKey.Replace(">","");
		sLikKey.Replace("<","");
		sLikKey.TrimLeft(" \n\r><\\/,;:\t");
		sLikKey.TrimLeft(" \n\r><\\/,;:\t");
	}
	CRYPT_END
}