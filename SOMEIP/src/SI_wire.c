/**
 * @file    SI_wire.c
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   "Implements SI_wire.h"
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include "SI_wire.h"

#include <string.h>     // for memcpy
#include "SI_types.h"
#include "SI_header.h"
#include "SI_endian.h"

/* **************************************************** */
/*                       Defines                        */
/* **************************************************** */

#define POINTER_OFFSET_BY_BYTES(p,offs) (&(p[offs]))

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
 * Converts SI_Header struct into byte array to be transmitted on wire.
 * Standard SOME/IP header is 16 bytes long, so out_p return value in parameter list should be at least this size.
 * 
 * @param in_header: Input header to be transformed
 * @param out_header: Output pointer to the transformed byte array
 */
void SI_WIRE_serialize_header(const struct SI_Header* in_header, uint8* out_header)
{
    // struct SI_MessageID: [service:16 | method:16]
    be_put_u16(POINTER_OFFSET_BY_BYTES(out_header, 0u), in_header->message_id.serviceID);
    be_put_u16(POINTER_OFFSET_BY_BYTES(out_header, 2u), in_header->message_id.methodID_or_eventID);

    // Length: 32 bit
    be_put_u32(POINTER_OFFSET_BY_BYTES(out_header, 4u), in_header->length);

    // struct SI_RequestID: [client_id:16 | session_id:16]
    be_put_u16(POINTER_OFFSET_BY_BYTES(out_header, 8u), in_header->request_id.clientID);
    be_put_u16(POINTER_OFFSET_BY_BYTES(out_header, 10u), in_header->request_id.sessionID);

    out_header[12] = in_header->protocol_version;
    out_header[13] = in_header->interface_version;
    out_header[14] = in_header->message_type;
    out_header[15] = in_header->return_code;
}

void SI_WIRE_deserialize_header(const uint8* in_header, struct SI_Header* out_header)
{
    // struct SI_MessageID: [service:16 | method:16]
    out_header->message_id.serviceID = be_get_u16(POINTER_OFFSET_BY_BYTES(in_header, 0u));
    out_header->message_id.methodID_or_eventID = be_get_u16(POINTER_OFFSET_BY_BYTES(in_header, 2u));

    // Length: 32 bit
    out_header->length = be_get_u32(POINTER_OFFSET_BY_BYTES(in_header, 4u));

    // struct SI_RequestID: [client_id:16 | session_id:16]
    out_header->request_id.clientID = be_get_u16(POINTER_OFFSET_BY_BYTES(in_header, 8u));
    out_header->request_id.sessionID = be_get_u16(POINTER_OFFSET_BY_BYTES(in_header, 10u));

    out_header->protocol_version = in_header[12];
    out_header->interface_version = in_header[13];
    out_header->message_type = in_header[14];
    out_header->return_code = in_header[15];
}

/* **************************************************** */
/*             Local function definitions               */
/* **************************************************** */

/* END OF SI_WIRE.C FILE */
