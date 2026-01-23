// Include guard starts here
#ifndef SI_SD_CONFIG_H_
#define SI_SD_CONFIG_H_

/**
 * @file    SI_SD_config.h
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   "Provides configuration options for Service Discovery.
 *           Modify values toughfully, improper configuration might cause unintended behaviour."
 * 
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include "SI_types.h"
#include "SI_SD_const.h"

/* **************************************************** */
/*                       Defines                        */
/* **************************************************** */

#define SI_SD_ENABLE_SERVICE_DISCOVERY      (TRUE)

/**
 * Maximum number of services that a node can handle
 */
#define SI_SD_CFG_MAX_REMOTE_SERVICES       (8u)

/**
 * Maximum number of subscriptions that a node can handle
 */
#define SI_SD_CFG_MAX_SUBSCRIPTIONS        (8u)

/**
 * Port used by SOME/IP-SD protocoll to communicate multicast messages.
 */
#define SI_SD_CFG_PORT_NUMBER               (SI_SD_CONST_DEFAULT_PORT_NUMBER)

/**
 * The maximum number of entries that a node can handle
 */
#define SI_SD_SERVICE_ENTRIES_MAX_NUMBER    (SI_SD_CFG_MAX_REMOTE_SERVICES)

/**
 * The maximum number of options that a node can handle
 */
#define SI_SD_SERVICE_OPTIONS_MAX_NUMBER    (SI_SD_CFG_MAX_REMOTE_SERVICES)

/* **************************************************** */
/*                  Type definitions                    */
/* **************************************************** */

/* **************************************************** */
/*               Function declarations                  */
/* **************************************************** */

// Include guard stops here
#endif // SI_SD_CONFIG_H_