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
#include "SI_config.h"
#include "SI_header.h"
#include "SI_message.h"

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
 */ 
typedef enum SI_ReturnCode_t (*SI_MethodHandler_fptr)(const struct SI_MessageContext* request,
                                                      struct SI_MessageBuilder* response);

struct SI_MethodEntry
{
    boolean valid;
    uint16 method_id;
    SI_MethodHandler_fptr handler_func;
};

enum SI_UsedTransmitProtocol_t
{
    SI_UsedTransmitProtocol_UDP,
    SI_UsedTransmitProtocol_TCP
};

/**
 * Service intance informations
 * Provides opportunity to handle separate instances of the same service on a singe ECU
 */
struct SI_ServiceInstance
{
    uint16 instance_id;
    uint16 port_be;                               // SOME/IP port (network order)
    enum SI_UsedTransmitProtocol_t protocol;
};

struct SI_Service
{
    boolean valid;
    uint16 service_id;
    struct SI_ServiceInstance instance;
    uint8 interface_version;
    struct SI_MethodEntry method[SI_CFG_MAX_METHODS];
    uint32 method_counter;
};

/* **************************************************** */
/*               Function declarations                  */
/* **************************************************** */

boolean SI_SERVMAN_add_service(const struct SI_Service* service);
boolean SI_SERVMAN_add_method(struct SI_Service* service, const struct SI_MethodEntry* method);
struct SI_Service* SI_SERVMAN_find_service(uint16 service_id, uint16 src_port, uint16 interface_version);
struct SI_MethodEntry* SI_SERVMAN_find_method(struct SI_Service* service, uint16 method_id);
boolean SI_SERVMAN_rmv_service(const struct SI_Service* service);
boolean SI_SERVMAN_rmv_method(struct SI_Service* service, const struct SI_MethodEntry* method);

// Include guard stops here
#endif // SI_SERVMAN_H_
