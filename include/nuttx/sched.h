/****************************************************************************
 * include/nuttx/sched.h
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

#ifndef __INCLUDE_NUTTX_SCHED_H
#define __INCLUDE_NUTTX_SCHED_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/types.h>
#include <stdbool.h>
#include <stdint.h>
#include <sched.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>

#include <nuttx/addrenv.h>
#include <nuttx/clock.h>
#include <nuttx/irq.h>
#include <nuttx/mutex.h>
#include <nuttx/semaphore.h>
#include <nuttx/queue.h>
#include <nuttx/wdog.h>
#include <nuttx/fs/fs.h>
#include <nuttx/net/net.h>
#include <nuttx/mm/map.h>
#include <nuttx/tls.h>
#include <nuttx/spinlock_type.h>

#include <arch/arch.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Configuration ************************************************************/

/* We need to track group members at least for:
 *
 * - To signal all tasks in a group. (eg. SIGCHLD)
 * - _exit() to collect siblings threads.
 */

#undef HAVE_GROUP_MEMBERS
#if !defined(CONFIG_DISABLE_PTHREAD)
#  define HAVE_GROUP_MEMBERS  1
#endif

/* Sporadic scheduling */

#ifndef CONFIG_SCHED_SPORADIC_MAXREPL
#  define CONFIG_SCHED_SPORADIC_MAXREPL 3
#endif

/* Task Management Definitions **********************************************/

/* Special task IDS.  Any negative PID is invalid. */

#define IDLE_PROCESS_ID            (pid_t)0
#define INVALID_PROCESS_ID         (pid_t)-1

/* This is the maximum number of times that a lock can be set */

#define MAX_LOCK_COUNT             127

/* Values for the struct tcb_s flags bits */

#define TCB_FLAG_TTYPE_SHIFT       (0)                           /* Bits 0-1: thread type */
#define TCB_FLAG_TTYPE_MASK        (3 << TCB_FLAG_TTYPE_SHIFT)
#  define TCB_FLAG_TTYPE_TASK      (0 << TCB_FLAG_TTYPE_SHIFT)   /*   Normal user task */
#  define TCB_FLAG_TTYPE_PTHREAD   (1 << TCB_FLAG_TTYPE_SHIFT)   /*   User pthread */
#  define TCB_FLAG_TTYPE_KERNEL    (2 << TCB_FLAG_TTYPE_SHIFT)   /*   Kernel thread */
#define TCB_FLAG_POLICY_SHIFT      (3)                           /* Bit 3-4: Scheduling policy */
#define TCB_FLAG_POLICY_MASK       (3 << TCB_FLAG_POLICY_SHIFT)
#  define TCB_FLAG_SCHED_FIFO      (0 << TCB_FLAG_POLICY_SHIFT)  /* FIFO scheding policy */
#  define TCB_FLAG_SCHED_RR        (1 << TCB_FLAG_POLICY_SHIFT)  /* Round robin scheding policy */
#  define TCB_FLAG_SCHED_SPORADIC  (2 << TCB_FLAG_POLICY_SHIFT)  /* Sporadic scheding policy */
#define TCB_FLAG_CPU_LOCKED        (1 << 5)                      /* Bit 5: Locked to this CPU */
#define TCB_FLAG_SIGNAL_ACTION     (1 << 6)                      /* Bit 6: In a signal handler */
#define TCB_FLAG_SYSCALL           (1 << 7)                      /* Bit 7: In a system call */
#define TCB_FLAG_EXIT_PROCESSING   (1 << 8)                      /* Bit 8: Exiting */
#define TCB_FLAG_FREE_STACK        (1 << 9)                      /* Bit 9: Free stack after exit */
#define TCB_FLAG_HEAP_CHECK        (1 << 10)                     /* Bit 10: Heap check */
#define TCB_FLAG_HEAP_DUMP         (1 << 11)                     /* Bit 11: Heap dump */
#define TCB_FLAG_DETACHED          (1 << 12)                     /* Bit 12: Pthread detached */
#define TCB_FLAG_FORCED_CANCEL     (1 << 13)                     /* Bit 13: Pthread cancel is forced */
#define TCB_FLAG_JOIN_COMPLETED    (1 << 14)                     /* Bit 14: Pthread join completed */
#define TCB_FLAG_FREE_TCB          (1 << 15)                     /* Bit 15: Free tcb after exit */
#define TCB_FLAG_PREEMPT_SCHED     (1 << 16)                     /* Bit 16: tcb is PREEMPT_SCHED */

/* Values for struct task_group tg_flags */

#define GROUP_FLAG_NOCLDWAIT       (1 << 0)                      /* Bit 0: Do not retain child exit status */
#define GROUP_FLAG_PRIVILEGED      (1 << 1)                      /* Bit 1: Group is privileged */
#define GROUP_FLAG_DELETED         (1 << 2)                      /* Bit 2: Group has been deleted but not yet freed */
#define GROUP_FLAG_EXITING         (1 << 3)                      /* Bit 3: Group exit is in progress */
                                                                 /* Bits 3-7: Available */

/* Values for struct child_status_s ch_flags */

#define CHILD_FLAG_TTYPE_SHIFT     (0)                           /* Bits 0-1: child thread type */
#define CHILD_FLAG_TTYPE_MASK      (3 << CHILD_FLAG_TTYPE_SHIFT)
#  define CHILD_FLAG_TTYPE_TASK    (0 << CHILD_FLAG_TTYPE_SHIFT) /* Normal user task */
#  define CHILD_FLAG_TTYPE_PTHREAD (1 << CHILD_FLAG_TTYPE_SHIFT) /* User pthread */
#  define CHILD_FLAG_TTYPE_KERNEL  (2 << CHILD_FLAG_TTYPE_SHIFT) /* Kernel thread */
#define CHILD_FLAG_EXITED          (1 << 2)                      /* Bit 2: The child thread has exit'ed */
                                                                 /* Bits 3-7: Available */

/* Sporadic scheduler flags */

#define SPORADIC_FLAG_ALLOCED      (1 << 0)                      /* Bit 0: Timer is allocated */
#define SPORADIC_FLAG_MAIN         (1 << 1)                      /* Bit 1: The main timer */
#define SPORADIC_FLAG_REPLENISH    (1 << 2)                      /* Bit 2: Replenishment cycle */
                                                                 /* Bits 3-7: Available */

/* Most internal nxsched_* interfaces are not available in the user space in
 * PROTECTED and KERNEL builds.  In that context, the application semaphore
 * interfaces must be used.  The differences between the two sets of
 * interfaces are:  (1) the nxsched_* interfaces do not cause cancellation
 * points and (2) they do not modify the errno variable.
 *
 * This is only important when compiling libraries (libc or libnx) that are
 * used both by the OS (libkc.a and libknx.a) or by the applications
 * (libc.a and libnx.a).  In that case, the correct interface must be
 * used for the build context.
 *
 * REVISIT:  In the flat build, the same functions must be used both by
 * the OS and by applications.  We have to use the normal user functions
 * in this case or we will fail to set the errno or fail to create the
 * cancellation point.
 */

