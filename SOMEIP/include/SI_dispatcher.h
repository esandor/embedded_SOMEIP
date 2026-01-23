// Include guard starts here
#ifndef SI_DISPATCHER_H_
#define SI_DISPATCHER_H_

/** 
 * @file    SI_dispatcher.h
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   "Provides response strategy for received SOME/IP messages."
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include "SI_types.h"
#include "SI_service_manager.h"
#include "SI_header.h"

/* **************************************************** */
/*                       Defines                        */
/* **************************************************** */

/* **************************************************** */
/*                  Type definitions                    */
/* **************************************************** */

enum
{
    SI_DISPATCHER_ErrType_REQ_HDR_SINTACT_VAL,
    SI_DISPATCHER_ErrType_REQ_HDR_SEMANT_VAL,
    SI_DISPATCHER_ErrType_REQ_HDR_VAL_FAIL_WO_RESP,
    SI_DISPATCHER_ErrType_LOC_SER_NOT_FOUND,
    SI_DISPATCHER_ErrType_LOC_SER_INTVER_NOT_COMPAT,
    SI_DISPATCHER_ErrType_LOC_SER_METID_NOT_COMPAT
} SI_DISPATCHER_ErrType_t;

/* **************************************************** */
/*               Function declarations                  */
/* **************************************************** */

boolean SI_DISPATCHER_dispatch(const struct SI_MessageContext* in_request,
                               struct SI_Header* inout_response_header,
                               boolean* out_need_call_handler,
                               boolean* out_need_send_response,
                               boolean* out_response_header_prepared,
                               struct SI_local_Service** out_service);

// Include guard stops here
#endif // SI_DISPATCHER_H_
