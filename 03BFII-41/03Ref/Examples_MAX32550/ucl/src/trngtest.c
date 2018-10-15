/** Global includes */
//#include <config.h>
/** Other projects includes */

#include <ucl_testing_config.h>
#ifdef USE_TRNG
#ifdef SYSTEM_JIBE
#include <printf_lite.h>
#endif

#ifdef SYSTEM_LIGHTHOUSE
#include <printf_lite.h>
#endif

#include <ucl/ucl_config.h>
#include <ucl/ucl_defs.h>
#include <ucl/ucl_retdefs.h>
#include <ucl/ucl_types.h>
#include <ucl/ucl_rng.h>
#include <ucl/ucl_sha256.h>
#include <ucl/ucl_sp80090a_hash_drbg.h>
#include <ucl/lighthouse_crypto.h>

#include <string.h>

int test_emv_unpredictable_number(void)
{
  u8 tvp[EMV_TVP_BYTESIZE];
  u8 tid[EMV_TID_BYTESIZE];
  u8 ifdsn[EMV_IFDSN_BYTESIZE];
  u8 rand[EMV_RAND_BYTESIZE];
  u8 ac[EMV_AC_BYTESIZE];
  u8 p[EMV_P_BYTESIZE];
  u8 q[EMV_Q_BYTESIZE];
  u8 un[EMV_UN_BYTESIZE];
  int i,nbtransactions;
  int j;
  PRINTF("EMV UN generation\n");
  //initializing the input parameters to random- variables, to mimic EMV parameters
  ucl_rng_read(rand,EMV_RAND_BYTESIZE,UCL_RAND_DEFAULT);
  ucl_rng_read(tvp,EMV_TVP_BYTESIZE,UCL_RAND_DEFAULT);
  ucl_rng_read(ifdsn,EMV_IFDSN_BYTESIZE,UCL_RAND_DEFAULT);
  ucl_rng_read(tid,EMV_TID_BYTESIZE,UCL_RAND_DEFAULT);
  ucl_rng_read(ac,EMV_AC_BYTESIZE,UCL_RAND_DEFAULT);
  ucl_rng_read(q,EMV_Q_BYTESIZE,UCL_RAND_DEFAULT);

  ucl_emv_unpredictable_number_power_up(q,tid,ifdsn,tvp,rand,p);
  nbtransactions=10;
  for(i=0;i<nbtransactions;i++)
    {
      ucl_emv_unpredictable_number_transaction_un_generation(un,rand,p);
      for(j=0;j<EMV_UN_BYTESIZE;j++)
	PRINTF("%02x",un[j]);
      PRINTF("\n");
      ucl_emv_unpredictable_number_transaction_p_refresh(p,tvp,rand,ac);
    }
  ucl_emv_unpredictable_number_power_down(q,p);
  return(UCL_OK);
}

void sample_128MB(void)
{
  int i;
  int j;
  u8 random[128];
  for(i=0;i<1024*1024;i++)
    {
      ucl_rng_read(random,128,UCL_RAND_DEFAULT);
      for(j=0;j<128;j++)
	PRINTF("%02x",random[j]);
    }
      PRINTF("\n");
}

void sample_128MB_emv(void)
{
  u8 tvp[EMV_TVP_BYTESIZE];
  u8 tid[EMV_TID_BYTESIZE];
  u8 ifdsn[EMV_IFDSN_BYTESIZE];
  u8 rand[EMV_RAND_BYTESIZE];
  u8 ac[EMV_AC_BYTESIZE];
  u8 p[EMV_P_BYTESIZE];
  u8 q[EMV_Q_BYTESIZE];
  u8 un[EMV_UN_BYTESIZE];
  int i;
  int j;
  int k;
  PRINTF("EMV UN 128MB generation\n");
  //initializing the input parameters to random variables, to mimic EMV parameters
  ucl_rng_read(rand,EMV_RAND_BYTESIZE,UCL_RAND_DEFAULT);
  ucl_rng_read(tvp,EMV_TVP_BYTESIZE,UCL_RAND_DEFAULT);
  ucl_rng_read(ifdsn,EMV_IFDSN_BYTESIZE,UCL_RAND_DEFAULT);
  ucl_rng_read(tid,EMV_TID_BYTESIZE,UCL_RAND_DEFAULT);
  ucl_rng_read(ac,EMV_AC_BYTESIZE,UCL_RAND_DEFAULT);
  ucl_rng_read(q,EMV_Q_BYTESIZE,UCL_RAND_DEFAULT);

  ucl_emv_unpredictable_number_power_up(q,tid,ifdsn,tvp,rand,p);

  for(i=0;i<1024*1024;i++)
    {
      for(j=0;j<128/EMV_UN_BYTESIZE;j++)
	{
	  ucl_emv_unpredictable_number_transaction_un_generation(un,rand,p);
	  for(k=0;k<EMV_UN_BYTESIZE;k++)
	    PRINTF("%02x",un[k]);
	  ucl_emv_unpredictable_number_transaction_p_refresh(p,tvp,rand,ac);
	}
      PRINTF("\n");
    }
}
void sample_128MB_whitening_sha256(void)
{
  int i;
  int j;
  //128MB
  int random_size=512;
  u8 random[512];
  for(i=0;i<1024*1024/4;i++)
    {
      ucl_sha256_whitening_rng_read(random,random_size);
      for(j=0;j<random_size;j++)
	PRINTF("%02x",random[j]);
      PRINTF("\n");
    }
}

