/**
 * @file    SI_parser.c
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   "Implements SI_parser.h"
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include "SI_parser.h"

#include "SI_types.h"
#include "SI_config.h"
#include "SI_header.h"
#include "SI_message.h"
#include "SI_wire.h"

/* **************************************************** */
/*                       Defines                        */
/* **************************************************** */

/* **************************************************** */
/*               Static global variables                */
/* **************************************************** */

/* **************************************************** */
/*                True global variables                 */
/* **************************************************** */

/* **************************************************** */
/*                Local type definitions                */
/* **************************************************** */

/* **************************************************** */
/*             Local function declarations              */
/* **************************************************** */

/* **************************************************** */
/*             Global function definitions              */
/* **************************************************** */

/**
 * Checks packet length according to UDP layer and SOME/IP packet header.
 * 
 * @param udp_payload: raw byte array from UDP layer
 * @param udp_payload_length: total length of udp_payload array
 * @param out_header: parsed SOME/IP header
 * @param out_payload: pointer to SOME/IP payload 
 * 
 * @returns TRUE if parse was successfull
 */
boolean SI_PARSER_parse_datagram(const uint8* udp_payload, uint32 udp_payload_length, struct SI_Header* out_header, struct SI_Payload* out_payload)
{
    if ((NULLPTR == udp_payload) || (NULLPTR == out_header) || (NULLPTR == out_payload) ||
        (SI_CONST_HEADER_LENGTH > udp_payload_length))
    {
        return FALSE;
    }

    SI_WIRE_deserialize_header(udp_payload, out_header);

    if ((out_header->length + SI_CONST_HEADER_PREFIX_LENGTH) != udp_payload_length)
    {
        return FALSE;
    }

    out_payload->data   = udp_payload + SI_CONST_HEADER_LENGTH;
    out_payload->length = udp_payload_length - SI_CONST_HEADER_LENGTH;

    return TRUE;
}

/* **************************************************** */
/*             Local function definitions               */
/* **************************************************** */

/* END OF SI_PARSER.C FILE */