#if !defined(CONFIG_BUILD_FLAT) && defined(__KERNEL__)
#  define _SCHED_GETPARAM(t,p)       nxsched_get_param(t,p)
#  define _SCHED_SETPARAM(t,p)       nxsched_set_param(t,p)
#  define _SCHED_GETSCHEDULER(t)     nxsched_get_scheduler(t)
#  define _SCHED_SETSCHEDULER(t,s,p) nxsched_set_scheduler(t,s,p)
#  define _SCHED_GETAFFINITY(t,c,m)  nxsched_get_affinity(t,c,m)
#  define _SCHED_SETAFFINITY(t,c,m)  nxsched_set_affinity(t,c,m)
#  define _SCHED_ERRNO(r)            (-(r))
#  define _SCHED_ERRVAL(r)           (r)
#else
#  define _SCHED_GETPARAM(t,p)       sched_getparam(t,p)
#  define _SCHED_SETPARAM(t,p)       sched_setparam(t,p)
#  define _SCHED_GETSCHEDULER(t)     sched_getscheduler(t)
#  define _SCHED_SETSCHEDULER(t,s,p) sched_setscheduler(t,s,p)
#  define _SCHED_GETAFFINITY(t,c,m)  sched_getaffinity(t,c,m)
#  define _SCHED_SETAFFINITY(t,c,m)  sched_setaffinity(t,c,m)
#  define _SCHED_ERRNO(r)            errno
#  define _SCHED_ERRVAL(r)           (-errno)
#endif

#if defined(CONFIG_BUILD_FLAT) || defined(__KERNEL__)
#  define _SCHED_GETTID()            nxsched_gettid()
#  define _SCHED_GETPID()            nxsched_getpid()
#  define _SCHED_GETPPID()           nxsched_getppid()
#else
#  define _SCHED_GETTID()            gettid()
#  define _SCHED_GETPID()            getpid()
#  define _SCHED_GETPPID()           getppid()
#endif

#define TCB_PID_OFF                  offsetof(struct tcb_s, pid)
#define TCB_STATE_OFF                offsetof(struct tcb_s, task_state)
#define TCB_PRI_OFF                  offsetof(struct tcb_s, sched_priority)
#if CONFIG_TASK_NAME_SIZE > 0
#  define TCB_NAME_OFF               offsetof(struct tcb_s, name)
#else
#  define TCB_NAME_OFF               0
#endif
#define TCB_REGS_OFF                 offsetof(struct tcb_s, xcp.regs)
#define TCB_REG_OFF(reg)             (reg * sizeof(uintptr_t))
#define TCB_STACK_OFF                offsetof(struct tcb_s, stack_base_ptr)
#define TCB_STACK_SIZE_OFF           offsetof(struct tcb_s, adj_stack_size)

/* Get a pointer to the process' memory map struct from the task_group */

#define get_group_mm(group)          (group ? &group->tg_mm_map : NULL)

/* Get a pointer to current the process' memory map struct */

#define get_current_mm()             (get_group_mm(nxsched_self()->group))

/* Get task name from tcb */

#if CONFIG_TASK_NAME_SIZE > 0
#  define get_task_name(tcb)         ((tcb)->name)
#else
#  define get_task_name(tcb)         "<noname>"
#endif

#define SMP_CALL_INITIALIZER(func, arg) {(func), (arg)}

/* These are macros to access the current CPU and the current task on a CPU.
 * These macros are intended to support a future SMP implementation.
 */

#ifdef CONFIG_SMP
#  define this_cpu()                 up_this_cpu()
#else
#  define this_cpu()                 (0)
#endif

#define running_regs()               ((FAR void **)(g_running_tasks[this_cpu()]->xcp.regs))

/****************************************************************************
 * Public Type Definitions
 ****************************************************************************/

#ifndef __ASSEMBLY__

/* General Task Management Types ********************************************/

/* This is the type of the task_state field of the TCB. NOTE: the order and
 * content of this enumeration is critical since there are some OS tables
 * indexed by these values.
 * The range of values is assumed to fit into a uint8_t in struct tcb_s.
 */

enum tstate_e
{
  TSTATE_TASK_INVALID    = 0, /* INVALID      - The TCB is uninitialized */
  TSTATE_TASK_PENDING,        /* READY_TO_RUN - Pending preemption unlock */
  TSTATE_TASK_READYTORUN,     /* READY-TO-RUN - But not running */
#ifdef CONFIG_SMP
  TSTATE_TASK_ASSIGNED,       /* READY-TO-RUN - Not running, but assigned to a CPU */
#endif
  TSTATE_TASK_RUNNING,        /* READY_TO_RUN - And running */

  TSTATE_TASK_INACTIVE,       /* BLOCKED      - Initialized but not yet activated */
  TSTATE_WAIT_SEM,            /* BLOCKED      - Waiting for a semaphore */
  TSTATE_WAIT_SIG,            /* BLOCKED      - Waiting for a signal */
#if !defined(CONFIG_DISABLE_MQUEUE) || !defined(CONFIG_DISABLE_MQUEUE_SYSV)
  TSTATE_WAIT_MQNOTEMPTY,     /* BLOCKED      - Waiting for a MQ to become not empty. */
  TSTATE_WAIT_MQNOTFULL,      /* BLOCKED      - Waiting for a MQ to become not full. */
#endif
#ifdef CONFIG_LEGACY_PAGING
  TSTATE_WAIT_PAGEFILL,       /* BLOCKED      - Waiting for page fill */
#endif
#ifdef CONFIG_SIG_SIGSTOP_ACTION
  TSTATE_TASK_STOPPED,        /* BLOCKED      - Waiting for SIGCONT */
#endif

  NUM_TASK_STATES             /* Must be last */
};

typedef enum tstate_e tstate_t;

/* The following definitions are determined by tstate_t.  Ordering of values
 * in the enumeration is important!
 */

#define FIRST_READY_TO_RUN_STATE   TSTATE_TASK_READYTORUN
#define LAST_READY_TO_RUN_STATE    TSTATE_TASK_RUNNING
#define FIRST_ASSIGNED_STATE       TSTATE_TASK_ASSIGNED
#define LAST_ASSIGNED_STATE        TSTATE_TASK_RUNNING
#define FIRST_BLOCKED_STATE        TSTATE_TASK_INACTIVE
#define LAST_BLOCKED_STATE         (NUM_TASK_STATES-1)

/* The following is the form of a thread start-up function */

typedef CODE void (*start_t)(void);
typedef CODE void (*sig_deliver_t)(FAR struct tcb_s *tcb);

/* This is the entry point into the main thread of the task or into a created
 * pthread within the task.
 */

union entry_u
{
  pthread_startroutine_t pthread;
  main_t main;
};

typedef union entry_u entry_t;

/* struct sporadic_s ********************************************************/

#ifdef CONFIG_SCHED_SPORADIC

/* This structure represents one replenishment interval.  This is what is
 * received by each timeout handler.
 */

struct sporadic_s;
struct replenishment_s
{
  FAR struct tcb_s *tcb;            /* The parent TCB structure             */
  struct wdog_s timer;              /* Timer dedicated to this interval     */
  uint32_t budget;                  /* Current budget time                  */
  uint8_t  flags;                   /* See SPORADIC_FLAG_* definitions      */
};

/* This structure is an allocated "plug-in" to the main TCB structure.  It is
 * allocated when the sporadic scheduling policy is assigned to a thread.
 * Thus, in the context of numerous threads of varying policies, there the
 * overhead from this significant allocation is only borne by the threads
 * with the sporadic scheduling policy.
 */

struct sporadic_s
{
  bool      suspended;              /* Thread is currently suspended         */
  uint8_t   hi_priority;            /* Sporadic high priority                */
  uint8_t   low_priority;           /* Sporadic low priority                 */
  uint8_t   max_repl;               /* Maximum number of replenishments      */
  uint8_t   nrepls;                 /* Number of active replenishments       */
  uint32_t  repl_period;            /* Sporadic replenishment period         */
  uint32_t  budget;                 /* Sporadic execution budget period      */
  clock_t   eventtime;              /* Time thread suspended or [re-]started */

  /* This is the last interval timer activated */

  FAR struct replenishment_s *active;

  /* This is the list of replenishment interval timers */

  struct replenishment_s replenishments[CONFIG_SCHED_SPORADIC_MAXREPL];
};

#endif /* CONFIG_SCHED_SPORADIC */

/* struct child_status_s ****************************************************/

