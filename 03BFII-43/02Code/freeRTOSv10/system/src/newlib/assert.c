/*
 * This file is part of the µOS++ III distribution.
 * Copyright (c) 2014 Liviu Ionescu.
 * Copyright (C) 2016 Maxim Integrated Products, Inc., All Rights Reserved.
 */

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

#include "diag/Trace.h"

/*---------------------------------------------------------------------------*/

void
__attribute__((noreturn))
__assert_func (const char *file, int line, const char *func,
               const char *failedexpr)
{
  trace_printf ("assertion \"%s\" failed: file \"%s\", line %d%s%s\n",
                failedexpr, file, line, func ? ", function: " : "",
                func ? func : "");
  abort ();
  /* NOTREACHED */
}

/*----------------------------------------------------------------------------*/
