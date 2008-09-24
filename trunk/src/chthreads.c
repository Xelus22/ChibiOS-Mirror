/*
    ChibiOS/RT - Copyright (C) 2006-2007 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @addtogroup Threads
 * @{
 */

#include <ch.h>

/*
 * Initializes a thread structure.
 */
Thread *init_thread(Thread *tp, tprio_t prio) {
  static tid_t nextid = 0;

  tp->p_tid = nextid++;
  tp->p_flags = P_MEM_MODE_STATIC;
  tp->p_prio = prio;
  tp->p_state = PRSUSPENDED;
#ifdef CH_USE_MUTEXES
  /* realprio is the thread's own, non-inherited, priority */
  tp->p_realprio = prio;
  tp->p_mtxlist = NULL;
#endif
#ifdef CH_USE_WAITEXIT
  list_init(&tp->p_waiting);
#endif
#ifdef CH_USE_MESSAGES
  fifo_init(&tp->p_msgqueue);
#endif
#ifdef CH_USE_EVENTS
  tp->p_epending = 0;
#endif
#ifdef CH_USE_EXIT_EVENT
  chEvtInit(&tp->p_exitesource);
#endif
#ifdef CH_USE_THREAD_EXT
  THREAD_EXT_INIT(tp);
#endif
  return tp;
}

#ifdef CH_USE_DEBUG
static void memfill(uint8_t *p, uint32_t n, uint8_t v) {

  while (n)
    *p++ = v, n--;
}
#endif

/**
 * Initializes a new thread.
 * The new thread is initialized but not inserted in the ready list, the
 * initial state is \p PRSUSPENDED.
 * @param prio the priority level for the new thread. Usually the threads are
 *             created with priority \p NORMALPRIO, priorities
 *             can range from \p LOWPRIO to \p HIGHPRIO.
 * @param workspace pointer to a working area dedicated to the thread stack
 * @param wsize size of the working area.
 * @param pf the thread function. Returning from this function automatically
 *           terminates the thread.
 * @param arg an argument passed to the thread function. It can be \p NULL.
 * @return the pointer to the \p Thread structure allocated for the
 *         thread into the working space area.
 * @note A thread can terminate by calling \p chThdExit() or by simply
 *       returning from its main function.
 * @note This function can be used to start a thead from within an interrupt
 *       handler by manually making it ready with \p chSchReadyI().
 */
Thread *chThdInit(void *workspace, size_t wsize,
                  tprio_t prio, tfunc_t pf, void *arg) {
  /* thread structure is layed out in the lower part of the thread workspace */
  Thread *tp = workspace;

  chDbgAssert((wsize >= UserStackSize(0)) && (prio <= HIGHPRIO) &&
              (workspace != NULL) && (pf != NULL),
              "chthreads.c, chThdInit()");
#ifdef CH_USE_DEBUG
  memfill(workspace, wsize, MEM_FILL_PATTERN);
#endif
  SETUP_CONTEXT(workspace, wsize, pf, arg);
  return init_thread(tp, prio);
}

/**
 * Creates a new thread into a static memory area.
 * @param workspace pointer to a working area dedicated to the thread stack
 * @param wsize size of the working area.
 * @param prio the priority level for the new thread. Usually the threads are
 *             created with priority \p NORMALPRIO, priorities
 *             can range from \p LOWPRIO to \p HIGHPRIO.
 * @param pf the thread function. Returning from this function automatically
 *           terminates the thread.
 * @param arg an argument passed to the thread function. It can be \p NULL.
 * @return the pointer to the \p Thread structure allocated for the
 *         thread into the working space area.
 * @note A thread can terminate by calling \p chThdExit() or by simply
 *       returning from its main function.
 */
Thread *chThdCreateStatic(void *workspace, size_t wsize,
                          tprio_t prio, tfunc_t pf, void *arg) {

  return chThdResume(chThdInit(workspace, wsize, prio, pf, arg));
}

#if defined(CH_USE_DYNAMIC) && defined(CH_USE_WAITEXIT) && defined(CH_USE_HEAP)
/**
 * Creates a new thread allocating the memory from the heap.
 * @param wsize size of the working area to be allocated
 * @param prio the priority level for the new thread. Usually the threads are
 *             created with priority \p NORMALPRIO, priorities
 *             can range from \p LOWPRIO to \p HIGHPRIO.
 * @param pf the thread function. Returning from this function automatically
 *           terminates the thread.
 * @param arg an argument passed to the thread function. It can be \p NULL.
 * @return the pointer to the \p Thread structure allocated for the
 *         thread into the working space area or \p NULL if the memory cannot
 *         be allocated.
 * @note A thread can terminate by calling \p chThdExit() or by simply
 *       returning from its main function.
 * @note The memory allocated for the thread is not released when the thread
 *       terminates but when a \p chThdWait() is performed.
 * @note The function is available only if the \p CH_USE_DYNAMIC,
 *       \p CH_USE_HEAP and \p CH_USE_WAITEXIT options are enabled
 *       in \p chconf.h.
 */
