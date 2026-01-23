// Include guard starts here
#ifndef SI_MESSAGE_H_
#define SI_MESSAGE_H_

/**
 * @file    SI_message.h
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   "Provides message transmission opportunity.
 *           Maintains inner Tx buffer for messages, does not allocate heap dynamically."
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include "SI_types.h"
#include "SI_config.h"
#include "SI_header.h"

#include "SI_wire.h"

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

boolean SI_MESSAGE_init(struct SI_MessageBuilder*);
boolean SI_MESSAGE_put(struct SI_MessageBuilder*, const uint8*, uint32);
boolean SI_MESSAGE_finalize(struct SI_MessageBuilder*, struct SI_Header*, uint32*);
boolean SI_MESSAGE_invalidate(struct SI_MessageBuilder*);

// Include guard stops here
#endif // SI_MESSAGE_H_
