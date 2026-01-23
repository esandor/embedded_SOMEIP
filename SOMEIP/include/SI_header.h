// Include guard starts here
#ifndef SI_HEADER_H_
#define SI_HEADER_H_

/**
 * @file    SI_header.h
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   "Interface for handling SOME/IP header, provides set/check functions for easy handling."
 * 
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include "SI_types.h"

#include "SI_config.h"

/* **************************************************** */
/*                       Defines                        */
/* **************************************************** */

#define SI_HEADER_METHOD_ID_MAX     ((uint16)(0x7FFFu))
#define SI_HEADER_EVENT_ID_MIN      ((uint16)(0x8000u))
#define SI_HEADER_EVENT_ID_MAX      ((uint16)(0xFFFFu))

/* **************************************************** */
/*                  Type definitions                    */
/* **************************************************** */

/**
 * 32 bit
 * First field of header layout.
 * Contains Service_ID and Method_ID.
 * @note Note: The assignment of the Message ID is up to the user / system designer. However,
 * the Message ID is assumed be unique for the whole system (i.e. the vehicle).
 */
struct SI_MessageID
{
    uint16 serviceID; // MSB, For distinguishing services.
    uint16 methodID_or_eventID;  // LSB, For distinguishing service elements (namely methods and/or events). Method range: 0x0000 .. 0x7FFF (first bit of Method-ID is 0). Event range:  0x8000 .. 0xFFFF (first bit of Method-ID is 1)
};

/**
 * 32 bits
 * Allows an endpoint to differentiate multiple parallel uses of the same service.
 * Contains Client_ID and Session_ID.
 * @note Unique for a request-response pair to differentiate between multiple calls of the same service.
 */
struct SI_RequestID
{
	uint16 clientID;  // MSB, Unique identifier for the calling client inside the ECU. The Client ID allows an ECU to differentiate calls from multiple clients to the same method.
	uint16 sessionID; // LSB, Unique identifier that allows to distinguish sequential messages or requests originating from the same sender from each other.
};

enum SI_MessageType_t
{
    SI_MessageType_REQUEST              = (uint8)0x00u,
    SI_MessageType_REQUEST_NO_RETURN    = (uint8)0x01u,
    SI_MessageType_NOTIFICATION         = (uint8)0x02u,
    SI_MessageType_RESPONSE             = (uint8)0x80u,
    SI_MessageType_ERROR                = (uint8)0x81u,
};

/**
 * Specification part 4.2.6.1: [PRS_SOMEIP_00191]
 * 0x10 .. 0x1f: Reserved for generic SOME/IP errors. These errors will be specified in future versions of the official specification document.
 * 0x20 .. 0x5E: Reserved for specific errors of services and methods. These errors are specified by the interface specification.
 */
enum SI_ReturnCode_t
{
    SI_ReturnCode_OK                        = (uint8)0x00u,
    SI_ReturnCode_NOT_OK                    = (uint8)0x01u,
    SI_ReturnCode_UNKNOWN_SERVICE           = (uint8)0x02u,
    SI_ReturnCode_UNKNOWN_METHOD            = (uint8)0x03u,
    SI_ReturnCode_NOT_READY                 = (uint8)0x04u,
    SI_ReturnCode_NOT_REACHABLE             = (uint8)0x05u,
    SI_ReturnCode_TIMEOUT                   = (uint8)0x06u,
    SI_ReturnCode_WRONG_PROTOCOL_VERSION    = (uint8)0x07u,
    SI_ReturnCode_WRONG_INTERFACE_VERSION   = (uint8)0x08u,
    SI_ReturnCode_MALFORMED_MESSAGE         = (uint8)0x09u,
    SI_ReturnCode_WRONG_MESSAGE_TYPE        = (uint8)0x0Au,
    SI_ReturnCode_E2E_REPEATED              = (uint8)0x0Bu,
    SI_ReturnCode_E2E_WRONG_SEQUENCE        = (uint8)0x0Cu,
    SI_ReturnCode_E2E                       = (uint8)0x0Du,
    SI_ReturnCode_E2E_NOT_AVAILABLE         = (uint8)0x0Eu,
    SI_ReturnCode_E2E_NO_NEW_DATA           = (uint8)0x0Fu
};

struct SI_Header
{
    struct SI_MessageID message_id;     // [service_id:16 | method_or_event_id:16]
    uint32 length;                      // bytes after this field
    struct SI_RequestID request_id;     // [client_id:16 | session_id:16]
    uint8 protocol_version;
    uint8 interface_version;
    enum SI_MessageType_t message_type;
    enum SI_ReturnCode_t return_code;
};

/* **************************************************** */
/*               Function declarations                  */
/* **************************************************** */

boolean SI_HEADER_check_messageID(struct SI_MessageID);
boolean SI_HEADER_set_messageID(struct SI_Header*, struct SI_MessageID );
boolean SI_HEADER_check_requestID(struct SI_RequestID);
boolean SI_HEADER_set_requestID(struct SI_Header*, struct SI_RequestID);
boolean SI_HEADER_check_protVer(uint8);
boolean SI_HEADER_set_protVer(struct SI_Header* h, uint8 version);
boolean SI_HEADER_check_intVer(uint8 version);
boolean SI_HEADER_set_intVer(struct SI_Header* h, uint8 version);
boolean SI_HEADER_check_retCode(const struct SI_Header*, enum SI_ReturnCode_t);
boolean SI_HEADER_set_retCode(struct SI_Header*, enum SI_ReturnCode_t);
boolean SI_HEADER_check_messageType(enum SI_MessageType_t);
boolean SI_HEADER_set_messageType(struct SI_Header*, enum SI_MessageType_t);
uint16  SI_HEADER_increment_sessionID(uint16);
boolean SI_HEADER_validate(const struct SI_Header*);

/* **************************************************** */
/*               Function definitions                   */
/* **************************************************** */

static inline boolean SI_HEADER_is_method(uint16 methodID_or_eventID)
{
    return (SI_HEADER_METHOD_ID_MAX >= methodID_or_eventID);
}
static inline boolean SI_HEADER_is_event(uint16 methodID_or_eventID)
{
    return ((SI_HEADER_EVENT_ID_MIN <= methodID_or_eventID) &&
            (SI_HEADER_EVENT_ID_MAX >= methodID_or_eventID));
}

// Include guard stops here
#endif // SI_HEADER_H_