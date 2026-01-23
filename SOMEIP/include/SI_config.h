// Include guard starts here
#ifndef SI_CONFIG_H_
#define SI_CONFIG_H_

/**
 * @file    SI_config.h
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   "Provides configuration options for SOME/IP Protocol.
 *           Modify values toughfully, improper configuration might cause unintended behaviour."
 * 
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include <assert.h>

#include "SI_types.h"
#include "SI_const.h"

/* **************************************************** */
/*                       Defines                        */
/* **************************************************** */

/**
 * TRUE: Session IDs incremented at every message, wraps at 0xFFFF -> 0x0001.
 * FALSE: Session IDs are always zero.
 */
#define SI_CFG_ENABLE_SESSION_HANDLING          (TRUE)

/**
 * Maximum number of services that an endpoint can handle.
 */
#define SI_CFG_MAX_SERVICES                     (8u)

/**
 * Maximum number of methods that a service can handle.
 */
#define SI_CFG_MAX_METHODS                      (SI_CFG_MAX_SERVICES)

/**
 * Maximum number of eventgroups that an endpoint can handle.
 */
#define SI_CFG_MAX_EVENT_GROUPS                 (8u)

/**
 * Size of a single Tx buffer
 * @note theoretical maximum value: 0xFFFF
 */
#define SI_CFG_MSG_TXPOOL_BLOCK_SIZE            (SI_CONST_UDP_MTU_LENGTH)

/**
 * Number of Tx buffers an enpoint can handle
 * @note theoretical maximum value: 0xFFFF
 */
#define SI_CFG_MSG_TXPOOL_ELEMENT_NUM           ((uint16)(8u))

/**
 * SOME/IP specifications states that middleware should operate with both connection-oriented and connectionless protocols .
 * However it specificly recommends to use UDP over TCP, because of the synchronization overhead of TCP.
 * 
 * TRUE: Communication over UDP protocol is enabled.
 * FALSE: Communication over UDP protocol is disabled.
 */
#define SI_CFG_ENABLE_UDP                       (TRUE)

/**
 * Application port for SOME/IP operation.
 */
#define SI_CFG_UNICAST_UDP_PORT                 (5005u)

/* **************************************************** */
/*                  Type definitions                    */
/* **************************************************** */

/* **************************************************** */
/*               Function declarations                  */
/* **************************************************** */

// Include guard stops here
#endif // SI_CONFIG_H_
