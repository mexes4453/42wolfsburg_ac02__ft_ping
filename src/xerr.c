#include "../inc/xerr.h"

static const XERR_t XERR__ErrTable[] =
    {
        {XAPP__enRetCode_TxPacket_SendToFailed, "sending packet"},
        {XAPP__enRetCode_RxPacket_Failed, "receiving packet"},
        {XAPP__enRetCode_ProcessOptionChar_InvalidOptFormatCount, "Invalid option format for (c)"},
        {XAPP__enRetCode_ProcessOptionChar_OptUsageHandled, XAPP__MSG_FMT_HELP},
        {XAPP__enRetCode_ProcessOptionChar_NoOptVal,        XAPP__ERR_MSG_OPT_COUNT_REQ_ARGS},
        {XAPP__enRetCode_CreateIcmpPayload_Failed, XERR__NIL},
        {XAPP__enRetCode_Max, NULL}
    };




void XERR__HandleError(int retCode, char *progTitle)
{
    int idx = 0;
    XERR_t *pErr = NULL;

    if (retCode == EXIT_SUCCESS) return;
    while (XERR__ErrTable[idx].strMsg != NULL)
    {
        //printf("found rc( %d ) - %d\n", retCode, XERR__ErrTable[idx].code);
        if (retCode == XERR__ErrTable[idx].code)
        {
            pErr = (XERR_t *)&(XERR__ErrTable[idx]);
            //printf("found");
            break;
        }
        idx++;
    }

    fprintf(stderr, "%s: ", progTitle);
    if (errno)
    {
       if (pErr)
       {
           perror(pErr->strMsg);
       }
       else
       {
           perror("error");
           fprintf(stderr, "%d\n", retCode);
       }
    }
    else
    {
        if( pErr)
        {
           fprintf(stderr, "%s\n", pErr->strMsg);
        }
    }
}