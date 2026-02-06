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
#include "SI_servman.h"
#include "SI_header.h"

/* **************************************************** */
/*                       Defines                        */
/* **************************************************** */

/* **************************************************** */
/*                  Type definitions                    */
/* **************************************************** */

struct SI_DISPATCHER_status
{
    boolean call_handler;
    boolean send_response;
    boolean error;
    enum SI_MessageType_t error_message_type;
    enum SI_ReturnCode_t error_return_code;
};

/* **************************************************** */
/*               Function declarations                  */
/* **************************************************** */

boolean SI_DISPATCHER_dispatch(const struct SI_MessageContext* in_request, struct SI_DISPATCHER_status* status);

// Include guard stops here
#endif // SI_DISPATCHER_H_
