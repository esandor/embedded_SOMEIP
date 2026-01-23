// Include guard starts here
#ifndef SI_SD_MESSAGE_H_
#define SI_SD_MESSAGE_H_

/**
 * @file    SI_SD_message.h
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   "Serves as a wrapper around SI_Message modul. Every SD message is transmitted through SI logic."
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include <string.h>         // for memcpy
#include <assert.h>

#include "SI_types.h"
#include "SI_const.h"
#include "SI_config.h"
#include "SI_message.h"

#include "SI_SD_const.h"
#include "SI_SD_header.h"
#include "SI_SD_wire.h"

/* **************************************************** */
/*                       Defines                        */
/* **************************************************** */

/* **************************************************** */
/*                  Type definitions                    */
/* **************************************************** */

/* **************************************************** */
/*               Function declarations                  */
/* **************************************************** */

boolean SI_SD_MESSAGE_init(struct SI_MessageBuilder* message);
boolean SI_SD_MESSAGE_put(struct SI_MessageBuilder* message, const uint8* payload, uint32 payload_length);
boolean SI_SD_MESSAGE_finalize(struct SI_MessageBuilder* message, struct SD_Header* header);
boolean SI_SD_MESSAGE_invalidate(struct SI_MessageBuilder* message);

// Include guard stops here
#endif // SI_SD_MESSAGE_H_
