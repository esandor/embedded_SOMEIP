// Include guard starts here
#ifndef SI_ENDIAN_H_
#define SI_ENDIAN_H_

/**
 * @file    SI_endian.h
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   "Provides equivalents for hton and ntoh functions"
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include "SI_types.h"

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
static inline void be_put_u16(uint8* out, uint16 value_16bit)
{
    out[0] = (uint8)((value_16bit >> 8) & 0xFFu);
    out[1] = (uint8)( value_16bit       & 0xFFu);
}

/**
 * Converts u24 value into byte array (Big-endian)
 */
static inline void be_put_u24(uint8 *out, uint32 value_24bit)
{
    out[0] = (uint8)((value_24bit >> 16) & 0xFFu);
    out[1] = (uint8)((value_24bit >> 8)  & 0xFFu);
    out[2] = (uint8)( value_24bit        & 0xFFu);
}

/**
 * Converts u32 value into byte array (Big-endian)
 */
static inline void be_put_u32(uint8* out, uint32 value_32bit)
{
    out[0] = (uint8)((value_32bit >> 24) & 0xFFu);
    out[1] = (uint8)((value_32bit >> 16) & 0xFFu);
    out[2] = (uint8)((value_32bit >> 8)  & 0xFFu);
    out[3] = (uint8)( value_32bit        & 0xFFu);
}

/**
 * Converts byte array into uint16 value (Big-endian)
 */
static inline uint16 be_get_u16(const uint8* p)
{
    return (uint16)(((uint16)p[0] << 8) | p[1]);
}

/**
 * Converts byte array into uint24 value (Big-endian)
 */
static inline uint32 be_get_u24(const uint8 *p)
{
    return ((uint32)p[0] << 16) | ((uint32)p[1] << 8) | (uint32)p[2];
}

/**
 * Converts byte array into uint32 value (Big-endian)
 */
static inline uint32 be_get_u32(const uint8* p)
{
    return ((uint32)p[0] << 24) | ((uint32)p[1] << 16) | ((uint32)p[2] << 8)  | ((uint32)p[3]);
}

// Include guard stops here
#endif // SI_ENDIAN_H_