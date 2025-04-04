#ifndef PROTOCOL_H
# define PROTOCOL_H


/**
 * @brief ICMP struct RFC 792
 * 
 */
typedef struct PROTOCOL__HdrIcmp_s
{
    char type;
    char code;
    int short checksum;
    int short identifier;
    int short seqnbr;
}   PROTOCOL__HdrIcmp_t;

#endif /* PROTOCOL_H */