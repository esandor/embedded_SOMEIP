/**
 * @file    SI_SD_process.c
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   ""
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include "SI_SD_process.h"

#include "lwip/pbuf.h"
#include "lwip/ip_addr.h"
#include "lwip/udp.h"
#include "SI_types.h"
#include "SI_SD_message.h"
#include "SI_SD_service_manager.h"
#include "SI_SD_parser.h"

/* **************************************************** */
/*                       Defines                        */
/* **************************************************** */

/* **************************************************** */
/*               Static global variables                */
/* **************************************************** */

/* **************************************************** */
/*                True global variables                 */
/* **************************************************** */

/* **************************************************** */
/*                Local type definitions                */
/* **************************************************** */

/* **************************************************** */
/*             Local function declarations              */
/* **************************************************** */

/* **************************************************** */
/*             Global function definitions              */
/* **************************************************** */

boolean SI_SD_PROCESS_multicast(struct udp_pcb *rx_udp_pcb, struct pbuf *rx_pbuf, const ip_addr_t *src_addr, u16_t src_port)
{
    struct SD_MessageContext sd_request;

    // ---- 0)
    if ((NULLPTR == rx_udp_pcb) || (NULLPTR == rx_pbuf) || (NULLPTR == src_addr))
    {
        return FALSE;
    }

    // ---- 1) Parse and validate
    if (FALSE == SI_SD_PARSER_parse_datagram((uint8*)rx_pbuf->payload, (uint32)rx_pbuf->tot_len,
                                            &(sd_request.header), &(sd_request.payload)))
    {
        return FALSE;
    }

    if (FALSE == SI_SD_PARSER_parse_payload(&sd_request.payload))
    {
        return FALSE;
    }

    // ---- 2) Update remote service registry
    SI_SD_PROVIDER_received_registry_resolver();

    return TRUE;
}

/* **************************************************** */
/*             Local function definitions               */
/* **************************************************** */

/* END OF SI_SD_PROCESS.C FILE */
