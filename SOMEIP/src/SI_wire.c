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

#include "SI_types.h"
#include "SI_config.h"
#include "SI_header.h"
#include "SI_endian.h"

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

static inline const uint8* offset_u8_const(const uint8* p, uint32 offs);
static inline uint8* offset_u8(uint8* p, uint32 offs);

/* **************************************************** */
/*             Global function definitions              */
/* **************************************************** */

/**
 * Converts SI_Header struct into byte array to be transmitted on wire.
 * Standard SOME/IP header is 16 bytes long, so out_p return value in parameter list should be at least this size.
 * [PRS_SOMEIP_00368]
 * 
 * @param in_header: Input header to be transformed
 * @param out_header: Output pointer to the byte array
 */
void SI_WIRE_serialize_header(const struct SI_Header* in_header, uint8* out_header)
{
    // struct SI_MessageID: [service:16 | method:16]
    u16_to_u8array(offset_u8(out_header, 0u), in_header->message_id.serviceID);
    u16_to_u8array(offset_u8(out_header, 2u), in_header->message_id.methodID_or_eventID);

    // Length: 32 bit
    u32_to_u8array(offset_u8(out_header, 4u), in_header->length);

    // struct SI_RequestID: [client_id:16 | session_id:16]
    u16_to_u8array(offset_u8(out_header, 8u), in_header->request_id.clientID);
    u16_to_u8array(offset_u8(out_header, 10u), in_header->request_id.sessionID);

    out_header[12] = in_header->protocol_version;
    out_header[13] = in_header->interface_version;
    out_header[14] = in_header->message_type;
    out_header[15] = in_header->return_code;
}

/**
 * Converts byte array into SI_Header struct.
 * 
 * @param in_header: Input array to be transformed
 * @param out_header: Output struct for the deserialized data
 */
void SI_WIRE_deserialize_header(const uint8* in_header, struct SI_Header* out_header)
{
    // struct SI_MessageID: [service:16 | method:16]
    out_header->message_id.serviceID = u8array_to_u16(offset_u8_const(in_header, 0u));
    out_header->message_id.methodID_or_eventID = u8array_to_u16(offset_u8_const(in_header, 2u));

    // Length: 32 bit
    out_header->length = u8array_to_u32(offset_u8_const(in_header, 4u));

    // struct SI_RequestID: [client_id:16 | session_id:16]
    out_header->request_id.clientID = u8array_to_u16(offset_u8_const(in_header, 8u));
    out_header->request_id.sessionID = u8array_to_u16(offset_u8_const(in_header, 10u));

    out_header->protocol_version = in_header[12u];
    out_header->interface_version = in_header[13u];
    out_header->message_type = in_header[14u];
    out_header->return_code = in_header[15u];
}

/* **************************************************** */
/*             Local function definitions               */
/* **************************************************** */

static inline const uint8* offset_u8_const(const uint8* p, uint32 offs)
{
    return (p + offs);
}

static inline uint8* offset_u8(uint8* p, uint32 offs)
{
    return (p + offs);
}

/* END OF SI_WIRE.C FILE */
