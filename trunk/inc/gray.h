/*----------------------------------------------------------------------------*\
 |	gray.h								      |
 |									      |
 |	Copyright © 2005-2006, The Gray Matter Team, original authors.	      |
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

/* C stuff: */
#include <ctype.h>
#include <inttypes.h>
#include <limits.h>
#include <pthread.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

/* C++ stuff: */
#include <fstream>
#include <list>

/* Gray Matter stuff: */
#include <board.h>
#include <config.h>
#include <history.h>
#include <opening.h>
#include <search.h>
#include <table.h>
#include <types.h>
#include <xboard.h>

/* Convenience macros: */
#define LESSER(j, k)	((j) < (k) ? (j) : (k))
#define GREATER(j, k)	((j) > (k) ? (j) : (k))

#endif