/* This structure is used to maintain information about child tasks.
 * pthreads work differently, they have join information.
 * This is only for child tasks.
 */

#ifdef CONFIG_SCHED_CHILD_STATUS
struct child_status_s
{
  FAR struct child_status_s *flink;

  uint8_t ch_flags;                 /* Child status:  See CHILD_FLAG_* defines */
  pid_t   ch_pid;                   /* Child task ID                           */
  int     ch_status;                /* Child exit status                       */
};
#endif

/* struct dspace_s **********************************************************/

/* This structure describes a reference counted D-Space region.
 * This must be a separately allocated "break-away" structure that
 * can be owned by a task and any pthreads created by the task.
 */

#ifdef CONFIG_PIC
struct dspace_s
{
  /* The life of the structure allocation is determined by this reference
   * count.  This count is number of threads that shared the same D-Space.
   * This includes the parent task as well as any pthreads created by the
   * parent task or any of its child threads.
   */

  uint16_t crefs;

  /* This is the allocated D-Space memory region.  This may be a physical
   * address allocated with kmm_malloc(), or it may be virtual address
   * associated with an address environment (if CONFIG_ARCH_ADDRENV=y).
   */

  FAR uint8_t *region;
};
#endif

/* struct stackinfo_s *******************************************************/

/* Used to report stack information */

struct stackinfo_s
{
  size_t    adj_stack_size;              /* Stack size after adjustment      */
                                         /* for hardware, processor, etc.    */
                                         /* (for debug purposes only)        */
  FAR void *stack_alloc_ptr;             /* Pointer to allocated stack       */
                                         /* Needed to deallocate stack       */
  FAR void *stack_base_ptr;              /* Adjusted initial stack pointer   */
                                         /* after the frame has been removed */
                                         /* from the stack.                  */
};

/* struct task_join_s *******************************************************/

/* Used to save task join information */

struct task_join_s
{
  sq_entry_t     entry;                  /* Implements link list            */
  pid_t          pid;                    /* Includes pid                    */
  pthread_addr_t exit_value;             /* Returned data                   */
};

/* struct task_group_s ******************************************************/

/* All threads created by pthread_create belong in the same task group (along
 * with the thread of the original task).  struct task_group_s is a shared
 * structure referenced by the TCB of each thread that is a member of the
 * task group.
 *
 * This structure should contain *all* resources shared by tasks and threads
 * that belong to the same task group:
 *
 *   Environment variables
 *   PIC data space and address environments
 *   File descriptors
 *   FILE streams
 *   Sockets
 *   Address environments.
 *
 * Each instance of struct task_group_s is reference counted. Each instance
 * is created with a reference count of one.  The reference incremented when
 * each thread joins the group and decremented when each thread exits,
 * leaving the group.  When the reference count decrements to zero,
 * the struct task_group_s is free.
 */

#ifdef CONFIG_BINFMT_LOADABLE
struct binary_s;                    /* Forward reference                        */
                                    /* Defined in include/nuttx/binfmt/binfmt.h */
#endif

struct task_group_s
{
  pid_t tg_pid;                     /* The ID of the task within the group      */
  pid_t tg_ppid;                    /* This is the ID of the parent thread      */
  uint8_t tg_flags;                 /* See GROUP_FLAG_* definitions             */

  /* User identity **********************************************************/

#ifdef CONFIG_SCHED_USER_IDENTITY
  uid_t   tg_uid;                   /* User identity                            */
  gid_t   tg_gid;                   /* User group identity                      */
  uid_t   tg_euid;                  /* Effective user identity                  */
  gid_t   tg_egid;                  /* Effective user group identity            */
#endif

  /* Group membership *******************************************************/

#ifdef HAVE_GROUP_MEMBERS
  sq_queue_t tg_members;            /* List of members for task             */
#endif

#ifdef CONFIG_BINFMT_LOADABLE
  /* Loadable module support ************************************************/

  FAR struct binary_s *tg_bininfo;  /* Describes resources used by program      */
#endif

#ifdef CONFIG_SCHED_HAVE_PARENT
  /* Child exit status ******************************************************/

#ifdef CONFIG_SCHED_CHILD_STATUS
  FAR struct child_status_s *tg_children; /* Head of a list of child status     */
#else
  uint16_t tg_nchildren;                  /* This is the number active children */
#endif
  /* Group exit status ******************************************************/

  int tg_exitcode;                        /* Exit code (status) for group   */
#endif /* CONFIG_SCHED_HAVE_PARENT */

#if defined(CONFIG_SCHED_WAITPID) && !defined(CONFIG_SCHED_HAVE_PARENT)
  /* waitpid support ********************************************************/

  /* Simple mechanism used only when there is no support for SIGCHLD        */

  uint8_t tg_nwaiters;              /* Number of waiters                    */
  uint8_t tg_waitflags;             /* User flags for waitpid behavior      */
  sem_t tg_exitsem;                 /* Support for waitpid                  */
  FAR int *tg_statloc;              /* Location to return exit status       */
#endif

#ifndef CONFIG_DISABLE_PTHREAD
  /* Pthreads ***************************************************************/

  sq_queue_t tg_joinqueue;          /* List of join status of tcb           */
#endif

  /* Thread local storage ***************************************************/

#ifndef CONFIG_MM_KERNEL_HEAP
  struct task_info_s tg_info_;
#endif
  FAR struct task_info_s *tg_info;

  /* POSIX Signal Control Fields ********************************************/

  sq_queue_t tg_sigactionq;         /* List of actions for signals              */
  sq_queue_t tg_sigpendingq;        /* List of pending signals                  */
#ifdef CONFIG_SIG_DEFAULT
  sigset_t tg_sigdefault;           /* Set of signals set to the default action */
#endif

#ifndef CONFIG_DISABLE_ENVIRON
  /* Environment variables **************************************************/

  FAR char **tg_envp;               /* Allocated environment strings        */
  ssize_t    tg_envpc;              /* Maximum entries of environment array */
  ssize_t    tg_envc;               /* Number of environment strings        */
#endif

#ifndef CONFIG_DISABLE_POSIX_TIMERS
  /* Interval timer *********************************************************/

  timer_t itimer;
#endif

  /* PIC data space and address environments ********************************/

  /* Logically the PIC data space belongs here (see struct dspace_s).  The
   * current logic needs review:  There are differences in the away that the
   * life of the PIC data is managed.
   */

  /* File descriptors *******************************************************/

  struct fdlist tg_fdlist;          /* Maps file descriptor to file         */

  /* Virtual memory mapping info ********************************************/

  struct mm_map_s tg_mm_map;        /* Task group virtual memory mappings   */

  spinlock_t tg_lock;               /* SpinLock for group */
  rmutex_t   tg_mutex;              /* Mutex for group */
};

/* struct tcb_s *************************************************************/

/* This is the common part of the task control block (TCB).
 * The TCB is the heart of the NuttX task-control logic.
 * Each task or thread is represented by a TCB that includes these common
 * definitions.
 */

struct tcb_s
{
  /* Fields used to support list management *********************************/

  FAR struct tcb_s *flink;               /* Doubly linked list              */
  FAR struct tcb_s *blink;

  /* Task Group *************************************************************/

  FAR struct task_group_s *group;        /* Pointer to shared task group    */

  /* Group membership *******************************************************/

#ifdef HAVE_GROUP_MEMBERS
  sq_entry_t member;                     /* List entry of task member       */
#endif

  /* Task join **************************************************************/

#ifndef CONFIG_DISABLE_PTHREAD
  sq_queue_t     join_queue;             /* List of wait entries for task   */
  sq_entry_t     join_entry;             /* List entry of task join         */
  sem_t          join_sem;               /* Semaphore for task join         */
  pthread_addr_t join_val;               /* Returned data                   */
#endif

