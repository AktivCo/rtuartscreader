#include <cmath>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <termios.h>

#include <PCSC/wintypes.h>

#include "ConsoleColor.h"
#include "TransportApiTest.h"

const char *errorMsg[] = {
    "incorrect packet header size",
    "incorrect header or protocol version",
    "incorrect packet type",
    "incorrect packet number",
    "incorrect data size",
    "incorrect CRC calculation",
    "absent initial packet control character",
    "absent last control character",
    "absent encoding of escape characters"
};

/*!************************************************************************
* \brief Convert wWordToPut variable to byte array
* \return null
****************************************************************************/
void PutWord(
    BYTE *abyOutBuffer, //!< [OUT] Output data
    WORD wWordToPut,    //!< [IN] Word
    BOOL bIsBigEndian   //!< [IN] Byte order is TRUE - big-endian
) {
    if (bIsBigEndian) {
        abyOutBuffer[0] = (BYTE)(wWordToPut >> 8);
        abyOutBuffer[1] = (BYTE)wWordToPut;
    } else {
        abyOutBuffer[0] = (BYTE)wWordToPut;
        abyOutBuffer[1] = (BYTE)(wWordToPut >> 8);
    }
}
/*!************************************************************************
* \brief Convert dwDwordToPut variable to byte array
*
* \return null
****************************************************************************/
void PutDword(
    BYTE *abyInBuffer,  //!< [OUT] Output data
    DWORD dwDwordToPut, //!< [IN] Double word
    BOOL bIsBigEndian   //!< [IN] Byte order is TRUE - big-endian
) {
    if (bIsBigEndian) {
        PutWord(abyInBuffer, (WORD)(dwDwordToPut >> 16), TRUE);
        PutWord(&abyInBuffer[2], (WORD)dwDwordToPut, TRUE);
    } else {
        PutWord(abyInBuffer, (WORD)dwDwordToPut, FALSE);
        PutWord(&abyInBuffer[2], (WORD)(dwDwordToPut >> 16), FALSE);
    }
}
/*!************************************************************************
* \brief   Create WORD from byte array abyInBuffer ordered in big-endian or little-endian
*
* \return WORD
****************************************************************************/
WORD GetWord(
    BYTE *abyInBuffer, //!< [IN] Input data
    BOOL bIsBigEndian  //!< [IN] Byte order is TRUE - big-endian
) {
    WORD wOut;

    if (bIsBigEndian)
        wOut = (WORD)(abyInBuffer[0] << 8) + abyInBuffer[1];
    else
        wOut = (WORD)(abyInBuffer[1] << 8) + abyInBuffer[0];
    return wOut;
}
/*!************************************************************************
* \brief  Create DWORD from byte array abyInBuffer ordered in big-endian or little-endian
*
* \return DWORD
****************************************************************************/
DWORD GetDword(
    BYTE *abyInBuffer, //!< [IN] Input data
    BOOL bIsBigEndian  //!< [IN] Byte order is TRUE - big-endian
) {
    DWORD dwOut;

    if (bIsBigEndian)
        dwOut = (DWORD)(GetWord(abyInBuffer, TRUE) << 16) + GetWord(&abyInBuffer[2], TRUE);
    else
        dwOut = (DWORD)(GetWord(&abyInBuffer[2], FALSE) << 16) + GetWord(abyInBuffer, FALSE);
    return dwOut;
}


LONG SendCmd(
    PDEVICE_HANDLE phDev,
    const BYTE *pSendBuff,
    DWORD dwSendLen,
    BYTE *pbyOutput,
    DWORD *pdwReplyLength,
    SetTestError_t Error) {
    DWORD lReturn;
    uint16_t dwLenRecv;
    dwLenRecv = *pdwReplyLength;

    lReturn = TrAPI_Test_Transmit(phDev, pSendBuff, dwSendLen, pbyOutput, &dwLenRecv, Error);

    *pdwReplyLength = dwLenRecv;
    return lReturn;
}

