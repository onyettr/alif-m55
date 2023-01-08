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

#include "py/builtin.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "shared/runtime/pyexec.h"

#include "RTE_Components.h"
#include CMSIS_device_header
#include "uart_tracelib.h"

extern void alif_evaluation_led_setup(uint8_t led);
extern void alif_evaluation_board_led_toggle(uint8_t led);
extern void alif_evaluation_board_led_enable(uint8_t led);
extern void alif_evaluation_board_led_disable(uint8_t led);
extern char _get_char(void);

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
#endif

#if 0 // MICROPY_ENABLE_COMPILER
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

static char *stack_top;
#if MICROPY_ENABLE_GC
static char heap[2048];
#endif

#if 0
// Send string of given length to stdout, converting \n to \r\n.
void mp_hal_stdout_tx_strn_cooked(const char *str, size_t len) {
    while (len--) {
        if (*str == '\n') {
            printf("\r");
        }
        printf("%c",*str++);
    }
}
#endif

mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    mp_raise_OSError(MP_ENOENT);
}

mp_import_stat_t mp_import_stat(const char *path) {
    return MP_IMPORT_STAT_NO_EXIST;
}

// Send string of given length
void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
#if MICROPY_MIN_USE_STDOUT
    int r = write(STDOUT_FILENO, str, len);
    (void)r;
#elif MICROPY_MIN_USE_STM32_MCU
#error No STM32 for us!
#else
    printf(str);
#endif
}

int mp_hal_stdin_rx_chr(void) {
    unsigned char c = 0;
#if MICROPY_MIN_USE_STDOUT
    int r = read(STDIN_FILENO, &c, 1);
    (void)r;
#elif MICROPY_MIN_USE_STM32_MCU
#error No STM32 for us!
    #else
    c = _get_char();
#endif
    return c;
}

void copy_vtor_table_to_ram(void)
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

#if MICROPY_ENABLE_GC
void gc_collect(void) {
    // WARNING: This gc_collect implementation doesn't try to get root
    // pointers from CPU registers, and thus may function incorrectly.
    void *dummy;
    gc_collect_start();
    gc_collect_root(&dummy, ((mp_uint_t)stack_top - (mp_uint_t)&dummy) / sizeof(mp_uint_t));
    gc_collect_end();
    gc_dump_info();
}
#endif

// Main entry point: initialise the runtime and execute demo strings.
int main(void) {
  int error_code;
  stack_top = (char *)&error_code;

  /**
   * ALIF specific setup
   */
  copy_vtor_table_to_ram();
  error_code = tracelib_init(NULL);
  alif_evaluation_led_setup(14);
  alif_evaluation_board_led_enable(14);

#if MICROPY_ENABLE_GC
  gc_init(heap, heap + sizeof(heap));
#endif
  mp_init();

#if MICROPY_ENABLE_COMPILER
#if MICROPY_REPL_EVENT_DRIVEN
  pyexec_event_repl_init();
  for (;;) {
    int c = mp_hal_stdin_rx_chr();
    if (pyexec_event_repl_process_char(c)) {
        break;
    }
  }
#else
  pyexec_friendly_repl();
#endif
// do_str("print('hello world!', list(x+1 for x in range(10)), end='eol\\n')", MP_PARSE_SINGLE_INPUT);
// do_str("for i in range(10):\r\n  print(i)", MP_PARSE_FILE_INPUT);
#else
  pyexec_frozen_module("frozentest.py");
#endif
  mp_deinit();

  printf("M55_55 uPython ENDS\n");
  alif_evaluation_board_led_disable(14);

  (void)error_code;

  return 0;
}

#if !MICROPY_DEBUG_PRINTERS
// With MICROPY_DEBUG_PRINTERS disabled DEBUG_printf is not defined but it
// is still needed by esp-open-lwip for debugging output, so define it here.
#include <stdarg.h>
int mp_vprintf(const mp_print_t *print, const char *fmt, va_list args);
int DEBUG_printf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int ret = mp_vprintf(MICROPY_DEBUG_PRINTER, fmt, ap);
    va_end(ap);
    return ret;
}
#endif

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
