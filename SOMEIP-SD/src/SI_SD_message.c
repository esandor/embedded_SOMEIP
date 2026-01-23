/**
 * @file    SI_SD_message.c
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   "Implements SI_SD_Message.h"
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include "SI_SD_message.h"

#include <string.h>         // for memcpy
#include <assert.h>

#include "SI_types.h"
#include "SI_const.h"
#include "SI_config.h"
#include "SI_message.h"

#include "SI_SD_const.h"
#include "SI_SD_header.h"
#include "SI_SD_wire.h"

_Static_assert(SI_CONST_UDP_MTU_LENGTH >= SI_SD_CONST_HEADER_LENGTH, "FATAL ERROR: SI_SD_CONST_HEADER_LENGTH is bigger than SI_CONST_UDP_MTU_LENGTH resulting in unprocessable Service Discovery packets.");

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

boolean SI_SD_MESSAGE_init(struct SI_MessageBuilder* message)
{
    if (NULLPTR == message)
    {
        return FALSE;
    }

    if(FALSE == SI_MESSAGE_init(message))
    {
        return FALSE;
    }

    // allocate space for Service Discovery preamble
    message->cursor += SI_SD_CONST_PREAMBLE_FLAGS_SIZE;
    message->cursor += SI_SD_CONST_PREAMBLE_RESERVED_SIZE;

    return TRUE;
}

/**
 * Empty payload is acceptable only if payload_length is zero.
 */
boolean SI_SD_MESSAGE_put(struct SI_MessageBuilder* message, const uint8* payload, uint32 payload_length)
{
    const boolean invalid_inputs = ((NULLPTR == message) ||
                                    ((0u < payload_length) && (NULLPTR == payload)));
    const boolean invalid_cursor = (message->cursor > message->cap);
    // IMPORTANT: payload_too_large check must be after invalid_cursor check in order to prevent issues due to unsigned integer overflow
    const boolean payload_too_large = (((message->cap - message->cursor) < payload_length) ||
                                       ((SI_CONST_UDP_MTU_LENGTH - SI_SD_CONST_HEADER_LENGTH) < payload_length));

    // IMPORTANT: payload_too_large check must be after invalid_cursor check in order to prevent issues due to unsigned integer overflow
    if (invalid_inputs || invalid_cursor || payload_too_large)
    {
        return FALSE;
    }

    memcpy(&message->data[message->cursor], payload, payload_length);
    message->cursor += payload_length;
    message->length += payload_length;

    return TRUE;
}

boolean SI_SD_MESSAGE_finalize(struct SI_MessageBuilder* message, struct SD_Header* header)
{
    const boolean invalid_inputs = ((NULLPTR == message) || (NULLPTR == header));
    const boolean invalid_cursor = ((SI_SD_CONST_HEADER_LENGTH > message->cursor) || (message->cursor > message->cap));
    const boolean invalid_cap = (SI_SD_CONST_HEADER_LENGTH > message->cap);
    const boolean packet_too_large = (SI_CONST_UDP_MTU_LENGTH < message->cursor);

    if (invalid_inputs || invalid_cursor || invalid_cap || packet_too_large)
    {
        return FALSE;
    }

    if (TRUE == SD_Header_validate(header))
    {
        header->length = (message->cursor - SI_CONST_HEADER_PREFIX_LENGTH);

        SI_SD_WIRE_serialize_header(header, message->data);
        message->length += (SI_CONST_HEADER_LENGTH + SI_SD_CONST_PREAMBLE_FLAGS_SIZE + SI_SD_CONST_PREAMBLE_RESERVED_SIZE);

        // cursor should be at the end of message buffer, should be equal to length
        if (message->cursor != message->length)
        {
            // FATAL ERROR: values are different
            return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}

boolean SI_SD_MESSAGE_invalidate(struct SI_MessageBuilder* message)
{
    return SI_MESSAGE_invalidate(message);
}

/* **************************************************** */
/*             Local function definitions               */
/* **************************************************** */

/* END OF SI_SD_MESSAGE.C FILE */
