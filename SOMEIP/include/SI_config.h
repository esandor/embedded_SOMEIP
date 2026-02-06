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
 * Setting this value to higher numbers will cause more memory usage.
 */
#define SI_CFG_MAX_SERVICES                     (8u)

/**
 * Maximum number of methods that a service can handle.
 * Setting this value to higher numbers will cause more memory usage.
 */
#define SI_CFG_MAX_METHODS                      (SI_CFG_MAX_SERVICES)

/**
 * TRUE: Event and eventgroup handling is enabled.
 * FALSE: Event and eventgroup handling is disabled.
 */
#define SI_CFG_ENABLE_EVENTS                    (FALSE)

#if (TRUE == SI_CFG_ENABLE_EVENTS)

/**
 * Maximum number of eventgroups that an endpoint can handle.
 */
#define SI_CFG_MAX_EVENT_GROUPS                 (8u)

#endif

/**
 * TRUE: SOME/IP Transmission protocol is implemented, message fragmentation is handled properly.
 * FALSE: SOME/IP Transmission protocol is missing, Tx size is limited.
 */
#define SI_CFG_TRANSMISSION_PROTOCOL_EXISTS     (FALSE)

/**
 * Size of a single Tx buffer
 * @note theoretical maximum value: 0xFFFF
 */
#define SI_CFG_MSG_TXPOOL_BLOCK_SIZE            (SI_CONST_UDP_MTU_LENGTH)

/**
 * Number of Tx buffers an enpoint has
 * Setting this value to higher numbers will cause more memory usage.
 */
#define SI_CFG_MSG_TXPOOL_ELEMENT_NUM           (8u)

/**
 * SOME/IP middleware should be able to operate with both connection-oriented and connectionless protocols.
 * However it specificly recommends to use UDP over TCP, because of the synchronization overhead of TCP.
 * 
 * FALSE: Communication only possible via UDP.
 * TRUE: Communication is possible with TCP and UDP.
 */
#define SI_CFG_ENABLE_TCP                       (FALSE)

/**
 * Application port for SOME/IP operation over UDP.
 */
#define SI_CFG_UNICAST_UDP_PORT                 (5005u)

#if (TRUE == SI_CFG_ENABLE_TCP)

/**
 * Application port for SOME/IP operation over TCP.
 */
#define SI_CFG_UNICAST_TCP_PORT                 (5005u)

#endif

/* **************************************************** */
/*                  Type definitions                    */
/* **************************************************** */

/* **************************************************** */
/*               Function declarations                  */
/* **************************************************** */

// Include guard stops here
#endif // SI_CONFIG_H_
