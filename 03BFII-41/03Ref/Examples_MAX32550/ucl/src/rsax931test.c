
#include <ucl_testing_config.h>
#ifdef SYSTEM_JIBE
#include <printf_lite.h>
#endif
#ifdef SYSTEM_LIGHTHOUSE
#include <printf_lite.h>
#endif

#include <rsax931test.h>

#include <stdio.h>
#include <string.h>
#include <ucl/ucl_retdefs.h>
#include <ucl/ucl_types.h>
#include <ucl/ucl_config.h>
#include <ucl/ucl_defs.h>
#include <ucl/ucl_sys.h>
#include <ucl/ucl_stack.h>
#include <ucl/ucl_sha1.h>
#include <ucl/ucl_rsa.h>
#include <ucl/ucl_data_conv.h>
#include <ucl/ucl_x931.h>
#include <ucl/ucl_x931_sha1.h>
#include <ucl/ucl_x931_sha256.h>

int test_x931_sha1(u8 *signature, u8 *message, u32 message_length, ucl_rsa_public_key_t *keyPu,ucl_rsa_private_key_t *keyPr)
{
  int i;
  int err;
  int loop;
  u8 hash[UCL_SHA1_HASHSIZE];
  ucl_sha1(hash,message,message_length);
  err=ucl_x931_ssa_sign(signature,hash,UCL_SHA1_HASHSIZE,keyPr);
  if(UCL_OK!=err)
    {
      PRINTF("ERROR during sign\n");
      return(UCL_ERROR);
    }

  err=ucl_x931_ssa_sha1_sign(signature,message,message_length,keyPr);
  if(UCL_OK!=err)
    {
      PRINTF("ERROR during SHA1 sign\n");
      return(UCL_ERROR);
    }

  err=ucl_x931_ssa_verify(signature,hash,UCL_SHA1_HASHSIZE,keyPu);
  if(UCL_OK!=err)
    {
      PRINTF("ERROR during verify %d\n",err);
      return(UCL_ERROR);
    }

  err=ucl_x931_ssa_sha1_verify(signature,message,message_length,keyPu);
  if(UCL_OK!=err)
    {
      PRINTF("ERROR during SHA1 verify %d\n",err);
      return(UCL_ERROR);
    }
  loop=10;
  PRINTF("RSA X9.31 sign %d\n",loop);
  for(i=0;i<loop;i++)
    {
      err=ucl_x931_ssa_sign(signature,hash,UCL_SHA1_HASHSIZE,keyPr);
      if(UCL_OK!=err)
	{
	  PRINTF("ERROR during sign\n");
	  return(UCL_ERROR);
	}

      err=ucl_x931_ssa_sha1_sign(signature,message,message_length,keyPr);
      if(UCL_OK!=err)
	{
	  PRINTF("ERROR during SHA1 sign\n");
	  return(UCL_ERROR);
	}
    }
  loop=10;
  PRINTF("RSA X9.31 verify %d\n",loop);
  for(i=0;i<loop;i++)
    {
      err=ucl_x931_ssa_verify(signature,hash,UCL_SHA1_HASHSIZE,keyPu);
      if(UCL_OK!=err)
	{
	  PRINTF("ERROR during verify %d\n",err);
	  return(UCL_ERROR);
	}
      err=ucl_x931_ssa_sha1_verify(signature,message,message_length,keyPu);
      if(UCL_OK!=err)
	{
	  PRINTF("ERROR during SHA1 verify %d\n",err);
	  return(UCL_ERROR);
	}
    }
  return(UCL_OK);
}

