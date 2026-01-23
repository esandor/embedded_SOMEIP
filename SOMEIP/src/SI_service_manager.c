/**
 * @file    SI_service_manager.c
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   "Implements SI_service_manager.h"
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include "SI_service_manager.h"

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

/* **************************************************** */
/*                True global variables                 */
/* **************************************************** */

static struct SI_local_Service g_services[SI_CFG_MAX_SERVICES];
static uint16 g_service_count = 0u;
static struct SI_MethodEntry g_methods[SI_CFG_MAX_METHODS];
static uint16 g_method_count = 0u;

/* **************************************************** */
/*                Local type definitions                */
/* **************************************************** */

/* **************************************************** */
/*             Local function declarations              */
/* **************************************************** */

/* **************************************************** */
/*             Global function definitions              */
/* **************************************************** */

boolean SI_SERVMAN_add_service(const struct SI_local_Service* service)
{
    uint16 i = 0u;

    if ((NULLPTR == service) || (SI_CFG_MAX_SERVICES <= g_service_count))
    {
        return FALSE;
    }

    for (i = 0u; i < g_service_count; i++)
    {
        if (g_services[i].service_id == service->service_id)
        {
            return FALSE;
        }
    }

    g_services[g_service_count] = *service;
    g_service_count += 1u;
    return TRUE;
}

boolean SI_SERVMAN_add_method(struct SI_local_Service* service, const struct SI_MethodEntry* method)
{
    uint16 i = 0u;
    struct SI_local_Service* service_to_add_method = NULLPTR;

    if ((NULLPTR == service) || (NULLPTR == method) || (SI_CFG_MAX_METHODS <= g_method_count))
    {
        return FALSE;
    }

    service_to_add_method = SI_SERVMAN_find_service(service->service_id);
    if (NULLPTR == service_to_add_method)
    {
        return FALSE;
    }

    if (NULLPTR != service_to_add_method->method)
    {
        return FALSE;
    }

    for (i = 0u; i < g_method_count; i++)
    {
        if (g_methods[i].method_id == method->method_id)
        {
            return FALSE;
        }
    }

    g_methods[g_method_count] = *method;
    service_to_add_method->method = &(g_methods[g_method_count]);
    g_method_count += 1u;

    return TRUE;
}

struct SI_local_Service* SI_SERVMAN_find_service(uint16 service_id)
{
    uint16 i = 0u;

    for (i = 0u; i < g_service_count; i++)
    {
        if (g_services[i].service_id == service_id)
        {
            return &g_services[i];
        }
    }
    return NULLPTR;
}

/* **************************************************** */
/*             Local function definitions               */
/* **************************************************** */

/* END OF SI_PROVIDER.C FILE */
