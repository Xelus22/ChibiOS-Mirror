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

#include <ch.h>

#include "test.h"

#if CH_USE_EVENTS

#define ALLOWED_DELAY MS2ST(5)

static EventSource es1, es2;

static char *evt1_gettest(void) {

  return "Events, wait and broadcast";
}

static void evt1_setup(void) {

  chEvtClear(ALL_EVENTS);
}

static msg_t thread(void *p) {

  chEvtBroadcast(&es1);
  chThdSleepMilliseconds(50);
  chEvtBroadcast(&es2);
  return 0;
}

static void evt1_execute(void) {
  eventmask_t m;
  EventListener el1, el2;
  systime_t target_time;

  /*
   * Test on chEvtWaitOne().
   */
  chEvtPend(5);
  m = chEvtWaitOne(ALL_EVENTS);
  test_assert(m == 1, "#1"); /* Single bit error.*/
  m = chEvtWaitOne(ALL_EVENTS);
  test_assert(m == 4, "#2"); /* Single bit error.*/
  m = chEvtClear(0);
  test_assert(m == 0, "#3"); /* Stuck event.*/

  /*
   * Test on chEvtWaitAny().
   */
  chEvtPend(5);
  m = chEvtWaitAny(ALL_EVENTS);
  test_assert(m == 5, "#4"); /* Unexpected pending.*/
  m = chEvtClear(0);
  test_assert(m == 0, "#5"); /* Stuck event.*/

  /*
   * Test on chEvtWaitAll(), chEvtRegisterMask() and chEvtUnregister().
   */
  chEvtInit(&es1);
  chEvtInit(&es2);
  chEvtRegisterMask(&es1, &el1, 1);
  chEvtRegisterMask(&es2, &el2, 4);
  target_time = chSysGetTime() + MS2ST(50);
  threads[0] = chThdCreateStatic(wa[0], WA_SIZE, chThdGetPriority()-1, thread, "A");
  m = chEvtWaitAll(5);
  test_assert_time_window(target_time, target_time + ALLOWED_DELAY);
  m = chEvtClear(0);
  test_assert(m == 0, "#6"); /* Stuck event.*/

  test_wait_threads();
  chEvtUnregister(&es1, &el1);
  chEvtUnregister(&es2, &el2);
  test_assert(!chEvtIsListening(&es1), "#7"); /* Stuck listener.*/
  test_assert(!chEvtIsListening(&es2), "#8"); /* Stuck listener.*/
}

const struct testcase testevt1 = {
  evt1_gettest,
  evt1_setup,
  NULL,
  evt1_execute
};

#endif /* CH_USE_EVENTS */

/*
 * Test sequence for events pattern.
 */
const struct testcase * const patternevt[] = {
#if CH_USE_EVENTS
  &testevt1,
#endif
  NULL
};