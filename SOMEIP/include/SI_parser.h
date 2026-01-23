// Include guard starts here
#ifndef SI_PARSER_H_
#define SI_PARSER_H_

/**
 * @file    SI_parser.h
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   "Convert received byte stream into SI_Header and SI_Payload structures.
 *           Hides SI_wire.h modul, provides easier use."
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include "SI_types.h"
#include "SI_header.h"
#include "SI_message.h"

#include "SI_config.h"
#include "SI_wire.h"

/* **************************************************** */
/*                       Defines                        */
/* **************************************************** */

/* **************************************************** */
/*                  Type definitions                    */
/* **************************************************** */

/* **************************************************** */
/*               Function declarations                  */
/* **************************************************** */

/**
 * Checks packet length according to UDP layer and SOME/IP packet header.
 * 
 * @param udp_payload: raw byte array from UDP layer
 * @param udp_payload_length: total length of udp_payload array
 * @param out_header: parsed SOME/IP header
 * @param out_payload: pointer to SOME/IP payload 
 */
boolean SI_PARSER_parse_datagram(const uint8* udp_payload, uint32 udp_payload_length,
                                 struct SI_Header* out_header, struct SI_Payload* out_payload);

// Include guard stops here
#endif // SI_PARSER_H_