#include <ucl_testing_config.h>
#include <ucl/ecdsa_generic_api.h>
#include <ucl/ucl_sp800_56.h>
#include <ucl/ucl_rng.h>
#include <ucl/ucl_ecc_keygen.h>

#include <string.h>

//test with the new ECC API, introduced in 2.4.9
int ecc_sp800_test(int maxloop)
{
  int i;
  int curve;
  int hash;
  int err;
  u8 dU[SECP521R1_BYTESIZE];
  u8 xU[SECP521R1_BYTESIZE];
  u8 yU[SECP521R1_BYTESIZE];
  u8 dV[SECP521R1_BYTESIZE];
  u8 xV[SECP521R1_BYTESIZE];
  u8 yV[SECP521R1_BYTESIZE];
  u8 z[SECP521R1_BYTESIZE];
  u32 zbytelength;
  u8 derivedkeyingmaterial[128];
  u8 derivedkeyingmaterialunique[128];
  u32 keydatalen=128*8;
  u8 otherinfo[10];
  u32 otherinfobitlength=0;
  int tab_curve[10];
  int i_tab_curve,nb_tab_curve; 
  ucl_type_curve *curve_params;

  char curve_name[MAX_CURVE][20];
  
  sprintf(curve_name[SECP160R1],"p160r1");
  sprintf(curve_name[SECP192R1],"p192r1");
  sprintf(curve_name[SECP224R1],"p224r1");
  sprintf(curve_name[SECP256R1],"p256r1");
  sprintf(curve_name[SECP384R1],"p384r1");
  sprintf(curve_name[SECP521R1],"p521r1");
  sprintf(curve_name[BP256R1],"bp256r1");
  sprintf(curve_name[BP384R1],"bp384r1");
  sprintf(curve_name[BP512R1],"bp512r1");
  nb_tab_curve=0;
  tab_curve[nb_tab_curve++]=SECP256R1;
  tab_curve[nb_tab_curve++]=SECP384R1;
  tab_curve[nb_tab_curve++]=BP512R1;
  tab_curve[nb_tab_curve++]=BP256R1;
  tab_curve[nb_tab_curve++]=BP384R1;
  tab_curve[nb_tab_curve++]=SECP521R1;
  tab_curve[nb_tab_curve++]=SECP160R1;
  tab_curve[nb_tab_curve++]=SECP192R1;
  tab_curve[nb_tab_curve++]=SECP224R1;
  
  ucl_type_ecc_u8_affine_point U,V;
  U.x=xU;
  U.y=yU;
  V.x=xV;
  V.y=yV;

  PRINTF("ECC SP800-56 TEST(%d)\n",maxloop);

  for(i_tab_curve=0;i_tab_curve<nb_tab_curve;i_tab_curve++)
    {
      curve=tab_curve[i_tab_curve];
      PRINTF("%s\n",curve_name[curve]);

      //configure the parameters
      curve_params=&secp256r1;
      hash=UCL_SHA256;
      if(curve==SECP256R1)
	{
	  curve_params=&secp256r1;
	  hash=UCL_SHA256;
	}
      if(curve==SECP384R1)
	{
	  curve_params=&secp384r1;
	  hash=UCL_SHA384;
	}
      if(curve==SECP192R1)
	{
	  curve_params=&secp192r1;
	  hash=UCL_SHA256;
	}
      if(curve==SECP224R1)
	{
	  curve_params=&secp224r1;
	  hash=UCL_SHA224;
	}
      if(curve==SECP521R1)
	{
	  curve_params=&secp521r1;
	  hash=UCL_SHA512;
	}
      if(curve==BP256R1)
	{
	  curve_params=&bp256r1;
	  hash=UCL_SHA256;
	}
      if(curve==BP384R1)
	{
	  curve_params=&bp384r1;
	  hash=UCL_SHA384;
	}
      if(curve==BP512R1)
	{
	  curve_params=&bp512r1;
	  hash=UCL_SHA512;
	}
      for(i=0;i<maxloop;i++)
	{
	  //generating the first entity key, V
	  if(UCL_OK!=ucl_ecc_keygeneration_generic(V,dV,curve_params))
	    return(UCL_ERROR);
	  if(UCL_OK!=ucl_ecc_point_on_curve_generic(V,curve_params))
	    return(UCL_ERROR);
	  
	  //generating the 2nd entity key, with 1st entity public key shared
	  //uses V; generates U
	  //and computing the derived key material, all that in one single function call
	  err=ucl_ecc_sp800_56_eum_ecc_cdh(derivedkeyingmaterialunique, keydatalen, dU, U, V, otherinfo, otherinfobitlength, curve_params,(curve<<UCL_CURVE_SHIFT)^(hash<<UCL_HASH_SHIFT));
	  if(UCL_OK!=err)
	    {
	      PRINTF("ERROR eum cdh: %d\n",err);
	      return(UCL_ERROR);
	    }
	  //computing the key on U side, so knowing U secret key and V public key
	  err=ucl_ecc_sp800_56_ecc_cdh(z,dU,V,curve_params);
	  if(UCL_OK!=err)
	    {
	      PRINTF("ERROR cdh: %d\n",err);
	      return(UCL_ERROR);
	    }
	  zbytelength=curve_params->curve_bsize;
	  //then computing the derived key material, so in two functions calls
	  err=ucl_sp800_56_concat_kdf(derivedkeyingmaterial,keydatalen,z,zbytelength,otherinfo,otherinfobitlength,hash);
	  if(UCL_OK!=err)
	    {
	      PRINTF("ERROR kdf: %d\n",err);
	      return(UCL_ERROR);
	    }
	  //compares the two ways of computing the derived key material, on both side
	  if(memcmp(derivedkeyingmaterial,derivedkeyingmaterialunique,keydatalen/8)!=0)
	    {
	      PRINTF("ERROR (curve=%d hash=%d): different generation\n",curve,hash);
	      return(UCL_ERROR);
	    }
	}
    }
  PRINTF("OK\n");
  return(UCL_OK);
}

