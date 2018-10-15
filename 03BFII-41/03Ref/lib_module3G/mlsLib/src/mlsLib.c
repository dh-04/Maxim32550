/*
 * mlsLib.c
 *
 *  Created on: Apr 7, 2017
 *      Author: vietdang
 */
#include "../inc/mlsLib.h"
#include <string.h>

#define EINVAL		3021
#define ERANGE		3002

int memcpy_s (void *dest, size_t destsz, const void *src, size_t count)
{
	int errno = 0;
	if(!count)
	{
		return 0;
	}
	if(!dest || !src)
	{
		if(dest)
		{
			memset(dest, 0, destsz);
		}
		errno = EINVAL;
		return errno;
	}
	if(destsz < count)
	{
		memset(dest, 0, destsz);
		errno = ERANGE;
		return errno;
	}

	memcpy(dest, src, count);
	return 0;
}
