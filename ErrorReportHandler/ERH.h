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

#define ERH_ERROR_NONE                       (0u)
#define ERH_REQ_HDR_SINTACT_VAL              (1u)
#define ERH_REQ_HDR_SEMANT_VAL               (2u)
#define ERH_REQ_HDR_VAL_FAIL_WO_RESP         (3u)
#define ERH_LOC_SER_NOT_FOUND                (4u)
#define ERH_LOC_SER_INTVER_NOT_COMPAT        (5u)
#define ERH_LOC_SER_METID_NOT_COMPAT         (6u)
#define ERH_FINALIZE_FAIL                    (7u)
#define ERH_UDP_TX_FAIL                      (8u)
#define ERH_TX_MSG_INVALIDATE_FAIL           (9u)
#define ERH_PROC_RESP_HDR_VAL                (10u)
#define ERH_PROC_RESP_FINALIZE_FAIL          (11u)
#define ERH_PROC_RESP_UDP_TX_FAIL            (12u)
#define ERH_PROC_RESP_TX_MSG_INVALIDATE_FAIL (13u)
#define ERH_SD_PORT_FALLBACK                 (14u)
#define ERH_INVALID_RX_CALLBACK_FUNC         (15u)
#define ERH_UNREACHABLE_CODE                 (16u)
#define ERH_UDP_RX_Error                     (17u)
#define ERH_OPT_TYPE_INVALID                 (23u)

#define ERH_NUMBER_OF_ERRORS                 (24u)
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