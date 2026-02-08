/**
 * @file    SI_servman.c
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   "Implements SI_servman.h"
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include "SI_servman.h"

#include "SI_types.h"
#include "SI_header.h"
#include "SI_message.h"
#include "SI_config.h"

/* **************************************************** */
/*                       Defines                        */
/* **************************************************** */

/* **************************************************** */
/*               Static global variables                */
/* **************************************************** */

static struct SI_Service local_service_registry[SI_CFG_MAX_SERVICES] = {0u};
static uint32 local_service_count = 0u;

/* **************************************************** */
/*                True global variables                 */
/* **************************************************** */

/* **************************************************** */
/*                Local type definitions                */
/* **************************************************** */

/* **************************************************** */
/*             Local function declarations              */
/* **************************************************** */

void SI_SERVMAN_erase_methodlist(struct SI_MethodEntry* method);

/* **************************************************** */
/*             Global function definitions              */
/* **************************************************** */

boolean SI_SERVMAN_add_service(const struct SI_Service* service)
{
    uint16 i = 0u;
    boolean valid_service_element = FALSE;
    boolean service_id_match = FALSE;
    boolean instance_id_match = FALSE;
    boolean interface_version_match = FALSE;

    if ((NULLPTR == service) || (SI_CFG_MAX_SERVICES <= local_service_count))
    {
        return FALSE;
    }

    for (i = 0u; i < SI_CFG_MAX_SERVICES; i++)
    {
        valid_service_element = (FALSE != local_service_registry[i].valid);
        service_id_match = (local_service_registry[i].service_id == service->service_id);
        instance_id_match = (local_service_registry[i].instance.instance_id == service->instance.instance_id);
        interface_version_match = (local_service_registry[i].interface_version == service->interface_version);

        if (valid_service_element && service_id_match && instance_id_match && interface_version_match)
        {
            // service already exists
            return TRUE;
        }
    }

    for (i = 0u; i < SI_CFG_MAX_SERVICES; i++)
    {
        if (FALSE == local_service_registry[i].valid)
        {
            local_service_registry[i] = *service;
            local_service_count += 1u;
            return TRUE;
        }
    }

    return FALSE;    
}

boolean SI_SERVMAN_add_method(struct SI_Service* service, const struct SI_MethodEntry* method)
{
    uint16 i = 0u;
    struct SI_Service* found_service = NULLPTR;
    boolean valid_method_element = FALSE;
    boolean method_id_match = FALSE;

    if ((NULLPTR == service) || (NULLPTR == method))
    {
        return FALSE;
    }

    found_service = SI_SERVMAN_find_service(service->service_id, service->instance.port_be, service->interface_version);
    if (NULLPTR == found_service)
    {
        // given service does not exists, can not add method to it
        return FALSE;
    }

    if (SI_CFG_MAX_METHODS <= found_service->method_counter)
    {
        // service can not handle more methods
        return FALSE;
    }

    for (i = 0u; i < SI_CFG_MAX_METHODS; i++)
    {
        valid_method_element = (FALSE != found_service->method[i].valid);
        method_id_match = (found_service->method[i].method_id == method->method_id);

        if (valid_method_element && method_id_match)
        {
            // service already has a method equivalent with given one
            return TRUE;
        }
    }

    for (i = 0u; i < SI_CFG_MAX_METHODS; i++)
    {
        if (FALSE == found_service->method[i].valid)
        {
            found_service->method[i] = *method;
            found_service->method_counter += 1u;
            return TRUE;
        }
    }

    return FALSE;
}

struct SI_Service* SI_SERVMAN_find_service(uint16 service_id, uint16 src_port, uint16 interface_version)
{
    uint16 i = 0u;
    boolean valid_service_element = FALSE;
    boolean service_id_match = FALSE;
    boolean instance_id_match = FALSE;
    boolean interface_version_match = FALSE;

