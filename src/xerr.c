#include "../inc/xerr.h"

static const XERR_t XERR__ErrTable[] =
    {
        {XAPP__enRetCode_TxPacket_SendToFailed, "TxPacket->SendTo Failed"},
        {XAPP__enRetCode_RxPacket_Failed, "RxPacket Failed"},
        {XAPP__enRetCode_ProcessOptionChar_InvalidOptFormatCount, "Invalid option format for (c)"},
        {XAPP__enRetCode_ProcessOptionChar_NoOptVal,        XAPP__ERR_MSG_OPT_COUNT_REQ_ARGS},
        {XAPP__enRetCode_ProcessOptionChar_CountValInvalid, XAPP__ERR_MSG_OPT_COUNT_VAL_INV},
        {XAPP__enRetCode_CreateIcmpPayload_Failed,          XERR__NIL},
        {XAPP__enRetCode_Max, NULL}
    };




void XERR__HandleError(int retCode, char *progTitle)
{
    int idx = 0;
    XERR_t *pErr = NULL;

    if (retCode == EXIT_SUCCESS) return;
    
    while (XERR__ErrTable[idx].strMsg != NULL)
    {
        if (retCode == XERR__ErrTable[idx].code)
        {
            pErr = (XERR_t *)&(XERR__ErrTable[idx]);
            break;
        }
        idx++;
    }

    if (errno || pErr)
    {
        fprintf(stderr, "%s: ", progTitle);
    }

    if (errno && pErr)
    {
        fprintf(stderr, "error! ");
        perror(pErr->strMsg);
    }
    else if (errno && (!pErr))
    {
        perror("error");
    }
    else if (pErr)
    {
        fprintf(stderr, "error! %s\n", pErr->strMsg);
    }
}
