
#include <ucl_testing_config.h>
#include <ucl/ucl_sys.h>
#ifdef SYSTEM_JIBE
#include <printf_lite.h>
#endif
#ifdef __lighthouse
#include <printf_lite.h>
#endif
#include <ucl/ucl_config.h>
#include <ucl/ucl_types.h>
#include <ucl/ucl_defs.h>
#include <ucl/ucl_retdefs.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <ucl/ucl_des_ecb.h>
#include <ucl/ucl_3des_ecb.h>
#include <ucl/ucl_tdes_cbc_mac.h>
#include <ucl/ucl_3des_cmac.h>
#include <ucl/ucl_3des_cbc.h>
#include <ucl/ucl_3des_ofb.h>
#include <ucl/ucl_3des_cfb.h>
#include <ucl/ucl_rng.h>
#include <ucl/ucl_sha256.h>

int sample_3descmac(void)
{
  //SP800-38D
  //D.4
  u8 keyd4[]={0x8a,0xa8,0x3b,0xf8,0xcb,0xda,0x10,0x62,0x0b,0xc1,0xbf,0x19,0xfb,0xb6,0xcd,0x58,0xbc,0x31,0x3d,0x4a,0x37,0x1c,0xa8,0xb5};
  u8 k1[]={0x91,0x98,0xe9,0xd3,0x14,0xe6,0x53,0x5f};
  u8 k2[]={0x23,0x31,0xd3,0xa6,0x29,0xcc,0xa6,0xa5};

  u8 tagd4_m0[]={0xb7,0xa6,0x88,0xe1,0x22,0xff,0xaf,0x95};

  u8 m64[]={0x6b,0xc1,0xbe,0xe2,0x2e,0x40,0x9f,0x96};
  u8 tagd4_m64[]={0x8e,0x8f,0x29,0x31,0x36,0x28,0x37,0x97};//update_cmac_examples nist document

  u8 m160[]={0x6b,0xc1,0xbe,0xe2,0x2e,0x40,0x9f,0x96,0xe9,0x3d,0x7e,0x11,0x73,0x93,0x17,0x2a,0xae,0x2d,0x8a,0x57};
  u8 tagd4_m160[]={0x74,0x3d,0xdb,0xe0,0xce,0x2d,0xc2,0xed};//update_cmac_examples nist document

  u8 m256[]={0x6b,0xc1,0xbe,0xe2,0x2e,0x40,0x9f,0x96,0xe9,0x3d,0x7e,0x11,0x73,0x93,0x17,0x2a,0xae,0x2d,0x8a,0x57,0x1e,0x03,0xac,0x9c,0x9e,0xb7,0x6f,0xac,0x45,0xaf,0x8e,0x51};
  u8 tagd4_m256[]={0x33,0xe6,0xb1,0x09,0x24,0x00,0xea,0xe5};


  u8 k1tmp[UCL_3DES_BLOCKSIZE];
  u8 k2tmp[UCL_3DES_BLOCKSIZE];
  u8 t[UCL_3DES_BLOCKSIZE];
  int ret;

  PRINTF("D4\n");
  ucl_3des_subkey_generation(k1tmp,k2tmp,keyd4,UCL_3DES_KEYSIZE);
  if(memcmp(k1,k1tmp,UCL_3DES_BLOCKSIZE)!=0)
    {
      PRINTF("ERROR wrong K1\n");
    }
  else
    PRINTF("K1 OK\n");
  if(memcmp(k2,k2tmp,UCL_3DES_BLOCKSIZE)!=0)
    {
      PRINTF("ERROR wrong K2\n");
    }
  else
    PRINTF("K2 OK\n");
  ret=ucl_3des_cmac_compute(t,UCL_3DES_BLOCKSIZE,m64,0,keyd4,UCL_3DES_KEYSIZE);
  if(UCL_OK!=ret)
    {
  PRINTF("ERROR ret=%d\n",ret);
  return(UCL_ERROR);
}
  ret=ucl_3des_cmac_verify(tagd4_m0,UCL_3DES_BLOCKSIZE,m64,0,keyd4,UCL_3DES_KEYSIZE);
  if(UCL_OK!=ret)
    {
  PRINTF("ERROR for verifying t m0\n");
  return(UCL_ERROR);
}
 else
    PRINTF("tag m0 OK\n");
  ret=ucl_3des_cmac_compute(t,UCL_3DES_BLOCKSIZE,m64,UCL_3DES_BLOCKSIZE,keyd4,UCL_3DES_KEYSIZE);
  if(UCL_OK!=ret)
    {
    PRINTF("ERROR ret=%d\n",ret);
    return(UCL_ERROR);
    }
  ret=ucl_3des_cmac_verify(tagd4_m64,UCL_3DES_BLOCKSIZE,m64,UCL_3DES_BLOCKSIZE,keyd4,UCL_3DES_KEYSIZE);
  if(UCL_OK!=ret)
    {
    PRINTF("ERROR for verifying t m64\n");
    return(UCL_ERROR);
    }
  else
    PRINTF("tag m64 OK\n");
  ret=ucl_3des_cmac_compute(t,UCL_3DES_BLOCKSIZE,m160,20,keyd4,UCL_3DES_KEYSIZE);
  if(UCL_OK!=ret)
    {
    PRINTF("ERROR ret=%d\n",ret);
    return(UCL_ERROR);
    }
  ret=ucl_3des_cmac_verify(tagd4_m160,UCL_3DES_BLOCKSIZE,m160,20,keyd4,UCL_3DES_KEYSIZE);
  if(UCL_OK!=ret)
    {
    PRINTF("ERROR for verifying t m160\n");
    return(UCL_ERROR);
    }
  else
    PRINTF("tag m160 OK\n");
  ret=ucl_3des_cmac_compute(t,UCL_3DES_BLOCKSIZE,m256,32,keyd4,UCL_3DES_KEYSIZE);
  if(UCL_OK!=ret)
    {
    PRINTF("ERROR ret=%d\n",ret);
    return(UCL_ERROR);
    }
  ret=ucl_3des_cmac_verify(tagd4_m256,UCL_3DES_BLOCKSIZE,m256,32,keyd4,UCL_3DES_KEYSIZE);
  if(UCL_OK!=ret)
    {
    PRINTF("ERROR for verifying t m256\n");
    return(UCL_ERROR);
    }
  else
    PRINTF("tag m256 OK\n");
  return(UCL_OK);
}


