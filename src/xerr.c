#include "../inc/xerr.h"

static const XERR_t XERR__ErrTable[] =
    {
        {0, XAPP__enRetCode_TxPacket_SendToFailed, "sending packet"},
        {1, XAPP__enRetCode_RxPacket_Failed, "receiving packet"},
        {2, XAPP__enRetCode_CreateIcmpPayload_Failed, NULL}};




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
        fprintf(stderr, "%s\n", pErr->strMsg);
    }
}