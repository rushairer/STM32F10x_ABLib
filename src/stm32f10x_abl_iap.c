#include "stm32f10x_abl_iap.h"
#include "stm32f10x_abl_strings.h"
#include <string.h>

uint32_t FLASH_PagesMask(__IO uint32_t Size)
{
    uint32_t pagenumber = 0x0;
    uint32_t size       = Size;

    if ((size % PAGE_SIZE) != 0) {
        pagenumber = (size / PAGE_SIZE) + 1;
    } else {
        pagenumber = size / PAGE_SIZE;
    }
    return pagenumber;
}

void IAP_Init(
    IAP_InitTypeDef *IAPx,
    SERIAL_InitTypeDef *Serial,
    uint32_t ApplicationAddress)
{
    IAPx->Serial             = Serial;
    IAPx->ApplicationAddress = ApplicationAddress;

    RCC_APB1PeriphClockCmd(RCC_APB1ENR_PWREN | RCC_APB1ENR_BKPEN, ENABLE);

    memset(&(IAPx->Tab1024), 0, 1024);

    IAPx->BlockNumber = (IAPx->ApplicationAddress - 0x08000000) >> 12;
#if defined(STM32F10X_MD) || defined(STM32F10X_MD_VL)
    IAPx->UserMemoryMask = ((uint32_t) ~((1 << IAPx->BlockNumber) - 1));
#else /* USE_STM3210E_EVAL */
    if (IAPx->BlockNumber < 62) {
        IAPx->UserMemoryMask = ((uint32_t) ~((1 << IAPx->BlockNumber) - 1));
    } else {
        IAPx->UserMemoryMask = ((uint32_t)0x80000000);
    }
#endif
    if ((FLASH_GetWriteProtectionOptionByte() & IAPx->UserMemoryMask) != IAPx->UserMemoryMask) {
        IAPx->FlashProtection = 1;
    } else {
        IAPx->FlashProtection = 0;
    }

    IAPx->FlashDestination = ApplicationAddress;
}

void IAP_ShowMenu(IAP_InitTypeDef *IAPx)
{
    IAP_OutputData(IAPx, "\r\n========================= Main Menu ======================\r\n");
    IAP_OutputData(IAPx, "\r\n  Execute The Program ---------------------------------- 0\r\n");
    IAP_OutputData(IAPx, "\r\n  Download Image To the STM32F10x Internal Flash ------- 1\r\n");
    // IAP_OutputData(IAPx, "\r\n  Upload Image From the STM32F10x Internal Flash ------- 2\r\n");

    if (IAPx->FlashProtection != 0) {
        IAP_OutputData(IAPx, "\r\n  Disable the write protection ------------------------- 4\r\n");
    }

    IAP_OutputData(IAPx, "\r\n==========================================================\r\n");
}

void IAP_WriteFlag(IAP_InitTypeDef *IAPx, uint16_t Flag)
{
    PWR->CR |= PWR_CR_DBP;
    BKP_WriteBackupRegister(IAP_FLAG_ADDR, Flag);
    PWR->CR &= ~PWR_CR_DBP;
}

uint16_t IAP_ReadFlag(IAP_InitTypeDef *IAPx)
{
    return BKP_ReadBackupRegister(IAP_FLAG_ADDR);
}

uint8_t IAP_ErasePages(IAP_InitTypeDef *IAPx, __IO uint32_t Size, uint8_t OutPutCont)
{
    uint32_t EraseCounter    = 0x0;
    uint32_t NbrOfPage       = 0;
    uint8_t EraseCont[3]     = {0};
    FLASH_Status FLASHStatus = FLASH_COMPLETE;

    NbrOfPage = FLASH_PagesMask(Size);

    /* Erase the FLASH pages */
    FLASH_Unlock();
    for (EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++) {
        FLASHStatus = FLASH_ErasePage(IAPx->ApplicationAddress + (PAGE_SIZE * EraseCounter));
        if (OutPutCont == 1) {
            STRINGS_Int2Str(EraseCounter + 1, EraseCont);
            IAP_OutputData(IAPx, (char *)EraseCont);
            IAP_OutputData(IAPx, "@");
        }
    }
    FLASH_Lock();
    if ((EraseCounter != NbrOfPage) || (FLASHStatus != FLASH_COMPLETE)) {
        return 0;
    }
    return 1;
}

int8_t IAP_Download(IAP_InitTypeDef *IAPx)
{
    uint8_t Number[10] = "";
    int32_t Size       = 0;
    Size               = IAP_ReceiveData(IAPx, &(IAPx->Tab1024[0]));
    if (Size > 0) {
        IAP_OutputData(IAPx, "\r\nUpdate Over!\r\n");
        IAP_OutputData(IAPx, " Name: ");
        IAP_OutputData(IAPx, (char *)IAPx->FileName);
        STRINGS_Int2Str(Size, Number);
        IAP_OutputData(IAPx, "\r\nSize: ");
        IAP_OutputData(IAPx, (char *)Number);
        IAP_OutputData(IAPx, " Bytes.\r\n");
        return 0;
    } else if (Size == -1) {
        IAP_OutputData(IAPx, "\r\nImage Too Big!\r\n");
        return -1;
    } else if (Size == -2) {
        IAP_OutputData(IAPx, "\r\nUpdate failed!\r\n");
        return -2;
    } else if (Size == -3) {
        IAP_OutputData(IAPx, "\r\nAborted by user.\r\n");
        return -3;
    } else {
        IAP_OutputData(IAPx, "Receive Filed.\r\n");
        return -4;
    }
}

