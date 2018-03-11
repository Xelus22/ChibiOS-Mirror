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
 * @file pwm.h
 * @brief PWM Driver macros and structures.
 * @addtogroup PWM
 * @{
 */

#ifndef _PWM_H_
#define _PWM_H_

#if CH_HAL_USE_PWM || defined(__DOXYGEN__)

/**
 * @brief Driver state machine possible states.
 */
typedef enum {
  PWM_UNINIT = 0,                       /**< @brief Not initialized.        */
  PWM_STOP = 1,                         /**< @brief Stopped.                */
  PWM_READY = 2,                        /**< @brief Ready.                  */
} pwmstate_t;

/**
 * @brief PWM edge for callbacks.
 */
typedef enum {
  PWM_NONE = 0,                         /**< @brief No callback.            */
  PWM_TO_ACTIVE_EDGE = 1,               /**< @brief Enable on idle->active. */
  PWM_TO_IDLE_EDGE = 2,                 /**< @brief Enable on active->idle. */
  PWM_BOTH_EDGES = 3                    /**< @brief Enable on both edges.   */
} pwmedge_t;

/**
 * @brief PWM logic mode.
 */
typedef enum {
  PWM_ACTIVE_LOW = 0,                   /**< @brief Idle is logic level 1.  */
  PWM_ACTIVE_HIGH = 1                   /**< @brief Idle is logic level 0.  */
} pwmmode_t;

/**
 * @brief PWM notification callback type.
 *
 * @param[in] active    current channel output state
 */
typedef void (*pwmcallback_t)(bool_t active);

#include "pwm_lld.h"

#ifdef __cplusplus
extern "C" {
#endif
  void pwmInit(void);
  void pwmObjectInit(PWMDriver *pwmp);
  void pwmStart(PWMDriver *pwmp, const PWMConfig *config);
  void pwmStop(PWMDriver *pwmp);
  void pwmSetCallback(PWMDriver *pwmp,
                      pwmchannel_t channel,
                      pwmedge_t edge,
                      pwmcallback_t callback);
  void pwmEnableChannel(PWMDriver *pwmp,
                        pwmchannel_t channel,
                        pwmcnt_t width);
  void pwmDisableChannel(PWMDriver *pwmp, pwmchannel_t channel);
#ifdef __cplusplus
}
#endif

#endif /* CH_HAL_USE_PWM */

#endif /* _PWM_H_ */

/** @} */