void sample_1GB_random_numbers(void)
{
  int i,j,k;
  //1GB=100x 10MB
  int random_size=1024;
  u8 random[1024];
  for(i=0;i<100;i++)
    {
      PRINTF("#%d ------------------------------\n",i);
      for(k=0;k<10*1024;k++)//10240x 1024=10MB
	{
	  ucl_rng_read(random,random_size,UCL_RAND_DEFAULT);
	  for(j=0;j<random_size;j++)
	    PRINTF("%02x",random[j]);
	}
      PRINTF("\n");
    }
}

int sample_trng(int maxtries)
{
  int i,j,k;
  int nbtries;
  int bit[2];
  int fourbit[16];
  int x;
  int runs[2][7];
  int trun,lrun,maxlrun;
  int runs_l[7]={0,2267,1079,502,223,90,90};
  int runs_h[7]={0,2733,1421,748,402,223,223};
  int method;
  int max_methods=3;
  u8 random[UCL_SHA256_HASHSIZE];
  int random_size=UCL_SHA256_HASHSIZE;
  int result;
  int nbtot;
  for(method=1;method<max_methods;method++)
    {
#ifdef VERBOSE
	      PRINTF("method %d\n",method);
#endif
      for(nbtries=0;nbtries<maxtries;nbtries++)
	{
	  for(i=0;i<16;i++)
	    fourbit[i]=0;
	  bit[0]=bit[1]=0;
	  //value of the run
	  trun=0;
	  //length of the run
	  lrun=0;
	  //maxrun length
	  maxlrun=0;
	  nbtot=0;
	  for(i=1;i<7;i++)
	    runs[0][i]=runs[1][i]=0;
	  for(i=0;i<1+2500/random_size;i++) // ie 20000 bits
	    {
	      if(1==method)
		result=ucl_rng_read(random, random_size, UCL_RAND_DEFAULT);
	      if(2==method)
		result=ucl_sha256_whitening_rng_read(random,random_size);
	      if(UCL_ERROR==result)
		{
		  PRINTF("production error\n");
		  return(UCL_ERROR);
		}
	      //nbtot is used to limit to 2500 bytes (i.e. 20000 bits)
	      //otherwise the range values are not correct
	      for(k=0;(k<random_size) && (nbtot<2500);k++,nbtot++)
		{
		  for(j=0;j<8;j++)
		    bit[(random[j]>>j)&1]++;
		  fourbit[random[k]>>4]++;
		  fourbit[random[k]&15]++;
		  for(j=0;j<8;j++)
		    // if same bit than run value
		    if(((random[k]>>j)&1)==trun)
		      // increment run length
		      {
			lrun++;
		      }
		    else
		      // otherwise, save info
		      {
			if(maxlrun<lrun)
			  maxlrun=lrun;
			if(lrun>6)
			  runs[trun][6]++;
			else
			  runs[trun][lrun]++;
			// save bit in new run value
			trun=(random[k]>>j)&1;
			// new run length is 1
			lrun=1;
		      }
		}
	    }
	  // finish process runs
	  if(maxlrun<lrun)
	    maxlrun=lrun;
	  if(lrun>6)
	    runs[trun][6]++;
	  else
	    runs[trun][lrun]++;
	  // save bit in new run value
	  trun=(random[0]>>j)&1;
	  // new run length is 1
	  lrun=1;

	  //int approx of poker test
	  x=0;
	  for(i=0;i<16;i++)
	    x+=fourbit[i]*fourbit[i];
	  x=16*x;
	  if(x>(57+5000)*5000 || x<(1+5000)*5000)
	    return(UCL_ERROR);

	  //runs test
	  for(i=1;i<7;i++)
	    for(j=0;j<2;j++)
	      if(runs[j][i]<runs_l[i] || runs[j][i]>runs_h[i])
		{
		  return(UCL_ERROR);
		}
	  //max run test
	  if(maxlrun>=34)
	    {
	      PRINTF("ERR MAX RUNS\n");
	      return(UCL_ERROR);
	    }
	}
    }

  return(UCL_OK);
}

