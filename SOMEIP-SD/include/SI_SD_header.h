// Include guard starts here
#ifndef SI_SD_HEADER_H_
#define SI_SD_HEADER_H_

/**
 * @file    SI_SD_Header.h
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   "Interface for handling SOME/IP Service Discovery header, provides set/check functions for easy handling."
 * 
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include "SI_types.h"
#include "SI_header.h"

#include "SI_SD_const.h"
#include "SI_SD_config.h"

/* **************************************************** */
/*                       Defines                        */
/* **************************************************** */

/* **************************************************** */
/*                  Type definitions                    */
/* **************************************************** */

struct SD_Header
{
    struct SI_MessageID message_id;     // [service_id:16 | method_or_event_id:16]
    uint32 length;                      // bytes after this field
    struct SI_RequestID request_id;     // [client_id:16 | session_id:16]
    uint8 protocol_version;
    uint8 interface_version;
    enum SI_MessageType_t message_type;
    enum SI_ReturnCode_t return_code;
    uint32 preamble;
};

/* **************************************************** */
/*               Function declarations                  */
/* **************************************************** */

boolean SD_Header_check_messageID(struct SI_MessageID id);
boolean SD_Header_set_messageID(struct SD_Header* header, struct SI_MessageID id);
boolean SD_Header_check_requestID(struct SI_RequestID id);
boolean SD_Header_set_requestID(struct SD_Header* header, struct SI_RequestID id);
boolean SD_Header_check_protVer(uint8 version);
boolean SD_Header_set_protVer(struct SD_Header *header, uint8 version);
boolean SD_Header_check_intVer(uint8 version);
boolean SD_Header_set_intVer(struct SD_Header* header, uint8 version);
boolean SD_Header_check_retCode(enum SI_ReturnCode_t code);
boolean SD_Header_set_retCode(struct SD_Header* header, enum SI_ReturnCode_t code);
boolean SD_Header_check_messageType(enum SI_MessageType_t type);
boolean SD_Header_set_messageType(struct SD_Header* header, enum SI_MessageType_t type);
boolean SD_Header_check_preamble(uint32 preamble);
boolean SD_Header_set_preamble(struct SD_Header* header, uint32 preamble);
uint16 SD_Header_increment_sessionID(const uint16 prev_session_id);
boolean SD_Header_validate(const struct SD_Header* header);

// Include guard stops here
#endif // SI_SD_HEADER_H_