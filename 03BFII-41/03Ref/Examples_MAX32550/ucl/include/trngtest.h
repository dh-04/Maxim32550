void ucl_rng_read_pinblock_format3(u8 *data,int datalen_bytes);
int test_trng(void);
int test_fips_rng(void);
int test_emv_unpredictable_number(void);
int test_whitening_trng(void);
int sample_trng(int maxtries);
void   sample_1GB_random_numbers(void);
int sp80090_usage(void);
int sp80090_cavp(void);
