/**
 * @file
 * @brief Error codes
 *
 */
/*
 * Error codes used throughout the OBC
 *
 * Jakob Kjaer <makob@makob.dk>
 *
 */

#ifndef _ERROR_H_
#define _ERROR_H_

/**
 * \addtogroup Error
 *
 * @{
 */



/* ************************************************************************ */

#define E_NO_ERR -1
#define E_NO_DEVICE -2
#define E_MALLOC_FAIL -3
#define E_THREAD_FAIL -4
#define E_NO_QUEUE -5
#define E_INVALID_BUF_SIZE -6
#define E_INVALID_PARAM -7
#define E_NO_SS -8
#define E_GARBLED_BUFFER -9
#define E_FLASH_ERROR -10
#define E_BOOT_SER -13
#define E_BOOT_DEBUG -14
#define E_BOOT_FLASH -15
#define E_TIMEOUT -16
#define E_NO_BUFFER -17
#define E_OUT_OF_MEM -18

/* ************************************************************************ */

//! Prototypes
char *error_string(int code);

/* ************************************************************************ */
/**
 * }@
 */
#endif