int test_trng(void)
{
  int i,j;
  int result;
  u8 random_numbers[1024];
  int random_size=1024;
  int loop=1;
  int loop2=1;
  PRINTF("TRNG testing----------\n");
  if(UCL_OK==ucl_rng_stest())
    {
      PRINTF("RNG health test OK\n");
    }
  else
    {
      PRINTF("RNG health test NOK\n");
      return(UCL_ERROR);
      }
  for(i=0;i<loop;i++)
    for(j=0;j<loop2;j++)
      {
	//result contains either an error code or the number of produced random bytes
	memset(random_numbers,0,1024);
	result=ucl_rng_read(random_numbers,random_size,UCL_RAND_DEFAULT);
	if(UCL_ERROR==result)
	  {
	    PRINTF("error %d\n",result);
	    return(UCL_ERROR);
	  }
	if(random_size!=result)
	  {
	    PRINTF("WARNING: %d bytes asked, %d bytes produced\n",random_size,result);
	  }
      }
  for(i=0;i<random_size;i++)
    PRINTF("%02x",random_numbers[i]);
  PRINTF("\n");
  return(UCL_OK);
}

int test_fips_rng(void)
{
  int i,j;
  int result;
  u8 random_numbers[1024];
  int random_size=1024;
  int loop=1;
  int loop2=1;
  PRINTF("FIPS RNG testing----------\n");
  for(i=0;i<loop;i++)
    for(j=0;j<loop2;j++)
      {
	//result contains either an error code or the number of produced random bytes
	result=ucl_fips_rng_read(random_numbers,random_size,UCL_RAND_DEFAULT);
	if(UCL_ERROR==result)
	  {
	    PRINTF("error %d\n",result);
	    return(UCL_ERROR);
	  }
	if(random_size!=result)
	  {
	    PRINTF("WARNING: %d bytes asked, %d bytes produced\n",random_size,result);
	  }
      }
  for(i=0;i<random_size;i++)
    PRINTF("%02x",random_numbers[i]);
  PRINTF("\n");
  return(UCL_OK);
}

int test_whitening_trng(void)
{
  int i,j,k;
  int result;
  u8 random_numbers[1024];
  int random_byte[256];
  int random_size=1024;
  int loop=10;
  int loop2=10;
  PRINTF("whitened TRNG testing----------\n");
  for(i=0;i<256;i++)
    random_byte[i]=0;

  for(i=0;i<loop;i++)
    for(j=0;j<loop2;j++)
      {
	result=ucl_sha256_whitening_rng_read(random_numbers,random_size);
	if(UCL_ERROR==result)
	  {
	    PRINTF("error %d\n",result);
	    return(UCL_ERROR);
	  }
	for(k=0;k<random_size;k++)
	  random_byte[random_numbers[k]]++;
      }
  for(i=0;i<256;i++)
    PRINTF("%02x -> %d\n",i,random_byte[i]);
  PRINTF("\n");
  return(UCL_OK);
}

int test_hash_drbg(void)
{
  int i;
  int resu;
  int entropy_byte_size=1024;
  u8 entropy_input[1024];
  int nonce_byte_size=1024;
  u8 nonce[1024];
  u8 pseudorandom_numbers[2048];
  int pseudorandom_numbers_byte_size=2048;
  ucl_sp80090a_internal_state_t internal_state;
  PRINTF("SP800-90A hash DRBG test\n");
  //startup
  PRINTF("trng\n");
  //the hash sp800-90a uses an internal state
  //the instantiation is used for initializing the internal state
  //before the generation of pseudorandom bits is possible.
  //it has to be done once.
  //For that, it needs a string of bits coming from the randomness source
  //the ucl_rng_read below is used to acquire this string of bits, stored in entropy_input
  PRINTF("instantiate\n");
  ucl_rng_read(entropy_input,entropy_byte_size,UCL_RAND_DEFAULT);
  //the nonce input can be a random string of bits (see SP800-90A section 8.6.7 for more options
  ucl_rng_read(nonce,nonce_byte_size,UCL_RAND_DEFAULT);
  ucl_sp80090a_instantiate_sha256(&internal_state,entropy_input,entropy_byte_size,nonce,nonce_byte_size,NULL,0);
  //once the instantiation has been performed, the pseudo random numbers generation can start
  //generating numbers
  PRINTF("drbg\n");
  //in real world, this generation step is a specific function called by the application
  resu=ucl_sp80090a_hash_drbg_sha256_generate(pseudorandom_numbers,pseudorandom_numbers_byte_size*8,&internal_state,NULL,0);
  if(UCL_RESEED_REQUIRED==resu)
    {
      //reseeding only, not generating any number
      ucl_rng_read(entropy_input,entropy_byte_size,UCL_RAND_DEFAULT);
      ucl_sp80090a_reseeding_sha256(&internal_state,entropy_input,entropy_byte_size,nonce,0);
    }
  else
    {
      if(UCL_OK!=resu)
	{
	  PRINTF("error hash drbg: %d\n",resu);
	  return(UCL_ERROR);
	}
    }
  for(i=0;i<pseudorandom_numbers_byte_size;i++)
    PRINTF("%02x",pseudorandom_numbers[i]);
  PRINTF("\n");
  return(UCL_OK);
}


  int entropy_byte_size=16;
  u8 entropy_input[1024];
  int nonce_byte_size=16;
  u8 nonce[1024];