int test_x931_sha1_crt(u8 *signature, u8 *message, u32 message_length, ucl_rsa_public_key_t *keyPu,ucl_rsa_crt_private_key_t *keyPr)
{
  int err;
  u8 hash[UCL_SHA1_HASHSIZE];
  ucl_sha1(hash,message,message_length);
  err=ucl_x931_ssa_crt_sign(signature,hash,UCL_SHA1_HASHSIZE,keyPr);
  if(UCL_OK!=err)
    {
      PRINTF("ERROR during sign\n");
      return(UCL_ERROR);
    }
  err=ucl_x931_ssa_sha1_crt_sign(signature,message,message_length,keyPr);
  if(UCL_OK!=err)
    {
      PRINTF("ERROR during sign\n");
      return(UCL_ERROR);
    }

  err=ucl_x931_ssa_verify(signature,hash,UCL_SHA1_HASHSIZE,keyPu);
  if(UCL_OK!=err)
    {
      PRINTF("ERROR during verify %d\n",err);
      return(UCL_ERROR);
    }

  err=ucl_x931_ssa_sha1_verify(signature,message,message_length,keyPu);
  if(UCL_OK!=err)
    {
      PRINTF("ERROR during SHA1 verify %d\n",err);
      return(UCL_ERROR);
    }
  return(UCL_OK);
}


int test_x931_sha256(u8 *signature, u8 *message, u32 message_length, ucl_rsa_public_key_t *keyPu,ucl_rsa_private_key_t *keyPr)
{
  int err;
  u8 hash[UCL_SHA256_HASHSIZE];
  ucl_sha256(hash,message,message_length);
  err=ucl_x931_ssa_sign(signature,hash,UCL_SHA256_HASHSIZE,keyPr);
  if(UCL_OK!=err)
    {
      PRINTF("ERROR during sign %d\n",err);
      return(UCL_ERROR);
    }

  err=ucl_x931_ssa_sha256_sign(signature,message,message_length,keyPr);
  if(UCL_OK!=err)
    {
      PRINTF("ERROR during SHA256 sign %d\n",err);
      return(UCL_ERROR);
    }

  err=ucl_x931_ssa_verify(signature,hash,UCL_SHA256_HASHSIZE,keyPu);
  if(UCL_OK!=err)
    {
      PRINTF("ERROR during verify %d\n",err);
      return(UCL_ERROR);
    }
  err=ucl_x931_ssa_sha256_verify(signature,message,message_length,keyPu);
  if(UCL_OK!=err)
    {
      PRINTF("ERROR during SHA256 verify %d\n",err);
      return(UCL_ERROR);
    }
  return(UCL_OK);
}

int test_x931_sha256_crt(u8 *signature, u8 *message, u32 message_length, ucl_rsa_public_key_t *keyPu,ucl_rsa_crt_private_key_t *keyPr)
{
  int err;
  u8 hash[UCL_SHA256_HASHSIZE];
  ucl_sha256(hash,message,message_length);

  err=ucl_x931_ssa_crt_sign(signature,hash,UCL_SHA256_HASHSIZE,keyPr);
  if(UCL_OK!=err)
    {
      PRINTF("ERROR during sign\n");
      return(UCL_ERROR);
    }
  err=ucl_x931_ssa_sha256_crt_sign(signature,message,message_length,keyPr);
  if(UCL_OK!=err)
    {
      PRINTF("ERROR during SHA256 sign\n");
      return(UCL_ERROR);
    }

  err=ucl_x931_ssa_verify(signature,hash,UCL_SHA256_HASHSIZE,keyPu);
  if(UCL_OK!=err)
    {
      PRINTF("ERROR during verify %d\n",err);
      return(UCL_ERROR);
    }
  err=ucl_x931_ssa_sha256_verify(signature,message,message_length,keyPu);
  if(UCL_OK!=err)
    {
      PRINTF("ERROR during SHA256 verify %d\n",err);
      return(UCL_ERROR);
    }
  return(UCL_OK);
}