uint8_t IAP_Execute(IAP_InitTypeDef *IAPx)
{
    if (((*(__IO uint32_t *)IAPx->ApplicationAddress) & 0x2FFE0000) == 0x20000000) {
        IAPx->ExecuteAddress     = *(__IO uint32_t *)(IAPx->ApplicationAddress + 4);
        IAPx->ExecuteApplication = (pIAPExecuteApplicationFunction)IAPx->ExecuteAddress;
        __set_MSP(*(__IO uint32_t *)IAPx->ApplicationAddress);
        IAPx->ExecuteApplication();
        IAP_OutputData(IAPx, "\r\nExecuting...\r\n");

        return 1;
    } else {
        IAP_OutputData(IAPx, "\r\nExecute Failed!\r\n");
        return 0;
    }
}

void IAP_OutputData(IAP_InitTypeDef *IAPx, char *String)
{
    SERIAL_SendString(IAPx->Serial, String);
}

// 1: Success  0: Timeout
int32_t IAP_YmodemReceiveByte(IAP_InitTypeDef *IAPx, uint8_t *Byte, uint32_t Timeout)
{
    while (Timeout-- > 0) {
        if (SERIAL_ReceiveByte(IAPx->Serial, Byte) == 1) {
            return 1;
        }
    }
    return 0;
}

// 1: Success 0: Timeout or Error -1: Abort by user
int32_t IAP_YmodemReceivePacket(IAP_InitTypeDef *IAPx, uint8_t *Data, int32_t *Length, uint32_t Timeout)
{
    uint16_t i, packetSize;
    uint8_t byte;
    *Length = 0;
    if (IAP_YmodemReceiveByte(IAPx, &byte, Timeout) != 1) {
        return 0;
    }
    switch (byte) {
        case STX_8B:
            packetSize = PACKET_8B_SIZE;
            break;
        case STX_16B:
            packetSize = PACKET_16B_SIZE;
            break;
        case STX_32B:
            packetSize = PACKET_32B_SIZE;
            break;
        case STX_64B:
            packetSize = PACKET_64B_SIZE;
            break;
        case STX_128B:
        case SOH: // 为了兼容超级终端
            packetSize = PACKET_128B_SIZE;
            break;
        case STX_256B:
            packetSize = PACKET_256B_SIZE;
            break;
        case STX_512B:
            packetSize = PACKET_512B_SIZE;
            break;
        case STX_1KB:
        case STX: // 为了兼容超级终端
            packetSize = PACKET_1KB_SIZE;
            break;
        case STX_2KB:
            packetSize = PACKET_2KB_SIZE;
            break;
        case EOT:
            return 1;
        case CA:
            if ((IAP_YmodemReceiveByte(IAPx, &byte, Timeout) == 1) && (byte == CA)) {
                *Length = -1;
                return 1;
            } else {
                return 0;
            }
        case ABORT1:
        case ABORT2:
            return -1;
        default:
            return 0;
    }
    *Data = byte;
    for (i = 1; i < (packetSize + PACKET_OVERHEAD); i++) {
        if (IAP_YmodemReceiveByte(IAPx, Data + i, Timeout) != 1) {
            return 0;
        }
    }
    if (Data[PACKET_SEQNO_INDEX] != ((Data[PACKET_SEQNO_COMP_INDEX] ^ 0xff) & 0xff)) {
        return 0;
    }
    *Length = packetSize;

    return 1;
}

