
/**
 * @file    SD_Header.c
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   "Implements SI_SD_Header.h"
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include "SI_SD_header.h"

#include "SI_types.h"
#include "SI_header.h"

#include "SI_SD_const.h"
#include "SI_SD_config.h"

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

boolean SD_Header_check_messageID(struct SI_MessageID id)
{
    const boolean serviceID_valid = (SI_SD_CONST_SERVICE_ID == id.serviceID);
    const boolean methodID_valid = (SI_SD_CONST_METHOD_ID == id.methodID_or_eventID);

    if (serviceID_valid && methodID_valid)
    {
        return TRUE;
    }
    return FALSE;
}

boolean SD_Header_set_messageID(struct SD_Header* header, struct SI_MessageID id)
{
    if (NULLPTR == header)
    {
        return FALSE;
    }

    if (TRUE == SD_Header_check_messageID(id))
    {
        header->message_id = id;
        return TRUE;
    }
    return FALSE;
}

boolean SD_Header_check_requestID(struct SI_RequestID id)
{
    return SI_HEADER_check_requestID(id);
}

boolean SD_Header_set_requestID(struct SD_Header* header, struct SI_RequestID id)
{
    if (NULLPTR == header)
    {
        return FALSE;
    }

    if (TRUE == SD_Header_check_requestID(id))
    {
        header->request_id = id;
        return TRUE;
    }
    return FALSE;
}

boolean SD_Header_check_protVer(uint8 version)
{
    return (SI_SD_CONST_PROTO_VERSION == version);
}

boolean SD_Header_set_protVer(struct SD_Header *header, uint8 version)
{ 
    if (NULLPTR == header)
    {
        return FALSE;
    }

    if (TRUE == SD_Header_check_protVer(version))
    {
        header->protocol_version = version;
        return TRUE;
    }
    return FALSE;
}

boolean SD_Header_check_intVer(uint8 version)
{
    return (SI_SD_CONST_INTERFACE_VERSION == version);
}

boolean SD_Header_set_intVer(struct SD_Header* header, uint8 version)
{
    if (NULLPTR == header)
    {
        return FALSE;
    }

    if (TRUE == SD_Header_check_intVer(version))
    {
        header->interface_version = version;
        return TRUE;
    }
    return FALSE;
}

boolean SD_Header_check_retCode(enum SI_ReturnCode_t code)
{
    return (SI_ReturnCode_OK == code);
}

boolean SD_Header_set_retCode(struct SD_Header* header, enum SI_ReturnCode_t code)
{
    if (NULLPTR == header)
    {
        return FALSE;
    }

    if (TRUE == SD_Header_check_retCode(code))
    {
        header->return_code = code;
        return TRUE;
    }
    return FALSE;
}

boolean SD_Header_check_messageType(enum SI_MessageType_t type)
{
    return (SI_MessageType_NOTIFICATION == type);
}

boolean SD_Header_set_messageType(struct SD_Header* header, enum SI_MessageType_t type)
{
    if (NULLPTR == header)
    {
        return FALSE;
    }

    if (TRUE == SD_Header_check_messageType(type))
    {
        header->message_type = type;
        return TRUE;
    }
    return FALSE;
}

/**
 * @param preamble: The first 32 bits in a Service Discovery message after the SOME/IP header (flags + reserved)
 */
boolean SD_Header_check_preamble(uint32 preamble)
{
    const uint32 preamble_usefull_bits = (uint32)((SI_SD_CONST_PREAMBLE_REBOOT_FLAG_MASK | SI_SD_CONST_PREAMBLE_UNICAST_FLAG_MASK) << SI_SD_CONST_PREAMBLE_FLAGS_OFFS);
    const uint32 reserved = (uint32)(preamble & ~preamble_usefull_bits);
    const boolean reserved_is_null = (0u == reserved);

    return (TRUE == reserved_is_null);
}

/**
 * @param preamble: The first 32 bits in a Service Discovery message after the SOME/IP header (flags + reserved)
 */
boolean SD_Header_set_preamble(struct SD_Header* header, uint32 preamble)
{
    if (NULLPTR == header)
    {
        return FALSE;
    }

    if (TRUE == SD_Header_check_preamble(preamble))
    {
        header->preamble = preamble;
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
uint16 SD_Header_increment_sessionID(const uint16 prev_session_id)
{
    return SI_HEADER_increment_sessionID(prev_session_id);
}

boolean SD_Header_validate(const struct SD_Header* header)
{
    if (NULLPTR == header)
    {
        return FALSE;
    }

    return (SD_Header_check_messageID(header->message_id) &&
            SD_Header_check_requestID(header->request_id) &&
            SD_Header_check_protVer(header->protocol_version) &&
            SD_Header_check_intVer(header->interface_version) &&
            SD_Header_check_messageType(header->message_type) &&
            SD_Header_check_retCode(header->return_code) &&
            SD_Header_check_preamble(header->preamble));
}

/* **************************************************** */
/*             Local function definitions               */
/* **************************************************** */

/* END OF SI_SD_HANDLER.C FILE */
