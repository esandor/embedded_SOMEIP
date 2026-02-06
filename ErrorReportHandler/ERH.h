// Include guard starts here
#ifndef ERH_H_
#define ERH_H_

/**
 * @file    ERH.h
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   "Provides global error reporting opportunity for SOME/IP Protocol and SOME/IP Service Discovery Protocol."
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include "SI_types.h"

/* **************************************************** */
/*                       Defines                        */
/* **************************************************** */

#define ERH_SI_PARSER_ERROR                  (1u)
#define ERH_SI_PROCESS_ERROR                 (2u)
#define ERH_SI_DISPATCHER_ERROR              (3u)
#define ERH_SI_MESSAGE_ERROR                 (4u)
#define ERH_SD_PORT_FALLBACK                 (5u)
#define ERH_INVALID_RX_CALLBACK_FUNC         (6u)
#define ERH_UNREACHABLE_CODE                 (7u)
#define ERH_OPT_TYPE_INVALID                 (8u)
#define ERH_APP_UDP_RX_ERROR				 (9u)

#define ERH_NUMBER_OF_ERRORS                 (10u)
#define ERH_SIZE_OF_ERH_BUFFER               (255u)

/* **************************************************** */
/*                  Type definitions                    */
/* **************************************************** */

struct ERH_Error
{
    uint8 type;
    boolean valid;
    uint32 field0;
    uint32 field1;
    uint32 field2;
    uint32 field3;
    uint32 field4;
    uint32 field5;
};

/* **************************************************** */
/*               Function declarations                  */
/* **************************************************** */

void ERH_report_error(uint8 type, uint32 field0, uint32 field1, uint32 field2, uint32 field3, uint32 field4, uint32 field5);

// Include guard stops here
#endif // ERH_H_