  /* Address Environment ****************************************************/

#ifdef CONFIG_ARCH_ADDRENV
  FAR struct addrenv_s *addrenv_own;     /* Task(group) own memory mappings */
  FAR struct addrenv_s *addrenv_curr;    /* Current active memory mappings  */
#endif

  /* Task Management Fields *************************************************/

  pid_t    pid;                          /* This is the ID of the thread    */
  uint8_t  sched_priority;               /* Current priority of the thread  */
  uint8_t  init_priority;                /* Initial priority of the thread  */

  start_t  start;                        /* Thread start function           */
  entry_t  entry;                        /* Entry Point into the thread     */

  uint8_t  task_state;                   /* Current state of the thread     */

#ifdef CONFIG_PRIORITY_INHERITANCE
  uint8_t  boost_priority;               /* Boosted priority of the thread  */
  uint8_t  base_priority;                /* Normal priority of the thread   */
  FAR struct semholder_s *holdsem;       /* List of held semaphores         */
#endif

#ifdef CONFIG_SMP
  uint8_t  cpu;                          /* CPU index if running/assigned   */
  cpu_set_t affinity;                    /* Bit set of permitted CPUs       */
#endif
  uint32_t flags;                        /* Misc. general status flags      */
  int16_t  lockcount;                    /* 0=preemptible (not-locked)      */
#ifdef CONFIG_IRQCOUNT
  int16_t  irqcount;                     /* 0=Not in critical section       */
#endif
  int16_t  errcode;                      /* Used to pass error information  */

#if CONFIG_RR_INTERVAL > 0 || defined(CONFIG_SCHED_SPORADIC)
  int32_t  timeslice;                    /* RR timeslice OR Sporadic budget */
                                         /* interval remaining              */
#endif
#ifdef CONFIG_SCHED_SPORADIC
  FAR struct sporadic_s *sporadic;       /* Sporadic scheduling parameters  */
#endif

  struct wdog_s waitdog;                 /* All timed waits use this timer  */

  /* Stack-Related Fields ***************************************************/

  size_t    adj_stack_size;              /* Stack size after adjustment     */
                                         /* for hardware, processor, etc.   */
                                         /* (for debug purposes only)       */
  FAR void *stack_alloc_ptr;             /* Pointer to allocated stack      */
                                         /* Needed to deallocate stack      */
  FAR void *stack_base_ptr;              /* Adjusted initial stack pointer  */
                                         /* after the frame has been        */
                                         /* removed from the stack.         */

  /* External Module Support ************************************************/

#ifdef CONFIG_PIC
  FAR struct dspace_s *dspace;           /* Area for .bss and .data         */
#endif

  /* POSIX Semaphore and Message Queue Control Fields ***********************/

  FAR void *waitobj;                     /* Object thread waiting on        */

  /* POSIX Signal Control Fields ********************************************/

  sigset_t   sigprocmask;                /* Signals that are blocked        */
  sigset_t   sigwaitmask;                /* Waiting for pending signals     */
  sq_queue_t sigpendactionq;             /* List of pending signal actions  */
  sq_queue_t sigpostedq;                 /* List of posted signals          */
  siginfo_t  *sigunbinfo;                /* Signal info when task unblocked */

  /* Robust mutex support ***************************************************/

#if !defined(CONFIG_DISABLE_PTHREAD) && !defined(CONFIG_PTHREAD_MUTEX_UNSAFE)
  FAR struct pthread_mutex_s *mhead;     /* List of mutexes held by thread  */
#endif

  /* CPU load monitoring support ********************************************/

#ifndef CONFIG_SCHED_CPULOAD_NONE
  clock_t ticks;                         /* Number of ticks on this thread  */
#endif

  /* Pre-emption monitor support ********************************************/

#if CONFIG_SCHED_CRITMONITOR_MAXTIME_THREAD >= 0
  clock_t run_start;                     /* Time when thread begin run      */
  clock_t run_max;                       /* Max time thread run             */
  clock_t run_time;                      /* Total time thread run           */
#endif

#if CONFIG_SCHED_CRITMONITOR_MAXTIME_PREEMPTION >= 0
  clock_t preemp_start;                  /* Time when preemption disabled   */
  clock_t preemp_max;                    /* Max time preemption disabled    */
  void   *preemp_caller;                 /* Caller of preemption disabled   */
  void   *preemp_max_caller;             /* Caller of max preemption        */
#endif

#if CONFIG_SCHED_CRITMONITOR_MAXTIME_CSECTION >= 0
  clock_t crit_start;                    /* Time critical section entered   */
  clock_t crit_max;                      /* Max time in critical section    */
  void   *crit_caller;                   /* Caller of critical section      */
  void   *crit_max_caller;               /* Caller of max critical section  */
#endif

  /* State save areas *******************************************************/

  /* The form and content of these fields are platform-specific.            */

  struct xcptcontext xcp;                /* Interrupt register save area    */

  /* The following function pointer is non-zero if there are pending signals
   * to be processed.
   */

  sig_deliver_t sigdeliver;
#if CONFIG_TASK_NAME_SIZE > 0
  char name[CONFIG_TASK_NAME_SIZE + 1];  /* Task name (with NUL terminator) */
#endif

#if CONFIG_SCHED_STACK_RECORD > 0
  FAR void *stackrecord_pc[CONFIG_SCHED_STACK_RECORD];
  FAR void *stackrecord_sp[CONFIG_SCHED_STACK_RECORD];
  FAR void *stackrecord_pc_deepest[CONFIG_SCHED_STACK_RECORD];
  FAR void *stackrecord_sp_deepest[CONFIG_SCHED_STACK_RECORD];
  FAR void *sp_deepest;
  size_t caller_deepest;
  size_t level_deepest;
  size_t level;
#endif

#ifndef CONFIG_PTHREAD_MUTEX_UNSAFE
  spinlock_t mutex_lock;
#endif
};

/* struct task_tcb_s ********************************************************/

/* This is the particular form of the task control block (TCB) structure used
 * by tasks (and kernel threads).  There are two TCB forms:  one for pthreads
 * and one for tasks.
 * Both share the common TCB fields (which must appear at the top of the
 * structure) plus additional fields unique to tasks and threads.
 * Having separate structures for tasks and pthreads adds some complexity,
 * but saves memory in that it prevents pthreads from being burdened with the
 * overhead required for tasks (and vice versa).
 */

struct task_tcb_s
{
  /* Common TCB fields ******************************************************/

  struct tcb_s cmn;                      /* Common TCB fields               */

  /* Task Group *************************************************************/

  struct task_group_s group;             /* Shared task group data          */
};

/* struct pthread_tcb_s *****************************************************/

/* This is the particular form of the task control block (TCB) structure used
 * by pthreads. There are two TCB forms:  one for pthreads and one for tasks.
 * Both share the common TCB fields (which must appear at the top of the
 * structure) plus additional fields unique to tasks and threads.
 * Having separate structures for tasks and pthreads adds some complexity,
 * but saves memory in that it prevents pthreads from being burdened with
 * the overhead required for tasks (and vice versa).
 */

#ifndef CONFIG_DISABLE_PTHREAD
struct pthread_tcb_s
{
  /* Common TCB fields ******************************************************/

  struct tcb_s cmn;                      /* Common TCB fields               */

  /* Task Management Fields *************************************************/

  pthread_trampoline_t trampoline;       /* User-space startup function     */
  pthread_addr_t arg;                    /* Startup argument                */
};
#endif /* !CONFIG_DISABLE_PTHREAD */

/* struct tcbinfo_s *********************************************************/

/* The structure save key filed offset of tcb_s while can be used by
 * debuggers to parse the tcb information
 */

