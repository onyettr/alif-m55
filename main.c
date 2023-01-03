/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014-2021 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <stdio.h>
#include <string.h>
#include "py/compile.h"
#include "py/runtime.h"
#include "py/compile.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/stackctrl.h"
#include "py/builtin.h"
#include "shared/runtime/gchelper.h"
#include "shared/runtime/pyexec.h"
#include "RTE_Components.h"
#include CMSIS_device_header
#include "uart_tracelib.h"

// Allocate memory for the MicroPython GC heap.
static char heap[4096];

#if   CPU == M55_HE || defined(M55_HE)
#include "M55_HE.h"
#elif CPU == M55_HP || defined(M55_HP)
#include "M55_HP.h"
#else
#error Target CPU is not defined
#endif

#if 0
/* Disable Semihosting */
#if __ARMCC_VERSION >= 6000000
    __asm(".global __use_no_semihosting");
#elif __ARMCC_VERSION >= 5000000
    #pragma import(__use_no_semihosting)
#else
    #error Unsupported compiler
#endif
#endif

// Send string of given length to stdout, converting \n to \r\n.
#if 0
void mp_hal_stdout_tx_strn_cooked(const char *str, size_t len) {
    while (len--) {
        if (*str == '\n') {
            printf("\r");
        }
        printf("%c",*str++);
    }
}
#endif

void copy_vtor_table_to_ram()
{
    extern const VECTOR_TABLE_Type __VECTOR_TABLE[496];
    static VECTOR_TABLE_Type MyVectorTable[496] __attribute__((aligned (2048)));

    for(int i = 0; i < 496; i++)
    {
        MyVectorTable[i] = __VECTOR_TABLE[i];
    }
    // Set the new vector table into use.
    SCB->VTOR = (uint32_t)(&MyVectorTable[0]);
    __DSB();
}

// Do a garbage collection cycle.
void gc_collect(void) {
    gc_collect_start();
    gc_helper_collect_regs_and_stack();
    gc_collect_end();
}

// Main entry point: initialise the runtime and execute demo strings.
//void bare_main(void) {
int main(void) {

  // VTOR must be copied into RAM to enable dynamic setting of interrupt handler
    // This can be removed once build time vector table is fixed to include MHU interrupts
    copy_vtor_table_to_ram();
    int ret = tracelib_init(NULL);

 // Initialise the MicroPython runtime.
   mp_stack_ctrl_init();
   gc_init(heap, heap + sizeof(heap));
   mp_init();
   // Start a normal REPL; will exit when ctrl-D is entered on a blank line.
   pyexec_friendly_repl();

   // Deinitialise the runtime.
   gc_sweep_all();
   mp_deinit();

   (void)ret;
   return 0;
}

// There is no filesystem so stat'ing returns nothing.
mp_import_stat_t mp_import_stat(const char *path) {
    return MP_IMPORT_STAT_NO_EXIST;
}

// There is no filesystem so opening a file raises an exception.
mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    mp_raise_OSError(MP_ENOENT);
}

// Called if an exception is raised outside all C exception-catching handlers.
void nlr_jump_fail(void *val) {
    for (;;) {
    }
}

#ifndef NDEBUG
// Used when debugging is enabled.
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    for (;;) {
    }
}
#endif