int test_3des(void);

int sample_3des(int maxloop)
{

  u8 tdesKey[3*8];
  u8 plainText[32], cipherText[32], decryptedText[32];
  int extraloop;
  PRINTF("3DES testing----------\n");
  if(sample_3descmac()==UCL_OK)
    PRINTF("3DES CMAC OK\n");
  else
    PRINTF("3DES CMAC NOK\n");
  PRINTF("TEST 3DES ECB ");
  for(extraloop=0;extraloop<maxloop;extraloop++)
    {
      ucl_rng_read(plainText, 32, UCL_RAND_DEFAULT);
      ucl_rng_read(tdesKey, 3*8, UCL_RAND_DEFAULT);

      /* ENCRYPTION */
      ucl_3des_ecb(cipherText, plainText, tdesKey, 32, UCL_CIPHER_ENCRYPT);

      /* DECRYPTION */
      ucl_3des_ecb(decryptedText, cipherText, tdesKey, 32, UCL_CIPHER_DECRYPT);

      if( memcmp(decryptedText, plainText, 32) != 0 )
	return(UCL_ERROR);
    }
  if( test_3des() != UCL_OK )
    return(UCL_ERROR);
  return(UCL_OK);
}

int test3desecb()
{
  u8 keyecb2[]={0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,0xFE,0xDC,0xBA,0x98,0x76,0x54,0x32,0x10,0x89,0xAB,0xCD,0xEF,0x01,0x23,0x45,0x67};
  u8 dataecb2[]={0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xE7};
  u8 resecb2[]={0xDE,0x0B,0x7C,0x06,0xAE,0x5E,0x0E,0xD5};
  u8 keyecbrome[]={0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,0x01,0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF};
  u8 dataecbrome[]={0x54,0x68,0x65,0x20,0x71,0x75,0x66,0x63};
  u8 dataecb1[]={0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
  u8 dataecb11[]={0x08,0x07,0x06,0x05,0x04,0x03,0x02,0x01};
  u8 dataecb12[]={0x04,0x03,0x02,0x01,0x08,0x07,0x06,0x05};
  u8 resecb1[]={0x77,0xA7,0xD6,0xBC,0xF5,0x79,0x62,0xB9};
  u8 keyecb1[]={0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
  u8 keyecb11[]={0x05,0x06,0x07,0x08,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x01,0x02,0x03,0x04};
  u8 keyecb12[]={0x04,0x03,0x02,0x01,0x08,0x07,0x06,0x05,0x04,0x03,0x02,0x01,0x08,0x07,0x06,0x05,0x04,0x03,0x02,0x01,0x08,0x07,0x06,0x05};
  u8 resu[128];
  int i;
  int err;
  PRINTF( "3DES ECB TEST START -----\n");
  PRINTF( "TEST 1\n");
  err=ucl_3des_ecb(resu,dataecbrome,keyecbrome,sizeof(dataecbrome),UCL_CIPHER_ENCRYPT);
  if(UCL_OK!=err)
    {
      PRINTF("ERROR 3des ecb rome\n");
      return(UCL_ERROR);
    }
  err=ucl_3des_ecb(resu,dataecb1,keyecb1,sizeof(dataecb1),UCL_CIPHER_ENCRYPT);
  if(UCL_OK!=err)
    {
      PRINTF("ERROR 3des ecb\n");
      return(UCL_ERROR);
    }
  err=ucl_3des_ecb(resu,dataecb1,keyecb11,sizeof(dataecb1),UCL_CIPHER_ENCRYPT);
  if(UCL_OK!=err)
    {
      PRINTF("ERROR 3des ecb\n");
      return(UCL_ERROR);
    }
  err=ucl_3des_ecb(resu,dataecb1,keyecb12,sizeof(dataecb1),UCL_CIPHER_ENCRYPT);
  if(UCL_OK!=err)
    {
      PRINTF("ERROR 3des ecb\n");
      return(UCL_ERROR);
    }
  err=ucl_3des_ecb(resu,dataecb11,keyecb1,sizeof(dataecb1),UCL_CIPHER_ENCRYPT);
  if(UCL_OK!=err)
    {
      PRINTF("ERROR 3des ecb\n");
      return(UCL_ERROR);
    }
  err=ucl_3des_ecb(resu,dataecb11,keyecb11,sizeof(dataecb1),UCL_CIPHER_ENCRYPT);
  if(UCL_OK!=err)
    {
      PRINTF("ERROR 3des ecb\n");
      return(UCL_ERROR);
    }
  err=ucl_3des_ecb(resu,dataecb11,keyecb12,sizeof(dataecb1),UCL_CIPHER_ENCRYPT);
  if(UCL_OK!=err)
    {
      PRINTF("ERROR 3des ecb\n");
      return(UCL_ERROR);
    }

  err=ucl_3des_ecb(resu,dataecb12,keyecb1,sizeof(dataecb1),UCL_CIPHER_ENCRYPT);
  if(UCL_OK!=err)
    {
      PRINTF("ERROR 3des ecb\n");
      return(UCL_ERROR);
    }
  err=ucl_3des_ecb(resu,dataecb12,keyecb11,sizeof(dataecb1),UCL_CIPHER_ENCRYPT);
  if(UCL_OK!=err)
    {
      PRINTF("ERROR 3des ecb\n");
      return(UCL_ERROR);
    }
  err=ucl_3des_ecb(resu,dataecb12,keyecb12,sizeof(dataecb1),UCL_CIPHER_ENCRYPT);
  if(UCL_OK!=err)
    {
      PRINTF("ERROR 3des ecb\n");
      return(UCL_ERROR);
    }

  err=ucl_3des_ecb(resu,dataecb1,keyecb1,sizeof(dataecb1),UCL_CIPHER_ENCRYPT);
  if(UCL_OK!=err)
    {
      PRINTF("ERROR 3des ecb\n");
      return(UCL_ERROR);
    }
  for(i=0;i<(int)sizeof(resecb1);i++)
    if(resu[i]!=resecb1[i])
      {
	PRINTF("ERROR on 3DES-ECB 1 ENC\n");
	return(UCL_ERROR);
      }
  ucl_3des_ecb(resu,resecb1,keyecb1,sizeof(resecb1),UCL_CIPHER_DECRYPT);
  for(i=0;i<(int)sizeof(dataecb1);i++)
    if(resu[i]!=dataecb1[i])
      {
	PRINTF("ERROR on 3DES-ECB 1 DEC\n");
	return(UCL_ERROR);
      }

  PRINTF( "TEST 2\n");
  err=ucl_3des_ecb(resu,dataecb2,keyecb2,sizeof(dataecb2),UCL_CIPHER_ENCRYPT);
  if(UCL_OK!=err)
    {
      PRINTF("ERROR 3des ecb\n");
      return(UCL_ERROR);
    }
  for(i=0;i<(int)sizeof(resecb2);i++)
    if(resu[i]!=resecb2[i])
      {
	PRINTF("ERROR on 3DES-ECB 2 ENC\n");
	return(UCL_ERROR);
      }
  err=ucl_3des_ecb(resu,resecb2,keyecb2,sizeof(resecb2),UCL_CIPHER_DECRYPT);
  if(UCL_OK!=err)
    {
      PRINTF("ERROR 3des ecb\n");
      return(UCL_ERROR);
    }
  for(i=0;i<(int)sizeof(dataecb2);i++)
    if(resu[i]!=dataecb2[i])
      {
	PRINTF("ERROR on 3DES-ECB 2 DEC\n");
	return(UCL_ERROR);
      }
  return(UCL_OK);
}

int test_montecarlo_cbc(u8 *data,u8 *resu,int datalen,u8 *key,u8 *iv)
{
  int i,j;
  u8 input[32];
  u8 output[32];
  u8 previous_output[32];
  u8 vib[32];
  PRINTF("3DES MONTECARLO\n");
  if(datalen>32)
    return(UCL_INVALID_INPUT);
  for(i=0;i<datalen;i++)
    {
      input[i]=data[i];
      vib[i]=iv[i];
    }
  for(i=0;i<10000;i++)
    {
      for(j=0;j<datalen;j++)
	{
	  previous_output[j]=output[j];
	}
      ucl_3des_cbc(output,input,key,vib,datalen,UCL_CIPHER_ENCRYPT);
      if(i==0)
	for(j=0;j<datalen;j++)
	  input[j]=vib[j];
      else
	for(j=0;j<datalen;j++)
	  input[j]=previous_output[j];
      for(j=0;j<datalen;j++)
	vib[j]=output[j];
    }
  for(i=0;i<datalen;i++)
    if(output[i]!=resu[i])
      {
	PRINTF("ERROR montecarlo 3DES cbc\n");
	return(UCL_ERROR);
      }
  return(UCL_OK);
}

int test3descbc()
{
  u8 key1[]={0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,0x01,0x45,0x67,0x89,0xAB,0xCD,0xEF,0x01,0x23};
  u8 key2[]={0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,0x01,0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF};
  u8 key3[]={0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF};
  u8 data[]={0x4E,0x6F,0x77,0x20,0x69,0x73,0x20,0x74,0x68,0x65,0x20,0x74,0x69,0x6D,0x65,0x20};
  u8 res0[]={0xE5,0xC7,0xCD,0xDE,0x87,0x2B,0xF2,0x7C,0x43,0xE9,0x34,0x00,0x8C,0x38,0x9C,0x0F};
  u8 res1[]={0xcb,0x19,0x1f,0x85,0xd1,0xed,0x84,0x39};
  u8 res2[]={0x35,0x76,0x11,0x56,0x5f,0xa1,0x8e,0x4d};
  u8 res3[]={0x54,0xf1,0x5a,0xf6,0xeb,0xe3,0xa4,0xb4};
  u8 VI[]={0x12,0x34,0x56,0x78,0x90,0xab,0xcd,0xef};
  u8 resu[16];
  int i;
#ifdef VERBOSE
  PRINTF( "3DES_CBC TEST START -----\n");
  PRINTF( "TEST 1\n");
#endif
  ucl_3des_cbc(resu,data,key3,VI,sizeof(data),UCL_CIPHER_ENCRYPT);
  for(i=0;i<(int)sizeof(res0);i++)
    if(resu[i]!=res0[i])
      {
	PRINTF("ERROR on 3DES-CBC 1 ENC\n");
	return(-1);
      }
  if(test_montecarlo_cbc(data,res1,8,key1,VI)!=0)
    return(UCL_ERROR);
  if(test_montecarlo_cbc(data,res2,8,key2,VI)!=0)
    return(UCL_ERROR);
  if(test_montecarlo_cbc(data,res3,8,key3,VI)!=0)
    return(UCL_ERROR);

  return(UCL_OK);
}

int test3descfb()
{
  u8 data[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  u8 res[]={0xf9,0xdd,0x49,0x8a,0xf8,0x84,0xa9,0x7f,0x5a,0x55,0xd6,0xa9,0x2e,0x6f,0x03,0x3a,0x76,0xf8,0x72,0xc3,0x53,0x12,0xbd,0xce};
  u8 key[]={0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef};
  u8 VI[]={0x0f,0x1e,0x2d,0x3c,0x4b,0x5a,0x69,0x78};
  u8 enc2[]={0x5a,0x55,0xd6,0xa9,0x2e,0x6f,0x03,0x3a};
  int i;
  int err;
  u8 resu[32];
  PRINTF( "3DES CFB TEST START -----\n");
  PRINTF( "TEST 1\n");
  err=ucl_3des_cfb(resu,data,key,VI,(u32)sizeof(data),UCL_CIPHER_ENCRYPT);
  if(err!=UCL_OK)
    {
      PRINTF("ERROR err=%d\n",err);
      return(UCL_ERROR);
    }
  for(i=0;i<(int)sizeof(data);i++)
    if(resu[i]!=res[i])
      {
	PRINTF("ERROR on 3DES-CFB 1 ENC\n");
	for(i=0;i<(int)sizeof(data);i++)
	  PRINTF("%02x",resu[i]);
	PRINTF("\n");
	for(i=0;i<(int)sizeof(data);i++)
	  PRINTF("%02x",res[i]);
	PRINTF("\n");

	ucl_3des_ecb(resu,enc2,key,8,UCL_CIPHER_ENCRYPT);
	for(i=0;i<8;i++)
	  PRINTF("%02x",resu[i]);
	PRINTF("\n");
	return(UCL_ERROR);
      }
  err=ucl_3des_cfb(resu,res,key,VI,sizeof(res),UCL_CIPHER_DECRYPT);
  if(UCL_OK!=err)
    {
      PRINTF("ERROR 3des cfb\n");
      return(UCL_ERROR);
    }
  for(i=0;i<(int)sizeof(data);i++)
    if(resu[i]!=data[i])
      {
	PRINTF("ERROR on 3DES-CFB 1 DEC\n");
	return(UCL_ERROR);
      }
  return(UCL_OK);
}

int test3desofb()
{
  u8 data[24]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  u8 res[]={0xf9,0xdd,0x49,0x8a,0xf8,0x84,0xa9,0x7f,0x5a,0x55,0xd6,0xa9,0x2e,0x6f,0x03,0x3a,0x76,0xf8,0x72,0xc3,0x53,0x12,0xbd,0xce};
  u8 key[]={0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef};
  u8 VI[]={0x0f,0x1e,0x2d,0x3c,0x4b,0x5a,0x69,0x78};
  int i;
  u8 resu[32];
  int err;
  PRINTF( "3DES OFB TEST START -----\n");
  PRINTF( "TEST 1\n");
  err=ucl_3des_ofb(resu,data,key,VI,sizeof(data),UCL_CIPHER_ENCRYPT);
  if(UCL_OK!=err)
    {
      PRINTF("ERROR 3des ofb\n");
      return(UCL_ERROR);
    }
  for(i=0;i<(int)sizeof(data);i++)
    if(resu[i]!=res[i])
      {
	PRINTF("ERROR on 3DES-OFB 1 ENC\n");
	return(UCL_ERROR);
      }
  err=ucl_3des_ofb(resu,res,key,VI,sizeof(res),UCL_CIPHER_DECRYPT);
  if(UCL_OK!=err)
    {
      PRINTF("ERROR 3des ofb\n");
      return(UCL_ERROR);
    }
  for(i=0;i<(int)sizeof(data);i++)
    if(resu[i]!=data[i])
      {
	PRINTF("ERROR on 3DES-OFB 1 DEC\n");
	return(UCL_ERROR);
      }
  return(UCL_OK);
}


int test_3des(void)
{
  int ret = 0;

  if(test3desecb()!=0)
    {
      PRINTF("ERROR 3DES-ECB\n");
      ret += 0x10;
    }
  if(test3descbc()!=0)
    {
      PRINTF("ERROR 3DES-CBC\n");
      ret += 0x11;
    }
  if(test3descfb()!=0)
    {
      PRINTF("ERROR 3DES-CFB\n");
      ret += 0x12;
    }
  if(test3desofb()!=0)
    {
      PRINTF("ERROR 3DES-OFB\n");
      ret += 0x13;
    }
  return ret;
}

