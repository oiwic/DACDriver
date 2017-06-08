/*
	FileName:USTCDACError.h
	Author:GuoCheng
	E-mail:fortune@mail.ustc.edu.cn
	All right reserved @ GuoCheng.
	Modified: 2017.4.3
	Description: Error define file of PC software.
*/

#pragma once

// Define format error code form of PC.
#define FORMAT(NO) (0xE1800000|NO)

// Message ID:OK
// Message text:
// No error and return seccessfully
#define OK 0

// Message ID:ERR_NOOBJ
// Message text:
// No object find
#define ERR_NOOBJ FORMAT(1)

// Message ID:ERR_WAIT
// Message text:
// WaitForSingleObject error
#define ERR_WAIT FORMAT(2)

// Message ID:ERR_PARA
// Message text:
// Parameter error
#define ERR_PARA FORMAT(3)

// Message ID:ERR_OUTRANGE
// Message text:
// The retrieve index out of range.
#define ERR_OUTRANGE FORMAT(4)

// Message ID:ERR_NOFUNC
// Message text:
// The task does not exsist.
#define ERR_NOFUNC FORMAT(5)

// Message ID:ERR_SOCKET
// Message text:
// Socket error, detail from data.
#define ERR_SOCKET FORMAT(6)