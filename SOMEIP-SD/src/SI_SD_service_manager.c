/**
 * @file    SI_SD_service_manager.c
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   "Implements SI_SD_service_manager.h"
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include "SI_SD_service_manager.h"

#include "lwip/pbuf.h"

#include "SI_dispatcher.h"
#include "SI_header.h"

#include "SI_SD_payload.h"
#include "ERH.h"

/* **************************************************** */
/*                       Defines                        */
/* **************************************************** */

/* **************************************************** */
/*               Static global variables                */
/* **************************************************** */

/* **************************************************** */
/*                True global variables                 */
/* **************************************************** */

struct SD_received_Service received_service_registry[SI_SD_SERVICE_ENTRIES_MAX_NUMBER];
struct SD_remote_Service remote_service_registry[SI_SD_CFG_MAX_REMOTE_SERVICES];

/* **************************************************** */
/*                Local type definitions                */
/* **************************************************** */

/* **************************************************** */
/*             Local function declarations              */
/* **************************************************** */

static inline boolean SI_SD_PROVIDER_service_expired(struct SD_Context *sd_context, struct SD_remote_Service *service);
static inline boolean SI_SD_PROVIDER_service_match(struct SD_remote_Service *service,
                                               uint16 service_id, uint16 instance_id, uint8 major);
static inline boolean SI_SD_PROVIDER_set_tx_handler(struct SD_Context *sd, const struct SD_TransportHandler_vtable *tx_handler);
static inline uint16 SI_SD_PROVIDER_set_port(struct SD_Context *sd_context, const uint16 sd_port_be);
static struct SD_remote_Service* SI_SD_PROVIDER_alloc_free_service(uint16 service_id, uint16 instance_id, uint8 major);
static struct SD_remote_Service* SI_SD_PROVIDER_alloc_used_service(uint16 service_id, uint16 instance_id, uint8 major);

/* **************************************************** */
/*             Global function definitions              */
/* **************************************************** */

/**
 * Assigns initial values to SD_Context main struct.
 * Should be called after system initialization.
 * 
 * @param sd_context: struct SD_Context service_discovery_context, the "brain" of SOME/IP-SD.
 * @param local_ipv4_be: local IPv4 address of ECU (in network byte order)
 * @param sd_port_be: port number used for Service Discovery protocol (in network byte order) 
 * @param sd_multicast_ipv4_be: multicast IPv4 address used for Service Discovery protocol (in network byte order) 
 * @param tx_handler: function pointer for user transmission handler function
 * @param tx_user_ctx: user context (if there is one)
 * 
 * @returns TRUE if initilaization is successfull
 */
boolean SI_SD_PROVIDER_init(struct SD_Context *sd_context, const uint32 local_ipv4_be,
                           const uint16 sd_port_be, const uint32 sd_multicast_ipv4_be,
                           const struct SD_TransportHandler_vtable *tx_handler, void *tx_user_ctx)
{
    uint16 i = 0u;

    if ((NULLPTR == sd_context) || (NULLPTR == tx_handler) || (local_ipv4_be == sd_multicast_ipv4_be))
    {
        return FALSE;
    }

    /* Clear service registry */
    for (i = 0; i < (uint16)SI_SD_CFG_MAX_REMOTE_SERVICES; ++i)
    {
        remote_service_registry[i].valid = FALSE;
        remote_service_registry[i].ttl = 0u;
    }

    if (SI_SD_PROVIDER_set_port(sd_context, sd_port_be) != sd_port_be)
    {
        ERH_report_error(ERH_SD_PORT_FALLBACK, 0u, 0u, 0u, 0u, 0u, 0u);
    }

    if (FALSE == SI_SD_PROVIDER_set_tx_handler(sd_context, tx_handler))
    {
        // ERROR! Given tx_handler function pointer is invalid. Calling handler function will result in UsageFault -> HardFault.
        ERH_report_error(ERH_INVALID_RX_CALLBACK_FUNC, 0u, 0u, 0u, 0u, 0u, 0u);
        return FALSE;
    }

    sd_context->local_ipv4_be = local_ipv4_be;
    sd_context->multicast_ipv4_be = sd_multicast_ipv4_be;   // must be set by user
    sd_context->unicast_supported = TRUE;
    sd_context->multicast_sessionID = 0u;
    sd_context->unicast_sessionID = 0u;
    sd_context->reboot_flag = TRUE;                     // advertise reboot until cleared
    sd_context->ttl_timebase = 0u;
    sd_context->tx_user_ctx = tx_user_ctx;
    sd_context->unicast_supported = TRUE;
    sd_context->reboot_flag = TRUE;

    return TRUE;
}

