// Include guard starts here
#ifndef SI_MESSAGE_H_
#define SI_MESSAGE_H_

/**
 * @file    SI_message.h
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   "Provides buffering solutions.
 *           Maintains inner buffers for messages, does not allocate heap dynamically."
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include <assert.h>
#include "SI_types.h"
#include "SI_config.h"
#include "SI_header.h"

/* **************************************************** */
/*                       Defines                        */
/* **************************************************** */

/* **************************************************** */
/*                  Type definitions                    */
/* **************************************************** */

struct SI_MessageBuilder
{
    uint8* data;
    uint32 cap;
    uint32 cursor;
    uint32 length;
};

struct SI_Payload
{
    const uint8* data;
    uint32 length;
};

static_assert(SI_CFG_MSG_TXPOOL_BLOCK_SIZE < INT16_MAX, "Tx buffer length exceeds theoretical maximum (0xFFFF)!");

struct SI_MESSAGE_tx_poolElement
{
    uint8 buffer[SI_CFG_MSG_TXPOOL_BLOCK_SIZE];
    boolean used;
};

struct SI_MESSAGE_tx_pool
{
    struct SI_MESSAGE_tx_poolElement pool[SI_CFG_MSG_TXPOOL_ELEMENT_NUM];
};

/* **************************************************** */
/*               Function declarations                  */
/* **************************************************** */

boolean SI_MESSAGE_init(struct SI_MessageBuilder* out_message);
boolean SI_MESSAGE_put(struct SI_MessageBuilder* message, const uint8* payload, uint32 payload_length);
boolean SI_MESSAGE_finalize(struct SI_MessageBuilder* message, struct SI_Header* header, uint32* out_len);
boolean SI_MESSAGE_invalidate(struct SI_MessageBuilder* message);

// Include guard stops here
#endif // SI_MESSAGE_H_
