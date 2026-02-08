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
#include "SI_const.h"
#include "SI_header.h"
#include "SI_message.h"
#include "SI_wire.h"
#include "ERH.h"

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

enum SI_PARS_ErrType_t
{
    SI_PARS_ErrType_length_mismatch = 0u
};

/* **************************************************** */
/*             Local function declarations              */
/* **************************************************** */

static void SI_PARSER_report_error(enum SI_PARS_ErrType_t type, uint32 udp_payload_length, uint32 header_length_field);

/* **************************************************** */
/*             Global function definitions              */
/* **************************************************** */

/**
 * Checks datagram length too
 * 
 * @param udp_payload: raw byte array from UDP layer
 * @param udp_payload_length: total length of udp_payload array
 * @param out_header: parsed SOME/IP header
 * @param out_payload: pointer to SOME/IP payload
 */
boolean SI_PARSER_parse_datagram(const uint8* udp_payload, uint32 udp_payload_length,
                                 struct SI_Header* out_header, struct SI_Payload* out_payload)
{
    const boolean nullptr_params = ((NULLPTR == udp_payload) || (NULLPTR == out_header) || (NULLPTR == out_payload));
    const boolean datagram_too_small = (SI_CONST_HEADER_LENGTH > udp_payload_length);

    if (nullptr_params || datagram_too_small)
    {
        return FALSE;
    }

    SI_WIRE_deserialize_header(udp_payload, out_header);

    if ((out_header->length + SI_CONST_HEADER_PREFIX_LENGTH) != udp_payload_length)
    {
        SI_PARSER_report_error(SI_PARS_ErrType_length_mismatch, udp_payload_length, out_header->length);
        return FALSE;
    }

    out_payload->data   = udp_payload + SI_CONST_HEADER_LENGTH;
    out_payload->length = udp_payload_length - SI_CONST_HEADER_LENGTH;

    return TRUE;
}

/* **************************************************** */
/*             Local function definitions               */
/* **************************************************** */

static void SI_PARSER_report_error(enum SI_PARS_ErrType_t type, uint32 udp_payload_length, uint32 header_length_field)
{
    ERH_report_error(ERH_SI_PARSER_ERROR, type, udp_payload_length, header_length_field, 0u, 0u, 0u);
}

/* END OF SI_PARSER.C FILE */