ucl_sp80090a_internal_state_t internal_state;
u8 personalization_string[10];

int sp80090_initialization(void)
{
  int resu;
  //the hash sp800-90a uses an internal state
  //the instantiation is used for initializing the internal state
  //before the generation of pseudorandom bits is possible.
  //it has to be done once.
  //For that, it needs a string of bits coming from the randomness source
  //the ucl_rng_read below is used to acquire this string of bits, stored in entropy_input
  PRINTF("instantiate\n");
#ifdef NULL_INPUT
  memset(entropy_input,0,entropy_byte_size);
#else
  resu=ucl_rng_read(entropy_input,entropy_byte_size,UCL_RAND_DEFAULT);
  if(entropy_byte_size!=resu)
    return(UCL_ERROR);
#endif
  //the nonce input can be a random string of bits (see SP800-90A section 8.6.7 for more options
#ifdef NULL_INPUT
  memset(nonce,0,nonce_byte_size);
#else
  resu=ucl_rng_read(nonce,nonce_byte_size,UCL_RAND_DEFAULT);
  if(nonce_byte_size!=resu)
    return(UCL_ERROR);
#endif
  resu=ucl_sp80090a_instantiate_sha256(&internal_state,entropy_input,entropy_byte_size,nonce,nonce_byte_size,personalization_string,0);
  return(resu);

}

int sp80090_read(u8 *pseudorandom_numbers,int pseudorandom_numbers_byte_size)
{
  int resu;
  //once the instantiation has been performed, the pseudo random numbers generation can start
  //generating numbers
  resu=ucl_sp80090a_hash_drbg_sha256_generate(pseudorandom_numbers,pseudorandom_numbers_byte_size*8,&internal_state,NULL,0);
  if(UCL_RESEED_REQUIRED==resu)
    {
      PRINTF("reseeding\n");
      //reseeding only, not generating any number
#ifdef NULL_INPUT
  memset(entropy_input,0,entropy_byte_size);
#else
      resu=ucl_rng_read(entropy_input,entropy_byte_size,UCL_RAND_DEFAULT);
      if(resu!=entropy_byte_size)
	return(UCL_ERROR);
#endif
      resu=ucl_sp80090a_reseeding_sha256(&internal_state,entropy_input,entropy_byte_size,nonce,0);
      PRINTF("reseed_counter=%llx\n",internal_state.reseed_counter);
      if(UCL_OK!=resu)
	return(resu);
      else
	{
	  resu=ucl_sp80090a_hash_drbg_sha256_generate(pseudorandom_numbers,pseudorandom_numbers_byte_size*8,&internal_state,NULL,0);
	  PRINTF("resu=%d\n",resu);
	  return(resu);
	}
    }
  else
    {
      if(UCL_OK!=resu)
	{
	  PRINTF("error hash drbg: %d\n",resu);
	  return(UCL_ERROR);
	}
    }
  return(UCL_OK);
}

int sp80090_usage(void)
{
  int resu;
  int total_numbers_byte_size;
  u8 pseudorandom_numbers[2048];
  int pseudorandom_numbers_byte_size=31;
  int i=0;
  int j;

  PRINTF("usage\n");
  total_numbers_byte_size=2048;

  sp80090_initialization();
  while(i<total_numbers_byte_size)
    {
      resu=sp80090_read(pseudorandom_numbers,pseudorandom_numbers_byte_size);
      if(UCL_OK!=resu)
	{
	  return(resu);
	}
      for(j=0;j<pseudorandom_numbers_byte_size;j++)
	PRINTF("%02x",pseudorandom_numbers[j]);
      PRINTF("\n");
      i+=pseudorandom_numbers_byte_size;
    }
  return(UCL_OK);
}


#endif//TRNG
