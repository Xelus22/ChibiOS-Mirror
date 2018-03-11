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

.text

.p2align 4,,15
.globl @chSysSwitchI@8
@chSysSwitchI@8:
        # Switch out
        push    %ebp
        push    %esi
        push    %edi
        push    %ebx
        movl    %esp,(%ecx)
        # Switch in
        movl    (%edx),%esp
        pop     %ebx
        pop     %edi
        pop     %esi
        pop     %ebp
        ret

.p2align 4,,15
.globl @threadstart@0
@threadstart@0:
        push    %eax
        call    _chThdExit