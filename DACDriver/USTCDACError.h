/*
	FileName:USTCDACError.h
	Author:GuoCheng
	E-mail:fortune@mail.ustc.edu.cn
	All right reserved @ GuoCheng.
	Modified: 2017.6.20
	Description: Error define file of PC software.
*/

#pragma once

// Message ID:USERDEF
// Message text:
// The Error Message is define by user.
#define USERDEF (0x20000000)

// Message ID:SCR_PC
// Message text:
// The Error occur on PC
#define SCR_PC (0x04000000)


// Message ID:ERRORLEVEL
// Message text:
// Error level
#define ERRORLEVEL (0xC0000000)

// Message ID:WARNINGLEVEL
// Message text:
// Warning level
#define WARNINGLEVEL (0x80000000)

// Message ID:INFOLEVEL
// Message text:
// Info level
#define INFOLEVEL (0x40000000)

// Define format error code of PC.
#define ERRORCODE(NO) (SCR_PC|ERRORLEVEL|USERDEF|NO)

// Define format warning code of PC.
#define WARNINGCODE(NO) (SCR_PC|WARNINGLEVEL|USERDEF|NO)

// Define format info code of PC.
#define INFOCODE(NO) (SCR_PC|INFOLEVEL|USERDEF|NO)

// Message ID:OK
// Message text:
// No error and return seccessfully
#define OK 0

// Message ID:ERR_ERR
// Message text:
// Undefined error code. 
#define ERR_ERR ERRORCODE(0)

// Message ID:ERR_NOOBJ
// Message text:
// No object find
#define ERR_NOOBJ ERRORCODE(1)

// Message ID:ERR_WAIT
// Message text:
// WaitForSingleObject error
#define ERR_WAIT ERRORCODE(2)

// Message ID:ERR_PARA
// Message text:
// Parameter(s) error.
#define ERR_PARA ERRORCODE(3)

// Message ID:ERR_OUTRANGE
// Message text:
// The retrieve index out of range.
#define ERR_OUTRANGE ERRORCODE(4)

// Message ID:ERR_NOFUNC
// Message text:
// The task does not exist.
#define ERR_NOFUNC ERRORCODE(5)

// Message ID:ERR_NOEXEC
// Message text:
// The task does not exec.
#define ERR_NOEXEC ERRORCODE(6)

// Message ID:ERR_WAITAB
// Message text:
// WaitForSingleObject abandoned.
#define ERR_WAITAB ERRORCODE(7)

// Message ID:ERR_NODATA
// Message text:
// The command does not has extra data.
#define ERR_NODATA ERRORCODE(8)


// Message ID:WAR_TIMEOUT
// Message text:
// The task(s) timeout
#define WAR_TIMEOUT WARNINGCODE(1)