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
 * @file mempools.h
 * @brief Memory Pools macros and structures.
 * @addtogroup MemoryPools
 * @{
 */

#ifndef _MEMPOOLS_H_
#define _MEMPOOLS_H_

#if CH_USE_MEMPOOLS

/**
 * @brief Memory pool free object header.
 */
struct pool_header {
  struct pool_header    *ph_next;
};

/**
 * @brief Memory pool descriptor.
 */
typedef struct {
  struct pool_header    *mp_next;       /**< Pointer to the header.*/
  size_t                mp_object_size; /**< Memory pool objects size.*/
} MemoryPool;

/**
 * @brief Data part of a static memory pool initializer.
 * @details This macro should be used when statically initializing a
 *          memory pool that is part of a bigger structure.
 * @param name the name of the memory pool variable
 * @param size size of the memory pool contained objects
 */
#define _MEMORYPOOL_DATA(name, size) {NULL, size}

/**
 * @brief Static memory pool initializer.
 * @details Statically initialized memory pools require no explicit
 *          initialization using @p chPoolInit().
 * @param name the name of the memory pool variable
 * @param size size of the memory pool contained objects
 */
#define MEMORYPOOL_DECL(name, size)                                     \
  MemoryPool name = _MEMORYPOOL_DATA(name, size)

#ifdef __cplusplus
extern "C" {
#endif
  void chPoolInit(MemoryPool *mp, size_t size);
  void *chPoolAllocI(MemoryPool *mp);
  void *chPoolAlloc(MemoryPool *mp);
  void chPoolFreeI(MemoryPool *mp, void *objp);
  void chPoolFree(MemoryPool *mp, void *objp);
#ifdef __cplusplus
}
#endif

#endif /* CH_USE_MEMPOOLS */

#endif /* _MEMPOOLS_H_ */

/** @} */