begin_packed_struct struct tcbinfo_s
{
  uint16_t pid_off;                      /* Offset of tcb.pid               */
  uint16_t state_off;                    /* Offset of tcb.task_state        */
  uint16_t pri_off;                      /* Offset of tcb.sched_priority    */
  uint16_t name_off;                     /* Offset of tcb.name              */
  uint16_t stack_off;                    /* Offset of tcb.stack_alloc_ptr   */
  uint16_t stack_size_off;               /* Offset of tcb.adj_stack_size    */
  uint16_t regs_off;                     /* Offset of tcb.regs              */
  uint16_t regs_num;                     /* Num of general regs             */

  /* Offset pointer of xcp.regs, order in GDB org.gnu.gdb.xxx feature.
   * Refer to the link of `reg_off` below for more information.
   *
   * value UINT16_MAX: This register was not provided by NuttX
   */

  begin_packed_struct
  union
  {
    uint8_t             u[8];
    FAR const uint16_t *p;
  }
  end_packed_struct reg_off; /* Refer to https://sourceware.org/gdb/current/onlinedocs/gdb.html/Standard-Target-Features.html */
} end_packed_struct;

/* This is the callback type used by nxsched_foreach() */

typedef CODE void (*nxsched_foreach_t)(FAR struct tcb_s *tcb, FAR void *arg);

/* This is the callback type used by nxsched_smp_call() */

#ifdef CONFIG_SMP
typedef CODE int (*nxsched_smp_call_t)(FAR void *arg);

struct smp_call_cookie_s;
struct smp_call_data_s
{
  nxsched_smp_call_t            func;
  FAR void                     *arg;
  FAR struct smp_call_cookie_s *cookie;
  sq_entry_t                    node[CONFIG_SMP_NCPUS];
};
#endif

#endif /* __ASSEMBLY__ */

/****************************************************************************
 * Public Data
 ****************************************************************************/

#ifndef __ASSEMBLY__
#undef EXTERN
#if defined(__cplusplus)
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/* Maximum time with pre-emption disabled or within critical section. */

#if CONFIG_SCHED_CRITMONITOR_MAXTIME_PREEMPTION >= 0
EXTERN clock_t g_preemp_max[CONFIG_SMP_NCPUS];
#endif /* CONFIG_SCHED_CRITMONITOR_MAXTIME_PREEMPTION  >= 0 */

#if CONFIG_SCHED_CRITMONITOR_MAXTIME_CSECTION >= 0
EXTERN clock_t g_crit_max[CONFIG_SMP_NCPUS];
#endif /* CONFIG_SCHED_CRITMONITOR_MAXTIME_CSECTION >= 0 */

/* g_running_tasks[] holds a references to the running task for each CPU.
 * It is valid only when up_interrupt_context() returns true.
 */

EXTERN FAR struct tcb_s *g_running_tasks[CONFIG_SMP_NCPUS];

EXTERN const struct tcbinfo_s g_tcbinfo;

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Name: nxsched_self
 *
 * Description:
 *   Return the current threads TCB.  Basically, this function just wraps the
 *   head of the ready-to-run list and manages access to the TCB from outside
 *   of the sched/ sub-directory.
 *
 ****************************************************************************/

FAR struct tcb_s *nxsched_self(void);

/****************************************************************************
 * Name: nxsched_foreach
 *
 * Description:
 *   Enumerate over each task and provide the TCB of each task to a user
 *   callback functions.
 *
 *   NOTE:  This function examines the TCB and calls each handler within a
 *   critical section.  However, that critical section is released and
 *   reacquired for each TCB.  When it is released, there may be changes in
 *   tasking.  If the caller requires absolute stability through the
 *   traversal, then the caller should establish the critical section BEFORE
 *   calling this function.
 *
 * Input Parameters:
 *   handler - The function to be called with the TCB of
 *     each task
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

void nxsched_foreach(nxsched_foreach_t handler, FAR void *arg);

/****************************************************************************
 * Name: nxsched_get_tcb
 *
 * Description:
 *   Given a task ID, this function will return the a pointer to the
 *   corresponding TCB (or NULL if there is no such task ID).
 *
 *   NOTE:  This function holds a critical section while examining TCB data
 *   data structures but releases that critical section before returning.
 *   When it is released, the TCB may become unstable.  If the caller
 *   requires absolute stability while using the TCB, then the caller
 *   should establish the critical section BEFORE calling this function and
 *   hold that critical section as long as necessary.
 *
 ****************************************************************************/

FAR struct tcb_s *nxsched_get_tcb(pid_t pid);

/****************************************************************************
 * Name:  nxsched_releasepid
 *
 * Description:
 *   When a task is destroyed, this function must be called to make its
 *   process ID available for reuse.
 *
 ****************************************************************************/

int nxsched_release_tcb(FAR struct tcb_s *tcb, uint8_t ttype);

/* File system helpers ******************************************************/

/* These functions all extract lists from the group structure associated with
 * the currently executing task.
 */

/****************************************************************************
 * Name: nxsched_get_fdlist_from_tcb
 *
 * Description:
 *   Return a pointer to the file descriptor list from task context.
 *
 * Input Parameters:
 *   tcb - Address of the new task's TCB
 *
 * Returned Value:
 *   A pointer to the errno.
 *
 * Assumptions:
 *
 ****************************************************************************/

FAR struct fdlist *nxsched_get_fdlist_from_tcb(FAR struct tcb_s *tcb);

/****************************************************************************
 * Name: nxsched_get_fdlist
 *
 * Description:
 *   Return a pointer to the file descriptor list for this thread.
 *
 * Input Parameters:
 *   None
 *
 * Returned Value:
 *   A pointer to the file descriptor list.
 *
 * Assumptions:
 *
 ****************************************************************************/

FAR struct fdlist *nxsched_get_fdlist(void);

/****************************************************************************
 * Name: nxtask_init
 *
 * Description:
 *   This function initializes a Task Control Block (TCB) in preparation for
 *   starting a new thread.  It performs a subset of the functionality of
 *   task_create()
 *
 *   Unlike task_create():
 *     1. Allocate the TCB.  The pre-allocated TCB is passed in argv.
 *     2. Allocate the stack.  The pre-allocated stack is passed in argv.
 *     3. Activate the task. This must be done by calling nxtask_activate().
 *
 *   Certain fields of the pre-allocated TCB may be set to change the
 *   nature of the created task.  For example:
 *
 *     - Task type may be set in the TCB flags to create kernel thread
 *
 * Input Parameters:
 *   tcb        - Address of the new task's TCB
 *   name       - Name of the new task (not used)
 *   priority   - Priority of the new task
 *   stack      - Start of the pre-allocated stack
 *   stack_size - Size (in bytes) of the stack allocated
 *   entry      - Application start point of the new task
 *   argv       - A pointer to an array of input parameters.  The array
 *                should be terminated with a NULL argv[] value. If no
 *                parameters are required, argv may be NULL.
 *   envp       - A pointer to the program's environment, envp may be NULL
 *
 * Returned Value:
 *   OK on success; negative error value on failure appropriately.  (See
 *   nxtask_setup_scheduler() for possible failure conditions).  On failure,
 *   the caller is responsible for freeing the stack memory and for calling
 *   nxsched_release_tcb() to free the TCB (which could be in most any
 *   state).
 *
 ****************************************************************************/

int nxtask_init(FAR struct task_tcb_s *tcb, const char *name, int priority,
                FAR void *stack, uint32_t stack_size, main_t entry,
                FAR char * const argv[], FAR char * const envp[],
                FAR const posix_spawn_file_actions_t *actions);

/****************************************************************************
 * Name: nxtask_uninit
 *
 * Description:
 *   Undo all operations on a TCB performed by task_init() and release the
 *   TCB by calling kmm_free().  This is intended primarily to support
 *   error recovery operations after a successful call to task_init() such
 *   was when a subsequent call to task_activate fails.
 *
 *   Caution:  Freeing of the TCB itself might be an unexpected side-effect.
 *
 * Input Parameters:
 *   tcb - Address of the TCB initialized by task_init()
 *
 * Returned Value:
 *   OK on success; negative error value on failure appropriately.
 *
 ****************************************************************************/

