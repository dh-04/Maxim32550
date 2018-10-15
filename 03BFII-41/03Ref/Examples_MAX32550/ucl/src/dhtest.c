#include <ucl_testing_config.h>
#include <ucl/ucl_config.h>
#include <ucl/ucl_types.h>
#include <ucl/ucl_defs.h>
#include <ucl/ucl_retdefs.h>
#include <ucl/ucl_dh.h>
#include <string.h>
#include "MAX325xx.h"
#include <mml_uart.h>

#define SAMPLE_DH_MAXLEN  384
#define CTEST_BUFFER_SIZE (2*(SAMPLE_DH_MAXLEN/4)+MFPA_CTX_SIZE)


int sample_dh(void)
{
  //rfc 5114
  u8 modulus_dh_2048_5114[] = {0x87,0xA8,0xE6,0x1D,0xB4,0xB6,0x66,0x3C,0xFF,0xBB,0xD1,0x9C,0x65,0x19,0x59,0x99,0x8C,0xEE,0xF6,0x08,0x66,0x0D,0xD0,0xF2,0x5D,0x2C,0xEE,0xD4,0x43,0x5E,0x3B,0x00,0xE0,0x0D,0xF8,0xF1,0xD6,0x19,0x57,0xD4,0xFA,0xF7,0xDF,0x45,0x61,0xB2,0xAA,0x30,0x16,0xC3,0xD9,0x11,0x34,0x09,0x6F,0xAA,0x3B,0xF4,0x29,0x6D,0x83,0x0E,0x9A,0x7C,0x20,0x9E,0x0C,0x64,0x97,0x51,0x7A,0xBD,0x5A,0x8A,0x9D,0x30,0x6B,0xCF,0x67,0xED,0x91,0xF9,0xE6,0x72,0x5B,0x47,0x58,0xC0,0x22,0xE0,0xB1,0xEF,0x42,0x75,0xBF,0x7B,0x6C,0x5B,0xFC,0x11,0xD4,0x5F,0x90,0x88,0xB9,0x41,0xF5,0x4E,0xB1,0xE5,0x9B,0xB8,0xBC,0x39,0xA0,0xBF,0x12,0x30,0x7F,0x5C,0x4F,0xDB,0x70,0xC5,0x81,0xB2,0x3F,0x76,0xB6,0x3A,0xCA,0xE1,0xCA,0xA6,0xB7,0x90,0x2D,0x52,0x52,0x67,0x35,0x48,0x8A,0x0E,0xF1,0x3C,0x6D,0x9A,0x51,0xBF,0xA4,0xAB,0x3A,0xD8,0x34,0x77,0x96,0x52,0x4D,0x8E,0xF6,0xA1,0x67,0xB5,0xA4,0x18,0x25,0xD9,0x67,0xE1,0x44,0xE5,0x14,0x05,0x64,0x25,0x1C,0xCA,0xCB,0x83,0xE6,0xB4,0x86,0xF6,0xB3,0xCA,0x3F,0x79,0x71,0x50,0x60,0x26,0xC0,0xB8,0x57,0xF6,0x89,0x96,0x28,0x56,0xDE,0xD4,0x01,0x0A,0xBD,0x0B,0xE6,0x21,0xC3,0xA3,0x96,0x0A,0x54,0xE7,0x10,0xC3,0x75,0xF2,0x63,0x75,0xD7,0x01,0x41,0x03,0xA4,0xB5,0x43,0x30,0xC1,0x98,0xAF,0x12,0x61,0x16,0xD2,0x27,0x6E,0x11,0x71,0x5F,0x69,0x38,0x77,0xFA,0xD7,0xEF,0x09,0xCA,0xDB,0x09,0x4A,0xE9,0x1E,0x1A,0x15,0x97};
  u8  generator_2048_5114[]={0x3F,0xB3,0x2C,0x9B,0x73,0x13,0x4D,0x0B,0x2E,0x77,0x50,0x66,0x60,0xED,0xBD,0x48,0x4C,0xA7,0xB1,0x8F,0x21,0xEF,0x20,0x54,0x07,0xF4,0x79,0x3A,0x1A,0x0B,0xA1,0x25,0x10,0xDB,0xC1,0x50,0x77,0xBE,0x46,0x3F,0xFF,0x4F,0xED,0x4A,0xAC,0x0B,0xB5,0x55,0xBE,0x3A,0x6C,0x1B,0x0C,0x6B,0x47,0xB1,0xBC,0x37,0x73,0xBF,0x7E,0x8C,0x6F,0x62,0x90,0x12,0x28,0xF8,0xC2,0x8C,0xBB,0x18,0xA5,0x5A,0xE3,0x13,0x41,0x00,0x0A,0x65,0x01,0x96,0xF9,0x31,0xC7,0x7A,0x57,0xF2,0xDD,0xF4,0x63,0xE5,0xE9,0xEC,0x14,0x4B,0x77,0x7D,0xE6,0x2A,0xAA,0xB8,0xA8,0x62,0x8A,0xC3,0x76,0xD2,0x82,0xD6,0xED,0x38,0x64,0xE6,0x79,0x82,0x42,0x8E,0xBC,0x83,0x1D,0x14,0x34,0x8F,0x6F,0x2F,0x91,0x93,0xB5,0x04,0x5A,0xF2,0x76,0x71,0x64,0xE1,0xDF,0xC9,0x67,0xC1,0xFB,0x3F,0x2E,0x55,0xA4,0xBD,0x1B,0xFF,0xE8,0x3B,0x9C,0x80,0xD0,0x52,0xB9,0x85,0xD1,0x82,0xEA,0x0A,0xDB,0x2A,0x3B,0x73,0x13,0xD3,0xFE,0x14,0xC8,0x48,0x4B,0x1E,0x05,0x25,0x88,0xB9,0xB7,0xD2,0xBB,0xD2,0xDF,0x01,0x61,0x99,0xEC,0xD0,0x6E,0x15,0x57,0xCD,0x09,0x15,0xB3,0x35,0x3B,0xBB,0x64,0xE0,0xEC,0x37,0x7F,0xD0,0x28,0x37,0x0D,0xF9,0x2B,0x52,0xC7,0x89,0x14,0x28,0xCD,0xC6,0x7E,0xB6,0x18,0x4B,0x52,0x3D,0x1D,0xB2,0x46,0xC3,0x2F,0x63,0x07,0x84,0x90,0xF0,0x0E,0xF8,0xD6,0x47,0xD1,0x48,0xD4,0x79,0x54,0x51,0x5E,0x23,0x27,0xCF,0xEF,0x98,0xC5,0x82,0x66,0x4B,0x4C,0x0F,0x6C,0xC4,0x16,0x59};
  u8 priv1_2048_5114[] ={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x81,0x38,0x2C,0xDB,0x87,0x66,0x0C,0x6D,0xC1,0x3E,0x61,0x49,0x38,0xD5,0xB9,0xC8,0xB2,0xF2,0x48,0x58,0x1C,0xC5,0xE3,0x1B,0x35,0x45,0x43,0x97,0xFC,0xE5,0x0E};
  u8 priv2_2048_5114[] ={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7D,0x62,0xA7,0xE3,0xEF,0x36,0xDE,0x61,0x7B,0x13,0xD1,0xAF,0xB8,0x2C,0x78,0x0D,0x83,0xA2,0x3B,0xD4,0xEE,0x67,0x05,0x64,0x51,0x21,0xF3,0x71,0xF5,0x46,0xA5,0x3D};

  u8 shared_secret_2048_5114[]  ={0x86,0xC7,0x0B,0xF8,0xD0,0xBB,0x81,0xBB,0x01,0x07,0x8A,0x17,0x21,0x9C,0xB7,0xD2,0x72,0x03,0xDB,0x2A,0x19,0xC8,0x77,0xF1,0xD1,0xF1,0x9F,0xD7,0xD7,0x7E,0xF2,0x25,0x46,0xA6,0x8F,0x00,0x5A,0xD5,0x2D,0xC8,0x45,0x53,0xB7,0x8F,0xC6,0x03,0x30,0xBE,0x51,0xEA,0x7C,0x06,0x72,0xCA,0xC1,0x51,0x5E,0x4B,0x35,0xC0,0x47,0xB9,0xA5,0x51,0xB8,0x8F,0x39,0xDC,0x26,0xDA,0x14,0xA0,0x9E,0xF7,0x47,0x74,0xD4,0x7C,0x76,0x2D,0xD1,0x77,0xF9,0xED,0x5B,0xC2,0xF1,0x1E,0x52,0xC8,0x79,0xBD,0x95,0x09,0x85,0x04,0xCD,0x9E,0xEC,0xD8,0xA8,0xF9,0xB3,0xEF,0xBD,0x1F,0x00,0x8A,0xC5,0x85,0x30,0x97,0xD9,0xD1,0x83,0x7F,0x2B,0x18,0xF7,0x7C,0xD7,0xBE,0x01,0xAF,0x80,0xA7,0xC7,0xB5,0xEA,0x3C,0xA5,0x4C,0xC0,0x2D,0x0C,0x11,0x6F,0xEE,0x3F,0x95,0xBB,0x87,0x39,0x93,0x85,0x87,0x5D,0x7E,0x86,0x74,0x7E,0x67,0x6E,0x72,0x89,0x38,0xAC,0xBF,0xF7,0x09,0x8E,0x05,0xBE,0x4D,0xCF,0xB2,0x40,0x52,0xB8,0x3A,0xEF,0xFB,0x14,0x78,0x3F,0x02,0x9A,0xDB,0xDE,0x7F,0x53,0xFA,0xE9,0x20,0x84,0x22,0x40,0x90,0xE0,0x07,0xCE,0xE9,0x4D,0x4B,0xF2,0xBA,0xCE,0x9F,0xFD,0x4B,0x57,0xD2,0xAF,0x7C,0x72,0x4D,0x0C,0xAA,0x19,0xBF,0x05,0x01,0xF6,0xF1,0x7B,0x4A,0xA1,0x0F,0x42,0x5E,0x3E,0xA7,0x60,0x80,0xB4,0xB9,0xD6,0xB3,0xCE,0xFE,0xA1,0x15,0xB2,0xCE,0xB8,0x78,0x9B,0xB8,0xA3,0xB0,0xEA,0x87,0xFE,0xBE,0x63,0xB6,0xC8,0xF8,0x46,0xEC,0x6D,0xB0,0xC2,0x6C,0x5D,0x7C};

  //RFC3526 KAT
  //prime number
  u8 modulus_dh_2048_3526[]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xC9,0x0F,0xDA,0xA2,0x21,0x68,0xC2,0x34,0xC4,0xC6,0x62,0x8B,0x80,0xDC,0x1C,0xD1,0x29,0x02,0x4E,0x08,0x8A,0x67,0xCC,0x74,0x02,0x0B,0xBE,0xA6,0x3B,0x13,0x9B,0x22,0x51,0x4A,0x08,0x79,0x8E,0x34,0x04,0xDD,0xEF,0x95,0x19,0xB3,0xCD,0x3A,0x43,0x1B,0x30,0x2B,0x0A,0x6D,0xF2,0x5F,0x14,0x37,0x4F,0xE1,0x35,0x6D,0x6D,0x51,0xC2,0x45,0xE4,0x85,0xB5,0x76,0x62,0x5E,0x7E,0xC6,0xF4,0x4C,0x42,0xE9,0xA6,0x37,0xED,0x6B,0x0B,0xFF,0x5C,0xB6,0xF4,0x06,0xB7,0xED,0xEE,0x38,0x6B,0xFB,0x5A,0x89,0x9F,0xA5,0xAE,0x9F,0x24,0x11,0x7C,0x4B,0x1F,0xE6,0x49,0x28,0x66,0x51,0xEC,0xE4,0x5B,0x3D,0xC2,0x00,0x7C,0xB8,0xA1,0x63,0xBF,0x05,0x98,0xDA,0x48,0x36,0x1C,0x55,0xD3,0x9A,0x69,0x16,0x3F,0xA8,0xFD,0x24,0xCF,0x5F,0x83,0x65,0x5D,0x23,0xDC,0xA3,0xAD,0x96,0x1C,0x62,0xF3,0x56,0x20,0x85,0x52,0xBB,0x9E,0xD5,0x29,0x07,0x70,0x96,0x96,0x6D,0x67,0x0C,0x35,0x4E,0x4A,0xBC,0x98,0x04,0xF1,0x74,0x6C,0x08,0xCA,0x18,0x21,0x7C,0x32,0x90,0x5E,0x46,0x2E,0x36,0xCE,0x3B,0xE3,0x9E,0x77,0x2C,0x18,0x0E,0x86,0x03,0x9B,0x27,0x83,0xA2,0xEC,0x07,0xA2,0x8F,0xB5,0xC5,0x5D,0xF0,0x6F,0x4C,0x52,0xC9,0xDE,0x2B,0xCB,0xF6,0x95,0x58,0x17,0x18,0x39,0x95,0x49,0x7C,0xEA,0x95,0x6A,0xE5,0x15,0xD2,0x26,0x18,0x98,0xFA,0x05,0x10,0x15,0x72,0x8E,0x5A,0x8A,0xAC,0xAA,0x68,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
 u8 generator_2048_3526[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02};
 u8 priv1_2048_3526[]={0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04};
 u8 priv2_2048_3526[]={0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4};

 u8 modulus_dh_3072[]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xC9,0x0F,0xDA,0xA2,0x21,0x68,0xC2,0x34,0xC4,0xC6,0x62,0x8B,0x80,0xDC,0x1C,0xD1,0x29,0x02,0x4E,0x08,0x8A,0x67,0xCC,0x74,0x02,0x0B,0xBE,0xA6,0x3B,0x13,0x9B,0x22,0x51,0x4A,0x08,0x79,0x8E,0x34,0x04,0xDD,0xEF,0x95,0x19,0xB3,0xCD,0x3A,0x43,0x1B,0x30,0x2B,0x0A,0x6D,0xF2,0x5F,0x14,0x37,0x4F,0xE1,0x35,0x6D,0x6D,0x51,0xC2,0x45,0xE4,0x85,0xB5,0x76,0x62,0x5E,0x7E,0xC6,0xF4,0x4C,0x42,0xE9,0xA6,0x37,0xED,0x6B,0x0B,0xFF,0x5C,0xB6,0xF4,0x06,0xB7,0xED,0xEE,0x38,0x6B,0xFB,0x5A,0x89,0x9F,0xA5,0xAE,0x9F,0x24,0x11,0x7C,0x4B,0x1F,0xE6,0x49,0x28,0x66,0x51,0xEC,0xE4,0x5B,0x3D,0xC2,0x00,0x7C,0xB8,0xA1,0x63,0xBF,0x05,0x98,0xDA,0x48,0x36,0x1C,0x55,0xD3,0x9A,0x69,0x16,0x3F,0xA8,0xFD,0x24,0xCF,0x5F,0x83,0x65,0x5D,0x23,0xDC,0xA3,0xAD,0x96,0x1C,0x62,0xF3,0x56,0x20,0x85,0x52,0xBB,0x9E,0xD5,0x29,0x07,0x70,0x96,0x96,0x6D,0x67,0x0C,0x35,0x4E,0x4A,0xBC,0x98,0x04,0xF1,0x74,0x6C,0x08,0xCA,0x18,0x21,0x7C,0x32,0x90,0x5E,0x46,0x2E,0x36,0xCE,0x3B,0xE3,0x9E,0x77,0x2C,0x18,0x0E,0x86,0x03,0x9B,0x27,0x83,0xA2,0xEC,0x07,0xA2,0x8F,0xB5,0xC5,0x5D,0xF0,0x6F,0x4C,0x52,0xC9,0xDE,0x2B,0xCB,0xF6,0x95,0x58,0x17,0x18,0x39,0x95,0x49,0x7C,0xEA,0x95,0x6A,0xE5,0x15,0xD2,0x26,0x18,0x98,0xFA,0x05,0x10,0x15,0x72,0x8E,0x5A,0x8A,0xAA,0xC4,0x2D,0xAD,0x33,0x17,0x0D,0x04,0x50,0x7A,0x33,0xA8,0x55,0x21,0xAB,0xDF,0x1C,0xBA,0x64,0xEC,0xFB,0x85,0x04,0x58,0xDB,0xEF,0x0A,0x8A,0xEA,0x71,0x57,0x5D,0x06,0x0C,0x7D,0xB3,0x97,0x0F,0x85,0xA6,0xE1,0xE4,0xC7,0xAB,0xF5,0xAE,0x8C,0xDB,0x09,0x33,0xD7,0x1E,0x8C,0x94,0xE0,0x4A,0x25,0x61,0x9D,0xCE,0xE3,0xD2,0x26,0x1A,0xD2,0xEE,0x6B,0xF1,0x2F,0xFA,0x06,0xD9,0x8A,0x08,0x64,0xD8,0x76,0x02,0x73,0x3E,0xC8,0x6A,0x64,0x52,0x1F,0x2B,0x18,0x17,0x7B,0x20,0x0C,0xBB,0xE1,0x17,0x57,0x7A,0x61,0x5D,0x6C,0x77,0x09,0x88,0xC0,0xBA,0xD9,0x46,0xE2,0x08,0xE2,0x4F,0xA0,0x74,0xE5,0xAB,0x31,0x43,0xDB,0x5B,0xFC,0xE0,0xFD,0x10,0x8E,0x4B,0x82,0xD1,0x20,0xA9,0x3A,0xD2,0xCA,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
 u8 generator_3072[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02};
 u8 priv1_3072[]={0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04};
 u8 priv2_3072[]={0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4};

  u32 buffer_A[CTEST_BUFFER_SIZE], buffer_B[CTEST_BUFFER_SIZE];
  ucl_dh_ctx_t dh_ctx_B, dh_ctx_A;
  u8 m_A[SAMPLE_DH_MAXLEN], m_B[SAMPLE_DH_MAXLEN], key_A[SAMPLE_DH_MAXLEN], key_B[SAMPLE_DH_MAXLEN];
  int dhsize;
  int example;
  int method;
  int nbmethods=1;
  int tmode[1]={UCL_MOD_EXP_METHOD_MONTY};
  int i;

  for(method=0;method<nbmethods;method++)
    {
      for(example=1;example<=3;example++)
	{
	  if(example==1)
	    {
	      dhsize=2048;
	      dh_ctx_B.module = modulus_dh_2048_5114;
	      dh_ctx_B.base = generator_2048_5114;
	      dh_ctx_B.secret = priv1_2048_5114;
	      dh_ctx_B.len = dhsize/8;
	      
	      dh_ctx_A.module = modulus_dh_2048_5114;
	      dh_ctx_A.base = generator_2048_5114;
	      dh_ctx_A.secret = priv2_2048_5114;
	      dh_ctx_A.len = dhsize/8;
	    }
	  if(example==2)
	    {
	      dhsize=2048;
	      dh_ctx_B.module = modulus_dh_2048_3526;
	      dh_ctx_B.base = generator_2048_3526;
	      dh_ctx_B.secret = priv1_2048_3526;
	      dh_ctx_B.len = dhsize/8;
	      
	      dh_ctx_A.module = modulus_dh_2048_3526;
	      dh_ctx_A.base = generator_2048_3526;
	      dh_ctx_A.secret = priv2_2048_3526;
	      dh_ctx_A.len = dhsize/8;
	    }
	  if(example==3)
	    {
	      dhsize=3072;
	      dh_ctx_B.module = modulus_dh_3072;
	      dh_ctx_B.base = generator_3072;
	      dh_ctx_B.secret = priv1_3072;
	      dh_ctx_B.len = dhsize/8;
	      
	      dh_ctx_A.module = modulus_dh_3072;
	      dh_ctx_A.base = generator_3072;
	      dh_ctx_A.secret = priv2_3072;
	      dh_ctx_A.len = dhsize/8;
	    }
	  // A: precomputation
	  ucl_dh_init(&dh_ctx_A, buffer_A, CTEST_BUFFER_SIZE,tmode[method]);
	  
	  // computation of ma=g^secret a
	  for(i=0;i<1;i++)
	    {
	      ucl_dh_gen_mess(m_A, &dh_ctx_A);
	    }
	  // B: precomputation
	  ucl_dh_init(&dh_ctx_B, buffer_B, CTEST_BUFFER_SIZE,tmode[method]);
	  // computation of mb=g^secret b
	  ucl_dh_gen_mess(m_B, &dh_ctx_B);
	  // exchange m_A and m_B
	  ucl_dh_gen_key(key_A, dhsize/8, m_B, &dh_ctx_A);
	  ucl_dh_gen_key(key_B, dhsize/8, m_A, &dh_ctx_B);
	  if(example==1 && memcmp(key_A,shared_secret_2048_5114,dhsize/8)==0)
	    {
	      PRINTF("KAT shared secret OK\n");
	    }
	  if( memcmp(key_A, key_B, 16) != 0 )
	    {
	      return(UCL_ERROR);
	    }
	  PRINTF("key generation OK\n");
	}
    }
  return(UCL_OK);
}


