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
	return pthread_create(thread, NULL, entry, arg) != 0 ? -1 : 1;
#elif defined(WINDOWS)
	return (*thread = CreateThread(NULL, 0, entry, 0, NULL)) == NULL ? -1 : 1;
#endif
}

/*----------------------------------------------------------------------------*\
 |				 thread_exit()				      |
\*----------------------------------------------------------------------------*/
void thread_exit()
{
#if defined(LINUX) || defined(OS_X)
	pthread_exit(NULL);
#elif defined(WINDOWS)
	ExitThread(0);
#endif
}

/*----------------------------------------------------------------------------*\
 |				 thread_wait()				      |
\*----------------------------------------------------------------------------*/
int thread_wait(thread_t *thread)
{
#if defined(LINUX) || defined(OS_X)
	return pthread_join(*thread, NULL) != 0 ? -1 : 1;
#elif defined(WINDOWS)
	return WaitForSingleObject(*thread, INFINITE) == WAIT_FAILED ? -1 : 1;
#endif
}

/*----------------------------------------------------------------------------*\
 |				  mutex_init()				      |
\*----------------------------------------------------------------------------*/
int mutex_init(mutex_t *m)
{
#if defined(LINUX) || defined(OS_X)
	pthread_mutex_init(m, NULL);
#elif defined(WINDOWS)
	InitializeCriticalSection(m);
#endif
	return 1;
}

/*----------------------------------------------------------------------------*\
 |				  mutex_lock()				      |
\*----------------------------------------------------------------------------*/
int mutex_lock(mutex_t *m)
{
#if defined(LINUX) || defined(OS_X)
	pthread_mutex_lock(m);
#elif defined(WINDOWS)
	EnterCriticalSection(m);
#endif
	return 1;
}

/*----------------------------------------------------------------------------*\
 |				 mutex_unlock()				      |
\*----------------------------------------------------------------------------*/
int mutex_unlock(mutex_t *m)
{
#if defined(LINUX) || defined(OS_X)
	pthread_mutex_unlock(m);
#elif defined(WINDOWS)
	LeaveCriticalSection(m);
#endif
	return 1;
}

/*----------------------------------------------------------------------------*\
 |				mutex_try_lock()			      |
\*----------------------------------------------------------------------------*/
int mutex_try_lock(mutex_t *m)
{
#if defined(LINUX) || defined(OS_X)
	return pthread_mutex_trylock(m) == EBUSY ? 0 : 1;
#elif defined(WINDOWS)
	return !TryEnterCriticalSection(m) ? 0 : 1;
#endif
}

/*----------------------------------------------------------------------------*\
 |				mutex_destroy()				      |
\*----------------------------------------------------------------------------*/
int mutex_destroy(mutex_t *m)
{
#if defined(LINUX) || defined(OS_X)
	pthread_mutex_destroy(m);
#elif defined(WINDOWS)
	DeleteCriticalSection(m);
#endif
	return 1;
}

/*----------------------------------------------------------------------------*\
 |				  cond_init()				      |
\*----------------------------------------------------------------------------*/
int cond_init(pthread_cond_t *cv, void *attr)
{
#if defined(LINUX) || defined(OS_X)
	pthread_cond_init(cv, (pthread_condattr_t *) attr);
#elif defined(WINDOWS)
	cv->waiters_count = 0;
	cv->was_broadcast = 0;
	cv->sem = CreatedSemaphore(NULL, 0, 0x7fffffff, NULL);
	InitializeCriticalSection(&cv->waiters_count_lock);
	cv->waiters_done = CreateEvent(NULL, FALSE, FALSE, NULL);
#endif
	return 1;
}

/*----------------------------------------------------------------------------*\
 |				  cond_wait()				      |
\*----------------------------------------------------------------------------*/
int cond_wait(cond_t *cv, mutex_t *m)
{
#if defined(LINUX) || defined(OS_X)
	pthread_cond_wait(cv, m);
#elif defined(WINDOWS)
	// Avoid race conditions.
	EnterCriticalSection(&cv->waiters_count_lock);
	cv->waiters_count++;
	LeaveCriticalSection(&cv->waiters_count_lock);

	// This call atomically releases the mutex and waits on the
	// semaphore until <pthread_cond_signal> or <pthread_cond_broadcast>
	// are called by another thread.
	SignalObjectAndWait(*m, cv->sem, INFINITE, FALSE);

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
	}
	else
	{
		// Always regain the external mutex since that's the guarantee we
		// give to our callers. 
		WaitForSingleObject(*m);
	}
#endif
	return 1;
}

/*----------------------------------------------------------------------------*\
 |				 cond_signal()				      |
\*----------------------------------------------------------------------------*/
int cond_signal(cond_t *cv)
{
#if defined(LINUX) || defined(OS_X)
	pthread_cond_signal(cv);
#elif defined(WINDOWS)
	EnterCriticalSection(&cv->waiters_count_lock);
	int have_waiters = cv->waiters_count > 0;
	LeaveCriticalSection(&cv->waiters_count_lock);
	if (have_waiters)
		ReleaseSemaphore(cv->sem, 1, 0);
#endif
	return 1;
}

/*----------------------------------------------------------------------------*\
 |				cond_broadcast()			      |
\*----------------------------------------------------------------------------*/
int cond_broadcast(cond_t *cv)
{
#if defined(LINUX) || defined(OS_X)
	pthread_cond_broadcast(cv);
#elif defined(WINDOWS)
	// This is needed to ensure that <waiters_count_> and <was_broadcast_> are
	// consistent relative to each other.
	EnterCriticalSection(&cv->waiters_count_lock);
	int have_waiters = 0;

	if (cv->waiters_count > 0)
	{
		// We are broadcasting, even if there is just one waiter...
		// Record that we are broadcasting, which helps optimize
		// <pthread_cond_wait> for the non-broadcast case.
		cv->was_broadcast = 1;
		have_waiters = 1;
	}

	if (have_waiters)
	{
		// Wake up all the waiters atomically.
		ReleaseSemaphore(cv->sem, cv->waiters_count, 0);

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
#endif
	return 1;
}

/*----------------------------------------------------------------------------*\
 |				 cond_destroy()				      |
\*----------------------------------------------------------------------------*/
int cond_destroy(pthread_cond_t *cv)
{
#if defined(LINUX) || defined(OS_X)
	pthread_cond_destroy(cv);
#elif defined(WINDOWS)
#endif
	return 1;
}