void SI_SD_PROVIDER_received_registry_resolver(void)
{
    uint16 i = 0u;
    struct SD_remote_Service *remote_service = NULLPTR;
    struct SD_remote_Service remote_service_to_be_saved;

    /* If service is already in registry, update */
    for (i = 0u; i < (uint16)SI_SD_SERVICE_ENTRIES_MAX_NUMBER; i++)
    {
        if ((TRUE == received_service_registry[i].used) && (SD_EntryTypes_Offer == received_service_registry[i].entry.type))
        {
            remote_service = SI_SD_PROVIDER_lookup_service(received_service_registry[i].entry.serviceID,
                                                            received_service_registry[i].entry.instanceID,
                                                            received_service_registry[i].entry.major_version);
            if (NULLPTR != remote_service)
            {
                remote_service->ttl = received_service_registry[i].entry.ttl;
                remote_service->valid = (0u == received_service_registry[i].entry.ttl) ? (FALSE) : (TRUE);

                remote_service->endpoint.ipv4_be = received_service_registry[i].option.IPv4_address;
                remote_service->endpoint.port_be = received_service_registry[i].option.port_number;

                received_service_registry[i].used = FALSE;
            }
        }
    }
    remote_service = NULLPTR;

    /* If service is unkown, save it */
    for (i = 0u; i < (uint16)SI_SD_SERVICE_ENTRIES_MAX_NUMBER; i++)
    {
        if ((TRUE == received_service_registry[i].used) && (SD_EntryTypes_Offer == received_service_registry[i].entry.type))
        {
            remote_service_to_be_saved.instance_id = received_service_registry[i].entry.instanceID;
            remote_service_to_be_saved.service_id = received_service_registry[i].entry.serviceID;
            remote_service_to_be_saved.major = received_service_registry[i].entry.major_version;
            remote_service_to_be_saved.minor = received_service_registry[i].entry.minor_version;
            remote_service_to_be_saved.ttl = received_service_registry[i].entry.ttl;
            remote_service_to_be_saved.valid = (0u == received_service_registry[i].entry.ttl) ? (FALSE) : (TRUE);
            remote_service_to_be_saved.endpoint.ipv4_be = received_service_registry[i].option.IPv4_address;
            remote_service_to_be_saved.endpoint.port_be = received_service_registry[i].option.port_number;

            if (FALSE == SI_SD_PROVIDER_allocate_service__soft(&remote_service_to_be_saved, &remote_service))
            {
                remote_service = SI_SD_PROVIDER_allocate_service__force(&remote_service_to_be_saved);
            }

            if (NULLPTR != remote_service)
            {
                *remote_service = remote_service_to_be_saved;

                received_service_registry[i].used = FALSE;
            }
            else
            {
                ERH_report_error(ERH_UNREACHABLE_CODE, 0u, 0u, 0u, 0u, 0u, 0u);
            }
        }
    }
}

/**
 * SOME/IP-SD handler
 * 
 * Searches a service with given parameters in remote service registry. Uses linear search.
 * @param sd_context: struct SD_Context service_discovery_context, the "brain" of SOME/IP-SD.
 * @param service_id: Service ID of requested service instance
 * @param instance_id: Instance ID of requested service instance
 * @param major: Major version number of requested service instance
 * 
 * @returns pointer for remote service instance, NULLPTR if service was not found
 */
struct SD_remote_Service* SI_SD_PROVIDER_lookup_service(uint16 service_id,
                                                              uint16 instance_id,
                                                              uint8 major)
{
    uint16 i = 0u;

    for (i = 0; i < SI_SD_CFG_MAX_REMOTE_SERVICES; ++i)
    {
        if (TRUE == SI_SD_PROVIDER_service_match(&(remote_service_registry[i]), service_id, instance_id, major))
        {
            return &remote_service_registry[i];
        }
    }

    return NULLPTR;
}

/**
 * Timekeeping: increment ttl_timebase.
 * @note Call this in every cycle!
 */
