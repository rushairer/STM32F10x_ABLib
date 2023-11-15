#ifndef __STM32F10X_ABL_IAP_H
#define __STM32F10X_ABL_IAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"

#define PAGE_SIZE        (0x400)   /* 1 Kbyte */
#define FLASH_SIZE       (0x20000) /* 128 KBytes */

#define FILE_NAME_LENGTH (256)
#define FILE_SIZE_LENGTH (16)

#define IAP_Connector    void *
typedef void (*pIAPExecuteApplicationFunction)(void);
typedef uint8_t (*pIAPReceiveDataFunction)(IAP_Connector, uint8_t *);
typedef void (*pIAPOutputStringFunction)(IAP_Connector, char *);

typedef enum {
    IAP_COMMAND_ANY                = 0x00,
    IAP_COMMAND_DOWNLOAD           = 0x01,
    IAP_COMMAND_UPLOAD             = 0x02,
    IAP_COMMAND_EXECUTE            = 0x03,
    IAP_COMMAND_DISABLE_PROTECTION = 0x04,
} IAP_Command;

typedef struct {
    uint32_t ApplicationAddress;
    uint32_t ExecuteAddress;
    pIAPExecuteApplicationFunction ExecuteApplication;
    uint32_t BlockNumber;
    uint32_t UserMemoryMask;
    uint32_t FlashProtection;
    IAP_Connector Connector;
    pIAPReceiveDataFunction ReceiveData;
    pIAPOutputStringFunction OutputString;

    uint32_t FlashDestination;
    uint32_t RamSource;
    uint8_t FileName[FILE_NAME_LENGTH];
    uint8_t Tab1024[1024];
} IAP_InitTypeDef;

void IAP_Init(
    IAP_InitTypeDef *IAPx,
    uint32_t ApplicationAddress,
    IAP_Connector Connector,
    pIAPReceiveDataFunction ReceiveData,
    pIAPOutputStringFunction OutputString);

void IAP_ShowMenu(IAP_InitTypeDef *IAPx);
int8_t IAP_Download(IAP_InitTypeDef *IAPx);
uint8_t IAP_Execute(IAP_InitTypeDef *IAPx);

void IAP_SendByte(IAP_InitTypeDef *IAPx, uint8_t Byte);
void IAP_OutputString(IAP_InitTypeDef *IAPx, char *String);
int8_t IAP_ReceiveDatat(IAP_InitTypeDef *IAPx, uint8_t *Data);

#ifdef __cplusplus
}
#endif

#endif