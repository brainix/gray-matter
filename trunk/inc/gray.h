/*----------------------------------------------------------------------------*\
 |	gray.h								      |
 |									      |
 |	Copyright � 2005-2007, The Gray Matter Team, original authors.	      |
 |		All rights reserved.					      |
\*----------------------------------------------------------------------------*/

/*
 | This program is Free Software; you can redistribute it and/or modify it under
 | the terms of the GNU General Public License as published by the Free Software
 | Foundation; either version 2 of the License, or (at your option) any later
 | version.
 |
 | This program is distributed in the hope that it will be useful, but WITHOUT
 | ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 | FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 | details.
 |
 | You should have received a copy of the GNU General Public License along with
 | this program; if not, write to:
 |
 |	The Free Software Foundation, Inc.
 |	59 Temple Place, Suite 330
 |	Boston MA 02111-1307
 */

#ifndef GRAY_H
#define GRAY_H

#define _REENTRANT

/* C stuff: */
#include <assert.h>
#include <ctype.h>
#include <getopt.h>
#include <inttypes.h>
#include <limits.h>
#include <pthread.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

/* Platform-specific stuff: */
#if PLATFORM == WINDOWS
#include <windows.h>
#endif

/* Convenience macros: */
#define LESSER(j, k)	((j) < (k) ? (j) : (k))
#define GREATER(j, k)	((j) > (k) ? (j) : (k))

#endif
