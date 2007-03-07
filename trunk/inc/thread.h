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

#ifndef THREAD_H
#define THREAD_H

#if defined(LINUX) || defined(OS_X)

#include <errno.h>
#include <pthread.h>

typedef pthread_t tid_t;
typedef void *(*thread_entry)(void *arg);
typedef pthread_mutex_t mutex_t;
typedef pthread_cond_t cond_t;

#elif defined(WINDOWS)

#include <windows.h>

typedef HANDLE tid_t;
typedef DWORD (*thread_entry)(LPVOID arg);
typedef CRITICAL_SECTION mutex_t;

typedef struct
{
	// Number of waiting threads.
	int waiters_count;

	// Serialize access to <waiters_count_>.
	CRITICAL_SECTION waiters_count_lock;

	// Semaphore used to queue up threads waiting for the condition to
	// become signaled. 
	HANDLE sem;

	// An auto-reset event used by the broadcast/signal thread to wait
	// for all the waiting thread(s) to wake up and be released from the
	// semaphore. 
	HANDLE waiters_done;

	// Keeps track of whether we were broadcasting or signaling.  This
	// allows us to optimize the code if we're just signaling.
	size_t was_broadcast;
} cond_t;

#endif

#ifdef __cplusplus
extern "C"
{
#endif

int thread_create(tid_t *tid, thread_entry entry, void *arg);
void thread_exit();
int thread_wait(tid_t *tid);

int mutex_init(mutex_t *m);
int mutex_lock(mutex_t *m);
int mutex_unlock(mutex_t *m);
int mutex_try_lock(mutex_t *m);
int mutex_destroy(mutex_t *m);

int cond_init(cond_t *cv, void *attr);
int cond_wait(cond_t *cv, mutex_t *m);
int cond_signal(cond_t *cv);
int cond_broadcast(cond_t *cv);
int cond_destroy(cond_t *cv);

#ifdef __cplusplus
}
#endif

#endif