int rsa_kat_x931(void)
{
  /*
ANSI X9.31 and IEEE1363 IFFSA test vector (http://grouper.ieee.org/groups/1363/P1363/testvector.txt)
two prime factors are
p =    D8CD81F0      35EC57EF      E8229551      49D3BFF7      0C53520D
       769D6D76      646C7A79      2E16EBD8      9FE6FC5B      606B56F6
       3EB11317      A8DCCDF2      03650EF2      8D0CB9A6      D2B2619C
       52480F51
q =    CC109249      5D867E64      065DEE3E      7955F2EB      C7D47A2D
       7C995338      8F97DDDC      3E1CA19C      35CA659E      DC3D6C08
       F64068EA      FEDBD911      27F9CB7E      DC174871      1B624E30
       B857CAAD

private exponent is
d =    1CCDA20B      CFFB8D51      7EE96668      66621B11      822C7950
       D55F4BB5      BEE37989      A7D17312      E326718B      E0D79546
       EAAE87A5      6623B919      B1715FFB      D7F16028      FC400774
       1961C88C      5D7B4DAA      AC8D36A9      8C9EFBB2      6C8A4A0E
       6BC15B35      8E528A1A      C9D0F042      BEB93BCA      16B541B3
       3F80C933      A3B76928      5C462ED5      677BFE89      DF07BED5
       C127FD13      241D3C4B

modulus n = pq is
n =    ACD1CC46      DFE54FE8      F9786672      664CA269      0D0AD7E5
       003BC642      7954D939      EEE8B271      52E6A947      450D7FA9
       80172DE0      64D6569A      28A83FE7      0FA840F5      E9802CB8
       984AB34B      D5C1E639      9EC21E4D      3A3A69BE      4E676F39
       5AAFEF7C      4925FD4F      AEE9F9E5      E48AF431      5DF0EC2D
       B9AD7A35      0B3DF2F4      D15DC003      9846D1AC      A3527B1A
       75049E3F      E34F43BD
  */
  u8 n[]={0xAC,0xD1,0xCC,0x46,0xDF,0xE5,0x4F,0xE8,0xF9,0x78,0x66,0x72,0x66,0x4C,0xA2,0x69,0x0D,0x0A,0xD7,0xE5,0x00,0x3B,0xC6,0x42,0x79,0x54,0xD9,0x39,0xEE,0xE8,0xB2,0x71,0x52,0xE6,0xA9,0x47,0x45,0x0D,0x7F,0xA9,0x80,0x17,0x2D,0xE0,0x64,0xD6,0x56,0x9A,0x28,0xA8,0x3F,0xE7,0x0F,0xA8,0x40,0xF5,0xE9,0x80,0x2C,0xB8,0x98,0x4A,0xB3,0x4B,0xD5,0xC1,0xE6,0x39,0x9E,0xC2,0x1E,0x4D,0x3A,0x3A,0x69,0xBE,0x4E,0x67,0x6F,0x39,0x5A,0xAF,0xEF,0x7C,0x49,0x25,0xFD,0x4F,0xAE,0xE9,0xF9,0xE5,0xE4,0x8A,0xF4,0x31,0x5D,0xF0,0xEC,0x2D,0xB9,0xAD,0x7A,0x35,0x0B,0x3D,0xF2,0xF4,0xD1,0x5D,0xC0,0x03,0x98,0x46,0xD1,0xAC,0xA3,0x52,0x7B,0x1A,0x75,0x04,0x9E,0x3F,0xE3,0x4F,0x43,0xBD};
  u8 d[]={0x1C,0xCD,0xA2,0x0B,0xCF,0xFB,0x8D,0x51,0x7E,0xE9,0x66,0x68,0x66,0x62,0x1B,0x11,0x82,0x2C,0x79,0x50,0xD5,0x5F,0x4B,0xB5,0xBE,0xE3,0x79,0x89,0xA7,0xD1,0x73,0x12,0xE3,0x26,0x71,0x8B,0xE0,0xD7,0x95,0x46,0xEA,0xAE,0x87,0xA5,0x66,0x23,0xB9,0x19,0xB1,0x71,0x5F,0xFB,0xD7,0xF1,0x60,0x28,0xFC,0x40,0x07,0x74,0x19,0x61,0xC8,0x8C,0x5D,0x7B,0x4D,0xAA,0xAC,0x8D,0x36,0xA9,0x8C,0x9E,0xFB,0xB2,0x6C,0x8A,0x4A,0x0E,0x6B,0xC1,0x5B,0x35,0x8E,0x52,0x8A,0x1A,0xC9,0xD0,0xF0,0x42,0xBE,0xB9,0x3B,0xCA,0x16,0xB5,0x41,0xB3,0x3F,0x80,0xC9,0x33,0xA3,0xB7,0x69,0x28,0x5C,0x46,0x2E,0xD5,0x67,0x7B,0xFE,0x89,0xDF,0x07,0xBE,0xD5,0xC1,0x27,0xFD,0x13,0x24,0x1D,0x3C,0x4B};
  u8 signature_abc[]={0x06,0x1D,0x35,0x52,0x37,0xE2,0xA0,0x58,0x66,0x86,0x71,0x10,0xD3,0x2F,0x1D,0x8D,0x3C,0x51,0x93,0xF5,0xCB,0x7A,0xC3,0x89,0x2B,0x7F,0xBE,0x89,0xD0,0xD8,0x5D,0xB5,0x44,0xE1,0x36,0xA5,0x49,0xDF,0xCF,0x75,0x29,0x7E,0xA9,0xEC,0xE2,0x1F,0x08,0x55,0x89,0x3B,0xBF,0x23,0x09,0x98,0x84,0xE5,0xED,0xAC,0x82,0xED,0xFA,0xE4,0x4A,0xF0,0x45,0x3A,0xB6,0x31,0xCC,0xBA,0x5C,0x76,0xED,0xD1,0x3C,0xBD,0x3D,0x51,0xF3,0x7F,0xE4,0x0B,0x9A,0x5D,0xD6,0x48,0x35,0x13,0x38,0x6F,0x5C,0x70,0x40,0x16,0x87,0xDF,0xC2,0x7D,0x1D,0xDA,0xF6,0xED,0xBD,0x18,0xCE,0xFA,0xD5,0xCF,0x81,0x75,0x04,0xC0,0x8F,0x48,0x2D,0x26,0x2A,0xD3,0x57,0x7A,0xA2,0x70,0x5A,0xAF,0x09,0x05,0x65,0x78};

  u8 public_exponent[1]={0x03};
  u8 message[]={'a','b','c'};
  u8 signature[128];
  ucl_rsa_public_key_t keyPu;
  ucl_rsa_private_key_t keyPr;
  int i;
  int err;
  u8 privexp[512];
  u8 modul[512];
  u8 hash[UCL_SHA1_HASHSIZE];

  keyPu.public_exponent_length=1;
  for(i=0;i<(int)keyPu.public_exponent_length;i++)
    keyPu.public_exponent[i]=public_exponent[i];
  keyPu.modulus_length=128;
  for(i=0;i<(int)keyPu.modulus_length;i++)
    keyPu.modulus[i]=n[i];
  keyPr.modulus_length=128;
  keyPr.modulus=modul;
  for(i=0;i<(int)keyPu.modulus_length;i++)
    keyPr.modulus[i]=n[i];
  keyPr.private_exponent=privexp;
  for(i=0;i<(int)keyPr.modulus_length;i++)
    keyPr.private_exponent[i]=d[i];

  ucl_sha1(hash,message,3);
  err=ucl_x931_ssa_sign(signature,hash,UCL_SHA1_HASHSIZE,&keyPr);
  if(UCL_OK!=err)
    {
      PRINTF("ERROR during sign\n");
      return(UCL_ERROR);
    }
  if(memcmp(signature,signature_abc,128)!=0)
    {
      PRINTF("ERROR computed signature (h) different\n");
      return(UCL_ERROR);
    }
  err=ucl_x931_ssa_sha1_sign(signature,message,3,&keyPr);
  if(UCL_OK!=err)
    {
      PRINTF("ERROR during SHA1 sign\n");
      return(UCL_ERROR);
    }
  if(memcmp(signature,signature_abc,128)!=0)
    {
      PRINTF("ERROR computed signature different\n");
      return(UCL_ERROR);
    }
  err=ucl_x931_ssa_verify(signature,hash,UCL_SHA1_HASHSIZE,&keyPu);
  if(UCL_OK!=err)
    {
      PRINTF("ERROR during verify\n");
      return(UCL_ERROR);
    }
  err=ucl_x931_ssa_sha1_verify(signature,message,sizeof(message),&keyPu);
  if(UCL_OK!=err)
    {
      PRINTF("ERROR during SHA1 verify\n");
      return(UCL_ERROR);
    }
  return(UCL_OK);
}

