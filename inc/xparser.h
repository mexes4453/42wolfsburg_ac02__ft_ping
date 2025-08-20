#ifndef XPARSER_H
# define XPARSER_H
# include <string.h>
# include <stdlib.h>
# define XPARSER__WHITESPACE_CHARS " \n\r\t\v"

int XPARSER__StripLeadingWhiteSpace( char *strInput, char **strOutput);
int XPARSER__StripTrailingWhiteSpace( char *strInput, char **strOutput);
/**
 * @brief stripwhitespace around string. Includes leading and trailing 
 * 
 * @param strInput 
 * @param strOutput 
 * @return int 
 */
int XPARSER__StripWhiteSpace( char *strInput, char **strOutput);
int XPARSER__IsWhiteSpaceInStr( char *str);


#endif /* XPARSER_H */