void nxtask_uninit(FAR struct task_tcb_s *tcb);

/****************************************************************************
 * Name: nxtask_create
 *
 * Description:
 *   This function creates and activates a new user task with a specified
 *   priority and returns its system-assigned ID.
 *
 *   The entry address entry is the address of the "main" function of the
 *   task.  This function will be called once the C environment has been
 *   set up.  The specified function will be called with four arguments.
 *   Should the specified routine return, a call to exit() will
 *   automatically be made.
 *
 *   Note that four (and only four) arguments must be passed for the spawned
 *   functions.
 *
 *   nxtask_create() is identical to the function task_create(), differing
 *   only in its return value:  This function does not modify the errno
 *   variable.  This is a non-standard, internal OS function and is not
 *   intended for use by application logic.  Applications should use
 *   task_create().
 *
 * Input Parameters:
 *   name       - Name of the new task
 *   priority   - Priority of the new task
 *   stack_size - size (in bytes) of the stack needed
 *   entry      - Entry point of a new task
 *   arg        - A pointer to an array of input parameters.  The array
 *                should be terminated with a NULL argv[] value. If no
 *                parameters are required, argv may be NULL.
 *   envp       - A pointer to an array of environment strings. Terminated
 *                with a NULL entry.
 *
 * Returned Value:
 *   Returns the positive, non-zero process ID of the new task or a negated
 *   errno value to indicate the nature of any failure.  If memory is
 *   insufficient or the task cannot be created -ENOMEM will be returned.
 *
 ****************************************************************************/

int nxtask_create(FAR const char *name, int priority,
                  FAR void *stack_addr, int stack_size, main_t entry,
                  FAR char * const argv[], FAR char * const envp[]);

/****************************************************************************
 * Name: nxtask_delete
 *
 * Description:
 *   This function causes a specified task to cease to exist.  Its stack and
 *   TCB will be deallocated.
 *
 *   The logic in this function only deletes non-running tasks.  If the
 *   'pid' parameter refers to the currently running task, then processing
 *   is redirected to exit(). This can only happen if a task calls
 *   nxtask_delete() in order to delete itself.
 *
 *   This function obeys the semantics of pthread cancellation:  task
 *   deletion is deferred if cancellation is disabled or if deferred
 *   cancellation is supported (with cancellation points enabled).
 *
 * Input Parameters:
 *   pid - The task ID of the task to delete.  A pid of zero
 *         signifies the calling task.
 *
 * Returned Value:
 *   OK on success; or negated errno on failure
 *
 ****************************************************************************/

int nxtask_delete(pid_t pid);

/****************************************************************************
 * Name: nxtask_activate
 *
 * Description:
 *   This function activates tasks initialized by nxtask_setup_scheduler().
 *   Without activation, a task is ineligible for execution by the
 *   scheduler.
 *
 * Input Parameters:
 *   tcb - The TCB for the task (same as the nxtask_init argument).
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

void nxtask_activate(FAR struct tcb_s *tcb);

/****************************************************************************
 * Name: nxtask_startup
 *
 * Description:
 *   This function is the user-space, task startup function.  It is called
 *   from up_task_start() in user-mode.
 *
 * Input Parameters:
 *   entrypt - The user-space address of the task entry point
 *   argc and argv - Standard arguments for the task entry point
 *
 * Returned Value:
 *   None.  This function does not return.
 *
 ****************************************************************************/

#ifndef CONFIG_BUILD_KERNEL
void nxtask_startup(main_t entrypt, int argc, FAR char *argv[]);
#endif

/****************************************************************************
 * Internal fork support.  The overall sequence is:
 *
 * 1) User code calls fork().  fork() is provided in architecture-specific
 *    code.
 * 2) fork()and calls nxtask_setup_fork().
 * 3) nxtask_setup_fork() allocates and configures the child task's TCB.
 *    This consists of:
 *    - Allocation of the child task's TCB.
 *    - Initialization of file descriptors and streams
 *    - Configuration of environment variables
 *    - Allocate and initialize the stack
 *    - Setup the input parameters for the task.
 *    - Initialization of the TCB (including call to up_initial_state())
 * 4) fork() provides any additional operating context. fork must:
 *    - Initialize special values in any CPU registers that were not
 *      already configured by up_initial_state()
 * 5) fork() then calls nxtask_start_fork()
 * 6) nxtask_start_fork() then executes the child thread.
 *
 * nxtask_abort_fork() may be called if an error occurs between
 * steps 3 and 6.
 *
 ****************************************************************************/

FAR struct task_tcb_s *nxtask_setup_fork(start_t retaddr);
pid_t nxtask_start_fork(FAR struct task_tcb_s *child);
void nxtask_abort_fork(FAR struct task_tcb_s *child, int errcode);

/****************************************************************************
 * Name: nxtask_argvstr
 *
 * Description:
 *   Safely read the contents of a task's argument vector, into a a safe
 *   buffer. Function skips the process's name.
 *
 * Input Parameters:
 *   tcb  - tcb of the task.
 *   args - Output buffer for the argument vector.
 *   size - Size of the buffer.
 *
 * Returned Value:
 *   The actual string length that was written.
 *
 ****************************************************************************/

size_t nxtask_argvstr(FAR struct tcb_s *tcb, FAR char *args, size_t size);

/****************************************************************************
 * Name: group_exitinfo
 *
 * Description:
 *   This function may be called to when a task is loaded into memory.  It
 *   will setup the to automatically unload the module when the task exits.
 *
 * Input Parameters:
 *   pid     - The task ID of the newly loaded task
 *   bininfo - This structure allocated with kmm_malloc().  This memory
 *             persists until the task exits and will be used unloads
 *             the module from memory.
 *
 * Returned Value:
 *   This is a NuttX internal function so it follows the convention that
 *   0 (OK) is returned on success and a negated errno is returned on
 *   failure.
 *
 ****************************************************************************/

#ifdef CONFIG_BINFMT_LOADABLE
struct binary_s;  /* Forward reference */
int group_exitinfo(pid_t pid, FAR struct binary_s *bininfo);
#endif

/****************************************************************************
 * Name: nxsched_resume_scheduler
 *
 * Description:
 *   Called by architecture specific implementations that block task
 *   execution.
 *   This function prepares the scheduler for the thread that is about to be
 *   restarted.
 *
 * Input Parameters:
 *   tcb - The TCB of the thread to be restarted.
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

#if defined(CONFIG_SCHED_RESUMESCHEDULER)
void nxsched_resume_scheduler(FAR struct tcb_s *tcb);
#else
#  define nxsched_resume_scheduler(tcb)
#endif

/****************************************************************************
 * Name: nxsched_suspend_scheduler
 *
 * Description:
 *   Called by architecture specific implementations to resume task
 *   execution.
 *   This function performs scheduler operations for the thread that is about
 *   to be suspended.
 *
 * Input Parameters:
 *   tcb - The TCB of the thread to be restarted.
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

#ifdef CONFIG_SCHED_SUSPENDSCHEDULER
void nxsched_suspend_scheduler(FAR struct tcb_s *tcb);
#else
#  define nxsched_suspend_scheduler(tcb)
#endif

/****************************************************************************
 * Name: nxsched_get_param
 *
 * Description:
 *   This function gets the scheduling priority of the task specified by
 *   pid.  It is identical in function, differing only in its return value:
 *   This function does not modify the errno variable.
 *
 *   This is a non-standard, internal OS function and is not intended for
 *   use by application logic.  Applications should use the standard
 *   sched_getparam().
 *
 * Input Parameters:
 *   pid - the task ID of the task.  If pid is zero, the priority
 *     of the calling task is returned.
 *   param - A structure whose member sched_priority is the integer
 *     priority.  The task's priority is copied to the sched_priority
 *     element of this structure.
 *
 * Returned Value:
 *   0 (OK) if successful, otherwise a negated errno value is returned to
 *   indicate the nature of the failure..
 *
 *   This function can fail if param is null (EINVAL) or if pid does
 *   not correspond to any task (ESRCH).
 *
 ****************************************************************************/