static const  u8 n_pkcs1[] = {0xd4,0x94,0xa3,0xcd,0x3f,0x7b,0x36,0x43,0x80,0xf1,0x3e,0xe8,0x47,0x56,0xe1,0x76,0x4d,0xa3,0x8e,0x24,0x27,0xfc,0x62,0x90,0xe3,0x3a,0x94,0xf5,0xb1,0xe3,0x4a,0x25,0xd8,0xb5,0x9a,0xd5,0x0c,0x96,0x03,0xd7,0x29,0xca,0x16,0x99,0x38,0x49,0x5e,0x35,0x03,0x59,0x11,0xa0,0x95,0xcf,0x7c,0xe2,0x08,0xed,0xc0,0x51,0x0e,0x66,0x91,0x10,0x20,0x75,0x76,0xce,0xe2,0x82,0x90,0x43,0xbc,0xbb,0xcb,0x27,0x0e,0xb4,0x56,0x34,0x23,0xae,0x6d,0x63,0xf0,0x13,0x5d,0xc5,0xe3,0xbd,0xaa,0xdb,0x9f,0x57,0x42,0xf9,0x40,0x45,0xa0,0x43,0x58,0x96,0x7b,0xb4,0x31,0xa2,0xb2,0x0e,0xf0,0xee,0x36,0xd5,0x82,0xef,0x49,0x4b,0xac,0x8f,0xc7,0x60,0x02,0x36,0x13,0xa0,0xff,0xf1,0x8c,0xe7};
  //SHAAlg = SHA1

