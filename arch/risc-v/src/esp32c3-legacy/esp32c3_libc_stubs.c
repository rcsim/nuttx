/****************************************************************************
 * arch/risc-v/src/esp32c3-legacy/esp32c3_libc_stubs.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdio.h>

#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include <nuttx/signal.h>
#include <nuttx/mutex.h>
#include <nuttx/lib/lib.h>

#include "rom/esp32c3_libc_stubs.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define _lock_t int

/****************************************************************************
 * Private Types
 ****************************************************************************/

static mutex_t g_nxlock_common;
static mutex_t g_nxlock_recursive;

/* Forward declaration */

struct _reent;

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int _close_r(struct _reent *r, int fd)
{
  return nx_close(fd);
}

int _fstat_r(struct _reent *r, int fd, struct stat *statbuf)
{
  return nx_fstat(fd, statbuf);
}

int _getpid_r(struct _reent *r)
{
  return nxsched_getpid();
}

int _kill_r(struct _reent *r, int pid, int sig)
{
  return nxsig_kill(pid, sig);
}

int _link_r(struct _reent *r, const char *oldpath, const char *newpath)
{
  /* TODO */

  return 0;
}

int _lseek_r(struct _reent *r, int fd, int offset, int whence)
{
  return nx_seek(fd, offset, whence);
}

int _open_r(struct _reent *r, const char *pathname, int flags, int mode)
{
  return nx_open(pathname, flags, mode);
}

int _read_r(struct _reent *r, int fd, void *buf, int count)
{
  return nx_read(fd, buf, count);
}

int _rename_r(struct _reent *r, const char *oldpath, const char *newpath)
{
  return rename(oldpath, newpath);
}

void *_sbrk_r(struct _reent *r, ptrdiff_t increment)
{
  /* TODO: sbrk is only supported on Kernel mode */

  errno = -ENOMEM;
  return (void *)-1;
}

int _stat_r(struct _reent *r, const char *pathname, struct stat *statbuf)
{
  return nx_stat(pathname, statbuf, 1);
}

clock_t _times_r(struct _reent *r, struct tms *buf)
{
  return times(buf);
}

int _unlink_r(struct _reent *r, const char *pathname)
{
  return nx_unlink(pathname);
}

int _write_r(struct _reent *r, int fd, const void *buf, int count)
{
  return nx_write(fd, buf, count);
}

int _gettimeofday_r(struct _reent *r, struct timeval *tv, void *tz)
{
  return gettimeofday(tv, tz);
}

void *_malloc_r(struct _reent *r, size_t size)
{
  return lib_malloc(size);
}

void *_realloc_r(struct _reent *r, void *ptr, size_t size)
{
  return lib_realloc(ptr, size);
}

void *_calloc_r(struct _reent *r, size_t nmemb, size_t size)
{
  return lib_calloc(nmemb, size);
}

void _free_r(struct _reent *r, void *ptr)
{
  lib_free(ptr);
}

void _abort(void)
{
  abort();
}

void _raise_r(struct _reent *r)
{
  /* FIXME */
}

void _lock_init(_lock_t *lock)
{
  nxmutex_init(&g_nxlock_common);
  nxsem_get_value(&g_nxlock_common.sem, lock);
}

void _lock_init_recursive(_lock_t *lock)
{
  nxmutex_init(&g_nxlock_recursive);
  nxsem_get_value(&g_nxlock_recursive.sem, lock);
}

void _lock_close(_lock_t *lock)
{
  nxmutex_destroy(&g_nxlock_common);
  *lock = 0;
}

void _lock_close_recursive(_lock_t *lock)
{
  nxmutex_destroy(&g_nxlock_recursive);
  *lock = 0;
}

void _lock_acquire(_lock_t *lock)
{
  nxmutex_lock(&g_nxlock_common);
  nxsem_get_value(&g_nxlock_common.sem, lock);
}

void _lock_acquire_recursive(_lock_t *lock)
{
  nxmutex_lock(&g_nxlock_recursive);
  nxsem_get_value(&g_nxlock_recursive.sem, lock);
}

int _lock_try_acquire(_lock_t *lock)
{
  nxmutex_trylock(&g_nxlock_common);
  nxsem_get_value(&g_nxlock_common.sem, lock);
  return 0;
}

int _lock_try_acquire_recursive(_lock_t *lock)
{
  nxmutex_trylock(&g_nxlock_recursive);
  nxsem_get_value(&g_nxlock_recursive.sem, lock);
  return 0;
}

void _lock_release(_lock_t *lock)
{
  nxmutex_unlock(&g_nxlock_common);
  nxsem_get_value(&g_nxlock_common.sem, lock);
}

void _lock_release_recursive(_lock_t *lock)
{
  nxmutex_unlock(&g_nxlock_recursive);
  nxsem_get_value(&g_nxlock_recursive.sem, lock);
}

struct _reent *__getreent(void)
{
  /* TODO */

  return (struct _reent *)NULL;
}

int _system_r(struct _reent *r, const char *command)
{
  /* TODO: Implement system() */

  return 0;
}

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const struct syscall_stub_table g_stub_table =
{
  .__getreent = &__getreent,
  ._malloc_r = &_malloc_r,
  ._free_r = &_free_r,
  ._realloc_r = &_realloc_r,
  ._calloc_r = &_calloc_r,
  ._abort = &_abort,
  ._system_r = &_system_r,
  ._rename_r = &_rename_r,
  ._times_r = &_times_r,
  ._gettimeofday_r = &_gettimeofday_r,
  ._raise_r = &_raise_r,
  ._unlink_r = &_unlink_r,
  ._link_r = &_link_r,
  ._stat_r = &_stat_r,
  ._fstat_r = &_fstat_r,
  ._sbrk_r = &_sbrk_r,
  ._getpid_r = &_getpid_r,
  ._kill_r = &_kill_r,
  ._exit_r = NULL,
  ._close_r = &_close_r,
  ._open_r = &_open_r,
  ._write_r = &_write_r,
  ._lseek_r = &_lseek_r,
  ._read_r = &_read_r,
  ._lock_init = &_lock_init,
  ._lock_init_recursive = &_lock_init_recursive,
  ._lock_close = &_lock_close,
  ._lock_close_recursive = &_lock_close_recursive,
  ._lock_acquire = &_lock_acquire,
  ._lock_acquire_recursive = &_lock_acquire_recursive,
  ._lock_try_acquire = &_lock_try_acquire,
  ._lock_try_acquire_recursive = &_lock_try_acquire_recursive,
  ._lock_release = &_lock_release,
  ._lock_release_recursive = &_lock_release_recursive,
  ._printf_float = NULL,
  ._scanf_float = NULL
};

/****************************************************************************
 * Name: setup_syscall_table
 *
 * Description:
 *   Configure the syscall table used by the ROM code for calling C library
 *   functions.
 *   ESP32-C3 ROM code contains implementations of some of C library
 *   functions. Whenever a function in ROM needs to use a syscall, it calls
 *   a pointer to the corresponding syscall implementation defined in the
 *   syscall_stub_table struct.
 *
 * Input Parameters:
 *   None.
 *
 * Returned Value:
 *   None.
 *
 ****************************************************************************/

void setup_syscall_table(void)
{
  syscall_table_ptr = &g_stub_table;
}