static long RESET_ACCESS_RIGHTS(
    PDEVICE_HANDLE hDev,  //!< [IN] Opened device descriptor
    BYTE *abAnswer,       //!< [OUT] Command output
    DWORD *dwReplyLength, //!< [OUT] Command output length
    SetTestError_t Error, //!< [IN] Tested error code
    BOOL printtext        //!< [IN] Debug output flag
) {
    if (printtext) printf("Testing command (RESET ACCESS RIGHTS)\r\n");
    BYTE abReset[] = { 0x80, 0x40, 0x00, 0x00 };
    *dwReplyLength = 0x02;
    return SendCmd(hDev, abReset, sizeof(abReset), abAnswer, dwReplyLength, Error);
}

static long SET_UART_SPEED(
    PDEVICE_HANDLE hDev,  //!< [IN] Opened device descriptor
    BYTE *abAnswer,       //!< [OUT] Command output
    DWORD *dwReplyLength, //!< [OUT] Command output length
    SetTestError_t Error, //!< [IN] Tested error code
    BOOL printtext,       //!< [IN] Debug output flag
    DWORD UartSpeed) {
    if (printtext) printf("Testing command (SET UART SPEED %lu)\r\n", UartSpeed);
    BYTE abReset[] = { 0x80, 0x57, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00 };
    PutDword(&abReset[5], UartSpeed, TRUE);
    *dwReplyLength = 0x02;
    return SendCmd(hDev, abReset, sizeof(abReset), abAnswer, dwReplyLength, Error);
}

static long GET_UART_SPEED_REAL(
    PDEVICE_HANDLE hDev,  //!< [IN] Opened device descriptor
    BYTE *abAnswer,       //!< [OUT] Command output
    DWORD *dwReplyLength, //!< [OUT] Command output length
    SetTestError_t Error, //!< [IN] Tested error code
    BOOL printtext        //!< [IN] Debug output flag
) {
    if (printtext) printf("Testing command (GET UART SPEED)\r\n");
    BYTE abCommGetATR[] = { 0x80, 0x57, 0x01, 0x01, 0x00 };
    *dwReplyLength = 0x06;
    return SendCmd(hDev, abCommGetATR, sizeof(abCommGetATR), abAnswer, dwReplyLength, Error);
}

static long GET_UART_SPEED(
    PDEVICE_HANDLE hDev,  //!< [IN] Opened device descriptor
    BYTE *abAnswer,       //!< [OUT] Command output
    DWORD *dwReplyLength, //!< [OUT] Command output length
    SetTestError_t Error, //!< [IN] Tested error code
    BOOL printtext        //!< [IN] Debug output flag
) {
    if (printtext) printf("Testing command (GET UART SPEED)\r\n");
    BYTE abCommGetATR[] = { 0x80, 0x57, 0x01, 0x01, 0x00 };
    *dwReplyLength = 0x06;
    return SendCmd(hDev, abCommGetATR, sizeof(abCommGetATR), abAnswer, dwReplyLength, Error);
}

static long GET_ATR(
    PDEVICE_HANDLE hDev,  //!< [IN] Opened device descriptor
    BYTE *abAnswer,       //!< [OUT] Command output
    DWORD *dwReplyLength, //!< [OUT] Command output length
    SetTestError_t Error, //!< [IN] Tested error code
    BOOL printtext        //!< [IN] Debug output flag
) {
    if (printtext) printf("Testing command (GET ATR)\r\n");
    BYTE abCommGetATR[] = { 0x00, 0xCA, 0x5F, 0x51, 0x00 };
    *dwReplyLength = 0x100 + 2;
    return SendCmd(hDev, abCommGetATR, sizeof(abCommGetATR), abAnswer, dwReplyLength, Error);
}

