//! Error codes for OBC framework
/*
 * Error formatting
 *
 * Karl Kaas Laursen
 */

#include <util/error.h>

/* ************************************************************************ */

//! Error code string format
/**
 * Provide fancy debugging/AL error string
 * @param code error code
 * @return pointer to error string
 */
char *error_string(int code) {
	switch (code) {
	case E_NO_ERR:
		return "No error";
	case E_NO_DEVICE:
		return "No device";
	case E_MALLOC_FAIL:
		return "Malloc fail";
	case E_THREAD_FAIL:
		return "Thread failure";
	case E_NO_QUEUE:
		return "No such queue";
	case E_INVALID_BUF_SIZE:
		return "Invalid buffer size";
	case E_INVALID_PARAM:
		return "Invalid paramater";
	case E_NO_SS:
		return "No such subsystem";
	case E_GARBLED_BUFFER:
		return "Rubbish in buffer";
	case E_FLASH_ERROR:
		return "FLASH error";
	case E_BOOT_SER:
		return "Thread boot fail: serial driver";
	case E_BOOT_DEBUG:
		return "Thread boot fail: debug console";
	case E_BOOT_FLASH:
		return "Thread boot fail: flash driver";
	case E_NO_BUFFER:
		return "No buffer";

	default:
		return "Unknown error";
	}
}

/* ************************************************************************ */
