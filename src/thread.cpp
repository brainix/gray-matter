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
	InitializeCriticalSection(&cond->lock);
	cond->count = 0;
	if ((cond->sema = CreateSemaphore(NULL, 0, 0x7FFFFFFF, NULL)) == NULL)
		return -1;
	if ((cond->done = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL)
		return -1;
	cond->bcast = FALSE;
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
	EnterCriticalSection(&cond->lock);
	cond->count++;
	LeaveCriticalSection(&cond->lock);

	if (SignalObjectAndWait(*mutex, cond->sema, INFINITE, FALSE) == WAIT_FAILED)
		return -1;

	EnterCriticalSection(&cond->lock);
	BOOL waiters = --cond->count || !cond->bcast;
	LeaveCriticalSection(&cond->lock);

	if (!waiters && SignalObjectAndWait(cond->done, *mutex, INFINITE, FALSE) == WAIT_FAILED)
		return -1;
	if (waiters && WaitForSingleObject(*mutex) == WAIT_FAILED)
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
	BOOL waiters = cond->count;
	LeaveCriticalSection(&cond->lock);

	if (waiters && !ReleaseSemaphore(cond->sema, 1, 0))
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
	BOOL waiters = FALSE;

	EnterCriticalSection(&cond->lock);
	if (cond->count)
	{
		if (!ReleaseSemaphore(cond->sema, cond->count, 0))
			return -1;
		cond->bcast = TRUE;
		waiters = TRUE;
	}
	LeaveCriticalSection(&cond->lock);

	if (waiters)
	{
		if (WaitForSingleObject(cond->done, INFINITE) == WAIT_FAILED)
			return -1;
		cond->bcast = FALSE;
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