static long LOGIN_USER(
    PDEVICE_HANDLE hDev,  //!< [IN] Opened device descriptor
    BYTE *abAnswer,       //!< [OUT] Command output
    DWORD *dwReplyLength, //!< [OUT] Command output length
    SetTestError_t Error, //!< [IN] Tested error code
    BOOL printtext        //!< [IN] Debug output flag
) {
    if (printtext) printf("Testing command (LOGIN USER)\r\n");
    BYTE abVerify[] = { 0x00, 0x20, 0x00, 0x02, 0x08, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38 };
    *dwReplyLength = 0x02;
    return SendCmd(hDev, abVerify, sizeof(abVerify), abAnswer, dwReplyLength, Error);
}

long LOGIN_ADMIN(
    PDEVICE_HANDLE hDev,  //!< [IN] Opened device descriptor
    BYTE *abAnswer,       //!< [OUT] Command output
    DWORD *dwReplyLength, //!< [OUT] Command output length
    SetTestError_t Error, //!< [IN] Tested error code
    BOOL printtext        //!< [IN] Debug output flag
) {
    if (printtext) printf("Testing command (LOGIN ADMIN)\r\n");
    BYTE abVerify[] = { 0x00, 0x20, 0x00, 0x01, 0x08, 0x38, 0x37, 0x36, 0x35, 0x34, 0x33, 0x32, 0x31 };
    *dwReplyLength = 0x02;
    return SendCmd(hDev, abVerify, sizeof(abVerify), abAnswer, dwReplyLength, Error);
}

static long READ_MF(
    PDEVICE_HANDLE hDev,  //!< [IN] Opened device descriptor
    BYTE *abAnswer,       //!< [OUT] Command output
    DWORD *dwReplyLength, //!< [OUT] Command output length
    SetTestError_t Error, //!< [IN] Tested error code
    BOOL printtext        //!< [IN] Debug output flag
) {
    if (printtext) printf("Testing command (READ MF)\r\n");
    BYTE abReadMF[] = { 0x00, 0xA4, 0x00, 0x00, 0x02, 0x3F, 0x00, 0x80 };
    *dwReplyLength = 0x02 + 0x80;
    return SendCmd(hDev, abReadMF, sizeof(abReadMF), abAnswer, dwReplyLength, Error);
}

typedef long TestFunc_t(PDEVICE_HANDLE hDev, BYTE *abAnswer, DWORD *dwReplyLength, SetTestError_t Error, BOOL printtext);
TestFunc_t *ArrayFunc[] = { RESET_ACCESS_RIGHTS, GET_ATR, LOGIN_USER, READ_MF };