static const  u8 d_pkcs1[] = {0x06,0x40,0x9b,0x67,0xeb,0x47,0x63,0x7a,0x74,0xbb,0xcd,0x24,0xf3,0x0a,0x15,0xb0,0xa7,0xee,0x38,0xe2,0xf2,0x1e,0x02,0xe6,0x24,0xcd,0x04,0x61,0x94,0x4a,0x73,0x1f,0x3b,0x14,0x66,0x6f,0xad,0x8b,0xf1,0x0d,0xdb,0x95,0x00,0xaa,0x27,0x4d,0x73,0xb6,0x43,0xdc,0xf8,0xfd,0x31,0x95,0x29,0x51,0xf1,0x34,0x2b,0x4d,0xad,0x99,0x9a,0xda,0xc6,0x0d,0xc1,0x6b,0xb2,0x11,0x7a,0xe2,0x89,0xbf,0x07,0x0c,0x5e,0xd2,0xaf,0xdb,0xea,0xd0,0xee,0x40,0xeb,0xc0,0x8b,0x53,0xc9,0x85,0x01,0xe3,0x53,0xdb,0x5d,0x15,0x89,0x84,0xa9,0x27,0x5f,0xe4,0xeb,0x4b,0x2f,0x6a,0x55,0x0b,0x7c,0x45,0x0a,0xdb,0x6a,0xb0,0x64,0x46,0x4c,0xa1,0x9e,0x42,0x28,0x29,0x08,0x83,0x53,0x01,0xe3,0xf1};

static const  u8 e_pkcs1[] = {0x11};


