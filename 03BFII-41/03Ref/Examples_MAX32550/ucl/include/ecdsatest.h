#include <ucl_testing_config.h>
#include <ucl/ecdsa_generic_api.h>
#include <ucl/ucl_hash.h>

#ifdef VERBOSE
void display(char *string,u8 *tab,int size);
void display_word(char *string,DIGIT *tab,int size);
#endif//VERBOSE

#ifdef P192
int test_eccbasic_p192r1(void);
#ifdef HASH_SHA1
int test_ecdsa_p192r1_sha1(void);
#endif
#ifdef HASH_SHA256
int test_ecdsa_p192r1_sha256(void);
#endif
#ifdef HASH_SIA256
int test_ecdsa_p192r1_sia256(void);
#endif

#endif//P192

#ifdef P256
int test_eccbasic_p256r1(void);
#ifdef HASH_SHA256
int test_ecdsa_p256r1_sha256(void);
#endif
#endif//P256
#ifdef P521
#ifdef HASH_SHA512
int test_ecdsa_p521r1_sha512(void);
#endif
#endif//P521
#ifdef P384
#ifdef HASH_SHA384
int test_ecdsa_p384r1_sha384(void);
#endif
#endif//P384
#ifdef P160
#ifdef HASH_SHA1
int test_ecdsa_p160r1_sha1(void);
#endif
#endif//P160
#ifdef SM2P256
#ifdef HASH_SM3
int test_ecdsa_p256sm2(void);
#endif
#endif

#ifdef NIST_CURVES
	int test_ecdsa_curves(void);
#endif /* NIST_CURVES */

#ifdef BP256
int test_ecdsa_bp256r1(void);
#endif

#ifdef BP384
int test_ecdsa_bp384r1(void);
#endif

#ifdef BP512
int test_ecdsa_bp512r1(void);
#endif

int ecdsa_selftests_testing(void);
int test_ecdsa_api(int maxloop);

int test_sm2_encryption_decryption(int maxloop);
