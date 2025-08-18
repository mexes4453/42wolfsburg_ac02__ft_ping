#include "../inc/xparser.h"



int XPARSER__StripLeadingWhiteSpace(char *strInput, char **strOutput)
{
    while ( strchr(XPARSER__WHITESPACE_CHARS, *strInput) && (*strInput != '\0'))
    {
        strInput++;
    }
    *strOutput = strInput;
    return (strlen(strInput));
}




int XPARSER__StripTrailingWhiteSpace(char *strInput, char **strOutput)
{
    int lenStrInput = strlen(strInput);
    while (lenStrInput > 0)
    {
        if ( !strchr(XPARSER__WHITESPACE_CHARS, *(strInput + lenStrInput - 1)) )
        {
            *(strInput + lenStrInput) = '\0';
            break ;
        }
        lenStrInput--;
    }
    *strOutput = strInput;
    return (lenStrInput);
}




int XPARSER__IsWhiteSpaceInStr(char *str)
{
    int retCode = 0;

    while ( !strchr(XPARSER__WHITESPACE_CHARS, *str) && (*str != '\0') )
    {
        str++;
    }

    /*>
     * Update retCode with boolean values
     * - 0 : current str pointer points to null char
     * - 1 : current str pointer points to non null char */
    retCode = (*str == '\0') ? 0 : 1;

    return (retCode);
}




int XPARSER__StripWhiteSpace(char *strInput, char **strOutput)
{
    int   lenStrOutput = 0;

    /* Remove all whitespace before the first non whitespace character */
    lenStrOutput = XPARSER__StripLeadingWhiteSpace(strInput, strOutput);
#ifdef XPARSER__DEBUG
    printf("string (%s), length(%d) \n", strOutput[0], lenStrOutput);
#endif

    /*>
     * Use the output string from StripLeadingWhiteSpace function as input 
     * string to StripTrailingWhiteSpace function */
    lenStrOutput = XPARSER__StripTrailingWhiteSpace(strOutput[0], strOutput);

#ifdef XPARSER__DEBUG
    printf("string (%s), length(%d) \n", strOutput[0], lenStrOutput);
#endif
    return (lenStrOutput);
}