int main(int argc, char **argv) {
    long lRet, lRetError;
    DEVICE_HANDLE hDev;
    SetTestError_t Error = ST_NoErr;
    BYTE abAnswer[256 + 2];
    DWORD dwReplyLength = sizeof(abAnswer);
    char dev_name[FILENAME_MAX];
    int ErrorSumm = 0;

    if (argc < 2) {
        printf("Usage: ./sample dev_name\n");
        return 1;
    }

    strncpy(dev_name, argv[1], FILENAME_MAX);

    printf("Rutoken ECP UART demo test\r\n");
    printf("Opening device %s...\r\n", dev_name);

    lRet = TrAPI_OpenDevice(&hDev, dev_name, B115200);
    if (lRet) {
        log_error("Error while opening device\r\n");
        goto __ENDPROGRAMM;
    } else {
        log_success("Ok!\n");
    }

    memset(abAnswer, 0xCC, sizeof(abAnswer));

    //Standart functionality test
    lRet = RESET_ACCESS_RIGHTS(&hDev, abAnswer, &dwReplyLength, Error, true);
    if (lRet) {
        log_error("Error while reset access rights\r\n");
        if (abAnswer[0] != 0xCC && abAnswer[1] != 0xCC)
            log_error("APDU return code: 0x%02x 0x%02x\r\n", abAnswer[0], abAnswer[1]);
        goto __ENDPROGRAMM;
    } else {
        log_success("Successful\r\n");
        log_success("APDU return code: 0x%02x 0x%02x\r\n", abAnswer[0], abAnswer[1]);
        memset(abAnswer, 0xCC, sizeof(abAnswer));
    }

    lRet = GET_ATR(&hDev, abAnswer, &dwReplyLength, Error, true);
    if (lRet) {
        log_error("Error while getting ATR\r\n");
        if (abAnswer[0] != 0xCC && abAnswer[1] != 0xCC)
            log_error("APDU return code: 0x%02x 0x%02x\r\n", abAnswer[0], abAnswer[1]);
        goto __ENDPROGRAMM;
    } else {
        log_success("ATR: ");
        for (DWORD i = 0; i < dwReplyLength - 2; i++) {
            log_success("%02x ", abAnswer[i]);
        }
        log_success("\r\n");
        log_success("APDU return code: 0x%02x 0x%02x\r\n", abAnswer[dwReplyLength - 2], abAnswer[dwReplyLength - 1]);
        memset(abAnswer, 0xCC, sizeof(abAnswer));
    }

    lRet = SET_UART_SPEED(&hDev, abAnswer, &dwReplyLength, Error, true, 921600);
    if (lRet) {
        log_error("Error while setting UART speed\r\n");
        if (abAnswer[0] != 0xCC && abAnswer[1] != 0xCC)
            log_error("APDU return code: 0x%02x 0x%02x\r\n", abAnswer[0], abAnswer[1]);
        goto __ENDPROGRAMM;

    } else {
        log_success("Successful\r\n");
        log_success("APDU return code: 0x%02x 0x%02x\r\n", abAnswer[dwReplyLength - 2], abAnswer[dwReplyLength - 1]);
        memset(abAnswer, 0xCC, sizeof(abAnswer));
    }

    lRet = LOGIN_ADMIN(&hDev, abAnswer, &dwReplyLength, Error, true);
    if (lRet) {
        log_error("Error while admin login\r\n");
        if (abAnswer[0] != 0xCC && abAnswer[1] != 0xCC)
            log_error("APDU return code: 0x%02x 0x%02x\r\n", abAnswer[0], abAnswer[1]);
        goto __ENDPROGRAMM;
    } else {
        log_success("Successful\r\n");
        log_success("APDU return code: 0x%02x 0x%02x\r\n", abAnswer[dwReplyLength - 2], abAnswer[dwReplyLength - 1]);
        memset(abAnswer, 0xCC, sizeof(abAnswer));
    }

    lRet = GET_UART_SPEED(&hDev, abAnswer, &dwReplyLength, Error, true);
    if (lRet) {
        log_error("Error while getting UART speed\r\n");
        if (abAnswer[0] != 0xCC && abAnswer[1] != 0xCC)
            log_error("APDU return code: 0x%02x 0x%02x\r\n", abAnswer[dwReplyLength - 2], abAnswer[dwReplyLength - 1]);
        goto __ENDPROGRAMM;
    } else {
        log_success("UART SPEED: %u \r\n", GetDword(abAnswer, true));
        log_success("APDU return code: 0x%02x 0x%02x\r\n", abAnswer[dwReplyLength - 2], abAnswer[dwReplyLength - 1]);
        memset(abAnswer, 0xCC, sizeof(abAnswer));
    }


    lRet = SET_UART_SPEED(&hDev, abAnswer, &dwReplyLength, Error, true, 921600);
    if (lRet) {
        log_error("Error while setting UART speed\r\n");
        if (abAnswer[0] != 0xCC && abAnswer[1] != 0xCC)
            log_error("APDU return code: 0x%02x 0x%02x\r\n", abAnswer[dwReplyLength - 2], abAnswer[dwReplyLength - 1]);
        goto __ENDPROGRAMM;
    } else {
        log_success("Successful\r\n");
        log_success("APDU return code: 0x%02x 0x%02x\r\n", abAnswer[dwReplyLength - 2], abAnswer[dwReplyLength - 1]);
        memset(abAnswer, 0xCC, sizeof(abAnswer));
    }

    lRet = GET_UART_SPEED(&hDev, abAnswer, &dwReplyLength, Error, true);
    if (lRet) {
        log_error("Error while getting UART speed\r\n");
        if (abAnswer[0] != 0xCC && abAnswer[1] != 0xCC)
            log_error("APDU return code: 0x%02x 0x%02x\r\n", abAnswer[dwReplyLength - 2], abAnswer[dwReplyLength - 1]);
        goto __ENDPROGRAMM;
    } else {
        log_success("UART SPEED: %u \r\n", GetDword(abAnswer, true));
        log_success("APDU return code: 0x%02x 0x%02x\r\n", abAnswer[dwReplyLength - 2], abAnswer[dwReplyLength - 1]);
        memset(abAnswer, 0xCC, sizeof(abAnswer));
    }

    lRet = SET_UART_SPEED(&hDev, abAnswer, &dwReplyLength, Error, true, 115200);
    if (lRet) {
        log_error("Error while setting UART speed\r\n");
        if (abAnswer[0] != 0xCC && abAnswer[1] != 0xCC)
            log_error("APDU return code: 0x%02x 0x%02x\r\n", abAnswer[dwReplyLength - 2], abAnswer[dwReplyLength - 1]);
        goto __ENDPROGRAMM;
    } else {
        log_success("APDU return code: 0x%02x 0x%02x\r\n", abAnswer[dwReplyLength - 2], abAnswer[dwReplyLength - 1]);
        memset(abAnswer, 0xCC, sizeof(abAnswer));
    }

    lRet = READ_MF(&hDev, abAnswer, &dwReplyLength, Error, true);
    if (lRet) {
        log_error("Error while setting UART speed\r\n");
        if (abAnswer[0] != 0xCC && abAnswer[1] != 0xCC)
            log_error("APDU return code: 0x%02x 0x%02x\r\n", abAnswer[dwReplyLength - 2], abAnswer[dwReplyLength - 1]);
        goto __ENDPROGRAMM;
    } else {
        log_success("APDU return code: 0x%02x 0x%02x\r\n", abAnswer[dwReplyLength - 2], abAnswer[dwReplyLength - 1]);
        memset(abAnswer, 0xCC, sizeof(abAnswer));
    }

    log_info("Testing transport protocol...\r\n");

    //Protocol error handling test
    for (int i = 0; i < 9; i++) {
        Error = (SetTestError_t)((uint32_t)(pow(2, i)));
        log_info("Testing handling of error: %s\r\n", errorMsg[i]);
        for (int func = 0; func < sizeof(ArrayFunc) / sizeof(TestFunc_t *); func++) {
            log_info("Testing error case\r\n");
            lRetError = (ArrayFunc[func](&hDev, abAnswer, &dwReplyLength, Error, false));
            log_info("Testing normal case\r\n");
            lRet = (ArrayFunc[func](&hDev, abAnswer, &dwReplyLength, ST_NoErr, false));

            if ((!lRetError || lRet) && (Error != ST_NoEnd && Error != ST_NoEscape)) {
                log_error("Test fail\r\n");
                goto __ENDPROGRAMM;
            }
        }
        {
            log_success("Ok!\n");
        }
    }

    for (int i = 0; i < 7; i++) {
        ErrorSumm += ((uint32_t)(pow(2, i)));
        Error = (SetTestError_t)ErrorSumm;

        log_info("Testing multiple errors: add %s to scope\r\n", errorMsg[i]);

        for (int func = 0; func < sizeof(ArrayFunc) / sizeof(TestFunc_t *); func++) {
            log_info("Testing error case\r\n");
            lRetError = (ArrayFunc[func](&hDev, abAnswer, &dwReplyLength, Error, false));
            log_info("Testing normal case\r\n");
            lRet = (ArrayFunc[func](&hDev, abAnswer, &dwReplyLength, ST_NoErr, false));

            if ((!lRetError || lRet) && (Error & (ST_NoEnd | ST_NoEscape))) {
                log_error("Test Fail\r\n");
                goto __ENDPROGRAMM;
            }
        }
        {
            log_success("Ok!\n");
        }
    }

    log_success("\nTest Ok");
__ENDPROGRAMM:
    TrAPI_CloseDevice(&hDev);

    return 0;
}
