#include <ucl_testing_config.h>
#include <ucl/ucl_config.h>
#include <ucl/ucl_types.h>
#include <ucl/ucl_sha1.h>
#include <ucl/ucl_sha256.h>
#include <ucl/ucl_defs.h>
#include <ucl/ucl_retdefs.h>
#include <ucl/ucl_pkcs5_pbkdf2.h>
#include <string.h>

int sample_pkcs5(void)
{
  /*
    rfc 3962
  */
  int err,i;
  u8 key[64];
  u8 expected_key3[32]={0x5c,0x08,0xeb,0x61,0xfd,0xf7,0x1e,0x4e,0x4e,0xc3,0xcf,0x6b,0xa1,0xf5,0x51,0x2b,
			0xa7,0xe5,0x2d,0xdb,0xc5,0xe5,0x14,0x2f,0x70,0x8a,0x31,0xe2,0xe6,0x2b,0x1e,0x13};
  u8 expected_key5[32]={0x9c,0xca,0xd6,0xd4,0x68,0x77,0x0c,0xd5,0x1b,0x10,0xe6,0xa6,0x87,0x21,0xbe,0x61
			,0x1a,0x8b,0x4d,0x28,0x26,0x01,0xdb,0x3b,0x36,0xbe,0x92,0x46,0x91,0x5e,0xc8,0x2a};

  //RFC 7194
  /*
    PBKDF2-HMAC-SHA-256 (P="passwd", S="salt",
                       c=1, dkLen=64) =
   55 ac 04 6e 56 e3 08 9f ec 16 91 c2 25 44 b6 05
   f9 41 85 21 6d de 04 65 e6 8b 9d 57 c2 0d ac bc
   49 ca 9c cc f1 79 b6 45 99 16 64 b3 9d 77 ef 31
   7c 71 b8 45 b1 e3 0b d5 09 11 20 41 d3 a1 97 83

   PBKDF2-HMAC-SHA-256 (P="Password", S="NaCl",
                        c=80000, dkLen=64) =
   4d dc d8 f6 0b 98 be 21 83 0c ee 5e f2 27 01 f9
   64 1a 44 18 d0 4c 04 14 ae ff 08 87 6b 34 ab 56
   a1 d4 25 a1 22 58 33 54 9a db 84 1b 51 c9 b3 17
   6a 27 2b de bb a1 d0 78 47 8f 62 b3 97 f3 3c 8d */

  PRINTF("TEST PKCS#5 PKDBF2\n");
  PRINTF("SHA1\n");
  // count is minimum>=1000 for security reasons
  // otherwise an invalid input arg is issued
  err=ucl_pkcs5_pbkdf2_hmac_sha1(key,16,(u8*)"password",strlen("password"),(u8*)"ATHENA.MIT.EDUraeburn",strlen("ATHENA.MIT.EDUraeburn"),1200);
  if(err!=0)
    return(UCL_ERROR);
  for(i=0;i<16;i++)
    if(key[i]!=expected_key3[i])
      {
	PRINTF("error %02x %02x\n",key[i],expected_key3[i]);
	return(UCL_ERROR);
      }
  err=ucl_pkcs5_pbkdf2_hmac_sha1(key,32,(u8*)"password",strlen("password"),(u8*)"ATHENA.MIT.EDUraeburn",strlen("ATHENA.MIT.EDUraeburn"),1200);
  if(err!=0)
    {
      return(UCL_ERROR);
    }
  for(i=0;i<32;i++)
    if(key[i]!=expected_key3[i])
      {
	PRINTF("error %02x %02x\n",key[i],expected_key3[i]);
	return(UCL_ERROR);
      }
  err=ucl_pkcs5_pbkdf2_hmac_sha1(key, 32, (u8*)"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", strlen("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"), (u8*)"pass phrase exceeds block size", strlen("pass phrase exceeds block size"), 1200);
  if(err!=0)
    return(UCL_ERROR);
  for(i=0;i<32;i++)
    if(key[i]!=expected_key5[i])
      {
	PRINTF("error %d %02x %02x\n",i,key[i],expected_key3[i]);
	PRINTF("ERROR\n");
	return(UCL_ERROR);
      }
  PRINTF("SHA2\n");

  err=ucl_pkcs5_pbkdf2_hmac_sha256(key, 64, (u8*)"Password", strlen("Password"), (u8*)"NaCl", strlen("NaCl"), 80000);
  if(err!=0)
    {
      PRINTF("%d\n",err);
      return(UCL_ERROR);
    }
  PRINTF("PBKDF2 SHA2 OK\n");
  return(UCL_OK);


}

int stress_pkcs5(int maxloop)
{
  u8 key[16];
  int i;
  for(i=0;i<maxloop;i++)
    ucl_pkcs5_pbkdf2_hmac_sha1(key, 32, (u8*)"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", strlen("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"), (u8*)"pass phrase exceeds block size", strlen("pass phrase exceeds block size"), 1200);
#ifdef VERBOSE
  PRINTF("[PERFS] PKCS5-HMAC-SHA1 (%d-1200) ",maxloop);
#endif
  return(UCL_OK);
}
