#include "stm32f10x_abl_iap.h"
#include <string.h>

/* YMODEM文件传输过程
 * 发送端                                                   接收端
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

uint32_t Str2Int(uint8_t *inputstr, int32_t *intnum)
{
    uint32_t i = 0, res = 0;
    uint32_t val = 0;

    if (inputstr[0] == '0' && (inputstr[1] == 'x' || inputstr[1] == 'X')) {
        if (inputstr[2] == '\0') {
            return 0;
        }
        for (i = 2; i < 11; i++) {
            if (inputstr[i] == '\0') {
                *intnum = val;
                /* return 1; */
                res = 1;
                break;
            }
            if (ISVALIDHEX(inputstr[i])) {
                val = (val << 4) + CONVERTHEX(inputstr[i]);
            } else {
                /* return 0, Invalid input */
                res = 0;
                break;
            }
        }
        /* over 8 digit hex --invalid */
        if (i >= 11) {
            res = 0;
        }
    } else /* max 10-digit decimal input */
    {
        for (i = 0; i < 11; i++) {
            if (inputstr[i] == '\0') {
                *intnum = val;
                /* return 1 */
                res = 1;
                break;
            } else if ((inputstr[i] == 'k' || inputstr[i] == 'K') && (i > 0)) {
                val     = val << 10;
                *intnum = val;
                res     = 1;
                break;
            } else if ((inputstr[i] == 'm' || inputstr[i] == 'M') && (i > 0)) {
                val     = val << 20;
                *intnum = val;
                res     = 1;
                break;
            } else if (ISVALIDDEC(inputstr[i])) {
                val = val * 10 + CONVERTDEC(inputstr[i]);
            } else {
                /* return 0, Invalid input */
                res = 0;
                break;
            }
        }
        /* Over 10 digit decimal --invalid */
        if (i >= 11) {
            res = 0;
        }
    }

    return res;
}

void Int2Str(uint8_t *str, int32_t intnum)
{
    uint32_t i, Div = 1000000000, j = 0, Status = 0;

    for (i = 0; i < 10; i++) {
        str[j++] = (intnum / Div) + 48;

        intnum = intnum % Div;
        Div /= 10;
        if ((str[j - 1] == '0') & (Status == 0)) {
            j = 0;
        } else {
            Status++;
        }
    }
}

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
    uint32_t ApplicationAddress,
    IAP_Connector Connector,
    pIAPYmodemReceiveDataFunction ReceiveData,
    pIAPOutputDataFunction OutputString)
{
    IAPx->ApplicationAddress = ApplicationAddress;
    IAPx->Connector          = Connector;
    IAPx->ReceiveData        = ReceiveData;
    IAPx->OutputString       = OutputString;

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
    IAP_OutputData(IAPx, "\r\n  Download Image To the STM32F10x Internal Flash ------- 1\r\n");
    IAP_OutputData(IAPx, "\r\n  Upload Image From the STM32F10x Internal Flash ------- 2\r\n");
    IAP_OutputData(IAPx, "\r\n  Execute The New Program ------------------------------ 3\r\n");

    if (IAPx->FlashProtection != 0) {
        IAP_OutputData(IAPx, "\r\n  Disable the write protection ------------------------- 4\r\n");
    }

    IAP_OutputData(IAPx, "\r\n==========================================================\r\n");
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
            Int2Str(EraseCont, EraseCounter + 1);
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
    Size               = IAP_ReceiveDatat(IAPx, &(IAPx->Tab1024)[0]);
    if (Size > 0) {
        IAP_OutputData(IAPx, "\r\nUpdate Over!\r\n");
        IAP_OutputData(IAPx, " Name: ");
        IAP_OutputData(IAPx, (char *)IAPx->FileName);
        Int2Str(Number, Size);
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

int8_t IAP_ReceiveDatat(IAP_InitTypeDef *IAPx, uint8_t *Data)
{
    return IAPx->ReceiveData(IAPx->Connector, Data);
}

void IAP_OutputData(IAP_InitTypeDef *IAPx, char *String)
{
    IAPx->OutputString(IAPx->Connector, String);
}
