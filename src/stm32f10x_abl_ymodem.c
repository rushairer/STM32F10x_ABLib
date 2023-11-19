#include "stm32f10x_abl_ymodem.h"
#include "stm32f10x_abl_delay.h"
#include <string.h>
#include <stdio.h>
#include "stm32f10x_abl_strings.h"

void YMODEM_Init(
    YMODEM_InitTypeDef *Ymodem,
    SERIAL_InitTypeDef *Serial,
    int32_t MaxSize,
    pYmodemErasePageFunction ErasePageFunction,
    pYmodemSaveApplicationDataFunction SaveApplicationDataFunction)
{
    Ymodem->Serial                      = Serial;
    Ymodem->MaxSize                     = MaxSize;
    Ymodem->ErasePageFunction           = ErasePageFunction;
    Ymodem->SaveApplicationDataFunction = SaveApplicationDataFunction;
}

// 1: Success  0: Timeout
int32_t YMODEM_ReceiveByte(YMODEM_InitTypeDef *Ymodem, uint8_t *Byte, uint32_t Timeout)
{
    while (Timeout-- > 0) {
        if (SERIAL_ReceiveByte(Ymodem->Serial, Byte) == 1) {
            return 1;
        }
    }
    return 0;
}

// 1: Success 0: Timeout or Error -1: Abort by user
int32_t YMODEM_ReceivePacket(YMODEM_InitTypeDef *Ymodem, uint8_t *Data, int32_t *Length, uint32_t Timeout)
{
    uint16_t i, packetSize;
    uint8_t byte;
    *Length = 0;
    if (YMODEM_ReceiveByte(Ymodem, &byte, Timeout) != 1) {
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
            if ((YMODEM_ReceiveByte(Ymodem, &byte, Timeout) == 1) && (byte == CA)) {
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
        if (YMODEM_ReceiveByte(Ymodem, Data + i, Timeout) != 1) {
            return 0;
        }
    }
    if (Data[PACKET_SEQNO_INDEX] != ((Data[PACKET_SEQNO_COMP_INDEX] ^ 0xff) & 0xff)) {
        return 0;
    }
    *Length = packetSize;
    return 1;
}

uint8_t YMODEM_ErasePages(YMODEM_InitTypeDef *Ymodem, __IO uint32_t Size, uint8_t OutPutCont)
{
    return Ymodem->ErasePageFunction(Ymodem, Size, OutPutCont);
}

uint8_t YMODEM_SaveApplicationData(YMODEM_InitTypeDef *Ymodem, uint32_t PacketLength, uint32_t Size, uint32_t RamSource)
{
    return Ymodem->SaveApplicationDataFunction(Ymodem, PacketLength, Size, RamSource);
}

int32_t YMODEM_ReceiveData(YMODEM_InitTypeDef *Ymodem, uint8_t *FileName, uint8_t *Data)
{
    uint8_t packetData[PACKET_1KB_SIZE + PACKET_OVERHEAD], fileSize[FILE_SIZE_LENGTH], *filePtr, *bufPtr;
    int32_t i, packetLength, sessionDone, fileDone, packetsReceived, errors, sessionBegin, size = 0;

    SERIAL_SendByte(Ymodem->Serial, CRC16);

    for (sessionDone = 0, errors = 0, sessionBegin = 0;;) {
        for (packetsReceived = 0, fileDone = 0, bufPtr = Data;;) {
            switch (YMODEM_ReceivePacket(Ymodem, packetData, &packetLength, NAK_TIMEOUT)) {
                case 1: // 成功收到数据
                    errors = 0;
                    switch (packetLength) {
                        /* Abort by sender */
                        case -1: // 接收失败
                            SERIAL_SendByte(Ymodem->Serial, ACK);
                            return 0;
                        /* End of transmission */
                        case 0: // 本次文件传送结束
                            SERIAL_SendByte(Ymodem->Serial, ACK);
                            fileDone = 1;
                            break;
                        /* Normal packet */
                        default: // 接收成功
                            if ((packetData[PACKET_SEQNO_INDEX] & 0xff) != (packetsReceived & 0xff)) {
                                SERIAL_SendByte(Ymodem->Serial, NAK);
                            } else {
                                if (packetsReceived == 0) // 文件信息(首包)
                                {
                                    /* Filename packet */
                                    if (packetData[PACKET_HEADER] != 0) // 文件名字
                                    {
                                        /* Filename packet has valid data */
                                        for (i = 0, filePtr = packetData + PACKET_HEADER; (*filePtr != 0) && (i < FILE_NAME_LENGTH);) {
                                            FileName[i++] = *filePtr++; // 保存文件名字
                                        }
                                        FileName[i++] = '\0'; // 字符串形式
                                        for (i = 0, filePtr++; (*filePtr != ' ') && (i < FILE_SIZE_LENGTH);) {
                                            fileSize[i++] = *filePtr++; // 文件大小
                                        }
                                        fileSize[i++] = '\0';
                                        STRINGS_Str2Int(fileSize, &size);

                                        /* Test the size of the image to be sent */
                                        /* Image size is greater than Flash size */
                                        if (size > (Ymodem->MaxSize - 1)) {
                                            /* End session */
                                            SERIAL_SendByte(Ymodem->Serial, CA);
                                            SERIAL_SendByte(Ymodem->Serial, CA);
                                            return -1;
                                        }

                                        /* Erase the needed pages where the user application will be loaded */
                                        /* Define the number of page to be erased */
                                        if (YMODEM_ErasePages(Ymodem, size, 0)) {
                                        } else {
                                            /* End session */
                                            SERIAL_SendByte(Ymodem->Serial, CA);
                                            SERIAL_SendByte(Ymodem->Serial, CA);
                                            return -1;
                                            // Erase failed
                                        }
                                        SERIAL_SendByte(Ymodem->Serial, ACK);
                                        SERIAL_SendByte(Ymodem->Serial, CRC16);
                                    }
                                    /* Filename packet is empty, end session */
                                    else {
                                        SERIAL_SendByte(Ymodem->Serial, ACK);
                                        fileDone    = 1;
                                        sessionDone = 1;
                                        break;
                                    }
                                }
                                /* Data packet */
                                else // 文件信息保存完之后开始接收数据
                                {
                                    memcpy(bufPtr, packetData + PACKET_HEADER, packetLength);

                                    if (YMODEM_SaveApplicationData(Ymodem, packetLength, size, (uint32_t)Data) != 1) {
                                        /* End session */
                                        SERIAL_SendByte(Ymodem->Serial, CA);
                                        SERIAL_SendByte(Ymodem->Serial, CA);
                                        return -2;
                                    }

                                    SERIAL_SendByte(Ymodem->Serial, ACK);
                                }
                                packetsReceived++;
                                sessionBegin = 1;
                            }
                    }
                    break;
                case -1: // 用户按下了'a'或'A'
                    SERIAL_SendByte(Ymodem->Serial, CA);
                    SERIAL_SendByte(Ymodem->Serial, CA);
                    return -3;
                default: // 检查错误
                    if (sessionBegin > 0) {
                        errors++;
                    }
                    if (errors > MAX_ERRORS) {
                        SERIAL_SendByte(Ymodem->Serial, CA);
                        SERIAL_SendByte(Ymodem->Serial, CA);
                        return 0;
                    }
                    SERIAL_SendByte(Ymodem->Serial, CRC16); // 发送校验值
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
