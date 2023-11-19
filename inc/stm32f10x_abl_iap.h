#ifndef __STM32F10X_ABL_IAP_H
#define __STM32F10X_ABL_IAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"
#include "stm32f10x_abl_serial.h"

#define PAGE_SIZE               (0x400)   /* 1 Kbyte */
#define FLASH_SIZE              (0x20000) /* 128 KBytes */

#define IAP_FLAG_ADDR           BKP_DR1
#define INIT_FLAG_DATA          0x0000 // 默认标志的数据(空片子的情况)

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

typedef void (*pIAPExecuteApplicationFunction)(void);

typedef enum {
    IAP_COMMAND_WAITING            = 0x00,
    IAP_COMMAND_DOWNLOAD           = 0x01,
    IAP_COMMAND_UPLOAD             = 0x02,
    IAP_COMMAND_EXECUTE            = 0x03,
    IAP_COMMAND_DISABLE_PROTECTION = 0x04,
} IAP_Command;

typedef struct {
    SERIAL_InitTypeDef *Serial;

    uint32_t ApplicationAddress;
    uint32_t ExecuteAddress;
    pIAPExecuteApplicationFunction ExecuteApplication;
    uint32_t BlockNumber;
    uint32_t UserMemoryMask;
    uint32_t FlashProtection;

    uint32_t FlashDestination;
    uint32_t RamSource;
    uint8_t FileName[FILE_NAME_LENGTH];
    uint8_t Tab1024[1024];
} IAP_InitTypeDef;

void IAP_Init(
    IAP_InitTypeDef *IAPx,
    SERIAL_InitTypeDef *Serial,
    uint32_t ApplicationAddress);

void IAP_ShowMenu(IAP_InitTypeDef *IAPx);
int8_t IAP_Download(IAP_InitTypeDef *IAPx);
uint8_t IAP_Execute(IAP_InitTypeDef *IAPx);
uint8_t IAP_ErasePages(IAP_InitTypeDef *IAPx, __IO uint32_t Size, uint8_t OutPutCont);

void IAP_WriteFlag(IAP_InitTypeDef *IAPx, uint16_t Flag);
uint16_t IAP_ReadFlag(IAP_InitTypeDef *IAPx);

void IAP_OutputData(IAP_InitTypeDef *IAPx, char *String);
int32_t IAP_ReceiveData(IAP_InitTypeDef *IAPx, uint8_t *Data);
void IAP_GetInputString(IAP_InitTypeDef *IAPx, uint8_t *Buf);

#ifdef __cplusplus
}
#endif

#endif
