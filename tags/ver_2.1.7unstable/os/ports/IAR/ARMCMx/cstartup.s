/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010 Giovanni Di Sirio.

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

        MODULE  ?cstartup

CONTROL_MODE_PRIVILEGED SET 0
CONTROL_MODE_UNPRIVILEGED SET 1
CONTROL_USE_MSP SET 0
CONTROL_USE_PSP SET 2

        AAPCS INTERWORK, VFP_COMPATIBLE, ROPI
        PRESERVE8

        SECTION CSTACK:DATA:NOROOT(3)
        SECTION .intvec:CODE:NOROOT(3)

        PUBLIC  __iar_program_start
        EXTERN  __vector_table
        EXTWEAK __iar_init_core
        EXTWEAK __iar_init_vfp
        EXTERN  __cmain

        SECTION .text:CODE:REORDER(2)
        REQUIRE __vector_table
        THUMB
__iar_program_start:
        cpsid   i
        ldr     r0, =sfe(CSTACK)
        msr     PSP, r0
        movs    r0, #CONTROL_MODE_PRIVILEGED | CONTROL_USE_PSP
        msr     CONTROL, r0
        isb
        bl      __early_init
        bl      __iar_init_core
        bl      __iar_init_vfp
        b       __cmain

        PUBWEAK __early_init
__early_init:
        bx      lr

        SECTION SYSHEAP:DATA:NOROOT(3)
        PUBLIC  __heap_base__
__heap_base__:
        DS32    2

        END