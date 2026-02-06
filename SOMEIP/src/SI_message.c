/**
 * @file    SI_message.c
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   "Implements SI_message.h"
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include "SI_message.h"

#include <string.h>         // for memcpy
#include <assert.h>

#include "SI_types.h"
#include "SI_config.h"
#include "SI_header.h"
#include "SI_wire.h"
#include "ERH.h"

static_assert(SI_CFG_MSG_TXPOOL_BLOCK_SIZE >= SI_CONST_HEADER_LENGTH, "FATAL ERROR: Configured SOME/IP message pool block size is below the minimum length!");
#if (FALSE == SI_CFG_TRANSMISSION_PROTOCOL_EXISTS)
static_assert(SI_CFG_MSG_TXPOOL_BLOCK_SIZE <= SI_CONST_UDP_MTU_LENGTH, "FATAL ERROR: Configured SOME/IP message pool block size is bigger than UDP MTL size and SW implementation does not support datagram segmentation!");
#endif

/* **************************************************** */
/*                       Defines                        */
/* **************************************************** */

/* **************************************************** */
/*               Static global variables                */
/* **************************************************** */

static struct SI_MESSAGE_tx_pool g_tx_message_pool;

/* **************************************************** */
/*                True global variables                 */
/* **************************************************** */

/* **************************************************** */
/*                Local type definitions                */
/* **************************************************** */

enum SI_MSG_ErrType_t
{
    SI_MSG_ErrType_length_mismatch = 0u,
    SI_MSG_ErrType_tx_pool_overflow = 1u
};

/* **************************************************** */
/*             Local function declarations              */
/* **************************************************** */

static void SI_MESSAGE_get_length(const struct SI_Header* header, uint32* out_len);
static uint32 SI_MESSAGE_allocate(void);
static void SI_MESSAGE_report_error(enum SI_MSG_ErrType_t type, struct SI_Header* header, uint32 cursor);

/* **************************************************** */
/*             Global function definitions              */
/* **************************************************** */

/**
 * @param out_message: Tx buffer will be allocated for this builder object
 */
boolean SI_MESSAGE_init(struct SI_MessageBuilder* out_message)
{
    uint32 index = SI_CFG_MSG_TXPOOL_ELEMENT_NUM;
    
    if (NULLPTR == out_message)
    {
        return FALSE;
    }

    index = SI_MESSAGE_allocate();
    if (SI_CFG_MSG_TXPOOL_ELEMENT_NUM == index)
    {
        SI_MESSAGE_report_error(SI_MSG_ErrType_tx_pool_overflow, NULLPTR, 0u);
        return FALSE;
    }

    out_message->cursor = SI_CONST_HEADER_LENGTH;   //place cursor to the start of payload
    out_message->data = g_tx_message_pool.pool[index].buffer;
    out_message->cap = SI_CFG_MSG_TXPOOL_BLOCK_SIZE;
    out_message->length = 0u;
    return TRUE;
}

/**
 * @param message: builder that's payload need to be filled
 * @param payload: pointer to array containing the data
 * @param payload_length: length of payload array in bytes
 */
boolean SI_MESSAGE_put(struct SI_MessageBuilder* message, const uint8* payload, uint32 payload_length)
{
    const boolean invalid_inputs = ((NULLPTR == message) ||
                                    ((0u < payload_length) && (NULLPTR == payload)));
    const boolean invalid_cursor = (message->cursor > message->cap);
    // IMPORTANT: payload_too_large check must be after invalid_cursor check in order to prevent issues due to unsigned integer overflow
    const boolean payload_too_large = (((message->cap - message->cursor) < payload_length) ||
                                       ((SI_CONST_UDP_MTU_LENGTH - SI_CONST_HEADER_LENGTH) < payload_length));

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

/**
 * @param message: builder, containing the payload already
 * @param header: header need to add to message
 * @param out_len (optional): provides caller with total message length value. If not needed give NULLPTR.
 */
boolean SI_MESSAGE_finalize(struct SI_MessageBuilder* message, struct SI_Header* header, uint32* out_len)
{
    const boolean invalid_inputs = ((NULLPTR == message) || (NULLPTR == header));
    const boolean invalid_cursor = ((SI_CONST_HEADER_LENGTH > message->cursor) || (message->cursor > message->cap));
    const boolean invalid_cap = (SI_CONST_HEADER_LENGTH > message->cap);
    const boolean packet_too_large = (SI_CONST_UDP_MTU_LENGTH <= message->cursor);

    if (invalid_inputs || invalid_cursor || invalid_cap || packet_too_large)
    {
        return FALSE;
    }

    if (TRUE == SI_HEADER_validate(header))
    {
        header->length = (message->cursor - SI_CONST_HEADER_PREFIX_LENGTH);

        SI_WIRE_serialize_header(header, message->data);
        message->length += SI_CONST_HEADER_LENGTH;

        // cursor should be at the end of message buffer, should be equal to length
        if (message->cursor != message->length)
        {
            SI_MESSAGE_report_error(SI_MSG_ErrType_length_mismatch, header, message->cursor);
            return FALSE;
        }

        SI_MESSAGE_get_length(header, out_len);
        return TRUE;
    }
    return FALSE;
}

boolean SI_MESSAGE_invalidate(struct SI_MessageBuilder* message)
{
    uint16 i = 0u;

    if ((NULLPTR == message) || (NULLPTR == message->data))
    {
        return FALSE;
    }

    for (i = 0u; i < SI_CFG_MSG_TXPOOL_ELEMENT_NUM; i++)
    {
        if (message->data == g_tx_message_pool.pool[i].buffer)
        {
            g_tx_message_pool.pool[i].used = FALSE;
            message->data = NULLPTR;
            message->cursor = 0u;
            message->cap = 0u;
            message->length = 0u;
            return TRUE;
        }
    }
    return FALSE;
}

/* **************************************************** */
/*             Local function definitions               */
/* **************************************************** */

/**
 * @returns Pool element index if successfull, SI_CFG_MSG_TXPOOL_ELEMENT_NUM if failed. 
 */
static uint32 SI_MESSAGE_allocate(void)
{
    uint32 i = 0u;

    for (i = 0u; i < SI_CFG_MSG_TXPOOL_ELEMENT_NUM; i++)
    {
        if (FALSE == g_tx_message_pool.pool[i].used)
        {
            g_tx_message_pool.pool[i].used = TRUE;
            break;
        }
    }
    return i;
}

static void SI_MESSAGE_get_length(const struct SI_Header* header, uint32* out_len)
{
    if (NULLPTR == header)
    {
        return;
    }

    if (NULLPTR != out_len)
    {
        *out_len = header->length + SI_CONST_HEADER_PREFIX_LENGTH;
    }
}

static void SI_MESSAGE_report_error(enum SI_MSG_ErrType_t type, struct SI_Header* header, uint32 cursor)
{
    if (NULLPTR != header)
    {
    ERH_report_error(ERH_SI_MESSAGE_ERROR,
                    type,
                    ((header->message_id.serviceID << 16u) | (header->message_id.methodID_or_eventID)),
                    ((header->request_id.clientID << 16u) | (header->request_id.sessionID)),
                    ((header->message_type << 24u) | (header->return_code << 16u) | (header->protocol_version << 8u) | (header->interface_version)),
                    (header->length), cursor);
    }
    else
    {
        ERH_report_error(ERH_SI_MESSAGE_ERROR, type, 0u, 0u, 0u, 0u, 0u);
    }
}

/* END OF SI_MESSAGE.C FILE */