int test_nist(void)
{
  const u8 NIST_c0_secp256r1_A[] = {
    /** private d */
    0x7D, 0x7D, 0xC5, 0xF7, 0x1E, 0xB2, 0x9D, 0xDA,
    0xF8, 0x0D, 0x62, 0x14, 0x63, 0x2E, 0xEA, 0xE0,
    0x3D, 0x90, 0x58, 0xAF, 0x1F, 0xB6, 0xD2, 0x2E,
    0xD8, 0x0B, 0xAD, 0xB6, 0x2B, 0xC1, 0xA5, 0x34,
    /** public Px */
    0xEA, 0xD2, 0x18, 0x59, 0x01, 0x19, 0xE8, 0x87,
    0x6B, 0x29, 0x14, 0x6F, 0xF8, 0x9C, 0xA6, 0x17,
    0x70, 0xC4, 0xED, 0xBB, 0xF9, 0x7D, 0x38, 0xCE,
    0x38, 0x5E, 0xD2, 0x81, 0xD8, 0xA6, 0xB2, 0x30,
    /** public Py */
    0x28, 0xAF, 0x61, 0x28, 0x1F, 0xD3, 0x5E, 0x2F,
    0xA7, 0x00, 0x25, 0x23, 0xAC, 0xC8, 0x5A, 0x42,
    0x9C, 0xB0, 0x6E, 0xE6, 0x64, 0x83, 0x25, 0x38,
    0x9F, 0x59, 0xED, 0xFC, 0xE1, 0x40, 0x51, 0x41
  };
  
  /** NIST test vector SECP256r1 test key B */
  const u8 NIST_c0_secp256r1_B[] = {
    /** public Px */
    0x70, 0x0C, 0x48, 0xF7, 0x7F, 0x56, 0x58, 0x4C,
    0x5C, 0xC6, 0x32, 0xCA, 0x65, 0x64, 0x0D, 0xB9,
    0x1B, 0x6B, 0xAC, 0xCE, 0x3A, 0x4D, 0xF6, 0xB4,
    0x2C, 0xE7, 0xCC, 0x83, 0x88, 0x33, 0xD2, 0x87,
	/** public Py */
    0xDB, 0x71, 0xE5, 0x09, 0xE3, 0xFD, 0x9B, 0x06,
    0x0D, 0xDB, 0x20, 0xBA, 0x5C, 0x51, 0xDC, 0xC5,
    0x94, 0x8D, 0x46, 0xFB, 0xF6, 0x40, 0xDF, 0xE0,
    0x44, 0x17, 0x82, 0xCA, 0xB8, 0x5F, 0xA4, 0xAC
  };

  const u8 NIST_c0_secp256r1_secret[] = {
    0x46, 0xFC, 0x62, 0x10, 0x64, 0x20, 0xFF, 0x01,
    0x2E, 0x54, 0xA4, 0x34, 0xFB, 0xDD, 0x2D, 0x25,
    0xCC, 0xC5, 0x85, 0x20, 0x60, 0x56, 0x1E, 0x68,
    0x04, 0x0D, 0xD7, 0x77, 0x89, 0x97, 0xBD, 0x7B
  };
  int err;
  u8 z[SECP521R1_BYTESIZE];
  ucl_type_ecc_u8_affine_point V;
  u8 dU[SECP521R1_BYTESIZE];
  ucl_type_curve *curve_params;
  
  curve_params=&secp256r1;
  memcpy(dU,&(NIST_c0_secp256r1_A[0]),32);
  V.x=(u8 *)&(NIST_c0_secp256r1_B[0]);
  V.y=(u8 *)&(NIST_c0_secp256r1_B[32]);
  
  err=ucl_ecc_sp800_56_ecc_cdh(z,dU,V,curve_params);
  if(err!=UCL_OK)
    PRINTF("err=%d\n",err);
  if(memcmp(NIST_c0_secp256r1_secret,z,32)==0)
    return(UCL_OK);
  else
    return(UCL_ERROR);
}

int test_sp800_56(int maxloop)
{
  if(ecc_sp800_test(maxloop)!=UCL_OK)
    return(UCL_ERROR);
  return(test_nist());
}

