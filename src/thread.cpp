/*----------------------------------------------------------------------------*\
 |	thread.cpp - cross-platform multithreading library implementation     |
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

#include "thread.h"

#ifndef WIN32

/*----------------------------------------------------------------------------*\
 |				thread_create()				      |
\*----------------------------------------------------------------------------*/
int thread_create(tid_t *tid, thread_entry fnEntry, void *arg)
{
	int i = pthread_create(tid, NULL, fnEntry, arg);
	if(i != 0) {
		//VLOG_MSG(LOG_LOW,"thread_create - pthread_create failed with: %d", i);
		return -1;
	}
	return 1;
}

/*----------------------------------------------------------------------------*\
 |				 thread_exit()				      |
\*----------------------------------------------------------------------------*/
void thread_exit()
{
	pthread_exit(NULL);
}

/*----------------------------------------------------------------------------*\
 |				 thread_wait()				      |
\*----------------------------------------------------------------------------*/
int thread_wait(tid_t *tid)
{
	if(pthread_join(*tid, NULL) != 0)
		return -1;
	return 1;
}

/*----------------------------------------------------------------------------*\
 |				  mutex_init()				      |
\*----------------------------------------------------------------------------*/
int mutex_init(mutex_t *m)
{
	pthread_mutex_init(m, NULL);
	return 1;
}

/*----------------------------------------------------------------------------*\
 |				  mutex_lock()				      |
\*----------------------------------------------------------------------------*/
int mutex_lock(mutex_t *m)
{
	pthread_mutex_lock(m);
	return 1;
}

/*----------------------------------------------------------------------------*\
 |				mutex_try_lock()			      |
\*----------------------------------------------------------------------------*/
int mutex_try_lock(mutex_t *m)
{
	int result = pthread_mutex_trylock(m);
	if(result == EBUSY)
		return 0; // mutex is locked
	return 1;
}

/*----------------------------------------------------------------------------*\
 |				 mutex_unlock()				      |
\*----------------------------------------------------------------------------*/
int mutex_unlock(mutex_t *m)
{
	pthread_mutex_unlock(m);
	return 1;
}

/*----------------------------------------------------------------------------*\
 |				mutex_destroy()				      |
\*----------------------------------------------------------------------------*/
int mutex_destroy(mutex_t *m)
{
	pthread_mutex_destroy(m);
	return 1;
}

#else

int thread_create(tid_t *tid, thread_entry fnEntry, void *arg)
{
	*tid = CreateThread(NULL, 0, fnEntry, 0, NULL);
	if(*tid == NULL) {
		//VLOG_MSG(LOG_LOW,"thread_create - pthread_create failed with: %d", i);
		// TODO: see GetLastError()
		return -1;
	}
	return 1;
}

void thread_exit()
{
	ExitThread(0);
}

int thread_wait(tid_t *tid)
{
	//
	// wait, indefinitely for the thread to quit
	//
	DWORD result = WaitForSingleObject(*tid, INFINITE);
	if(result == WAIT_FAILED) {
		// TODO: see GetLastError
		return -1;
	}
	return 1;
}

int mutex_init(mutex_t *m)
{
	InitializeCriticalSection(m);
	return 1;
}

int mutex_lock(mutex_t *m)
{
	EnterCriticalSection(m);
	return 1;
}

int mutex_try_lock(mutex_t *m)
{
	// returns 0 if another thread is in this critical section
	BOOL b = TryEnterCriticalSection(m);
	if(!b)
		return 0; // mutex is locked
	return 1;
}

int mutex_unlock(mutex_t *m)
{
	LeaveCriticalSection(m);
	return 1;
}

int mutex_destroy(mutex_t *m)
{
	DeleteCriticalSection(m);
	return 1;
}

int cond_init(pthread_cond_t *cv, void *attrs)
{
	cv->waiters_count = 0;
	cv->was_broadcast = 0;
	cv->sema = CreatedSemaphore(NULL,       // no security
								0,          // initially 0
								0x7fffffff, // max count
								NULL);      // unnamed 
	InitializeCriticalSection(&cv->waiters_count_lock);
	cv->waiters_done = CreateEvent(NULL,  // no security
									FALSE, // auto-reset
									FALSE, // non-signaled initially
									NULL); // unnamed
	return 1;
}

int cond_destroy(pthread_cond_t *cv)
{
	// 
	// TODO
	// 
	return 1;
}

int cond_wait(thread_cond_t *cv, mutex_t *m)
{
	// Avoid race conditions.
	EnterCriticalSection(&cv->waiters_count_lock);
	cv->waiters_count++;
	LeaveCriticalSection(&cv->waiters_count_lock);

	// This call atomically releases the mutex and waits on the
	// semaphore until <pthread_cond_signal> or <pthread_cond_broadcast>
	// are called by another thread.
	SignalObjectAndWait(*m, cv->sema, INFINITE, FALSE);

	// Reacquire lock to avoid race conditions.
	EnterCriticalSection(&cv->waiters_count_lock);

	// We're no longer waiting...
	cv->waiters_count--;

	// Check to see if we're the last waiter after <pthread_cond_broadcast>.
	int last_waiter = cv->was_broadcast && cv->waiters_count == 0;

	LeaveCriticalSection(&cv->waiters_count_lock);

	// If we're the last waiter thread during this particular broadcast
	// then let all the other threads proceed.
	if (last_waiter)
	{
		// Call atomically signals the <waiters_done> event and waits until
		// it can acquire the <external_mutex>.  Required to ensure fairness. 
		SignalObjectAndWait (cv->waiters_done, *m, INFINITE, FALSE);
	} else {
		// Always regain the external mutex since that's the guarantee we
		// give to our callers. 
		WaitForSingleObject(*m);
	}
	return 1;
}

int cond_signal(thread_cond_t *cv)
{
	EnterCriticalSection(&cv->waiters_count_lock);
	int have_waiters = cv->waiters_count > 0;
	LeaveCriticalSection(&cv->waiters_count_lock);

	// If there aren't any waiters, then this is a no-op.  
	if(have_waiters)
		ReleaseSemaphore(cv->sema, 1, 0);
}

int cond_broadcast(thread_cond_t *cv)
{
	// This is needed to ensure that <waiters_count_> and <was_broadcast_> are
	// consistent relative to each other.
	EnterCriticalSection(&cv->waiters_count_lock);
	int have_waiters = 0;

	if(cv->waiters_count > 0)
	{
		// We are broadcasting, even if there is just one waiter...
		// Record that we are broadcasting, which helps optimize
		// <pthread_cond_wait> for the non-broadcast case.
		cv->was_broadcast = 1;
		have_waiters = 1;
	}

	if(have_waiters)
	{
		// Wake up all the waiters atomically.
		ReleaseSemaphore(cv->sema, cv->waiters_count, 0);

		LeaveCriticalSection(&cv->waiters_count_lock);

		// Wait for all the awakened threads to acquire the counting
		// semaphore. 
		WaitForSingleObject(cv->waiters_done, INFINITE);
		// This assignment is okay, even without the <waiters_count_lock_> held 
		// because no other waiter threads can wake up to access it.
		cv->was_broadcast = 0;
	}
	else
		LeaveCriticalSection(&cv->waiters_count_lock);
}

#endif
