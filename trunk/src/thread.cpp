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

/*----------------------------------------------------------------------------*\
 |				thread_create()				      |
\*----------------------------------------------------------------------------*/
int thread_create(thread_t *thread, entry_t entry, void *arg)
{
#if defined(LINUX) || defined(OS_X)
	return pthread_create(thread, NULL, entry, arg) ? -1 : 1;
#elif defined(WINDOWS)
	return (*thread = CreateThread(NULL, 0, entry, 0, NULL)) == NULL ? -1 : 1;
#endif
}

/*----------------------------------------------------------------------------*\
 |				 thread_exit()				      |
\*----------------------------------------------------------------------------*/
int thread_exit()
{
#if defined(LINUX) || defined(OS_X)
	pthread_exit(NULL);
#elif defined(WINDOWS)
	ExitThread(0);
#endif
	return -1;
}

/*----------------------------------------------------------------------------*\
 |				 thread_wait()				      |
\*----------------------------------------------------------------------------*/
int thread_wait(thread_t *thread)
{
#if defined(LINUX) || defined(OS_X)
	return pthread_join(*thread, NULL) ? -1 : 1;
#elif defined(WINDOWS)
	return WaitForSingleObject(*thread, INFINITE) == WAIT_FAILED ? -1 : 1;
#endif
}

/*----------------------------------------------------------------------------*\
 |				  mutex_init()				      |
\*----------------------------------------------------------------------------*/
int mutex_init(mutex_t *mutex)
{
#if defined(LINUX) || defined(OS_X)
	return pthread_mutex_init(mutex, NULL) ? -1 : 1;
#elif defined(WINDOWS)
	InitializeCriticalSection(mutex);
	return 1;
#endif
}

/*----------------------------------------------------------------------------*\
 |				mutex_try_lock()			      |
\*----------------------------------------------------------------------------*/
int mutex_try_lock(mutex_t *mutex)
{
#if defined(LINUX) || defined(OS_X)
	switch (pthread_mutex_trylock(mutex))
	{
		default    : return -1;
		case EBUSY : return  0;
		case 0     : return  1;
	}
#elif defined(WINDOWS)
	return !TryEnterCriticalSection(mutex) ? 0 : 1;
#endif
}

/*----------------------------------------------------------------------------*\
 |				  mutex_lock()				      |
\*----------------------------------------------------------------------------*/
int mutex_lock(mutex_t *mutex)
{
#if defined(LINUX) || defined(OS_X)
	return pthread_mutex_lock(mutex) ? -1 : 1;
#elif defined(WINDOWS)
	EnterCriticalSection(mutex);
	return 1;
#endif
}

/*----------------------------------------------------------------------------*\
 |				 mutex_unlock()				      |
\*----------------------------------------------------------------------------*/
int mutex_unlock(mutex_t *mutex)
{
#if defined(LINUX) || defined(OS_X)
	return pthread_mutex_unlock(mutex) ? -1 : 1;
#elif defined(WINDOWS)
	LeaveCriticalSection(mutex);
	return 1;
#endif
}

/*----------------------------------------------------------------------------*\
 |				mutex_destroy()				      |
\*----------------------------------------------------------------------------*/
int mutex_destroy(mutex_t *mutex)
{
#if defined(LINUX) || defined(OS_X)
	return pthread_mutex_destroy(mutex) ? -1 : 1;
#elif defined(WINDOWS)
	DeleteCriticalSection(mutex);
	return 1;
#endif
}

