/* -------------------------------------------------------------------------
//	File Name	:	MD5.h
//	Author		:	Li Jiahan
//	Create Time	:	2012-3-15 14:32:15
//	Description	:
//
// -----------------------------------------------------------------------*/

#ifndef __MD5_H__
#define __MD5_H__

#include "../CrossPlatformConfig.h"

// -------------------------------------------------------------------------

class MD5
{
public:
	MD5();
	~MD5();
	
	void update ( Byte *input, UInt32 inputLen);
	void final (Byte digest[16]);
	void reset ();
	
	//////////////////////////////////////////////////////////////////////////
	// Helper
private:
	void _Transform (UInt32 state[4], Byte block[64]);
	void _Memcpy (Byte* output, Byte* input,UInt32 len);
	void _Encode (Byte *output, UInt32 *input,UInt32 len);
	void _Decode (UInt32 *output, Byte *input, UInt32 len);
	void _Memset (Byte* output,Int32 value,UInt32 len);
	
	//////////////////////////////////////////////////////////////////////////
	//
	private:
	UInt32	state[4];					// state (ABCD)
	UInt32	count[2];					// number of bits, modulo 2^64 (lsb first) 
	Byte	buffer[64];					// input buffer
	Byte	PADDING[64];				// What? 
};

//--------------------------------------------------------------------------
#endif /* __MD5_H__ */