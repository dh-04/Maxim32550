/*
 * mlsLib.h
 *
 *  Created on: Apr 7, 2017
 *      Author: vietdang
 */

#ifndef SOURCE_STYL_MLSLIB_INC_MLSLIB_H_
#define SOURCE_STYL_MLSLIB_INC_MLSLIB_H_

#include <stddef.h>

/**
 * @brief Copies specified number of bytes from a source buffer to a destination buffer.
 * @param[out]	dest	pointer to the object to copy to
 * @param[in] 	destsz	max number of bytes to modify in the destination (typically the size of the destination object)
 * @param[in]	src		pointer to the object to copy from
 * @param[in]	count	number of bytes to copy
 * @return	Returns zero on success and non-zero value on error.
 * Also on error, if dest is not a null pointer and destsz is valid,
 * writes destsz zero bytes in to the destination array.
 */
int memcpy_s (void *dest, size_t destsz, const void *src, size_t count);

#endif /* SOURCE_STYL_MLSLIB_INC_MLSLIB_H_ */
