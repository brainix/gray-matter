/*----------------------------------------------------------------------------*\
 |	thread.h - cross-platform multithreading library interface	      |
 |									      |
 |	Copyright © 2005-2007, The Gray Matter Team, original authors.	      |
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

/*
 | This is our cross-platform multithreading library.  All of these data types
 | and functions are defined twice: once for POSIX, again for Win32.
 |
 | Throughout this endeavor, I've learned that writing a multithreaded program
 | for Windows is akin to building a Ferrari out of Duplo.  Sadly, the world is
 | populated with toddlers with stubby fingers that are too clumsy to manipulate
 | Lego.  I just wish that that knucklehead Steve Ballmer could stop throwing
 | chairs long enough to make his thrice-damned operating system POSIX
 | conformant like every other operating system on this planet.  I hate myself.
 */

#ifndef THREAD_H
#define THREAD_H

/* For the functions defined here, these are the standard return values: */
#define CRITICAL	-1 // Critical failure.      :-(
#define NON_CRITICAL	 0 // Non-critical failure.  :-|
#define SUCCESSFUL	 1 // Success.               :-)

#if defined(LINUX) || defined(OS_X)

#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>

/* Data types: */
typedef pthread_t thread_t;
typedef void *(*entry_t)(void *arg);
typedef pthread_mutex_t mutex_t;
typedef pthread_cond_t cond_t;

#elif defined(WINDOWS)

#include <windows.h>

/* Data types: */
typedef HANDLE thread_t;
typedef DWORD (*entry_t)(LPVOID arg);
typedef HANDLE mutex_t;
typedef struct
{
	CRITICAL_SECTION lock; // Lock to protect count, bcast.
	int count;             // Number of waiting threads.
	HANDLE sema;           // Queue of waiting threads.
	HANDLE done;           // Whether all waiting threads have woken up.
	BOOL bcast;            // TRUE if broadcasting, FALSE if signaling.
} cond_t;

#endif

/* Function prototypes related to threads: */
int thread_create(thread_t *thread, entry_t entry, void *arg);
int thread_exit();
int thread_wait(thread_t *thread);

/* Function prototypes related to mutexes: */
int mutex_init(mutex_t *mutex);
int mutex_try_lock(mutex_t *mutex);
int mutex_lock(mutex_t *mutex);
int mutex_unlock(mutex_t *mutex);
int mutex_destroy(mutex_t *mutex);

/* Function prototypes related to condition variables: */
int cond_init(cond_t *cond, void *attr);
int cond_wait(cond_t *cond, mutex_t *mutex);
int cond_signal(cond_t *cond);
int cond_broadcast(cond_t *cond);
int cond_destroy(cond_t *cond);

/* Function prototypes related to timers: */
int timer_function(void (*function)());
int timer_set(int sec);
int timer_cancel();

#endif
