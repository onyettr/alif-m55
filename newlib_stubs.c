/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/**
 * @file  newlib_stubs.c
 * @brief stubs for incomplete libc function definitions
 * @par
 */

/******************************************************************************
 *  I N C L U D E   F I L E S
 *****************************************************************************/
#include "newlib_stubs.h"

/*******************************************************************************
 *  M A C R O   D E F I N E S
 ******************************************************************************/

/*******************************************************************************
 *  T Y P E D E F S
 ******************************************************************************/

/*******************************************************************************
 *  G L O B A L   V A R I A B L E S
 ******************************************************************************/

/*******************************************************************************
 *  C O D E
 ******************************************************************************/

/**
 * @brief stub for newlib _close function
 * @param file
 */
int _close(int file)
{
  return -1;
}

/**
 * @brief stub for newlib close_r function
 * @param file
 */
int _close_r(int file)
{
  return -1;
}
/**
 * @brief stub for newlib _fstat function
 * @param file
 */
int _fstat(int file, struct stat *st)
{
  return 0;
}

/**
 * @brief stub for newlib _getpid function
 */
int _getpid(void)
{
  return 1;
}

/**
 * @brief stub for newlib _isatty function
 * @param file
 */
int _isatty(int file)
{
  return 1;
}

/**
 * @brief stub for newlib _kill function
 * @param file
 * @param sig
 */
int _kill(int pid, int sig)
{
  return -1;
}

/**
 * @brief stub for newlib _lseek function
 * @param file
 * @param ptr
 * @param dir
 */
int _lseek(int file, int ptr, int dir)
{
  return 0;
}

/**
 * @brief stub for newlib _read function
 * @param file
 * @param ptr
 * @param len
 */
int _read(int file, char *ptr, int len)
{
  return 0;
}

/**
 * @brief stub for newlib _write function
 * @param file
 * @param ptr
 * @param len
 */
int _write(int file, char *ptr, int len)
{
  return len;
}
