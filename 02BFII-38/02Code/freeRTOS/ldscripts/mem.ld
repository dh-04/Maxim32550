/*
 * Memory Spaces Definitions.
 *
 * The values below can be addressed in further linker scripts
 * using functions like 'ORIGIN(RAM)' or 'LENGTH(RAM)'.
 */

MEMORY
{
  RAM (xrw)         : ORIGIN = 0x20000000, LENGTH = 181K
  FBRAM (xrw)       : ORIGIN = 0x2002D400, LENGTH = 75K
  HEADER (rx)		: ORIGIN = 0x10000000, LENGTH = 0x00000100
  VECTORS (rx)      : ORIGIN = 0x10000100, LENGTH = 0x00000100
  FLASH (rx)		: ORIGIN = 0x10000200, LENGTH = 1M - 0x200
}

REGION_ALIAS("isr_vector_ro", VECTORS);
REGION_ALIAS("isr_vector_rw", RAM);