int nxsched_get_param(pid_t pid, FAR struct sched_param *param);

/****************************************************************************
 * Name:  nxsched_set_param
 *
 * Description:
 *   This function sets the priority of a specified task.  It is identical
 *   to the function sched_setparam(), differing only in its return value:
 *   This function does not modify the errno variable.
 *
 *   NOTE: Setting a task's priority to the same value has a similar effect
 *   to sched_yield() -- The task will be moved to  after all other tasks
 *   with the same priority.
 *
 *   This is a non-standard, internal OS function and is not intended for
 *   use by application logic.  Applications should use the standard
 *   sched_setparam().
 *
 * Input Parameters:
 *   pid - the task ID of the task to reprioritize.  If pid is zero, the
 *      priority of the calling task is changed.
 *   param - A structure whose member sched_priority is the integer priority.
 *      The range of valid priority numbers is from SCHED_PRIORITY_MIN
 *      through SCHED_PRIORITY_MAX.
 *
 * Returned Value:
 *   0 (OK) if successful, otherwise a negated errno value is returned to
 *   indicate the nature of the failure..
 *
 *   EINVAL The parameter 'param' is invalid or does not make sense for the
 *          current scheduling policy.
 *   EPERM  The calling task does not have appropriate privileges.
 *   ESRCH  The task whose ID is pid could not be found.
 *
 ****************************************************************************/

int nxsched_set_param(pid_t pid, FAR const struct sched_param *param);

/****************************************************************************
 * Name: nxsched_get_scheduler
 *
 * Description:
 *   sched_getscheduler() returns the scheduling policy currently
 *   applied to the task identified by pid.  If pid equals zero, the
 *   policy of the calling task will be retrieved.
 *
 *   This functions is identical to the function sched_getscheduler(),
 *   differing only in its return value:  This function does not modify
 *   the errno variable.
 *
 *   This is a non-standard, internal OS function and is not intended for
 *   use by application logic.  Applications should use the standard
 *   sched_getscheduler().
 *
 * Input Parameters:
 *   pid - the task ID of the task to query.  If pid is zero, the
 *     calling task is queried.
 *
 * Returned Value:
 *    On success, sched_getscheduler() returns the policy for the task
 *    (either SCHED_FIFO or SCHED_RR).  On error,  a negated errno value
 *    returned:
 *
 *      ESRCH  The task whose ID is pid could not be found.
 *
 ****************************************************************************/

int nxsched_get_scheduler(pid_t pid);

/****************************************************************************
 * Name: nxsched_set_scheduler
 *
 * Description:
 *   nxsched_set_scheduler() sets both the scheduling policy and the priority
 *   for the task identified by pid. If pid equals zero, the scheduler of
 *   the calling task will be set.  The parameter 'param' holds the priority
 *   of the thread under the new policy.
 *
 *   nxsched_set_scheduler() is identical to the function sched_getparam(),
 *   differing only in its return value:  This function does not modify the
 *    errno variable.
 *
 *   This is a non-standard, internal OS function and is not intended for
 *   use by application logic.  Applications should use the standard
 *   sched_getparam().
 *
 * Input Parameters:
 *   pid - the task ID of the task to modify.  If pid is zero, the calling
 *      task is modified.
 *   policy - Scheduling policy requested (either SCHED_FIFO or SCHED_RR)
 *   param - A structure whose member sched_priority is the new priority.
 *      The range of valid priority numbers is from SCHED_PRIORITY_MIN
 *      through SCHED_PRIORITY_MAX.
 *
 * Returned Value:
 *   On success, nxsched_set_scheduler() returns OK (zero).  On error, a
 *   negated errno value is returned:
 *
 *   EINVAL The scheduling policy is not one of the recognized policies.
 *   ESRCH  The task whose ID is pid could not be found.
 *
 ****************************************************************************/

int nxsched_set_scheduler(pid_t pid, int policy,
                          FAR const struct sched_param *param);

/****************************************************************************
 * Name: nxsched_get_affinity
 *
 * Description:
 *   nxsched_get_affinity() writes the affinity mask of the thread whose ID
 *   is pid into the cpu_set_t pointed to by mask.  The  cpusetsize
 *   argument specifies the size (in bytes) of mask.  If pid is zero, then
 *   the mask of the calling thread is returned.
 *
 *   nxsched_get_affinity() is identical to the function sched_getaffinity(),
 *   differing only in its return value:  This function does not modify the
 *   errno variable.
 *
 *   This is a non-standard, internal OS function and is not intended for
 *   use by application logic.  Applications should use the standard
 *   sched_getparam().
 *
 * Input Parameters:
 *   pid        - The ID of thread whose affinity set will be retrieved.
 *   cpusetsize - Size of mask.  MUST be sizeofcpu_set_t().
 *   mask       - The location to return the thread's new affinity set.
 *
 * Returned Value:
 *   Zero (OK) if successful.  Otherwise, a negated errno value is returned:
 *
 *     ESRCH  The task whose ID is pid could not be found.
 *
 ****************************************************************************/

#ifdef CONFIG_SMP
int nxsched_get_affinity(pid_t pid, size_t cpusetsize, FAR cpu_set_t *mask);
#endif

/****************************************************************************
 * Name: nxsched_set_affinity
 *
 * Description:
 *   sched_setaffinity() sets the CPU affinity mask of the thread whose ID
 *   is pid to the value specified by mask.  If pid is zero, then the
 *   calling thread is used.  The argument cpusetsize is the length (i
 *   bytes) of the data pointed to by mask.  Normally this argument would
 *   be specified as sizeof(cpu_set_t).
 *
 *   If the thread specified by pid is not currently running on one of the
 *   CPUs specified in mask, then that thread is migrated to one of the
 *   CPUs specified in mask.
 *
 *   nxsched_set_affinity() is identical to the function sched_setparam(),
 *   differing only in its return value:  This function does not modify
 *   the errno variable.  This is a non-standard, internal OS function and
 *   is not intended for use by application logic.  Applications should
 *   use the standard sched_setparam().
 *
 * Input Parameters:
 *   pid        - The ID of thread whose affinity set will be modified.
 *   cpusetsize - Size of mask.  MUST be sizeofcpu_set_t().
 *   mask       - The location to return the thread's new affinity set.
 *
 * Returned Value:
 *   Zero (OK) if successful.  Otherwise, a negated errno value is returned:
 *
 *     ESRCH  The task whose ID is pid could not be found.
 *
 ****************************************************************************/

#ifdef CONFIG_SMP
int nxsched_set_affinity(pid_t pid, size_t cpusetsize,
                         FAR const cpu_set_t *mask);
#endif

/****************************************************************************
 * Name: nxsched_get_stackinfo
 *
 * Description:
 *   Report information about a thread's stack allocation.
 *
 * Input Parameters:
 *   pid       - Identifies the thread to query.  Zero is interpreted as the
 *               the calling thread, -1 is interpreted as the calling task.
 *   stackinfo - User-provided location to return the stack information.
 *
 * Returned Value:
 *   Zero (OK) if successful.  Otherwise, a negated errno value is returned.
 *
 *     -ENOENT  Returned if pid does not refer to an active task
 *     -EACCES  The calling thread does not have privileges to access the
 *              stack of the thread associated with the pid.
 *
 ****************************************************************************/

int nxsched_get_stackinfo(pid_t pid, FAR struct stackinfo_s *stackinfo);

