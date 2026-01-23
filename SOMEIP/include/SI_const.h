// Include guard starts here
#ifndef SI_CONST_H_
#define SI_CONST_H_

/**
 * @file    SI_const.h
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   "Provides constant values for SOME/IP Protocol. 
 *           Modification of the values below is strictly prohibited."
 * 
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include "SI_types.h"

/* **************************************************** */
/*                       Defines                        */
/* **************************************************** */

/**
 * SOME/IP protocol version value. Equals to 0x01 per specification.
 */
#define SI_CONST_PROTO_VERSION                  (0x01u)

/**
 * Total header length in bytes
 */ 
#define SI_CONST_HEADER_LENGTH                  ((uint32)(16u))

/**
 * Length of the second part of header that IS included into the length field
 */
#define SI_CONST_HEADER_TAIL_LENGTH             ((uint32)(8u))

/**
 * Length of the first part of header that is NOT included into the length field
 */
#define SI_CONST_HEADER_PREFIX_LENGTH           (SI_CONST_HEADER_LENGTH - SI_CONST_HEADER_TAIL_LENGTH)

/**
 * Ethernet II Maximum Transmission Unit value.
 * Eth II MTU is 1500 byte, bigger payloads will be fragmented and sent via multiple UDP messages.
 * In order to keep this application simple, user can not send too big data for one MTU.
 */
#define SI_CONST_UDP_MTU_LENGTH                 ((uint32)(1400u))

/* **************************************************** */
/*                  Type definitions                    */
/* **************************************************** */

/* **************************************************** */
/*               Function declarations                  */
/* **************************************************** */

// Include guard stops here
#endif // SI_CONST_H_