Thread *chThdCreateFromHeap(size_t wsize, tprio_t prio,
                            tfunc_t pf, void *arg) {

  void *workspace = chHeapAlloc(wsize);
  if (workspace == NULL)
    return NULL;
  Thread *tp = chThdInit(workspace, wsize, prio, pf, arg);
  tp->p_flags = P_MEM_MODE_HEAP;
  return chThdResume(tp);
}
#endif /* defined(CH_USE_DYNAMIC) && defined(CH_USE_WAITEXIT) && defined(CH_USE_HEAP) */

#if defined(CH_USE_DYNAMIC) && defined(CH_USE_WAITEXIT) && defined(CH_USE_MEMPOOLS)
/**
 * Creates a new thread allocating the memory from the specified Memory Pool.
 * @param mp the memory pool
 * @param prio the priority level for the new thread. Usually the threads are
 *             created with priority \p NORMALPRIO, priorities
 *             can range from \p LOWPRIO to \p HIGHPRIO.
 * @param pf the thread function. Returning from this function automatically
 *           terminates the thread.
 * @param arg an argument passed to the thread function. It can be \p NULL.
 * @return the pointer to the \p Thread structure allocated for the
 *         thread into the working space area or \p NULL if the memory cannot
 *         be allocated.
 * @note A thread can terminate by calling \p chThdExit() or by simply
 *       returning from its main function.
 * @note The memory allocated for the thread is not released when the thread
 *       terminates but when a \p chThdWait() is performed.
 * @note The function is available only if the \p CH_USE_DYNAMIC,
 *       \p CH_USE_MEMPOOLS and \p CH_USE_WAITEXIT options are enabled
 *       in \p chconf.h.
 */
Thread *chThdCreateFromMemoryPool(MemoryPool *mp, tprio_t prio,
                                  tfunc_t pf, void *arg) {

  void *workspace = chPoolAlloc(mp);
  if (workspace == NULL)
    return NULL;
  Thread *tp = chThdInit(workspace, mp->mp_object_size, prio, pf, arg);
  tp->p_flags = P_MEM_MODE_MEMPOOL;
  tp->p_mpool = mp;
  return chThdResume(tp);
}
#endif /* defined(CH_USE_DYNAMIC) && defined(CH_USE_WAITEXIT) && defined(CH_USE_MEMPOOLS) */

/**
 * Creates a new thread.
 * @param prio the priority level for the new thread. Usually the threads are
 *             created with priority \p NORMALPRIO, priorities
 *             can range from \p LOWPRIO to \p HIGHPRIO.
 * @param mode the creation option flags for the thread. The following options
 *             can be OR'ed in this parameter:<br>
 *             <ul>
 *             <li>\p P_SUSPENDED, the thread is created in the
 *                 \p PRSUSPENDED state and a subsequent call to
 *                 \p chThdResume() will make it ready for
 *                 execution.</li>
 *             <li>\p P_TERMINATED, this flag is usually set
 *                 by the \p chThdTerminate() function and it is not
 *                 normally used as parameter for this function. The
 *                 result would be to create a thread with a termination
 *                 request already pending.</li>
 *             </ul>
 * @param workspace pointer to a working area dedicated to the thread stack
 * @param wsize size of the working area.
 * @param pf the thread function. Returning from this function automatically
 *           terminates the thread.
 * @param arg an argument passed to the thread function. It can be \p NULL.
 * @return the pointer to the \p Thread structure allocated for the
 *         thread into the working space area.
 * @note A thread can terminate by calling \p chThdExit() or by simply
 *       returning from its main function.
 * @deprecated Please use \p chThdCreateStatic() or \p chThdInit() instead,
 *             this function will be removed in version 1.0.0.
 */
Thread *chThdCreate(tprio_t prio, tmode_t mode, void *workspace,
                    size_t wsize, tfunc_t pf, void *arg) {

  Thread *tp = chThdInit(workspace, wsize, prio, pf, arg);
  if (mode & P_SUSPENDED)
    return tp;
  return chThdResume(tp);
}

/**
 * Changes the running thread priority, reschedules if necessary.
 *
 * @param newprio the new priority of the running thread
 */
