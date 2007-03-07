
/*
 * Library for handling process threads
 *
 * Copyright (c) 2001-2007,  James D. Taylor.
 * All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 * 
 * contact James D. Taylor:  <james.d.taylor@gmail.com>
 */

#ifndef _METRIC_THREAD_H
#define _METRIC_THREAD_H

#ifndef WIN32

#include <pthread.h>
#include <errno.h>

typedef pthread_t tid_t;
typedef void* (*thread_entry)(void *arg);
typedef pthread_mutex_t mutex_t;

#define thread_cond_t pthread_cond_t
#define thread_cond_init pthread_cond_init
#define thread_cond_destroy pthread_cond_destroy
#define thread_cond_wait pthread_cond_wait
#define thread_cond_signal pthread_cond_signal
#define thread_cond_broadcast pthread_cond_broadcast

#else

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
	HANDLE sema;

	// An auto-reset event used by the broadcast/signal thread to wait
	// for all the waiting thread(s) to wake up and be released from the
	// semaphore. 
	HANDLE waiters_done;

	// Keeps track of whether we were broadcasting or signaling.  This
	// allows us to optimize the code if we're just signaling.
	size_t was_broadcast;
} thread_cond_t;

#endif

#ifdef __cplusplus
extern "C" {
#endif

int thread_create(tid_t *tid, thread_entry fnEntry, void *arg);
void thread_exit();
int thread_wait(tid_t *tid);

int mutex_init(mutex_t *m);
int mutex_lock(mutex_t *m);
int mutex_try_lock(mutex_t *m);
int mutex_unlock(mutex_t *m);
int mutex_destroy(mutex_t *m);

#ifdef WIN32
int thread_cond_init(thread_cond_t *cv, void *attrs);
int thread_cond_destroy(thread_cond_t *cv);
int thread_cond_wait(thread_cond_t *cv, mutex_t *external_mutex);
int thread_cond_signal(thread_cond_t *cv);
int thread_cond_broadcast(thread_cond_t *cv);
#endif

#ifdef __cplusplus
}
#endif

#endif

