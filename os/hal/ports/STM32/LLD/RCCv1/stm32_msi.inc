/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

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

/**
 * @file    RCCv1/stm32_msi.inc
 * @brief   Shared MSI clock handler.
 *
 * @addtogroup STM32_MSI_HANDLER
 * @{
 */

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/**
 * @name    RCC_CR register bits definitions
 * @{
 */
#define STM32_MSIRANGE_MASK     (15U << RCC_CR_MSIRANGE_Pos)
#define STM32_MSIRANGE_100K     (0U << RCC_CR_MSIRANGE_Pos)
#define STM32_MSIRANGE_200K     (1U << RCC_CR_MSIRANGE_Pos)
#define STM32_MSIRANGE_400K     (2U << RCC_CR_MSIRANGE_Pos)
#define STM32_MSIRANGE_800K     (3U << RCC_CR_MSIRANGE_Pos)
#define STM32_MSIRANGE_1M       (4U << RCC_CR_MSIRANGE_Pos)
#define STM32_MSIRANGE_2M       (5U << RCC_CR_MSIRANGE_Pos)
#define STM32_MSIRANGE_4M       (6U << RCC_CR_MSIRANGE_Pos)
#define STM32_MSIRANGE_8M       (7U << RCC_CR_MSIRANGE_Pos)
#define STM32_MSIRANGE_16M      (8U << RCC_CR_MSIRANGE_Pos)
#define STM32_MSIRANGE_24M      (9U << RCC_CR_MSIRANGE_Pos)
#define STM32_MSIRANGE_32M      (10U << RCC_CR_MSIRANGE_Pos)
#define STM32_MSIRANGE_48M      (11U << RCC_CR_MSIRANGE_Pos)
/** @} */

/**
 * @name    RCC_CSR register bits definitions
 * @{
 */
#define STM32_MSISRANGE_MASK    (15U << RCC_CSR_MSISRANGE_Pos)
#define STM32_MSISRANGE_1M      (4U << RCC_CSR_MSISRANGE_Pos)
#define STM32_MSISRANGE_2M      (5U << RCC_CSR_MSISRANGE_Pos)
#define STM32_MSISRANGE_4M      (6U << RCC_CSR_MSISRANGE_Pos)
#define STM32_MSISRANGE_8M      (7U << RCC_CSR_MSISRANGE_Pos)
/** @} */

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/* Registry checks for robustness.*/
#if !defined(STM32_RCC_HAS_MSI)
#error "STM32_RCC_HAS_MSI not defined in stm32_registry.h"
#endif

/* Checks on configurations.*/
#if !defined(STM32_MSIPLL_ENABLED)
#error "STM32_MSIPLL_ENABLED not defined in mcuconf.h"
#endif

#if !defined(STM32_MSIRANGE)
#error "STM32_MSIRANGE not defined in mcuconf.h"
#endif

#if !defined(STM32_MSISRANGE)
#error "STM32_MSISRANGE not defined in mcuconf.h"
#endif

#if !defined(STM32_LSE_ENABLED)
#error "STM32_LSE_ENABLED not defined in mcuconf.h"
#endif

#if STM32_MSIPLL_ENABLED && !STM32_LSE_ENABLED
#error "STM32_MSIPLL_ENABLED requires LSE"
#endif

/**
 * @brief   MSI frequency.
 */
#if STM32_MSIRANGE == STM32_MSIRANGE_100K
  #define STM32_MSICLK              100000U

#elif STM32_MSIRANGE == STM32_MSIRANGE_200K
  #define STM32_MSICLK              200000U

#elif STM32_MSIRANGE == STM32_MSIRANGE_400K
  #define STM32_MSICLK              400000U

#elif STM32_MSIRANGE == STM32_MSIRANGE_800K
  #define STM32_MSICLK              800000U

#elif STM32_MSIRANGE == STM32_MSIRANGE_1M
  #define STM32_MSICLK              1000000U

#elif STM32_MSIRANGE == STM32_MSIRANGE_2M
  #define STM32_MSICLK              2000000U

#elif STM32_MSIRANGE == STM32_MSIRANGE_4M
  #define STM32_MSICLK              4000000U

#elif STM32_MSIRANGE == STM32_MSIRANGE_8M
  #define STM32_MSICLK              8000000U

#elif STM32_MSIRANGE == STM32_MSIRANGE_16M
  #define STM32_MSICLK              16000000U

#elif STM32_MSIRANGE == STM32_MSIRANGE_24M
  #define STM32_MSICLK              24000000U

#elif STM32_MSIRANGE == STM32_MSIRANGE_32M
  #define STM32_MSICLK              32000000U

#elif STM32_MSIRANGE == STM32_MSIRANGE_48M
  #define STM32_MSICLK              48000000U

#else
  #error "invalid STM32_MSIRANGE value specified"
#endif

/**
 * @brief   MSIS frequency.
 */
#if STM32_MSISRANGE == STM32_MSISRANGE_1M
  #define STM32_MSISCLK             1000000U

#elif STM32_MSISRANGE == STM32_MSISRANGE_2M
  #define STM32_MSISCLK             2000000U

#elif STM32_MSISRANGE == STM32_MSISRANGE_4M
  #define STM32_MSISCLK             4000000U

#elif STM32_MSISRANGE == STM32_MSISRANGE_8M
  #define STM32_MSISCLK             8000000U

#else
  #error "invalid STM32_MSISRANGE value specified"
#endif

/* Some headers do not have this definition.*/
#if !defined(RCC_CFGR_SWS_MSI)
#define RCC_CFGR_SWS_MSI            0U
#endif

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

static inline void msi_reset(void) {

  /* Resetting to the MSI clock in case we come here after an initialization,
     because a debugger for example.*/
  RCC->CR = RCC_CR_MSION;
  while ((RCC->CR & RCC_CR_MSIRDY) == 0U) {
    /* Wait until MSI is stable.*/
  }

  /* Clocking from MSI, in case MSI was not the default source.*/
  RCC->CFGR = 0U;
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_MSI) {
    /* Wait until MSI is selected.*/
  }
}

static inline void msi_init(void) {
  uint32_t cr, csr;

  /* Initial clocks setup and wait for MSI stabilization, the MSI clock is
     always enabled because it is the fall back clock when PLL the fails.
     Trim fields are not altered from reset values.*/

  /* MSIRANGE can be set only when MSI is OFF or READY, it is ready after
     reset.*/
#if STM32_MSIPLL_ENABLED
  cr = STM32_MSIRANGE | RCC_CR_MSIPLLEN | RCC_CR_MSION;
#else
  cr = STM32_MSIRANGE | RCC_CR_MSION;
#endif
  RCC->CR = cr;
  while ((RCC->CR & RCC_CR_MSIRDY) == 0U) {
    /* Wait until MSI is stable.*/
  }

  /* Clocking from MSI, in case MSI was not the default source.*/
  RCC->CFGR = 0U;
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_MSI)
    ;                                       /* Wait until MSI is selected.  */

  /* Updating MSISRANGE value. MSISRANGE can be set only when MSIRGSEL is high.
     This range is used exiting the Standby mode until MSIRGSEL is set.*/
  cr      |= RCC_CR_MSIRGSEL;
  RCC->CR  = cr;

  csr      = RCC->CSR;
  csr     &= ~STM32_MSISRANGE_MASK;
  csr     |= STM32_MSISRANGE;
  RCC->CSR = csr;
}

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/** @} */
