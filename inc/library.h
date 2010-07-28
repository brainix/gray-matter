/*----------------------------------------------------------------------------*\
 |  library.h - cross-platform library interface                              |
 |                                                                            |
 |  Copyright © 2005-2008, The Gray Matter Team, original authors.            |
\*----------------------------------------------------------------------------*/

/*
 | This program is free software: you can redistribute it and/or modify it under
 | the terms of the GNU General Public License as published by the Free Software
 | Foundation, either version 3 of the License, or (at your option) any later
 | version.
 |
 | This program is distributed in the hope that it will be useful, but WITHOUT
 | ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 | FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 | details.
 |
 | You should have received a copy of the GNU General Public License along with
 | this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 | This is our cross-platform library.  All of these data types and functions
 | are defined twice: once for POSIX, again for Win32.
 |
 | This interface file and its corresponding implementation file were originally
 | ripped off from Doug Taylor's excellent libMetric.  You can find it here:
 | http://jdtaylor.org/  Hopefully, we'll do some meaningful work Doug will be
 | able to merge upstream.
 |
 | Throughout this endeavor, I've learned programming for Windows is akin to
 | building a Ferrari out of Duplo.  Sadly, the world is populated with toddlers
 | who have stubby fingers too clumsy to manipulate Lego.  I just wish that
 | knucklehead Steve Ballmer could stop throwing chairs long enough to make his
 | thrice-damned operating system POSIX conformant like every other operating
 | system on this planet.  I hate myself.
 */

#ifndef LIBRARY_H
#define LIBRARY_H

// For the functions defined here, these are the standard return values:
#define CRITICAL        -1 // Critical failure.      :-(
#define NON_CRITICAL     0 // Non-critical failure.  :-|
#define SUCCESSFUL       1 // Success.               :-)

#if defined(LINUX) || defined(OS_X)

// POSIX header files:
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/time.h>

// POSIX data types:
typedef pthread_t thread_t;          // Thread.
typedef void *(*entry_t)(void *arg); // Entry point.
typedef pthread_mutex_t mutex_t;     // Mutex.
typedef pthread_cond_t cond_t;       // Condition variable.

#elif defined(_MINGW_WINDOWS)

// Win32 header files:
#ifdef _MSDEV_WINDOWS
#include "MSDEVstdint.h"
#else
#include <stdint.h>
#endif
#include <windows.h>

// Win32 data types:
typedef HANDLE thread_t;              // Thread.
typedef DWORD (*entry_t)(LPVOID arg); // Entry point.
typedef HANDLE mutex_t;               // Mutex.
typedef struct                        // Condition variable.
{
    CRITICAL_SECTION lock; ///< Lock to protect count, bcast.
    int count;             ///< Number of waiting threads.
    HANDLE sema;           ///< Queue of waiting threads.
    HANDLE done;           ///< Whether all waiting threads have woken up.
    BOOL bcast;            ///< TRUE if broadcasting, FALSE if signaling.
} cond_t;

#endif


class Library
{
  public:
  //this method helps look up bits
  static const int MultiplyDeBruijnBitPosition[32];

  //Windows needs a variable to hold the time per move
  //since it seems to get lost/corrupted in the current
  //threading model
  static unsigned int timePerMove;

  // Function prototypes related to threads:
  static int thread_create(thread_t *thread, entry_t entry, void *arg);
  static int thread_wait(thread_t *thread);
  static int thread_destroy(thread_t *thread);

  // Function prototypes related to mutexes:
  static int mutex_create(mutex_t *mutex);
  static int mutex_try_lock(mutex_t *mutex);
  static int mutex_lock(mutex_t *mutex);
  static int mutex_unlock(mutex_t *mutex);
  static int mutex_destroy(mutex_t *mutex);

  // Function prototypes related to condition variables:
  static int cond_create(cond_t *cond, void *attr);
  static int cond_wait(cond_t *cond, mutex_t *mutex);
  static int cond_signal(cond_t *cond);
  static int cond_broadcast(cond_t *cond);
  static int cond_destroy(cond_t *cond);

  // Function prototypes related to timers:
  static int timer_function(void (*function)(void *), void *data);
  static int timer_set(int csec);
  static int timer_cancel();

  // Function prototypes related to 64-bit pseudo-random number generation and
  // bitwise operations:
  static uint64_t rand_64();
  static int count_64(uint64_t n);
  static int find_64(uint64_t n);
  static int find_32(uint32_t n);

  // Filesystem utilities
  static char* get_home_directory();

  static void (*callback)(void*);
  static void *callback_data;
};

#if defined(LINUX) || defined(OS_X)
  void timer_handler(int num);
#endif
#if defined(_MINGW_WINDOWS)
  unsigned long timer_handler(LPVOID arg);
#endif

#endif