int32_t IAP_ReceiveData(IAP_InitTypeDef *IAPx, uint8_t *Data)
{

    IAPx->FlashDestination = IAPx->ApplicationAddress;

    uint8_t packetData[PACKET_1KB_SIZE + PACKET_OVERHEAD] = {0}, fileSize[FILE_SIZE_LENGTH], *filePtr, *bufPtr;
    int32_t i, j, packetLength, sessionDone, fileDone, packetsReceived, errors, sessionBegin, size = 0;

    SERIAL_SendByte(IAPx->Serial, CRC16);

    for (sessionDone = 0, errors = 0, sessionBegin = 0;;) {
        for (packetsReceived = 0, fileDone = 0, bufPtr = Data;;) {
            int32_t packet = IAP_YmodemReceivePacket(IAPx, packetData, &packetLength, NAK_TIMEOUT);
            switch (packet) {
                case 1: // 成功收到数据
                    errors = 0;
                    switch (packetLength) {
                        /* Abort by sender */
                        case -1: // 接收失败
                            SERIAL_SendByte(IAPx->Serial, ACK);
                            return 0;
                        /* End of transmission */
                        case 0: // 本次文件传送结束
                            SERIAL_SendByte(IAPx->Serial, ACK);
                            fileDone = 1;
                            break;
                        /* Normal packet */
                        default: // 接收成功
                            if ((packetData[PACKET_SEQNO_INDEX] & 0xff) != (packetsReceived & 0xff)) {
                                SERIAL_SendByte(IAPx->Serial, NAK);
                            } else {
                                if (packetsReceived == 0) // 文件信息(首包)
                                {
                                    /* Filename packet */
                                    if (packetData[PACKET_HEADER] != 0) // 文件名字
                                    {
                                        /* Filename packet has valid data */
                                        for (i = 0, filePtr = packetData + PACKET_HEADER; (*filePtr != 0) && (i < FILE_NAME_LENGTH);) {
                                            IAPx->FileName[i++] = *filePtr++; // 保存文件名字
                                        }
                                        IAPx->FileName[i++] = '\0'; // 字符串形式
                                        for (i = 0, filePtr++; (*filePtr != ' ') && (i < FILE_SIZE_LENGTH);) {
                                            fileSize[i++] = *filePtr++; // 文件大小
                                        }
                                        fileSize[i++] = '\0';
                                        STRINGS_Str2Int(fileSize, &size);

                                        /* Test the size of the image to be sent */
                                        /* Image size is greater than Flash size */
                                        if (size > (FLASH_SIZE - 1)) {
                                            /* End session */
                                            SERIAL_SendByte(IAPx->Serial, CA);
                                            SERIAL_SendByte(IAPx->Serial, CA);
                                            return -1;
                                        }

                                        /* Erase the needed pages where the user application will be loaded */
                                        /* Define the number of page to be erased */
                                        if (IAP_ErasePages(IAPx, size, 0)) {
                                        } else {
                                            /* End session */
                                            SERIAL_SendByte(IAPx->Serial, CA);
                                            SERIAL_SendByte(IAPx->Serial, CA);
                                            return -1;
                                            // Erase failed
                                        }
                                        SERIAL_SendByte(IAPx->Serial, ACK);
                                        SERIAL_SendByte(IAPx->Serial, CRC16);
                                    }
                                    /* Filename packet is empty, end session */
                                    else {
                                        SERIAL_SendByte(IAPx->Serial, ACK);
                                        fileDone    = 1;
                                        sessionDone = 1;
                                        break;
                                    }
                                }
                                /* Data packet */
                                else // 文件信息保存完之后开始接收数据
                                {
                                    memcpy(bufPtr, packetData + PACKET_HEADER, packetLength);
                                    IAPx->RamSource = (uint32_t)Data;
                                    for (j = 0; (j < packetLength) && (IAPx->FlashDestination < IAPx->ApplicationAddress + size); j += 4) {
                                        /* Program the data received into STM32F10x Flash */
                                        FLASH_Unlock();
                                        FLASH_ProgramWord(IAPx->FlashDestination, *(uint32_t *)IAPx->RamSource);
                                        FLASH_Lock();
                                        if (*(uint32_t *)IAPx->FlashDestination != *(uint32_t *)IAPx->RamSource) {
                                            return -2;
                                        }
                                        IAPx->FlashDestination += 4;
                                        IAPx->RamSource += 4;
                                    }
                                    SERIAL_SendByte(IAPx->Serial, ACK);
                                }
                                packetsReceived++;
                                sessionBegin = 1;
                            }
                    }
                    break;
                case -1: // 用户按下了'a'或'A'
                    SERIAL_SendByte(IAPx->Serial, CA);
                    SERIAL_SendByte(IAPx->Serial, CA);
                    return -3;
                default: // 检查错误
                    if (sessionBegin > 0) {
                        errors++;
                    }
                    if (errors > MAX_ERRORS) {
                        SERIAL_SendByte(IAPx->Serial, CA);
                        SERIAL_SendByte(IAPx->Serial, CA);
                        return 0;
                    }
                    SERIAL_SendByte(IAPx->Serial, CRC16); // 发送校验值
                    break;
            }
            if (fileDone != 0) {
                break;
            }
        }
        if (sessionDone != 0) // 文件发送完成
        {
            break;
        }
    }
    return (int32_t)size;
}

void IAP_GetInputString(IAP_InitTypeDef *IAPx, uint8_t *Buf)
{
    uint32_t bytesRead = 0;
    uint8_t byte       = 0;
    do {
        while (1) {
            if (SERIAL_ReceiveByte(IAPx->Serial, &byte) == 1) {
                break;
            }
        }
        if (byte == '\n') {
            if (Buf[bytesRead - 1] == '\r')
                break;
        }

        if (byte == '\b') /* Backspace */
        {
            if (bytesRead > 0) {
                bytesRead--;
            }
            continue;
        }
        if (bytesRead >= 128) {
            bytesRead = 0;
            continue;
        }
        if ((byte >= 0x20 && byte <= 0x7E) || byte == '\r') {
            Buf[bytesRead++] = byte;
        }
    } while (1);
    Buf[bytesRead - 1] = '\0';
}