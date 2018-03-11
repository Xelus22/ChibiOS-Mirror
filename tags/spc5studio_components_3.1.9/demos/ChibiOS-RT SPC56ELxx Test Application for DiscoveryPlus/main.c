/*
    ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/* Inclusion of the main header files of all the imported components in the
   order specified in the application wizard. The file is generated
   automatically.*/
#include "components.h"

#include "shellcmd.h"

/*
 * Shell configuration structure, the first field is the serial port used by
 * the shell, the second is the array of commands accepted by the shell and
 * defined in shellcmd.c.
 */
static const ShellConfig shell_cfg1 = {
  (BaseSequentialStream *)&SD1,
  shell_commands
};

/*
 * LEDs blinker thread, times are in milliseconds.
 */
static WORKING_AREA(waThread1, 128);
static msg_t Thread1(void *arg) {

  (void)arg;
  chRegSetThreadName("blinker");

  while (TRUE) {
    unsigned i;

    for (i = 0; i < 4; i++) {
      palClearPad(PORT_A, PA_LED5);
      osalThreadSleepMilliseconds(100);
      palClearPad(PORT_A, PA_LED6);
      osalThreadSleepMilliseconds(100);
      palClearPad(PORT_G, PG_LED7);
      osalThreadSleepMilliseconds(100);
      palSetPad(PORT_A, PA_LED5);
      osalThreadSleepMilliseconds(100);
      palSetPad(PORT_A, PA_LED6);
      osalThreadSleepMilliseconds(100);
      palSetPad(PORT_G, PG_LED7);
      osalThreadSleepMilliseconds(300);
    }

    for (i = 0; i < 4; i++) {
      palTogglePad(PORT_A, PA_LED5);
      osalThreadSleepMilliseconds(250);
      palTogglePad(PORT_A, PA_LED5);
      palTogglePad(PORT_A, PA_LED6);
      osalThreadSleepMilliseconds(250);
      palTogglePad(PORT_A, PA_LED6);
      palTogglePad(PORT_G, PG_LED7);
      osalThreadSleepMilliseconds(250);
      palTogglePad(PORT_G, PG_LED7);
    }

    palSetPort(PORT_A, PAL_PORT_BIT(PA_LED5) | PAL_PORT_BIT(PA_LED6));
    palSetPort(PORT_G, PAL_PORT_BIT(PG_LED7));
  }
  return 0;
}

/*
 * Application entry point.
 */
int main(void) {
  Thread *shelltp = NULL;

  /* Initialization of all the imported components in the order specified in
     the application wizard. The function is generated automatically.*/
  componentsInit();

  /*
   * Activates the serial driver 1 using the driver default configuration.
   */
  sdStart(&SD1, NULL);

  /*
   * Creates the blinker thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

  /* Application main loop.*/
  while (1) {
    if (!shelltp)
      shelltp = shellCreate(&shell_cfg1, SHELL_WA_SIZE, NORMALPRIO);
    else if (chThdTerminated(shelltp)) {
      chThdRelease(shelltp);    /* Recovers memory of the previous shell.   */
      shelltp = NULL;           /* Triggers spawning of a new shell.        */
    }
    chThdSleepMilliseconds(1000);
  }
}