int test_x931(int maxloop)
{

#define IDF_SHA1 0
#define IDF_SHA256 1
#define IDF_RIPEMD160 2
#define UCL_RSA_KEY_MAXPRE (UCL_RSA_KEY_MAXSIZE/4)

  u8 message[]={'a','b','c'};
  u8 resu[UCL_RSA_KEY_MAXPRE*4];
  u32 *n,*p,*q,*d;
  u32 *dp,*dq,*qinv;
  u32 _e2048_[1] = { 0x10001 };
  u8 e2048[]={0x01, 0x00,0x01 };
  int maxsizekey,s;
  ucl_rsa_public_key_t keyPu;
  ucl_rsa_private_key_t keyPr;
  ucl_rsa_crt_private_key_t keyPr_crt;
  u8 privexp[512];
  u8 modul[512];

  int i,j;
  int err,iloop;
  int thesize;
  PRINTF("RSA X931 testing----------\n");
  //RSA PSS verification
  PRINTF("RSA X9.31 KAT ");
  if(UCL_OK!=rsa_kat_x931())
    PRINTF("NOK\n");
  else
    PRINTF("OK\n");
  maxsizekey=64;
  s=maxsizekey;
  if (ucl_stack_alloc(&n, s) != UCL_OK)
    return UCL_STACK_ERROR;
  if (ucl_stack_alloc(&p, s/2) != UCL_OK)
    return UCL_STACK_ERROR;
  if (ucl_stack_alloc(&q, s/2) != UCL_OK)
    return UCL_STACK_ERROR;
  if (ucl_stack_alloc(&d, s) != UCL_OK)
    return UCL_STACK_ERROR;
  if (ucl_stack_alloc(&dp, s/2) != UCL_OK)
    return UCL_STACK_ERROR;
  if (ucl_stack_alloc(&dq, s/2) != UCL_OK)
    return UCL_STACK_ERROR;
  if (ucl_stack_alloc(&qinv, s/2) != UCL_OK)
    return UCL_STACK_ERROR;

  PRINTF("RSA X9.31 test(%d) ",maxloop);
  for(iloop=0;iloop<maxloop;iloop++)
    {
      PRINTF("TEST1 ");
      keyPu.public_exponent_length=1;
      for(i=0;i<(int)keyPu.public_exponent_length;i++)
	keyPu.public_exponent[i]=e_pkcs1[i];
      keyPu.modulus_length=128;
      for(i=0;i<(int)keyPu.modulus_length;i++)
	keyPu.modulus[i]=n_pkcs1[i];
      keyPr.modulus_length=128;
      keyPr.modulus=modul;
      keyPr.private_exponent=privexp;
      for(i=0;i<(int)keyPu.modulus_length;i++)
	keyPr.modulus[i]=n_pkcs1[i];
      for(i=0;i<(int)keyPr.modulus_length;i++)
	keyPr.private_exponent[i]=d_pkcs1[i];
      err=test_x931_sha1(resu,message,sizeof(message), &keyPu,&keyPr);
      if(err!=UCL_OK)
	{
	  PRINTF("ERROR on n_pkcs1\n");
	}
      else
	PRINTF("OK\n");
      err=test_x931_sha256(resu,message,sizeof(message), &keyPu,&keyPr);
      if(err!=UCL_OK)
	{
	  PRINTF("ERROR on n_pkcs1 SHA256\n");
	}
      else
	PRINTF("OK\n");
      PRINTF("TEST2 (%dbits-%dbits)\n",32*32,maxsizekey*32);
      for(thesize=32;thesize<=maxsizekey;thesize+=32)
	{
	  PRINTF("%d bits ",thesize*32);
	  for(j=0;j<3;j++)
	    {
	      PRINTF(".%d ",j);
	      err=ucl_rsa_param_gen(n,p,q,d,(u32 *)_e2048_,1,thesize);
	      PRINTF(" generated\n");
	      keyPr.modulus_length=thesize*4;
	      ucl_data_int2os(keyPr.modulus, keyPr.modulus_length,n,thesize);
	      ucl_data_int2os(keyPr.private_exponent, keyPr.modulus_length,d,thesize);
	      keyPu.public_exponent_length=3;
	      for(i=0;i<(int)keyPu.public_exponent_length;i++)
		keyPu.public_exponent[i]=e2048[i];
	      keyPu.modulus_length=thesize*4;
	      for(i=0;i<(int)keyPu.modulus_length;i++)
		keyPu.modulus[i]=keyPr.modulus[i];
	      err=test_x931_sha1(resu,message,sizeof(message), &keyPu,&keyPr);
	      if(err!=UCL_OK)
		{
		  PRINTF("ERROR on x9.31\n");
		  return(UCL_ERROR);
		}
	      err=test_x931_sha256(resu,message,sizeof(message), &keyPu,&keyPr);
	      if(err!=UCL_OK)
		{
		  PRINTF("ERROR on x9.31 SHA256\n");
		  return(UCL_ERROR);
		}
	    }
	  PRINTF(" OK\n");
	}
      PRINTF("TEST3 CRT (%dbits-%dbits)\n",32*32,maxsizekey*32);
      for(thesize=32;thesize<=maxsizekey;thesize+=32)
	{
	  PRINTF("%d bits ",thesize*32);
	  for(j=0;j<3;j++)
	    {
	      PRINTF(".%d ",j);
	      err=ucl_rsa_crt_param_gen_legacy(n,p,q,dp,dq,qinv,(u32 *)_e2048_,1,thesize);
	      PRINTF(" generated\n");
	      keyPr_crt.modulus_length=thesize*4;
	      ucl_data_int2os(keyPr_crt.exponent1, keyPr_crt.modulus_length/2,dp,thesize/2);
	      ucl_data_int2os(keyPr_crt.exponent2, keyPr_crt.modulus_length/2,dq,thesize/2);
	      ucl_data_int2os(keyPr_crt.coefficient, keyPr_crt.modulus_length/2,qinv,thesize/2);
	      ucl_data_int2os(keyPr_crt.prime1, keyPr_crt.modulus_length/2,p,thesize/2);
	      ucl_data_int2os(keyPr_crt.prime2, keyPr_crt.modulus_length/2,q,thesize/2);
	      keyPr_crt.public_exponent_length=3;
	      for(i=0;i<(int)keyPr_crt.public_exponent_length;i++)
		keyPr_crt.public_exponent[i]=e2048[i];

	      keyPu.modulus_length=thesize*4;
	      ucl_data_int2os(keyPu.modulus, keyPu.modulus_length,n,thesize);
	      keyPu.public_exponent_length=3;
	      for(i=0;i<(int)keyPu.public_exponent_length;i++)
		keyPu.public_exponent[i]=e2048[i];
	      err=test_x931_sha1_crt(resu,message,sizeof(message), &keyPu,&keyPr_crt);
	      if(err!=UCL_OK)
		{
		  PRINTF("ERROR on x9.31\n");
		  return(UCL_ERROR);
		}
	      err=test_x931_sha256_crt(resu,message,sizeof(message), &keyPu,&keyPr_crt);
	      if(err!=UCL_OK)
		{
		  PRINTF("ERROR on x9.31 CRT SHA256\n");
		  return(UCL_ERROR);
		}
	    }
	  PRINTF(" OK\n");
	}
      PRINTF("OK\n");
    }
  if (ucl_stack_free(&qinv) != UCL_OK)
    return UCL_STACK_ERROR;
  if (ucl_stack_free(&dq) != UCL_OK)
    return UCL_STACK_ERROR;
  if (ucl_stack_free(&dp) != UCL_OK)
    return UCL_STACK_ERROR;
  if (ucl_stack_free(&d) != UCL_OK)
    return UCL_STACK_ERROR;
  if (ucl_stack_free(&q) != UCL_OK)
    return UCL_STACK_ERROR;
  if (ucl_stack_free(&p) != UCL_OK)
    return UCL_STACK_ERROR;
  if (ucl_stack_free(&n) != UCL_OK)
    return UCL_STACK_ERROR;
PRINTF("OK\n");
  return(UCL_OK);
}
