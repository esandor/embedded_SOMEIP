// Include guard starts here
#ifndef SI_SD_CONST_H_
#define SI_SD_CONST_H_

/**
 * @file    SI_SD_const.h
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   "Provides constant values for Service Discovery. 
 *           Modification of the values below is strictly prohibited."
 * 
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include "SI_types.h"
#include "SI_const.h"

/* **************************************************** */
/*                       Defines                        */
/* **************************************************** */

/**
 * SOME/IP Service Discovery protocol version value. Equals to 0x01 per specification.
 */
#define SI_SD_CONST_PROTO_VERSION                   (0x01)

/**
 * SOME/IP Service Discovery interface version value. Equals to 0x01 per specification.
 */
#define SI_SD_CONST_INTERFACE_VERSION               (0x01)

/**
 * per specification for SOME/IP Service Discovery specification
 */
#define SI_SD_CONST_SERVICE_ID                      (0xFFFFu)

/**
 * per specification for SOME/IP Service Discovery specification
 */
#define SI_SD_CONST_METHOD_ID                       (0x8100u)

/**
 * per specification for SOME/IP Service Discovery specification
 */
#define SI_SD_CONST_MESSAGE_TYPE                    (SI_MessageType_NOTIFICATION)

/**
 * per specification for SOME/IP Service Discovery specification
 */
#define SI_SD_CONST_RETURN_CODE                     (SI_ReturnCode_OK)

/**
 * Port used by SOME/IP-SD protocoll to communicate multicast messages.
 */
#define SI_SD_CONST_DEFAULT_PORT_NUMBER             (30490u)

/**
 * The minimum length of SOME/IP-SD message (in bytes), containing only the header
 */
#define SI_SD_CONST_HEADER_LENGTH                   (SI_CONST_HEADER_LENGTH + 12u)

/**
 * Size of "Flags" field in the Service Discovery preamble (in bytes)
 */
#define SI_SD_CONST_PREAMBLE_FLAGS_SIZE             (1u)

/**
 * Size of reserved bits in the Service Discovery preamble (in bytes)
 */
#define SI_SD_CONST_PREAMBLE_RESERVED_SIZE          (3u)

/**
 * In SOME/IP Service Discovery messages after SI_Header, the next byte is Flags.
 */
#define SI_SD_CONST_PREAMBLE_FLAGS_OFFS             (24u)
#define SI_SD_CONST_PREAMBLE_FLAGS_MASK             (0xFF000000u)

/**
 * Flags (8b): bit7=Reboot, bit6=Unicast, others=Reserved
 */
#define SI_SD_CONST_PREAMBLE_REBOOT_FLAG_OFFS       (7u)
#define SI_SD_CONST_PREAMBLE_REBOOT_FLAG_MASK       (0x80u)

/**
 * In SOME/IP Service Discovery messages after SI_Header: flags (8b): bit7=Reboot, bit6=Unicast, others=Reserved
 */
#define SI_SD_CONST_PREAMBLE_UNICAST_FLAG_OFFS      (6u)
#define SI_SD_CONST_PREAMBLE_UNICAST_FLAG_MASK      (0x40u)

/**
 * The size of a single entry in a Service Discovery payload (in bytes)
 */
#define SI_SD_CONST_ENTRY_ARRAY_SIZE                (16u)

/**
 * The size of a single option in a Service Discovery payload (in bytes)
 */
#define SI_SD_CONST_OPTION_ARRAY_SIZE               (12u)

/**
 * Size of the "Entries array length" field 
 */
#define SI_SD_CONST_ENTRIES_ARRAY_LENGTH_SIZE       (4u)

/**
 * Size of the "Options array length" field 
 */
#define SI_SD_CONST_OPTIONS_ARRAY_LENGTH_SIZE       (4u)

/**
 *  Dividing by SI_SD_CONST_ENTRY_ARRAY_SIZE equals to shifting right SI_SD_CONST_ENTRY_ARRAY_DIV_RATIO times
 */
#define SI_SD_CONST_ENTRY_ARRAY_DIV_RATIO           (4u)

#define SI_SD_CONST_IPV4_PROTOCOL_TYPE_UDP          (0x11u)

/* **************************************************** */
/*                  Type definitions                    */
/* **************************************************** */

/* **************************************************** */
/*               Function declarations                  */
/* **************************************************** */

// Include guard stops here
#endif // SI_SD_CONST_H_