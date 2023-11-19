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

    YMODEM_Init(IAPx->Ymodem, Serial, FLASH_SIZE, (pYmodemErasePageFunction)IAP_ErasePages, (pYmodemSaveApplicationDataFunction)IAP_SaveApplicationData);

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

uint8_t IAP_SaveApplicationData(IAP_InitTypeDef *IAPx, uint32_t PacketLength, uint32_t Size, uint32_t RamSource)
{
    for (uint8_t j = 0; (j < PacketLength) && (IAPx->FlashDestination < IAPx->ApplicationAddress + Size); j += 4) {
        /* Program the data received into STM32F10x Flash */
        FLASH_Unlock();
        FLASH_ProgramWord(IAPx->FlashDestination, *(uint32_t *)RamSource);
        FLASH_Lock();
        if (*(uint32_t *)IAPx->FlashDestination != *(uint32_t *)RamSource) {
            return 0;
        }
        IAPx->FlashDestination += 4;
        RamSource += 4;
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

int32_t IAP_ReceiveDatat(IAP_InitTypeDef *IAPx, uint8_t *Data)
{

    IAPx->FlashDestination = IAPx->ApplicationAddress;

    YMODEM_ReceiveData(IAPx->Ymodem, IAPx->FileName, Data);
    return 0;
}