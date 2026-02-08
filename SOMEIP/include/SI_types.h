// Include guard starts here
#ifndef SI_TYPES_H_
#define SI_TYPES_H_

/**
 * @file    SI_types.h
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   "Provides aliases for common variable types"
 * 
 * [PRS_SOMEIP_00065]
 * 
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include <stdint.h>

/* **************************************************** */
/*                       Defines                        */
/* **************************************************** */

#ifndef TRUE
#define TRUE 		(1u)
#endif
#ifndef FALSE
#define FALSE 		(0u)
#endif
#ifndef NULLPTR
#define NULLPTR 	((void*)(0u))
#endif

/* **************************************************** */
/*                  Type definitions                    */
/* **************************************************** */

/* Shortest natively supported type */
typedef uint8_t                 boolean;

/* 8bit unsigned: 0 .. 255 (0x00 .. 0xFF) */
typedef uint8_t                 uint8;

/* 16bit unsigned: 0 .. 65535 (0x0000 .. 0xFFFF) */
typedef uint16_t                uint16;

/* 32bit unsigned: 0 .. 4294967295 (0x00000000 .. 0xFFFFFFFF) */
typedef uint32_t                uint32;

/* 64bit unsigned: 0 .. 18446744073709551615 (0x0000000000000000 .. 0xFFFFFFFFFFFFFFFF) */
typedef uint64_t                uint64;

/* 8bit signed, 7bit + 1bit sign: -128 .. +127 (0x80 .. 0x7F) */
typedef int8_t                  sint8;

/* 16bit signed, 15bit + 1bit sign: -32768 .. +32767 (0x8000 .. 0x7FFF) */
typedef int16_t                 sint16;

/* 32bit signed, 31bit + 1bit sign: -2147483648 .. +2147483647 (0x80000000 .. 0x7FFFFFFF) */
typedef int32_t                 sint32;

/* 64bit signed, 63bit + 1bit sign: -9223372036854775808 .. +9223372036854775807 (0x8000000000000000 .. 0x7FFFFFFFFFFFFFFF) */
typedef int64_t                 sint64;

/* IEEE754-2008 single precision: -3.4028235e+38 .. +3.4028235e+38 */
typedef float                   float32;

/* IEEE754-2008 double precision: -1.7976931348623157+308 .. +1.7976931348623157e+308 */
typedef double                  float64;


/* **************************************************** */
/*               Function declarations                  */
/* **************************************************** */

// Include guard stops here
#endif // SI_TYPES_H_