void chThdSetPriority(tprio_t newprio) {

  chDbgAssert(newprio <= HIGHPRIO, "chthreads.c, chThdSetPriority()");
  chSysLock();

#ifdef CH_USE_MUTEXES
  if (currp->p_prio != currp->p_realprio) {
    if (newprio > currp->p_prio)
      currp->p_prio = newprio;
  }
  else
    currp->p_prio = newprio;
  currp->p_realprio = newprio;
#else
  currp->p_prio = newprio;
#endif
  chSchRescheduleS();

  chSysUnlock();
}

/**
 * Suspends the invoking thread.
 *
 * @param tpp pointer to a \p Thread pointer, the \p Thread pointer is set
 *            to point to the suspended process before it enters the
 *            \p PRSUSPENDED state, it is set to \p NULL after it is resumed.
 *            This allows to implement a "test and resume" on the variable
 *            into interrupt handlers.
 */
void chThdSuspend(Thread **tpp) {

  chSysLock();
  chDbgAssert(*tpp == NULL, "chthreads.c, chThdSuspend()");
  *tpp = currp;
  chSchGoSleepS(PRSUSPENDED);
  *tpp = NULL;
  chSysUnlock();
}

/**
 * Resumes a suspended thread.
 * @param tp the pointer to the thread
 * @return the pointer to the thread
 */
Thread *chThdResume(Thread *tp) {

  chSysLock();
  chDbgAssert(tp->p_state == PRSUSPENDED, "chthreads.c, chThdResume()");
  chSchWakeupS(tp, RDY_OK);
  chSysUnlock();
  return tp;
}

/**
 * Requests a thread termination.
 * @param tp the pointer to the thread
 * @note The thread is not termitated but a termination request is added to
 *       its \p p_flags field. The thread can read this status by
 *       invoking \p chThdShouldTerminate() and then terminate cleanly.
 */
void chThdTerminate(Thread *tp) {

  chSysLock();
  tp->p_flags |= P_TERMINATE;
  chSysUnlock();
}

/**
 * Terminates the current thread by specifying an exit status code.
 *
 * @param msg the thread exit code. The code can be retrieved by using
 *            \p chThdWait().
 */
void chThdExit(msg_t msg) {

  chSysLock();

  currp->p_exitcode = msg;
#ifdef CH_USE_WAITEXIT
  while (notempty(&currp->p_waiting))
    chSchReadyI(list_remove(&currp->p_waiting));
#endif
#ifdef CH_USE_EXIT_EVENT
  chEvtBroadcastI(&currp->p_exitesource);
#endif
  chSchGoSleepS(PREXIT);
}

#ifdef CH_USE_WAITEXIT
/**
 * Blocks the execution of the invoking thread until the specified thread
 * terminates then the exit code is returned.
 * The memory used by the exited thread is handled in different ways depending
 * on the API that spawned the thread:
 * <ul>
 * <li>If the thread was spawned by \p chThdCreateStatic() or by \p chThdInit()
 *     then nothing happens and the thread working area is not released or
 *     modified in any way. This is the default, totally static, behavior.</li>
 * <li>If the thread was spawned by \p chThdCreateFromHeap() then the working
 *     area is returned to the system heap.</li>
 * <li>If the thread was spawned by \p chThdCreateFromMemoryPool() then the
 *     working area is returned to the owning memory pool.</li>
 * </ul>
 * @param tp the thread pointer
 * @return the exit code from the terminated thread
 * @note After invoking \p chThdWait() the thread pointer becomes invalid and
 *       must not be used as parameter for further system calls.
 * @note The function is available only if the \p CH_USE_WAITEXIT
 *       option is enabled in \p chconf.h.
 */
msg_t chThdWait(Thread *tp) {
  msg_t msg;

  chSysLock();
  chDbgAssert((tp != NULL) && (tp != currp), "chthreads.c, chThdWait()");
  if (tp->p_state != PREXIT) {
    list_insert(currp, &tp->p_waiting);
    chSchGoSleepS(PRWAIT);
  }
  msg = tp->p_exitcode;
#ifndef CH_USE_DYNAMIC
  chSysUnlock();
  return msg;
#else /* CH_USE_DYNAMIC */
  if (notempty(&tp->p_waiting)) {
    chSysUnlock();
    return msg;
  }

  /* This is the last thread waiting for termination, returning memory.*/
  tmode_t mode = tp->p_flags & P_MEM_MODE_MASK;
  chSysUnlock();

  switch (mode) {
#ifdef CH_USE_HEAP
  case P_MEM_MODE_HEAP:
    chHeapFree(tp);
    break;
#endif
#ifdef CH_USE_MEMPOOLS
  case P_MEM_MODE_MEMPOOL:
    chPoolFree(tp->p_mpool, tp);
    break;
#endif
  }
  return msg;
#endif /* CH_USE_DYNAMIC */
}
#endif /* CH_USE_WAITEXIT */

/** @} */
