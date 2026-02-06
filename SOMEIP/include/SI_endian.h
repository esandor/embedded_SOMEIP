// Include guard starts here
#ifndef SI_ENDIAN_H_
#define SI_ENDIAN_H_

/**
 * @file    SI_endian.h
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   "Provides substitution for hton and ntoh functions.
 *           Network endianness is big-endian.
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include "SI_types.h"
#include "SI_config.h"

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
 * Converts u16 value into byte array (Big-endian)
 */
static inline void u16_to_u8array(uint8* out, uint16 value_16bit)
{
    out[0u] = (uint8)((value_16bit >> 8u) & 0xFFu);
    out[1u] = (uint8)( value_16bit        & 0xFFu);
}

/**
 * Converts u24 value into byte array (Big-endian)
 */
static inline void u24_to_u8array(uint8 *out, uint32 value_24bit)
{
    out[0u] = (uint8)((value_24bit >> 16u) & 0xFFu);
    out[1u] = (uint8)((value_24bit >> 8u)  & 0xFFu);
    out[2u] = (uint8)( value_24bit         & 0xFFu);
}

/**
 * Converts u32 value into byte array (Big-endian)
 */
static inline void u32_to_u8array(uint8* out, uint32 value_32bit)
{
    out[0u] = (uint8)((value_32bit >> 24u) & 0xFFu);
    out[1u] = (uint8)((value_32bit >> 16u) & 0xFFu);
    out[2u] = (uint8)((value_32bit >> 8u)  & 0xFFu);
    out[3u] = (uint8)( value_32bit         & 0xFFu);
}

/**
 * Converts byte array into uint16 value (Big-endian)
 */
static inline uint16 u8array_to_u16(const uint8* p)
{
    return (uint16)(((uint16)p[0u] << 8u) | p[1u]);
}

/**
 * Converts byte array into uint24 value (Big-endian)
 */
static inline uint32 u8array_to_u24(const uint8 *p)
{
    return ((uint32)p[0u] << 16u) | ((uint32)p[1u] << 8u) | (uint32)p[2u];
}

/**
 * Converts byte array into uint32 value (Big-endian)
 */
static inline uint32 u8array_to_u32(const uint8* p)
{
    return ((uint32)p[0u] << 24u) | ((uint32)p[1u] << 16u) | ((uint32)p[2u] << 8u)  | ((uint32)p[3u]);
}

// Include guard stops here
#endif // SI_ENDIAN_H_