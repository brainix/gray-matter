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

typedef pthread_t thread_t;
typedef void *(*entry_t)(void *arg);
typedef pthread_mutex_t mutex_t;
typedef pthread_cond_t cond_t;

#elif defined(WINDOWS)

#include <windows.h>

typedef HANDLE thread_t;
typedef DWORD (*entry_t)(LPVOID arg);
typedef CRITICAL_SECTION mutex_t;
typedef struct
{
	CRITICAL_SECTION lock; // Serializes access to <waiters_count_>.
	int count;             // Number of waiting threads.
	HANDLE sema;           // Semaphore used to queue up threads waiting for the condition to become signaled.
	HANDLE done;           // An auto-reset event used by the broadcast/signal thread to wait for all the waiting threads to wake up and be released from the semaphore.
	int bcast;             // Keeps track of whether we were broadcasting or signaling.  This allows us to optimize the code if we're just signaling.
} cond_t;

#endif

int thread_create(thread_t *thread, entry_t entry, void *arg);
int thread_exit();
int thread_wait(thread_t *thread);

int mutex_init(mutex_t *mutex);
int mutex_try_lock(mutex_t *mutex);
int mutex_lock(mutex_t *mutex);
int mutex_unlock(mutex_t *mutex);
int mutex_destroy(mutex_t *mutex);

int cond_init(cond_t *cond, void *attr);
int cond_wait(cond_t *cond, mutex_t *mutex);
int cond_signal(cond_t *cond);
int cond_broadcast(cond_t *cond);
int cond_destroy(cond_t *cond);

#endif