void SI_SD_PROVIDER_tick(struct SD_Context *sd_context, const uint32 elapsed_time_sec)
{
    uint16 i = 0u;

    sd_context->ttl_timebase += elapsed_time_sec;

    for (i = 0u; i < (uint16)SI_SD_CFG_MAX_REMOTE_SERVICES; i++)
    {
    	if (TRUE == remote_service_registry[i].valid)
    	{
    		remote_service_registry[i].ttl -= elapsed_time_sec;
    	}
    }

    for (i = 0u; i < (uint16)SI_SD_CFG_MAX_REMOTE_SERVICES; i++)
    {
        if (TRUE == SI_SD_PROVIDER_service_expired(sd_context, &(remote_service_registry[i])))
        {
            remote_service_registry[i].valid = FALSE; 
        }
    }
}

boolean SI_SD_PROVIDER_allocate_service__soft(struct SD_remote_Service* to_be_saved, struct SD_remote_Service **allocated_space)
{
    if (NULLPTR == allocated_space)
    {
        return FALSE;
    }

    *allocated_space = NULLPTR;

    *allocated_space = SI_SD_PROVIDER_lookup_service(to_be_saved->service_id, to_be_saved->instance_id, to_be_saved->major);
    if (NULLPTR != *allocated_space)
    {
        return TRUE;
    }

    *allocated_space = SI_SD_PROVIDER_alloc_free_service(to_be_saved->service_id, to_be_saved->instance_id, to_be_saved->major);
    if (NULLPTR != *allocated_space)
    {
        return TRUE;
    }
    return FALSE;
}

struct SD_remote_Service* SI_SD_PROVIDER_allocate_service__force(struct SD_remote_Service* to_be_saved)
{
    struct SD_remote_Service* allocated_space = NULLPTR;
    allocated_space = SI_SD_PROVIDER_alloc_used_service(to_be_saved->service_id, to_be_saved->instance_id, to_be_saved->major);
    return allocated_space;
}

/* **************************************************** */
/*             Local function definitions               */
/* **************************************************** */

static inline boolean SI_SD_PROVIDER_service_expired(struct SD_Context *sd_context, struct SD_remote_Service *service)
{
    return ((TRUE == service->valid) &&
            (0u != service->ttl) &&
            (sd_context->ttl_timebase >= service->ttl));
}

static inline boolean SI_SD_PROVIDER_service_match(struct SD_remote_Service *service,
                                               uint16 service_id, uint16 instance_id, uint8 major)
{
    return ((TRUE == service->valid) &&
            (service_id == service->service_id) &&
            (instance_id == service->instance_id) &&
            (major == service->major));
}

static inline boolean SI_SD_PROVIDER_set_tx_handler(struct SD_Context *sd, const struct SD_TransportHandler_vtable *tx_handler)
{
    if (NULLPTR == tx_handler)
    {
        return FALSE;
    }
    
    sd->tx_handler = *tx_handler;
    return TRUE;
}

/**
 * Sets sd_port_be field of sd_context.
 * 
 * @param sd_context: struct SD_Context service_discovery_context, the "brain" of SOME/IP-SD.
 * @param sd_port_be:
*/
static inline uint16 SI_SD_PROVIDER_set_port(struct SD_Context *sd_context, const uint16 sd_port_be)
{
    if (0u == sd_port_be)
    {
        sd_context->sd_port_be = (uint16)SI_SD_CFG_PORT_NUMBER;
        return SI_SD_CFG_PORT_NUMBER;
    }

    sd_context->sd_port_be = sd_port_be;
    return sd_port_be;
}

static struct SD_remote_Service* SI_SD_PROVIDER_alloc_free_service(uint16 service_id, uint16 instance_id, uint8 major)
{
    uint16 i = 0u;

    for (i = 0; i < (uint16)SI_SD_CFG_MAX_REMOTE_SERVICES; i++)
    {
        if (FALSE == remote_service_registry[i].valid)
        {
            return (&(remote_service_registry[i]));
        }
    }
    return NULLPTR;
}

static struct SD_remote_Service* SI_SD_PROVIDER_alloc_used_service(uint16 service_id, uint16 instance_id, uint8 major)
{
    uint16 i = 0u;
    uint16 evict = 0u;
    uint32 best = remote_service_registry[0].ttl;

    // Evict the one with nearest expiry
    for (i = 1u; i < (uint16)SI_SD_CFG_MAX_REMOTE_SERVICES; i++) 
    {
        if (remote_service_registry[i].ttl < best) 
        {
            best = remote_service_registry[i].ttl;
            evict = i;
        }
    }
    return (&(remote_service_registry[evict]));
}

/* END OF SI_SD_HANDLER.C FILE */
