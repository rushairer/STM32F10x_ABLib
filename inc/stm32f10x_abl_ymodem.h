#ifndef __STM32F10X_ABL_YMODEM_H
#define __STM32F10X_ABL_YMODEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"
#include "stm32f10x_abl_serial.h"

#define PACKET_SEQNO_INDEX      (1)
#define PACKET_SEQNO_COMP_INDEX (2)

#define PACKET_HEADER           (3)
#define PACKET_TRAILER          (2)
#define PACKET_OVERHEAD         (PACKET_HEADER + PACKET_TRAILER)
#define PACKET_8B_SIZE          (8)
#define PACKET_16B_SIZE         (16)
#define PACKET_32B_SIZE         (32)
#define PACKET_64B_SIZE         (64)
#define PACKET_128B_SIZE        (128)
#define PACKET_256B_SIZE        (256)
#define PACKET_512B_SIZE        (512)
#define PACKET_1KB_SIZE         (1024)
#define PACKET_2KB_SIZE         (2048)

#define FILE_NAME_LENGTH        (256)
#define FILE_SIZE_LENGTH        (16)

#define SOH                     (0x01) /* start of 128-byte data packet */
#define STX                     (0x02) /* start of 1024-byte data packet */
#define STX_8B                  (0xA1)
#define STX_16B                 (0xA2)
#define STX_32B                 (0xA3)
#define STX_64B                 (0xA4) /* start of 64-byte data packet */
#define STX_128B                (0xA5)
#define STX_256B                (0xA6)
#define STX_512B                (0xA7)
#define STX_1KB                 (0xA8)
#define STX_2KB                 (0XA9)

#define EOT                     (0x04) /* end of transmission */
#define ACK                     (0x06) /* acknowledge */
#define NAK                     (0x15) /* negative acknowledge */
#define CA                      (0x18) /* two of these in succession aborts transfer */
#define CRC16                   (0x43) /* 'C' == 0x43, request 16-bit CRC */

#define ABORT1                  (0x41) /* 'A' == 0x41, abort by user */
#define ABORT2                  (0x61) /* 'a' == 0x61, abort by user */

#define NAK_TIMEOUT             (0x100000)
#define MAX_ERRORS              (5)

/* YMODEM文件传输过程
 * 发送端                                接收端
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<   C
 * SOH 00 FF [filename 00] [filesize 00] [NUL..] CRCH CRCL >>>
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<   C
 * SOH 00 FF [filename 00] [filesize 00] [NUL..] CRCH CRCL >>>
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<   ACK
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<   C
 * STX 01 FE data[1024] CRC CRC>>>>>>>>>>>>>>
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<   ACK
 * STX 02 FD data[1024] CRC CRC>>>>>>>>>>>>>>
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<   ACK
 * STX 03 FC data[1024] CRC CRC>>>>>>>>>>>>>>
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<   ACK
 * STX 04 FB data[1024] CRC CRC>>>>>>>>>>>>>>
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<   ACK
 * SOH 05 FA data[100]  1A[28] CRC CRC>>>>>>>
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<   ACK
 * EOT >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<   NAK
 * EOT>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<   ACK
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<   C
 * SOH 00 FF NUL[128] CRC CRC >>>>>>>>>>>>>>>
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<   ACK
 */

#define IAPHandler void *

typedef uint8_t (*pYmodemErasePageFunction)(IAPHandler, __IO uint32_t, uint8_t);
typedef uint8_t (*pYmodemSaveApplicationDataFunction)(IAPHandler, uint32_t, uint32_t, uint32_t);

typedef struct {
    SERIAL_InitTypeDef *Serial;
    int32_t MaxSize;
    pYmodemErasePageFunction ErasePageFunction;
    pYmodemSaveApplicationDataFunction SaveApplicationDataFunction;
} YMODEM_InitTypeDef;

void YMODEM_Init(
    YMODEM_InitTypeDef *Ymodem,
    SERIAL_InitTypeDef *Serial,
    int32_t MaxSize,
    pYmodemErasePageFunction ErasePageFunction,
    pYmodemSaveApplicationDataFunction SaveApplicationDataFunction);

int32_t YMODEM_ReceiveData(YMODEM_InitTypeDef *Ymodem, uint8_t *FileName, uint8_t *Data);

#ifdef __cplusplus
}
#endif

#endif
