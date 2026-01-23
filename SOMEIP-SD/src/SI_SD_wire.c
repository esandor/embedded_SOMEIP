
/**
 * @file    SI_SD_wire.c
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   "Implements SI_SD_wire.h"
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include "SI_SD_wire.h"

#include "SI_types.h"
#include "SI_endian.h"

#include "SI_SD_config.h"
#include "SI_SD_message.h"

/* **************************************************** */
/*                       Defines                        */
/* **************************************************** */

#define POINTER_OFFSET_BY_BYTES(p,offs) (&(p[offs]))

#define LOWER_NIBBLE_MASK		(0x0Fu)
#define LOWER_NIBBLE_OFFSET		(0u)
#define UPPER_NIBBLE_MASK		(0xF0u)
#define UPPER_NIBBLE_OFFSET		(4u)
#define DISCARDABLE_FLAG_MASK   (0x80u)
#define DISCARDABLE_FLAG_OFFSET (7u)

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

void SI_SD_WIRE_serialize_header(const struct SD_Header* in_header, uint8* out_header)
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

    be_put_u32(POINTER_OFFSET_BY_BYTES(out_header, 16u), in_header->preamble);
}

void SI_SD_WIRE_deserialize_header(const uint8* in_header, struct SD_Header* out_header)
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

    out_header->preamble = be_get_u32(POINTER_OFFSET_BY_BYTES(in_header, 16u));
}

void SI_SD_WIRE_deserialize_ServiceEntry(uint8 *in_entry, struct SI_SD_ServiceEntry *out_entry)
{
    out_entry->type = in_entry[0u];
    out_entry->index_1st_option_run = in_entry[1u];
    out_entry->index_2nd_option_run = in_entry[2u];
    out_entry->number_of_options1 = ((in_entry[3u] & UPPER_NIBBLE_MASK) >> UPPER_NIBBLE_OFFSET);
    out_entry->number_of_options2 = ((in_entry[3u] & LOWER_NIBBLE_MASK) >> LOWER_NIBBLE_OFFSET);
    out_entry->serviceID = be_get_u16(&(in_entry[4u]));
    out_entry->instanceID = be_get_u16(&(in_entry[6u]));
    out_entry->major_version = in_entry[8u];
    out_entry->ttl = be_get_u24(&(in_entry[9u]));
    out_entry->minor_version = be_get_u32(&(in_entry[12u]));
}

void SI_SD_WIRE_deserialize_IPv4EndpointOption(uint8 *in_option, struct SI_SD_IPv4EndpointOption *out_option)
{
    out_option->length = be_get_u16(&(in_option[0u]));
    out_option->type = in_option[2u];
    out_option->discardable_flag = ((in_option[3u] & DISCARDABLE_FLAG_MASK) >> DISCARDABLE_FLAG_OFFSET);
    out_option->IPv4_address = be_get_u32(&(in_option[4u]));
    out_option->l4_proto = in_option[9u];
    out_option->port_number = in_option[10u];
}

/* **************************************************** */
/*             Local function definitions               */
/* **************************************************** */

/* END OF SI_SD_WIRE.C FILE */