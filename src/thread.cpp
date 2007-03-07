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
	return pthread_create(thread, NULL, entry, arg) ? CRITICAL : SUCCESSFUL;
#elif defined(WINDOWS)
	return (*thread = CreateThread(NULL, 0, entry, 0, NULL)) == NULL ? CRITICAL : SUCCESSFUL;
#endif
}

/*----------------------------------------------------------------------------*\
 |				 thread_exit()				      |
\*----------------------------------------------------------------------------*/
int thread_exit()
{
#if defined(LINUX) || defined(OS_X)
	pthread_exit(NULL);
#elif defined(WINDOWS) && !defined(__cplusplus)
	ExitThread(0);
#endif
	return CRITICAL;
}

/*----------------------------------------------------------------------------*\
 |				 thread_wait()				      |
\*----------------------------------------------------------------------------*/
int thread_wait(thread_t *thread)
{
#if defined(LINUX) || defined(OS_X)
	return pthread_join(*thread, NULL) ? CRITICAL : SUCCESSFUL;
#elif defined(WINDOWS)
	return WaitForSingleObject(*thread, INFINITE) == WAIT_FAILED ? CRITICAL : SUCCESSFUL;
#endif
}

/*----------------------------------------------------------------------------*\
 |				  mutex_init()				      |
\*----------------------------------------------------------------------------*/
int mutex_init(mutex_t *mutex)
{
#if defined(LINUX) || defined(OS_X)
	return pthread_mutex_init(mutex, NULL) ? CRITICAL : SUCCESSFUL;
#elif defined(WINDOWS)
	InitializeCriticalSection(mutex);
	return SUCCESSFUL;
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
		default    : return CRITICAL;
		case EBUSY : return NON_CRITICAL;
		case 0     : return SUCCESSFUL;
	}
#elif defined(WINDOWS)
	return !TryEnterCriticalSection(mutex) ? NON_CRITICAL : SUCCESSFUL;
#endif
}

/*----------------------------------------------------------------------------*\
 |				  mutex_lock()				      |
\*----------------------------------------------------------------------------*/
int mutex_lock(mutex_t *mutex)
{
#if defined(LINUX) || defined(OS_X)
	return pthread_mutex_lock(mutex) ? CRITICAL : SUCCESSFUL;
#elif defined(WINDOWS)
	EnterCriticalSection(mutex);
	return SUCCESSFUL;
#endif
}

/*----------------------------------------------------------------------------*\
 |				 mutex_unlock()				      |
\*----------------------------------------------------------------------------*/
int mutex_unlock(mutex_t *mutex)
{
#if defined(LINUX) || defined(OS_X)
	return pthread_mutex_unlock(mutex) ? CRITICAL : SUCCESSFUL;
#elif defined(WINDOWS)
	LeaveCriticalSection(mutex);
	return SUCCESSFUL;
#endif
}

/*----------------------------------------------------------------------------*\
 |				mutex_destroy()				      |
\*----------------------------------------------------------------------------*/
int mutex_destroy(mutex_t *mutex)
{
#if defined(LINUX) || defined(OS_X)
	return pthread_mutex_destroy(mutex) ? CRITICAL : SUCCESSFUL;
#elif defined(WINDOWS)
	DeleteCriticalSection(mutex);
	return SUCCESSFUL;
#endif
}

/*----------------------------------------------------------------------------*\
 |				  cond_init()				      |
\*----------------------------------------------------------------------------*/
int cond_init(cond_t *cond, void *attr)
{
#if defined(LINUX) || defined(OS_X)
	return pthread_cond_init(cond, (pthread_condattr_t *) attr) ? CRITICAL : SUCCESSFUL;
#elif defined(WINDOWS)
	InitializeCriticalSection(&cond->lock);
	cond->count = 0;
	if ((cond->sema = CreateSemaphore(NULL, 0, 0x7FFFFFFF, NULL)) == NULL)
		return CRITICAL;
	if ((cond->done = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL)
		return CRITICAL;
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
	return pthread_cond_wait(cond, mutex) ? CRITICAL : SUCCESSFUL;
#elif defined(WINDOWS)
	EnterCriticalSection(&cond->lock);
	cond->count++;
	LeaveCriticalSection(&cond->lock);

	if (SignalObjectAndWait(*mutex, cond->sema, INFINITE, FALSE) == WAIT_FAILED)
		return CRITICAL;

	EnterCriticalSection(&cond->lock);
	BOOL waiter = --cond->count || !cond->bcast;
	LeaveCriticalSection(&cond->lock);

	if (!waiter && SignalObjectAndWait(cond->done, *mutex, INFINITE, FALSE) == WAIT_FAILED)
		return CRITICAL;
	if (waiter && WaitForSingleObject(*mutex) == WAIT_FAILED)
		return CRITICAL;
	return SUCCESSFUL;
#endif
}

/*----------------------------------------------------------------------------*\
 |				 cond_signal()				      |
\*----------------------------------------------------------------------------*/
int cond_signal(cond_t *cond)
{
#if defined(LINUX) || defined(OS_X)
	return pthread_cond_signal(cond) ? CRITICAL : SUCCESSFUL;
#elif defined(WINDOWS)
	EnterCriticalSection(&cond->lock);
	BOOL waiter = cond->count;
	LeaveCriticalSection(&cond->lock);

	if (waiter && !ReleaseSemaphore(cond->sema, 1, 0))
		return CRITICAL;
	return SUCCESSFUL;
#endif
}

/*----------------------------------------------------------------------------*\
 |				cond_broadcast()			      |
\*----------------------------------------------------------------------------*/
int cond_broadcast(cond_t *cond)
{
#if defined(LINUX) || defined(OS_X)
	return pthread_cond_broadcast(cond) ? CRITICAL : SUCCESSFUL;
#elif defined(WINDOWS)
	BOOL waiter = FALSE;

	EnterCriticalSection(&cond->lock);
	if (cond->count)
	{
		if (!ReleaseSemaphore(cond->sema, cond->count, 0))
			return CRITICAL;
		cond->bcast = TRUE;
		waiter = TRUE;
	}
	LeaveCriticalSection(&cond->lock);

	if (waiter)
	{
		if (WaitForSingleObject(cond->done, INFINITE) == WAIT_FAILED)
			return CRITICAL;
		cond->bcast = FALSE;
	}
	return SUCCESSFUL;
#endif
}

/*----------------------------------------------------------------------------*\
 |				 cond_destroy()				      |
\*----------------------------------------------------------------------------*/
int cond_destroy(cond_t *cond)
{
#if defined(LINUX) || defined(OS_X)
	return pthread_cond_destroy(cond) ? CRITICAL : SUCCESSFUL;
#elif defined(WINDOWS)
	if (!CloseHandle(cond->done))
		return CRITICAL;
	if (!CloseHandle(cond->sema))
		return CRITICAL;
	DeleteCriticalSection(&cond->lock);
	return SUCCESSFUL;
#endif
}