/*----------------------------------------------------------------------------*\
 |				  cond_init()				      |
\*----------------------------------------------------------------------------*/
int cond_init(cond_t *cond, void *attr)
{
#if defined(LINUX) || defined(OS_X)
	return pthread_cond_init(cond, (pthread_condattr_t *) attr) ? -1 : 1;
#elif defined(WINDOWS)
	cond->count = 0;
	cond->bcast = 0;
	if ((cond->sema = CreateSemaphore(NULL, 0, 0x7FFFFFFF, NULL)) == NULL)
		return -1;
	InitializeCriticalSection(&cond->lock);
	if ((cond->done = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL)
		return -1;
	return 1;
#endif
}

/*----------------------------------------------------------------------------*\
 |				  cond_wait()				      |
\*----------------------------------------------------------------------------*/
int cond_wait(cond_t *cond, mutex_t *mutex)
{
#if defined(LINUX) || defined(OS_X)
	return pthread_cond_wait(cond, mutex) ? -1 : 1;
#elif defined(WINDOWS)
	// Avoid race conditions.
	EnterCriticalSection(&cond->lock);
	cond->count++;
	LeaveCriticalSection(&cond->lock);

	// This call atomically releases the mutex and waits on the
	// semaphore until <pthread_cond_signal> or <pthread_cond_broadcast>
	// are called by another thread.
	if (SignalObjectAndWait(*mutex, cond->sema, INFINITE, FALSE) == WAIT_FAILED)
		return -1;

	// Reacquire lock to avoid race conditions.
	EnterCriticalSection(&cond->lock);

	// We're no longer waiting...
	cond->count--;

	// Check to see if we're the last waiter after <pthread_cond_broadcast>.
	int last_waiter = cond->bcast && cond->count == 0;

	LeaveCriticalSection(&cond->lock);

	// If we're the last waiter thread during this particular broadcast
	// then let all the other threads proceed.
	if (last_waiter)
	{
		// Call atomically signals the <waiters_done> event and waits until
		// it can acquire the <external_mutex>.  Required to ensure fairness. 
		if (SignalObjectAndWait (cond->done, *mutex, INFINITE, FALSE) == WAIT_FAILED)
			return -1;
	}
	else
		// Always regain the external mutex since that's the guarantee we
		// give to our callers. 
		if (WaitForSingleObject(*mutex) == WAIT_FAILED)
			return -1;
	return 1;
#endif
}

/*----------------------------------------------------------------------------*\
 |				 cond_signal()				      |
\*----------------------------------------------------------------------------*/
int cond_signal(cond_t *cond)
{
#if defined(LINUX) || defined(OS_X)
	return pthread_cond_signal(cond) ? -1 : 1;
#elif defined(WINDOWS)
	EnterCriticalSection(&cond->lock);
	int have_waiters = cond->count > 0;
	LeaveCriticalSection(&cond->lock);
	if (have_waiters)
		if (!ReleaseSemaphore(cond->sema, 1, 0))
			return -1;
	return 1;
#endif
}

/*----------------------------------------------------------------------------*\
 |				cond_broadcast()			      |
\*----------------------------------------------------------------------------*/
int cond_broadcast(cond_t *cond)
{
#if defined(LINUX) || defined(OS_X)
	return pthread_cond_broadcast(cond) ? -1 : 1;
#elif defined(WINDOWS)
	// This is needed to ensure that <waiters_count_> and <was_broadcast_> are
	// consistent relative to each other.
	EnterCriticalSection(&cond->lock);
	int have_waiters = 0;

	if (cond->count > 0)
	{
		// We are broadcasting, even if there is just one waiter...
		// Record that we are broadcasting, which helps optimize
		// <pthread_cond_wait> for the non-broadcast case.
		cond->bcast = 1;
		have_waiters = 1;
	}

	if (have_waiters)
		// Wake up all the waiters atomically.
		if (!ReleaseSemaphore(cond->sema, cond->count, 0))
			return -1;

	LeaveCriticalSection(&cond->lock);

	if (have_waiters)
		// Wait for all the awakened threads to acquire the counting
		// semaphore. 
		if (WaitForSingleObject(cond->done, INFINITE) == WAIT_FAILED)
			return -1;
		// This assignment is okay, even without the <waiters_count_lock_> held 
		// because no other waiter threads can wake up to access it.
		cond->bcast = 0;
	}
	return 1;
#endif
}

/*----------------------------------------------------------------------------*\
 |				 cond_destroy()				      |
\*----------------------------------------------------------------------------*/
int cond_destroy(cond_t *cond)
{
#if defined(LINUX) || defined(OS_X)
	return pthread_cond_destroy(cond) ? -1 : 1;
#elif defined(WINDOWS)
	if (!CloseHandle(cond->done))
		return -1;
	DeleteCriticalSection(&cond->lock);
	return 1;
#endif
}
