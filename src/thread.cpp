/*----------------------------------------------------------------------------*\
 |	thread.cpp - cross-platform multithreading library implementation     |
 |									      |
 |	Copyright � 2005-2007, The Gray Matter Team, original authors.	      |
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

#include <stdio.h>
#include "thread.h"

/*----------------------------------------------------------------------------*\
 |				thread_create()				      |
\*----------------------------------------------------------------------------*/
int thread_create(thread_t *thread, entry_t entry, void *arg)
{
#if defined(LINUX) || defined(OS_X)
	return pthread_create(thread, NULL, entry, arg) ? CRITICAL : SUCCESSFUL;
#elif defined(WINDOWS)
	return (*thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) entry, arg, 0, NULL)) == NULL ? CRITICAL : SUCCESSFUL;
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
 |				 thread_terminate()				      |
\*----------------------------------------------------------------------------*/
int thread_terminate(thread_t *thread)
{
#if defined(LINUX) || defined(OS_X)
	return pthread_kill(*thread, SIGTERM) ? CRITICAL : SUCCESSFUL;
#elif defined(WINDOWS)
	if(!TerminateThread(thread, 0))
		return CRITICAL;
	*thread = INVALID_HANDLE_VALUE;
	return SUCCESSFUL;
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
	*mutex = CreateMutex(NULL, FALSE, NULL);
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
	return WaitForSingleObject(*mutex, 0) == WAIT_TIMEOUT ? 0 : 1;
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
	return WaitForSingleObject(*mutex, INFINITE) == WAIT_FAILED ? CRITICAL : SUCCESSFUL;
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
	return ReleaseMutex(*mutex) ? SUCCESSFUL : CRITICAL;
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
	return CloseHandle(*mutex) ? SUCCESSFUL : CRITICAL;
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
	if (waiter && WaitForSingleObject(*mutex, INFINITE) == WAIT_FAILED)
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

/* Global variables: */
void (*callback)();
#if defined(WINDOWS)
thread_t timer_thread = INVALID_HANDLE_VALUE;
#endif

/*----------------------------------------------------------------------------*\
 |				timer_handler()				      |
\*----------------------------------------------------------------------------*/
#if defined(LINUX) || defined(OS_X)
void timer_handler(int num)
{

/* The alarm has sounded.  Call the previously specified function. */

	(*callback)();
}

#elif defined(WINDOWS)
DWORD timer_handler(LPVOID arg)
{
	unsigned long long ms = *((unsigned int*)arg); /* number of ms to wait */
	HANDLE timer_id = INVALID_HANDLE_VALUE;

	if ((timer_id = CreateWaitableTimer(NULL, TRUE, NULL)) == NULL)
		goto exit_timer_handler;

	LARGE_INTEGER relTime;
	/* negative means relative time in intervals of 100 nanoseconds */
	relTime.QuadPart = -(ms * 100000000L); 
	if (!SetWaitableTimer(timer_id, &relTime, 0, NULL, NULL, FALSE))
		goto exit_timer_handler;

	if (WaitForSingleObject(timer_id, INFINITE))
		goto exit_timer_handler;

	(*callback)();

exit_timer_handler:
	if (timer_id != INVALID_HANDLE_VALUE)
		CloseHandle(timer_id);
	timer_thread = INVALID_HANDLE_VALUE;
	thread_exit();
	return 0;
}
#endif

/*----------------------------------------------------------------------------*\
 |				timer_function()			      |
\*----------------------------------------------------------------------------*/
int timer_function(void (*function)())
{

/* Specify the function to be called once the alarm has sounded. */

#if defined(LINUX) || defined(OS_X)
	signal(SIGALRM, timer_handler);
#endif
	callback = function;
	return SUCCESSFUL;
}

/*----------------------------------------------------------------------------*\
 |				  timer_set()				      |
\*----------------------------------------------------------------------------*/
int timer_set(int sec)
{

/* Set the alarm to sound after the specified number of seconds. */

#if defined(LINUX) || defined(OS_X)
	struct itimerval itimerval;
	itimerval.it_interval.tv_sec = 0;
	itimerval.it_interval.tv_usec = 0;
	itimerval.it_value.tv_sec = sec;
	itimerval.it_value.tv_usec = 0;
	return setitimer(ITIMER_REAL, &itimerval, NULL) == -1 ? CRITICAL : SUCCESSFUL;
#elif defined(WINDOWS)
	if (timer_thread != INVALID_HANDLE_VALUE)
		return CRITICAL; /* only allow one timer */

	unsigned int ms = sec * 1000;
	return thread_create(&timer_thread, (entry_t)timer_handler, &ms);
#endif
}

/*----------------------------------------------------------------------------*\
 |				 timer_cancel()				      |
\*----------------------------------------------------------------------------*/
int timer_cancel()
{

/* Cancel any pending alarm. */

#if defined(LINUX) || defined(OS_X)
	struct itimerval itimerval;
	itimerval.it_interval.tv_sec = 0;
	itimerval.it_interval.tv_usec = 0;
	itimerval.it_value.tv_sec = 0;
	itimerval.it_value.tv_usec = 0;
	return setitimer(ITIMER_REAL, &itimerval, NULL) == -1 ? CRITICAL : SUCCESSFUL;
#elif defined(WINDOWS)
	if (timer_thread == INVALID_HANDLE_VALUE)
		return CRITICAL;
	return thread_terminate(&timer_thread);
#endif
}
