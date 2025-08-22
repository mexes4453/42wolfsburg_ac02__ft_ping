#ifndef XPARSER_H
# define XPARSER_H
# include <stdlib.h>
# include "../xlib/libft/libft.h"
# define XPARSER__WHITESPACE_CHARS " \n\r\t\v"
# define XPARSER__NULL_CHAR        '\0'

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

/**
 * @brief Check that all non-whitespace characters in a string are digit char
 *
 * @param s : input string
 * @return rcBool: 1 -> true:  all character are digit characters.
 *                 0 -> false: otherwise
 * */
int XPARSER__IsNbr( char *s );


#endif /* XPARSER_H */
