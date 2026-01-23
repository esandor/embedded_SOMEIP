/**
 * @file    SI_process.c
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   "Implements SI_process.h"
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include "SI_process.h"

#include "lwip/pbuf.h"
#include "lwip/ip_addr.h"
#include "lwip/udp.h"
#include "SomeIP_udp.h"
#include "SI_types.h"
#include "SI_dispatcher.h"
#include "SI_parser.h"
#include "SI_header.h"
#include "SI_service_manager.h"
#include "SI_message.h"
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

/* **************************************************** */
/*                Local type definitions                */
/* **************************************************** */

/* **************************************************** */
/*             Local function declarations              */
/* **************************************************** */

/* **************************************************** */
/*             Global function definitions              */
/* **************************************************** */

boolean SI_PROCESS_unicast(struct udp_pcb *rx_udp_pcb, struct pbuf *rx_pbuf, const ip_addr_t *src_addr, u16_t src_port)
{
    struct SI_Header response_header;
    struct SI_MessageContext request;
    boolean need_call_handler = FALSE;
    boolean need_send_response = FALSE;
    boolean response_header_prepared = FALSE;
    boolean response_send_possible = FALSE;
    struct SI_local_Service* requested_service = NULLPTR; 
    struct SI_MessageBuilder builder;
    enum SI_ReturnCode_t handler_return_code = SI_ReturnCode_OK;

    if ((NULLPTR == rx_udp_pcb) || (NULLPTR == rx_pbuf) || (NULLPTR == src_addr))
    {
        return FALSE;
    }

    if (FALSE == SI_PARSER_parse_datagram((const uint8*)rx_pbuf->payload, (uint32)rx_pbuf->tot_len,
                                          &request.header, &request.payload))
    {
        return FALSE;
    }

    request.cookie = NULLPTR;   // cookie not used now
    response_send_possible = SI_MESSAGE_init(&builder);

    if(FALSE == SI_DISPATCHER_dispatch(&request, &response_header,
                                       &need_call_handler, &need_send_response, &response_header_prepared,
                                       &requested_service))
    {
        if ((TRUE == response_header_prepared) && (TRUE == need_send_response))
        {
            if (FALSE == SI_MESSAGE_finalize(&builder, &response_header, NULLPTR))
            {
                ERH_report_error(ERH_FINALIZE_FAIL, builder.cursor, builder.length, response_header.length, 0u, 0u, 0u);
                return FALSE;
            }

            if (ERR_OK != SomeIP_udp_transmit(rx_udp_pcb, (uint32)src_addr->addr, (uint16)src_port, &builder))
            {
                ERH_report_error(ERH_UDP_TX_FAIL, (uint32)src_addr->addr, (uint32)src_port, 0u, 0u, 0u, 0u);
                return FALSE;
            }

            if (FALSE == SI_MESSAGE_invalidate(&builder))
            {
                ERH_report_error(ERH_TX_MSG_INVALIDATE_FAIL, (uint32)(&builder), (uint32)(builder.data), 0u, 0u, 0u, 0u);
                return FALSE;
            }
        }
        return FALSE;
    }

    if (TRUE == need_call_handler)
    {
        handler_return_code = requested_service->method->handler_func(&request, &builder, requested_service->user_ctx);

        if ((TRUE == need_send_response) && (TRUE == response_send_possible))
        {
            (void)SI_HEADER_set_messageID(&response_header, request.header.message_id);
            (void)SI_HEADER_set_protVer(&response_header, request.header.protocol_version);
            (void)SI_HEADER_set_intVer(&response_header, request.header.interface_version);
            (void)SI_HEADER_set_requestID(&response_header, request.header.request_id);
            (void)SI_HEADER_set_messageType(&response_header, SI_MessageType_RESPONSE);
            (void)SI_HEADER_set_retCode(&response_header, handler_return_code);

            if (FALSE == SI_HEADER_validate(&response_header))
            {
                ERH_report_error(ERH_PROC_RESP_HDR_VAL, ((response_header.message_id.serviceID << 16u) | (response_header.message_id.methodID_or_eventID)),
                                                                ((response_header.request_id.clientID << 16u) | (response_header.request_id.sessionID)),
                                                                ((response_header.message_type << 24u) | (response_header.return_code << 16u) | (response_header.protocol_version << 8u) | (response_header.interface_version)),
                                                                (response_header.length), 0u, 0u);
                return FALSE;
            }

            if (FALSE == SI_MESSAGE_finalize(&builder, &response_header, NULLPTR))
            {
                ERH_report_error(ERH_PROC_RESP_FINALIZE_FAIL, builder.cursor, builder.length, response_header.length, 0u, 0u, 0u);
                return FALSE;
            }

            if (ERR_OK != SomeIP_udp_transmit(rx_udp_pcb, (uint32)src_addr->addr, (uint16)src_port, &builder))
            {
                ERH_report_error(ERH_PROC_RESP_UDP_TX_FAIL, (uint32)src_addr->addr, (uint32)src_port, 0u, 0u, 0u, 0u);
                return FALSE;
            }

            if (FALSE == SI_MESSAGE_invalidate(&builder))
            {
                ERH_report_error(ERH_PROC_RESP_TX_MSG_INVALIDATE_FAIL, (uint32)(&builder), (uint32)(builder.data), 0u, 0u, 0u, 0u);
                return FALSE;
            }
        }
    }
    return TRUE;
}

/* **************************************************** */
/*             Local function definitions               */
/* **************************************************** */

/* END OF SI_PROCESS.C FILE */
