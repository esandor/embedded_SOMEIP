// Include guard starts here
#ifndef SI_WIRE_H_
#define SI_WIRE_H_

/**
 * @file    SI_wire.h
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   "Interface for converting elementary units of received byte streams into structs."
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include "SI_types.h"
#include "SI_header.h"

#include <string.h>     // for memcpy
#include "SI_endian.h"

/* **************************************************** */
/*                       Defines                        */
/* **************************************************** */

/* **************************************************** */
/*                  Type definitions                    */
/* **************************************************** */

/* **************************************************** */
/*               Function declarations                  */
/* **************************************************** */

void SI_WIRE_serialize_header(const struct SI_Header* h, uint8* out_p);
void SI_WIRE_deserialize_header(const uint8* in_p, struct SI_Header* out_header);

// Include guard stops here
#endif // SI_WIRE_H_