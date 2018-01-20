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

#include "hal.h"
#include "oslib_test_root.h"

/**
 * @file    oslib_test_sequence_002.c
 * @brief   Test Sequence 002 code.
 *
 * @page oslib_test_sequence_002 [2] Memory Pools
 *
 * File: @ref oslib_test_sequence_002.c
 *
 * <h2>Description</h2>
 * This sequence tests the ChibiOS library functionalities related to
 * memory pools.
 *
 * <h2>Conditions</h2>
 * This sequence is only executed if the following preprocessor condition
 * evaluates to true:
 * - CH_CFG_USE_MEMPOOLS
 * .
 *
 * <h2>Test Cases</h2>
 * - @subpage oslib_test_002_001
 * - @subpage oslib_test_002_002
 * - @subpage oslib_test_002_003
 * .
 */

#if (CH_CFG_USE_MEMPOOLS) || defined(__DOXYGEN__)

/****************************************************************************
 * Shared code.
 ****************************************************************************/

#define MEMORY_POOL_SIZE 4

static uint32_t objects[MEMORY_POOL_SIZE];
static MEMORYPOOL_DECL(mp1, sizeof (uint32_t), PORT_NATURAL_ALIGN, NULL);

#if CH_CFG_USE_SEMAPHORES
static GUARDEDMEMORYPOOL_DECL(gmp1, sizeof (uint32_t), PORT_NATURAL_ALIGN);
#endif

static void *null_provider(size_t size, unsigned align) {

  (void)size;
  (void)align;

  return NULL;
}

/****************************************************************************
 * Test cases.
 ****************************************************************************/

/**
 * @page oslib_test_002_001 [2.1] Loading and emptying a memory pool
 *
 * <h2>Description</h2>
 * The memory pool functionality is tested by loading and emptying it,
 * all conditions are tested.
 *
 * <h2>Test Steps</h2>
 * - [2.1.1] Adding the objects to the pool using chPoolLoadArray().
 * - [2.1.2] Emptying the pool using chPoolAlloc().
 * - [2.1.3] Now must be empty.
 * - [2.1.4] Adding the objects to the pool using chPoolFree().
 * - [2.1.5] Emptying the pool using chPoolAlloc() again.
 * - [2.1.6] Now must be empty again.
 * - [2.1.7] Covering the case where a provider is unable to return
 *   more memory.
 * .
 */

static void oslib_test_002_001_setup(void) {
  chPoolObjectInit(&mp1, sizeof (uint32_t), NULL);
}

static void oslib_test_002_001_execute(void) {
  unsigned i;

  /* [2.1.1] Adding the objects to the pool using chPoolLoadArray().*/
  test_set_step(1);
  {
    chPoolLoadArray(&mp1, objects, MEMORY_POOL_SIZE);
  }

  /* [2.1.2] Emptying the pool using chPoolAlloc().*/
  test_set_step(2);
  {
    for (i = 0; i < MEMORY_POOL_SIZE; i++)
      test_assert(chPoolAlloc(&mp1) != NULL, "list empty");
  }

  /* [2.1.3] Now must be empty.*/
  test_set_step(3);
  {
    test_assert(chPoolAlloc(&mp1) == NULL, "list not empty");
  }

  /* [2.1.4] Adding the objects to the pool using chPoolFree().*/
  test_set_step(4);
  {
    for (i = 0; i < MEMORY_POOL_SIZE; i++)
      chPoolFree(&mp1, &objects[i]);
  }

  /* [2.1.5] Emptying the pool using chPoolAlloc() again.*/
  test_set_step(5);
  {
    for (i = 0; i < MEMORY_POOL_SIZE; i++)
      test_assert(chPoolAlloc(&mp1) != NULL, "list empty");
  }

  /* [2.1.6] Now must be empty again.*/
  test_set_step(6);
  {
    test_assert(chPoolAlloc(&mp1) == NULL, "list not empty");
  }

  /* [2.1.7] Covering the case where a provider is unable to return
     more memory.*/
  test_set_step(7);
  {
    chPoolObjectInit(&mp1, sizeof (uint32_t), null_provider);
    test_assert(chPoolAlloc(&mp1) == NULL, "provider returned memory");
  }
}

static const testcase_t oslib_test_002_001 = {
  "Loading and emptying a memory pool",
  oslib_test_002_001_setup,
  NULL,
  oslib_test_002_001_execute
};