/****************************************************************************
 * Name: nxsched_get_stateinfo
 *
 * Description:
 *   Report information about a thread's state
 *
 * Input Parameters:
 *   tcb    - The TCB for the task (same as the nxtask_init argument).
 *   state  - User-provided location to return the state information.
 *   length - The size of the state
 *
 ****************************************************************************/

void nxsched_get_stateinfo(FAR struct tcb_s *tcb, FAR char *state,
                           size_t length);

/****************************************************************************
 * Name: nxsched_waitpid
 *
 * Description:
 *   This functions will obtain status information pertaining to one
 *   of the caller's child processes. This function will suspend
 *   execution of the calling thread until status information for one of the
 *   terminated child processes of the calling process is available, or until
 *   delivery of a signal whose action is either to execute a signal-catching
 *   function or to terminate the process. If more than one thread is
 *   suspended in nxsched_waitpid() awaiting termination of the same process,
 *   exactly one thread will return the process status at the time of the
 *   target process termination. If status information is available prior to
 *   the call to nxsched_waitpid(), return will be immediate.
 *
 * Input Parameters:
 *   pid - The task ID of the thread to waid for
 *   stat_loc - The location to return the exit status
 *   options - Modifiable behavior, see sys/wait.h.
 *
 * Returned Value:
 *   If nxsched_waitpid() returns because the status of a child process is
 *   available, it will return a value equal to the process ID of the child
 *   process for which status is reported.
 *
 *   If nxsched_waitpid() returns due to the delivery of a signal to the
 *   calling process, -1 will be returned and errno set to EINTR.
 *
 *   If nxsched_waitpid() was invoked with WNOHANG set in options, it has
 *   at least one child process specified by pid for which status is not
 *   available, and status is not available for any process specified by
 *   pid, 0 is returned.
 *
 *   Otherwise, (pid_t)-1 will be returned, and errno set to indicate the
 *   error:
 *
 *   ECHILD - The process specified by pid does not exist or is not a child
 *            of the calling process, or the process group specified by pid
 *            does not exist does not have any member process that is a child
 *            of the calling process.
 *   EINTR - The function was interrupted by a signal. The value of the
 *           location pointed to by stat_loc is undefined.
 *   EINVAL - The options argument is not valid.
 *
 ****************************************************************************/

#ifdef CONFIG_SCHED_WAITPID
pid_t nxsched_waitpid(pid_t pid, FAR int *stat_loc, int options);
#endif

/****************************************************************************
 * Name: nxsched_gettid
 *
 * Description:
 *   Get the thread ID of the currently executing thread.
 *
 * Input parameters:
 *   None
 *
 * Returned Value:
 *   On success, returns the thread ID of the calling process.
 *
 ****************************************************************************/

pid_t nxsched_gettid(void);

/****************************************************************************
 * Name: nxsched_getpid
 *
 * Description:
 *   Get the Process ID of the currently executing task.
 *
 * Input parameters:
 *   None
 *
 * Returned Value:
 *   Normally when called from user applications, nxsched_getpid() will
 *   return the Process ID of the currently executing task. that is,
 *   the main task for the task groups. There is no specification for
 *   any errors returned from nxsched_getpid().
 *
 ****************************************************************************/

pid_t nxsched_getpid(void);

/****************************************************************************
 * Name: nxsched_getppid
 *
 * Description:
 *   Get the parent task ID of the currently executing task.
 *
 * Input parameters:
 *   None
 *
 * Returned Value:
 *   Normally when called from user applications, nxsched_getppid() will
 *   return the parent task ID of the currently executing task, that is,
 *   the task at the head of the ready-to-run list.
 *   There is no specification for any errors returned from
 *   nxsched_getppid().
 *
 *   nxsched_getppid(), however, may be called from within the OS in some
 *   cases. There are certain situations during context switching when the
 *   OS data structures are in flux and where the current task at the head
 *   of the ready-to-run task list is not actually running.
 *   In that case, nxsched_getppid() will return the error: -ESRCH
 *
 ****************************************************************************/

pid_t nxsched_getppid(void);

/****************************************************************************
 * Name: nxsched_collect_deadlock
 *
 * Description:
 *   Check if there is a deadlock and get the thread pid of the deadlock.
 *
 * Input parameters:
 *   pid   - The array to store the thread pid of the deadlock.
 *   count - The size of the pid array.
 *
 * Returned Value:
 *   The number of thread deadlocks.
 *
 ****************************************************************************/

size_t nxsched_collect_deadlock(FAR pid_t *pid, size_t count);

/****************************************************************************
 * Name: nxsched_dumponexit
 *
 * Description:
 *   Dump the state of all tasks whenever on task exits.  This is debug
 *   instrumentation that was added to check file-related reference counting
 *   but could be useful again sometime in the future.
 *
 ****************************************************************************/

#ifdef CONFIG_SCHED_DUMP_ON_EXIT
void nxsched_dumponexit(void);
#else
#  define nxsched_dumponexit()
#endif /* CONFIG_SCHED_DUMP_ON_EXIT */

#ifdef CONFIG_SMP
/****************************************************************************
 * Name: nxsched_smp_call_handler
 *
 * Description:
 *   SMP function call handler
 *
 * Input Parameters:
 *   irq     - Interrupt id
 *   context - Regs context before irq
 *   arg     - Interrupt arg
 *
 * Returned Value:
 *   Result
 *
 ****************************************************************************/

int nxsched_smp_call_handler(int irq, FAR void *context,
                             FAR void *arg);

/****************************************************************************
 * Name: nxsched_smp_call_init
 *
 * Description:
 *   Init call_data
 *
 * Input Parameters:
 *   data - Call data
 *   func - Function
 *   arg  - Function args
 *
 * Returned Value:
 *   Result
 *
 ****************************************************************************/

void nxsched_smp_call_init(FAR struct smp_call_data_s *data,
                           nxsched_smp_call_t func, FAR void *arg);

/****************************************************************************
 * Name: nxsched_smp_call_single
 *
 * Description:
 *   Call function on single processor, wait function callback
 *
 * Input Parameters:
 *   cpuid - Target cpu id
 *   func  - Function
 *   arg   - Function args
 *
 * Returned Value:
 *   Result
 *
 ****************************************************************************/

int nxsched_smp_call_single(int cpuid, nxsched_smp_call_t func,
                            FAR void *arg);

/****************************************************************************
 * Name: nxsched_smp_call
 *
 * Description:
 *   Call function on multi processors, wait function callback
 *
 * Input Parameters:
 *   cpuset - Target cpuset
 *   func   - Function
 *   arg    - Function args
 *
 * Returned Value:
 *   Result
 *
 ****************************************************************************/

int nxsched_smp_call(cpu_set_t cpuset, nxsched_smp_call_t func,
                     FAR void *arg);

/****************************************************************************
 * Name: nxsched_smp_call_single_async
 *
 * Description:
 *   Call function on single processor async
 *
 * Input Parameters:
 *   cpuset - Target cpuset
 *   data   - Call data
 *
 * Returned Value:
 *   Result
 *
 ****************************************************************************/

int nxsched_smp_call_single_async(int cpuid,
                                  FAR struct smp_call_data_s *data);

/****************************************************************************
 * Name: nxsched_smp_call_async
 *
 * Description:
 *   Call function on multi processors async
 *
 * Input Parameters:
 *   cpuset - Target cpuset
 *   data   - Call data
 *
 * Returned Value:
 *   Result
 *
 ****************************************************************************/

int nxsched_smp_call_async(cpu_set_t cpuset,
                           FAR struct smp_call_data_s *data);
#endif

#undef EXTERN
#if defined(__cplusplus)
}
#endif
#endif /* __ASSEMBLY__ */

#endif /* __INCLUDE_NUTTX_SCHED_H */
