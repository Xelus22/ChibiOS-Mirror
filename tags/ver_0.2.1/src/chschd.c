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
 * @addtogroup Scheduler
 * @{
 */

#include <ch.h>

/** @cond never*/

static ReadyList rlist;

#ifndef CH_CURRP_REGISTER_CACHE
Thread *currp;
#endif

static UWORD16 preempt;
#ifdef CH_USE_SYSTEMTIME
t_time stime;
#endif

/** @endcond */

/**
 * Scheduler initialization.
 * @note Internally invoked by the \p chSysInit().
 */
void chSchInit(void) {

  rlist.p_next = rlist.p_prev = (Thread *)&rlist;
  rlist.p_prio = MAXPRIO;
  preempt = CH_TIME_QUANTUM;
#ifdef CH_USE_SYSTEMTIME
  stime = 0;
#endif
}

/**
 * Inserts a thread in the Ready List.
 * @param tp the Thread to be made ready
 * @return the Thread pointer
 * @note The function must be called in the system mutex zone.
 * @note The function does not reschedule, the \p chSchRescheduleI() should
 *       be called soon after.
 * @note The function is not meant to be used in the user code directly.
 */
Thread *chSchReadyI(Thread *tp) {
  Thread *cp;
  t_prio prio = tp->p_prio;

  tp->p_state = PRREADY;
  tp->p_rdymsg = RDY_OK;
  cp = rlist.p_next;
  while (cp->p_prio < prio)
    cp = cp->p_next;
  tp->p_prev = (tp->p_next = cp)->p_prev;
  tp->p_prev->p_next = cp->p_prev = tp;
  return tp;
}

/*
 * Switches to the next thread in the ready list, the ready list is assumed
 * to contain at least a thread.
 */
static void nextready(void) {
  Thread *otp = currp;

  (currp = dequeue(rlist.p_prev))->p_state = PRCURR;
  preempt = CH_TIME_QUANTUM;
  chSysSwitchI(&otp->p_ctx, &currp->p_ctx);
}

/**
 * Puts the current thread to sleep into the specified state, the next highest
 * priority thread becomes running. The threads states are described into
 * \p threads.h
 * @param newstate the new thread state
 * @return the wakeup message
 * @note The function must be called in the system mutex zone.
 * @note The function is not meant to be used in the user code directly.
 */
void chSchGoSleepI(t_tstate newstate) {

  currp->p_state = newstate;
  nextready();
}

/**
 * Wakeups a thread, the thread is inserted into the ready list or made
 * running directly depending on its relative priority compared to the current
 * thread.
 * @param tp the Thread to be made ready
 * @param msg wakeup message to the awakened thread
 * @note The function must be called in the system mutex zone.
 * @note The function is not meant to be used in the user code directly.
 * @note It is equivalent to a \p chSchReadyI() followed by a
 *       \p chSchRescheduleI() but much more efficient.
 */
void chSchWakeupI(Thread *tp, t_msg msg) {
  Thread *ctp = currp;

  if (tp->p_prio <= ctp->p_prio)
    chSchReadyI(tp)->p_rdymsg = msg;
  else {
    chSchReadyI(ctp);
    (currp = tp)->p_state = PRCURR;
    tp->p_rdymsg = msg;
    preempt = CH_TIME_QUANTUM;
    chSysSwitchI(&ctp->p_ctx, &tp->p_ctx);
  }
}

/**
 * If a thread with an higher priority than the current thread is in the
 * ready list then it becomes running.
 * @note The function must be called in the system mutex zone.
 */
void chSchRescheduleI(void) {

  if (isempty(&rlist) || lastprio(&rlist) <= currp->p_prio)
    return;

  chSchDoRescheduleI();
}

/**
 * Performs a reschedulation. It is meant to be called if
 * \p chSchRescRequired() evaluates to \p TRUE.
 */
void chSchDoRescheduleI(void) {

  chSchReadyI(currp);
  nextready();
}

/**
 * Evaluates if a reschedulation is required.
 * @return \p TRUE if there is a thread that should go in running state
 *         immediatly else \p FALSE.
 */
BOOL chSchRescRequiredI(void) {

  if (isempty(&rlist))
    return FALSE;

  if (preempt) {
    if (lastprio(&rlist) <= currp->p_prio)
      return FALSE;
  }
  else { /* Time quantum elapsed. */
    if (lastprio(&rlist) < currp->p_prio)
      return FALSE;
  }
  return TRUE;
}

/**
 * Preemption routine, this function must be called into an interrupt
 * handler invoked by a system timer.
 * The frequency of the timer determines the system tick granularity and,
 * together with the \p CH_TIME_QUANTUM macro, the round robin interval.
 */
void chSchTimerHandlerI(void) {

  if (preempt)
    preempt--;

#ifdef CH_USE_SYSTEMTIME
  stime++;
#endif

#ifdef CH_USE_VIRTUAL_TIMERS
  chVTDoTickI();
#endif
}

/** @} */