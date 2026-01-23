// Include guard starts here
#ifndef SI_SD_WIRE_H_
#define SI_SD_WIRE_H_

/**
 * @file    SI_SD_wire.h
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   "Interface for converting elementary units of received byte streams into structs."
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include "SI_types.h"
#include "SI_endian.h"
#include "SI_header.h"
#include "SI_message.h"
#include "SI_config.h"

#include "SI_SD_service_manager.h"
#include "SI_SD_header.h"

/* **************************************************** */
/*                       Defines                        */
/* **************************************************** */

/* **************************************************** */
/*                  Type definitions                    */
/* **************************************************** */

/* **************************************************** */
/*               Function declarations                  */
/* **************************************************** */

void SI_SD_WIRE_serialize_header(const struct SD_Header* in_header, uint8* out_header);
void SI_SD_WIRE_deserialize_header(const uint8* in_header, struct SD_Header* out_header);

void SI_SD_WIRE_deserialize_ServiceEntry(uint8 *in_entry, struct SI_SD_ServiceEntry *out_entry);
void SI_SD_WIRE_deserialize_IPv4EndpointOption(uint8 *in_option, struct SI_SD_IPv4EndpointOption *out_option);

/* **************************************************** */
/*               Function definitions                   */
/* **************************************************** */

#endif /* SI_SD_WIRE_H_ */
