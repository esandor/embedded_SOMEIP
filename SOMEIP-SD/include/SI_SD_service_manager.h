// Include guard starts here
#ifndef SI_SD_SERVICE_MANAGER_H_
#define SI_SD_SERVICE_MANAGER_H_

/**
 * @file    SI_SD_service_manager.h
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   "Interface for SOME/IP SD service manager.
 *           Handles remote service registry."
 * 
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include "lwip/ip_addr.h"
#include "SI_types.h"

#include "SI_header.h"
#include "SI_message.h"

#include "SI_SD_config.h"
#include "SI_SD_parser.h"
#include "SI_SD_payload.h"

/* **************************************************** */
/*                       Defines                        */
/* **************************************************** */

/* **************************************************** */
/*                  Type definitions                    */
/* **************************************************** */

/**
 * Contains Entry and Option fields from received SOME/IP Service Discovery messages
*/
struct SD_received_Service
{
    struct SI_SD_ServiceEntry entry;
    enum SI_SD_EntryRelatedAction_t entry_action;
    struct SI_SD_IPv4EndpointOption option;
    enum SI_SD_OptionRelatedAction_t option_action;
    boolean used;
};

/**
 * Contains endpoint information: IPv4 address, port number
 * Always uses UDP protocol
 */
struct SD_Endpoint
{
    uint32 ipv4_be; /* network order */
    uint16 port_be; /* network order */
};

/**
 * Defines a service instance
 * Contains IDs, endpoint info, ttl and validity
 */
struct SD_remote_Service
{
    uint16 service_id;
    uint16 instance_id;
    uint8  major;                       // major version number
    uint32 minor;                       // minor version number
    struct SD_Endpoint endpoint;        // IP address, port number, protocol type
    uint32  ttl;                        // [sec]; 0u means "not valid"
    boolean valid;
};

/**
 * Separating SOME/IP Service Discovery (layer 7) from UDP protocol (layer 4).
 * Binding generic Service Discovery send() with UDP specific function.
 * SOME/IP-SD invokes UDP send() function indirectly.
 */
struct SD_TransportHandler_vtable
{
    /**
     * @param dst_ipv4_be: destination IP address for this SD message (network order)
     * @param dst_port_be: destination port number for this SD message (network order)
     * @param data: pointer to the complete UDP payload (SOME/IP header + SD payload)
     * @param len: length of the complete UDP payload (SOME/IP header + SD payload)
     * @param user_ctx: user context (if there is one)
     */
    boolean (*send)(uint32 dst_ipv4_be, uint16 dst_port_be, const uint8 *data, uint32 len, void *user_ctx);
};

/**
 * Service Discovery context
 * Main structure of SOME/IP Service Discovery
 * Contains: local address, session counters, service registry, Tx handler
 */
struct SD_Context
{
    uint32   local_ipv4_be;             // ECU address (network order)
    uint16   sd_port_be;                // SOME/IP-SD port (network order)
    uint32   multicast_ipv4_be;         // configured SD multicast group
    boolean  unicast_supported;

    uint16   multicast_sessionID;       // session counter for multicast (wrap at 0xFFFF → 0x0001)
    uint16   unicast_sessionID;         // session counter for unicast (wrap at 0xFFFF → 0x0001)

    boolean  reboot_flag;               // set TRUE after boot; clear after a session ID cycle

    uint32   ttl_timebase;

    struct SD_TransportHandler_vtable tx_handler;    // function pointer of Transport layer send() function

    void *tx_user_ctx;
};

/* **************************************************** */
/*               Function declarations                  */
/* **************************************************** */

boolean SI_SD_PROVIDER_init(struct SD_Context *sd_context, const uint32 local_ipv4_be,
                           const uint16 sd_port_be, const uint32 sd_multicast_ipv4_be,
                           const struct SD_TransportHandler_vtable *tx_handler, void *tx_user_ctx);
void SI_SD_PROVIDER_received_registry_resolver(void);
struct SD_remote_Service* SI_SD_PROVIDER_lookup_service(uint16 service_id,
                                                              uint16 instance_id,
                                                              uint8 major);
void SI_SD_PROVIDER_tick(struct SD_Context *sd_context, const uint32 elapsed_time_sec);

boolean SI_SD_PROVIDER_allocate_service__soft(struct SD_remote_Service* to_be_saved, struct SD_remote_Service **allocated_space);
struct SD_remote_Service* SI_SD_PROVIDER_allocate_service__force(struct SD_remote_Service* to_be_saved);

#endif /* SI_SD_SERVICE_MANAGER_H_ */
