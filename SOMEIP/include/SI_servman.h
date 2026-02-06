// Include guard starts here
#ifndef SI_SERVMAN_H_
#define SI_SERVMAN_H_

/**
 * @file    SI_servman.h
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   "Interface for SOME/IP service manager.
 *           Handles only local services."
 * 
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include "SI_types.h"
#include "SI_header.h"
#include "SI_message.h"

#include "SI_config.h"

/* **************************************************** */
/*                       Defines                        */
/* **************************************************** */

/* **************************************************** */
/*                  Type definitions                    */
/* **************************************************** */

struct SI_MessageContext
{
    struct SI_Header header;
    struct SI_Payload payload; // ptr+len
};

/**
 * Method handler signature. Returns the response Return Code from the response message header.
 * @param req: the full received message context
 * @param resp_builder: builder for response message
 * @param user_ctx: app specific context
 */ 
typedef enum SI_ReturnCode_t (*SI_MethodHandler_fptr)(const struct SI_MessageContext* request,
                                                      struct SI_MessageBuilder* response,
                                                      void* user_ctx);

struct SI_MethodEntry
{
    uint16 method_id;
    SI_MethodHandler_fptr handler_func;
};

struct SI_local_Service
{
    uint16 service_id;
    uint8 interface_version;
    struct SI_MethodEntry *method;
    void* user_ctx;                 // app specific context
};

/* **************************************************** */
/*               Function declarations                  */
/* **************************************************** */

boolean SI_SERVMAN_add_service(const struct SI_local_Service* service);
boolean SI_SERVMAN_add_method(struct SI_local_Service* service, const struct SI_MethodEntry* method);
struct SI_local_Service* SI_SERVMAN_find_service(uint16 service_id);

// Include guard stops here
#endif // SI_SERVMAN_H_
