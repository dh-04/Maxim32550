#include <ucl_testing_config.h>
#ifdef SYSTEM_JIBE
#include <printf_lite.h>
#endif//jibe

#include <ucl/ucl_hash.h>
#ifdef HASH_SM3
int test_sm3(void);
#endif


#ifdef HASH_SHA1
int test_sha1(void);
#endif

#ifdef HASH_SHA256
int test_sha256(void);
int sample_hmac256(void);
#endif

#ifdef HASH_SIA256
int test_sia256(void);
#endif

#ifdef HASH_SHA1
int test_sha1(void);
#endif

#ifdef HASH_SHA384
int test_sha384(void);
int sample_hmac384(void);
#endif

#ifdef HASH_SHA224
int test_sha224(void);
int sample_hmac224(void);
#endif

#ifdef HASH_SHA512
int test_sha512(void);
int sample_hmac512(void);
#endif

