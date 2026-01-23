// Include guard starts here
#ifndef SI_SD_PARSER_H_
#define SI_SD_PARSER_H_

/**
 * @file    SI_SD_parser.h
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   "Parser modul responsible to turn received byte arrays into organized structures.
 *           The main task of the modul is to fill received_service_registry variable for SI_SD_provider.
 * 
 *           The modul is implemented with major simplifications:
 *           It assumes that in every received Service Discovery message there is only one Service Entry (type 1 entry)
 *           and only one IPv4 Endpoint type Option. The modul is partially prepared for bigger SD messages and it can be 
 *           fixed by re-writing the SI_SD_PARSER_fill_received_service_registry() function logic."
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include "SI_types.h"

#include "SI_SD_config.h"
#include "SI_SD_header.h"

/* **************************************************** */
/*                       Defines                        */
/* **************************************************** */

/* **************************************************** */
/*                  Type definitions                    */
/* **************************************************** */

struct SD_Payload;

/* **************************************************** */
/*               Function declarations                  */
/* **************************************************** */

boolean SI_SD_PARSER_parse_datagram(uint8* udp_payload, uint32 udp_payload_length, struct SD_Header* out_header, struct SD_Payload* out_payload);
boolean SI_SD_PARSER_parse_payload(struct SD_Payload* in_payload);

// Include guard stops here
#endif // SI_SD_PARSER_H_