    for (i = 0u; i < SI_CFG_MAX_SERVICES; i++)
    {
        valid_service_element = (FALSE != local_service_registry[i].valid);
        service_id_match = (local_service_registry[i].service_id == service_id);
        instance_id_match = (local_service_registry[i].instance.port_be == src_port);
        interface_version_match = (local_service_registry[i].interface_version == interface_version);

        if (valid_service_element && service_id_match && instance_id_match && interface_version_match)
        {
            return &(local_service_registry[i]);
        }
    }
    return NULLPTR;
}

struct SI_MethodEntry* SI_SERVMAN_find_method(struct SI_Service* service, uint16 method_id)
{
    uint16 i = 0u;

    if (NULLPTR == service)
    {
        return NULLPTR;
    }

    for (i = 0u; i < SI_CFG_MAX_METHODS; i++)
    {
        if (service->method[i].method_id == method_id)
        {
            return &(service->method[i]);
        }
    }

    return NULLPTR;
}

boolean SI_SERVMAN_rmv_service(const struct SI_Service* service)
{
    uint16 i = 0u;
    boolean valid_service_element = FALSE;
    boolean service_id_match = FALSE;
    boolean instance_id_match = FALSE;
    boolean interface_version_match = FALSE;

    if ((NULLPTR == service) || (0u == local_service_count))
    {
        return FALSE;
    }

    for (i = 0u; i < SI_CFG_MAX_SERVICES; i++)
    {
        valid_service_element = (FALSE != local_service_registry[i].valid);
        service_id_match = (local_service_registry[i].service_id == service->service_id);
        instance_id_match = (local_service_registry[i].instance.instance_id == service->instance.instance_id);
        interface_version_match = (local_service_registry[i].interface_version == service->interface_version);

        if (valid_service_element && service_id_match && instance_id_match && interface_version_match)
        {
            local_service_registry[i].valid = FALSE;
            local_service_registry[i].service_id = 0u;
            local_service_registry[i].interface_version = 0u;
            local_service_registry[i].method_counter = 0u;
            local_service_registry[i].instance.instance_id = 0u;
            local_service_registry[i].instance.port_be = 0u;
            SI_SERVMAN_erase_methodlist(local_service_registry[i].method);

            local_service_count -= 1u;
            return TRUE;
        }
    }
    return FALSE;
}

boolean SI_SERVMAN_rmv_method(struct SI_Service* service, const struct SI_MethodEntry* method)
{
    uint16 i = 0u;
    struct SI_Service* found_service = NULLPTR;
    boolean valid_method_element = FALSE;
    boolean method_id_match = FALSE;

    if ((NULLPTR == service) || (NULLPTR == method))
    {
        return FALSE;
    }

    found_service = SI_SERVMAN_find_service(service->service_id, service->instance.port_be, service->interface_version);
    if (NULLPTR == found_service)
    {
        // given service does not exists, can not remove method from it
        return FALSE;
    }

    if (0u == found_service->method_counter)
    {
        // no method to remove
        return FALSE;
    }

    for (i = 0u; i < SI_CFG_MAX_METHODS; i++)
    {
        valid_method_element = (FALSE != found_service->method[i].valid);
        method_id_match = (found_service->method[i].method_id == method->method_id);

        if (valid_method_element && method_id_match)
        {
            found_service->method[i].valid = FALSE;
            found_service->method[i].handler_func = NULLPTR;
            found_service->method[i].method_id = 0u;

            found_service->method_counter -= 1u;
            return TRUE;
        }
    }
    return FALSE;
}

/* **************************************************** */
/*             Local function definitions               */
/* **************************************************** */

void SI_SERVMAN_erase_methodlist(struct SI_MethodEntry* method)
{
    uint16 i = 0u;

    for (i = 0u; i < SI_CFG_MAX_METHODS; i++)
    {
        method[i].valid = FALSE;
        method[i].handler_func = NULLPTR;
        method[i].method_id = 0u;
    }
}

/* END OF SI_SERVMAN.C FILE */
