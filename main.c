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
#include "RTE_Components.h"
#include CMSIS_device_header
#include "uart_tracelib.h"

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

#if 0
static const char *demo_single_input =
    "print('hello world!', list(x + 1 for x in range(10)), end='eol\\n')";

static const char *demo_file_input =
    "import micropython\n"
    "\n"
    "print(dir(micropython))\n"
    "\n"
    "for i in range(10):\n"
    "    print('iter {:08}'.format(i))";

static void do_str(const char *src, mp_parse_input_kind_t input_kind) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        // Compile, parse and execute the given string.
        mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src, strlen(src), 0);
        qstr source_name = lex->source_name;
        mp_parse_tree_t parse_tree = mp_parse(lex, input_kind);
        mp_obj_t module_fun = mp_compile(&parse_tree, source_name, true);
        mp_call_function_0(module_fun);
        nlr_pop();
    } else {
        // Uncaught exception: print it out.
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
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

// Main entry point: initialise the runtime and execute demo strings.
//void bare_main(void) {
int main(void) {
    // VTOR must be copied into RAM to enable dynamic setting of interrupt handler
    // This can be removed once build time vector table is fixed to include MHU interrupts
    copy_vtor_table_to_ram();

    int ret = tracelib_init(NULL);


    while (1)
    	tracef("bare_main starts\n");
#if 0
    mp_init();
    do_str(demo_single_input, MP_PARSE_SINGLE_INPUT);
    do_str(demo_file_input, MP_PARSE_FILE_INPUT);
    mp_deinit();
#endif
    (void)ret;
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