#if (CH_CFG_USE_SEMAPHORES) || defined(__DOXYGEN__)
/**
 * @page oslib_test_002_002 [2.2] Loading and emptying a guarded memory pool without waiting
 *
 * <h2>Description</h2>
 * The memory pool functionality is tested by loading and emptying it,
 * all conditions are tested.
 *
 * <h2>Conditions</h2>
 * This test is only executed if the following preprocessor condition
 * evaluates to true:
 * - CH_CFG_USE_SEMAPHORES
 * .
 *
 * <h2>Test Steps</h2>
 * - [2.2.1] Adding the objects to the pool using
 *   chGuardedPoolLoadArray().
 * - [2.2.2] Emptying the pool using chGuardedPoolAllocTimeout().
 * - [2.2.3] Now must be empty.
 * - [2.2.4] Adding the objects to the pool using chGuardedPoolFree().
 * - [2.2.5] Emptying the pool using chGuardedPoolAllocTimeout() again.
 * - [2.2.6] Now must be empty again.
 * .
 */

static void oslib_test_002_002_setup(void) {
  chGuardedPoolObjectInit(&gmp1, sizeof (uint32_t));
}

static void oslib_test_002_002_execute(void) {
  unsigned i;

  /* [2.2.1] Adding the objects to the pool using
     chGuardedPoolLoadArray().*/
  test_set_step(1);
  {
    chGuardedPoolLoadArray(&gmp1, objects, MEMORY_POOL_SIZE);
  }

  /* [2.2.2] Emptying the pool using chGuardedPoolAllocTimeout().*/
  test_set_step(2);
  {
    for (i = 0; i < MEMORY_POOL_SIZE; i++)
      test_assert(chGuardedPoolAllocTimeout(&gmp1, TIME_IMMEDIATE) != NULL, "list empty");
  }

  /* [2.2.3] Now must be empty.*/
  test_set_step(3);
  {
    test_assert(chGuardedPoolAllocTimeout(&gmp1, TIME_IMMEDIATE) == NULL, "list not empty");
  }

  /* [2.2.4] Adding the objects to the pool using
     chGuardedPoolFree().*/
  test_set_step(4);
  {
    for (i = 0; i < MEMORY_POOL_SIZE; i++)
      chGuardedPoolFree(&gmp1, &objects[i]);
  }

  /* [2.2.5] Emptying the pool using chGuardedPoolAllocTimeout()
     again.*/
  test_set_step(5);
  {
    for (i = 0; i < MEMORY_POOL_SIZE; i++)
      test_assert(chGuardedPoolAllocTimeout(&gmp1, TIME_IMMEDIATE) != NULL, "list empty");
  }

  /* [2.2.6] Now must be empty again.*/
  test_set_step(6);
  {
    test_assert(chGuardedPoolAllocTimeout(&gmp1, TIME_IMMEDIATE) == NULL, "list not empty");
  }
}

static const testcase_t oslib_test_002_002 = {
  "Loading and emptying a guarded memory pool without waiting",
  oslib_test_002_002_setup,
  NULL,
  oslib_test_002_002_execute
};
#endif /* CH_CFG_USE_SEMAPHORES */

#if (CH_CFG_USE_SEMAPHORES) || defined(__DOXYGEN__)
/**
 * @page oslib_test_002_003 [2.3] Guarded Memory Pools timeout
 *
 * <h2>Description</h2>
 * The timeout features for the Guarded Memory Pools is tested.
 *
 * <h2>Conditions</h2>
 * This test is only executed if the following preprocessor condition
 * evaluates to true:
 * - CH_CFG_USE_SEMAPHORES
 * .
 *
 * <h2>Test Steps</h2>
 * - [2.3.1] Trying to allocate with 100mS timeout, must fail because
 *   the pool is empty.
 * .
 */

static void oslib_test_002_003_setup(void) {
  chGuardedPoolObjectInit(&gmp1, sizeof (uint32_t));
}

static void oslib_test_002_003_execute(void) {

  /* [2.3.1] Trying to allocate with 100mS timeout, must fail because
     the pool is empty.*/
  test_set_step(1);
  {
    test_assert(chGuardedPoolAllocTimeout(&gmp1, TIME_MS2I(100)) == NULL, "list not empty");
  }
}

static const testcase_t oslib_test_002_003 = {
  "Guarded Memory Pools timeout",
  oslib_test_002_003_setup,
  NULL,
  oslib_test_002_003_execute
};
#endif /* CH_CFG_USE_SEMAPHORES */

/****************************************************************************
 * Exported data.
 ****************************************************************************/

/**
 * @brief   Array of test cases.
 */
const testcase_t * const oslib_test_sequence_002_array[] = {
  &oslib_test_002_001,
#if (CH_CFG_USE_SEMAPHORES) || defined(__DOXYGEN__)
  &oslib_test_002_002,
#endif
#if (CH_CFG_USE_SEMAPHORES) || defined(__DOXYGEN__)
  &oslib_test_002_003,
#endif
  NULL
};

/**
 * @brief   Memory Pools.
 */
const testsequence_t oslib_test_sequence_002 = {
  "Memory Pools",
  oslib_test_sequence_002_array
};

#endif /* CH_CFG_USE_MEMPOOLS */
