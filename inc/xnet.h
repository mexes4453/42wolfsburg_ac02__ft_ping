#ifndef XNET_H
# define XNET_H

/* https://beej.us/guide/bgnet/html/ */
# include <poll.h>
# include <stdlib.h>
# include <stdio.h>
# include <sys/socket.h>
# include <netdb.h>
# include <netinet/in.h>
# include <unistd.h>
# include <sys/types.h>
# include "../inc/xnet_utils.h"
# include "../xlib/libft/libft.h"

#define XNET__LITTLE_ENDIAN
//#define XNET__DEBUG

#define XNET__MSG_FMT_ADDR_INFO "\n=== ADDR INFO === \
                                 \ncannon name: %s \
                                 \nip_family: %d \
                                 \nip_port: %d \
                                 \nip_addr: %d \n\n"

#define XNET__FAILURE           (-1)
#define XNET__SUCCESS           (0)
#define XNET__ERR_MSG_NO_SOCK   "Socket creation failed\n"
#define XNET__ERR_CREATSOCK     (-2)





/*
 * @brief Initialise the addr info struct
 * 
 * @param pAi 
 */
void XNET__InitAddrInfo(struct addrinfo *pAi, int family,
                                              int socktyp,
                                              int protocol,
                                              int flags);




/**
 * @brief display details of all addr info in linked list
 * 
 * @param pAi - linked list head
 */
void XNET__ShowAddrInfo(struct addrinfo *pAi);




/**
 * @brief return the first socket created successfully
 * 
 * @param pAi - linked list to addresses 
 * @return int - socket file descriptor
 */
int  XNET__CreateSocket(struct addrinfo *pAi);





void XNET__ShowRecvdMsg(char unsigned *buf, ssize_t bufSz);

#endif /* XNET_H */