/**
 * @file    SI_header.c
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   "Implements SI_header.h"
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include "SI_header.h"

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

boolean SI_HEADER_check_messageID(struct SI_MessageID id)
{
    const boolean is_method = SI_HEADER_is_method(id.methodID_or_eventID);
    const boolean is_event  = SI_HEADER_is_event(id.methodID_or_eventID);

    if (is_method || is_event)
    {
        return TRUE;
    }
    return FALSE;
}

boolean SI_HEADER_set_messageID(struct SI_Header* header, struct SI_MessageID id)
{
    if (NULLPTR == header)
    {
        return FALSE;
    }

    if (TRUE == SI_HEADER_check_messageID(id))
    {
        header->message_id = id;
        return TRUE;
    }
    return FALSE;
}

boolean SI_HEADER_check_requestID(struct SI_RequestID id)
{
#if (FALSE == SI_CFG_ENABLE_SESSION_HANDLING)
    // all session ID value has to be zero
    return (0u == id.sessionID);
#else
    // all session ID value has to be between 0x0001 and 0xFFFF.
    return (1u <= id.sessionID);
#endif
}

boolean SI_HEADER_set_requestID(struct SI_Header* header, struct SI_RequestID id)
{
    if (NULLPTR == header)
    {
        return FALSE;
    }

    if (TRUE == SI_HEADER_check_requestID(id))
    {
        header->request_id = id;
        return TRUE;
    }
    return FALSE;
}

boolean SI_HEADER_check_protVer(uint8 version)
{
    return (SI_CONST_PROTO_VERSION == version);
}

boolean SI_HEADER_set_protVer(struct SI_Header* header, uint8 version)
{
    if (NULLPTR == header)
    {
        return FALSE;
    }

    if (TRUE == SI_HEADER_check_protVer(version))
    {
        header->protocol_version = version;
        return TRUE;
    }
    return FALSE;
}

boolean SI_HEADER_check_intVer(uint8 version)
{
    // implement stricter policy if needed
    // currently every value is acceptable
    return TRUE;
}

boolean SI_HEADER_set_intVer(struct SI_Header* header, uint8 version)
{
    if (NULLPTR == header)
    {
        return FALSE;
    }

    if (TRUE == SI_HEADER_check_intVer(version))
    {
        header->interface_version = version;
        return TRUE;
    }
    return FALSE;
}

/**
 * Checks Return Code value according to Message Type field.
 * @returns TRUE if protocol rules are followed.
*/
boolean SI_HEADER_check_retCode(const struct SI_Header* header, enum SI_ReturnCode_t code)
{
    // In case of REQUEST, REQ_NO_RETURN and NOTIFY the return code must be OK
    if (((SI_MessageType_REQUEST == header->message_type) ||
         (SI_MessageType_REQUEST_NO_RETURN == header->message_type) ||
         (SI_MessageType_NOTIFICATION == header->message_type))
         && (SI_ReturnCode_OK == code))
    {
        return TRUE;
    }
    // In case of RESPONSE the return code can be anything
    else if (SI_MessageType_RESPONSE == header->message_type)
    {
        return TRUE;
    }
    // In case of ERROR the return code can be anything but OK
    else if ((SI_MessageType_ERROR == header->message_type) && (SI_ReturnCode_OK != code))
    {
        return TRUE;
    }
    return FALSE;
}

boolean SI_HEADER_set_retCode(struct SI_Header* header, enum SI_ReturnCode_t code)
{
    if (NULLPTR == header)
    {
        return FALSE;
    }

    if (TRUE == SI_HEADER_check_retCode(header, code))
    {
        header->return_code = code;
        return TRUE;
    }
    return FALSE;
}

boolean SI_HEADER_check_messageType(enum SI_MessageType_t message_type)
{
    return ((SI_MessageType_REQUEST == message_type) ||
            (SI_MessageType_REQUEST_NO_RETURN == message_type) ||
            (SI_MessageType_NOTIFICATION == message_type) ||
            (SI_MessageType_RESPONSE == message_type) ||
            (SI_MessageType_ERROR == message_type));
}

boolean SI_HEADER_set_messageType(struct SI_Header* header, enum SI_MessageType_t message_type)
{
    if (NULLPTR == header)
    {
        return FALSE;
    }

    if (TRUE == SI_HEADER_check_messageType(message_type))
    {
        header->message_type = message_type;
        return TRUE;
    }
    return FALSE;
}

/**
 * Increments sessionID by one and returns new value.
 * If SI_CFG_ENABLE_SESSION_HANDLING is true, then wraps at 0xFFFF → 0x0001.
 * Else returns zero.
 * 
 * @returns incremented sessionID value
 */
uint16 SI_HEADER_increment_sessionID(uint16 prev_session_id)
{
#if (FALSE == SI_CFG_ENABLE_SESSION_HANDLING)
    // [PRS_SOMEIP_00932]
    (void)prev_session_id;
    return 0x00u;
#else
    // [PRS_SOMEIP_00934], [PRS_SOMEIP_00521]
    if (0xFFFF == prev_session_id)
    {
        return 0x01u;
    }
    else
    {
        return (prev_session_id + 1u);
    }
#endif
}

boolean SI_HEADER_validate(const struct SI_Header* header)
{
    if (NULLPTR == header)
    {
        return FALSE;
    }

    return (SI_HEADER_check_messageID(header->message_id) &&
            SI_HEADER_check_requestID(header->request_id) &&
            SI_HEADER_check_protVer(header->protocol_version) &&
            SI_HEADER_check_messageType(header->message_type) &&
            SI_HEADER_check_retCode(header, header->return_code));  // _check_retCode() must be after _check_messageType()
}

/* **************************************************** */
/*             Local function definitions               */
/* **************************************************** */

/* END OF SI_HEADER.C FILE */
