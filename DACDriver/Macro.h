/*
	FileName:Macro.h
	Author:GuoCheng
	E-mail:fortune@mail.ustc.edu.cn
	All right reserved @ GuoCheng.
	Modified: 2017.2.15
	Description:
*/

#pragma once

/* Define the channel of a DAC */
#define CHANNEL_NUM 4

/* Define the sample point of a channel. */
#define CHANNEL_POT 32768

/* Define the sequency number of a channel. */
#define CHANNEL_SEQ 4096

/* Define max wait task num */
#define WAIT_TASK_MAX 256

/* Define run mode of software. */
#define PARALLEL 1
#define SERIAL 0

/* Define the status return codes */
#define STAT_SUCCESS          ( 0x0 )
#define STAT_ERROR            ( 0x1 )
#define STAT_CMDERR           ( 0x2 )
#define STAT_RDERR            ( 0x3 )
#define STAT_WRERR            ( 0x4 )
#define STAT_MEM_RANGE_ERR    ( 0x5 )
#define STAT_MEM_ALIGN_ERR    ( 0x6 )
#define STAT_SIZE_ERR         ( 0x7 )
#define STAT_SIZE_ALIGN_ERR   ( 0x8 )
#define STAT_ADDR_ALIGN_ERR   ( 0x9 )
#define STAT_DATAIF_BUSY      ( 0xA )
#define STAT_DATAIF_ERR       ( 0xB )
#define STAT_DMA_ERR          ( 0xC )
#define STAT_LAST             ( STAT_DMA_ERR )

/* Define software version */
#define DAC_DESCRIPTION ("DAC communication driver v1.0")
