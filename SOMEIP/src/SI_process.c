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
#include "SI_servman.h"
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

enum SI_PROC_ErrType_t
{
    SI_PROC_ErrType_error_invalid_dispatcher_retval = 0u,
    SI_PROC_ErrType_error_response_finalize_fail = 1u,
    SI_PROC_ErrType_error_udp_tx_fail = 2u,
    SI_PROC_ErrType_error_response_invalidate_fail = 3u,
    SI_PROC_ErrType_buffering_malfuntion = 4u,
    SI_PROC_ErrType_service_not_needed = 5u,
    SI_PROC_ErrType_local_service_not_found = 6u,
    SI_PROC_ErrType_local_method_not_found = 7u,
    SI_PROC_ErrType_local_service_not_compatible = 8u,
    SI_PROC_ErrType_invalid_handler_retval = 10u,
    SI_PROC_ErrType_response_finalize_fail = 11u,
    SI_PROC_ErrType_udp_tx_fail = 12u,
    SI_PROC_ErrType_response_invalidate_fail = 13u,
};

/* **************************************************** */
/*             Local function declarations              */
/* **************************************************** */

static boolean SI_PROCESS_construct_header(const struct SI_MessageContext* req, struct SI_Header* resp_header, enum SI_MessageType_t type, enum SI_ReturnCode_t code);
static void SI_PROCESS_report_error(enum SI_PROC_ErrType_t type, const void* field0, const void* field1, const void* field2, const void* field3, const void* field4);

/* **************************************************** */
/*             Global function definitions              */
/* **************************************************** */

boolean SI_PROCESS_unicast(struct udp_pcb *rx_udp_pcb, struct pbuf *rx_pbuf, const ip_addr_t *src_addr, u16_t src_port)
{
    struct SI_MessageContext request;
    struct SI_Header response_header;
    struct SI_MessageBuilder response;
    struct SI_DISPATCHER_status dispatcher_status;
    boolean response_possible = FALSE;
    boolean error_condition = FALSE;
    boolean interface_mismatch = FALSE;
    struct SI_Service* requested_service = NULLPTR; 
    struct SI_MethodEntry* requested_method = NULLPTR;
    
    enum SI_ReturnCode_t handler_return_code = SI_ReturnCode_OK;

    // ---- 0) Input validation
    if ((NULLPTR == rx_udp_pcb) || (NULLPTR == rx_pbuf) || (NULLPTR == src_addr))
    {
        return FALSE;
    }

    // ---- 1) Parse
    // Note: uint8 must be a typedef for unsigned char. Otherwise effective type / strict aliasing / alignment problems might arise.
    if (FALSE == SI_PARSER_parse_datagram((const uint8*)rx_pbuf->payload, (uint32)rx_pbuf->len, &request.header, &request.payload))
    {
        return FALSE;
    }

    // ---- 2) Determine response actions based on request header
    if(FALSE == SI_DISPATCHER_dispatch(&request, &dispatcher_status))
    {
        return FALSE;
    }

    // ---- 3) Allocate Tx buffer
    if (TRUE == dispatcher_status.send_response)
    {
        response_possible = SI_MESSAGE_init(&response);
    }

    // ---- 4) Faliure -> send error message
    if ((TRUE == dispatcher_status.error) && (TRUE == response_possible))
    {
        error_condition = TRUE;

        if (FALSE == SI_PROCESS_construct_header(&request, &response_header,
                                                 dispatcher_status.error_message_type,
                                                 dispatcher_status.error_return_code))
        {
            SI_PROCESS_report_error(SI_PROC_ErrType_error_invalid_dispatcher_retval, &dispatcher_status, 0u, 0u, 0u, 0u);
            return FALSE;
        }

        if (FALSE == SI_MESSAGE_finalize(&response, &response_header, NULLPTR))
        {
            SI_PROCESS_report_error(SI_PROC_ErrType_error_response_finalize_fail, &response, &response_header, 0u, 0u, 0u);
            return FALSE;
        }

        if (ERR_OK != SomeIP_udp_transmit(rx_udp_pcb, (uint32)src_addr->addr, (uint16)src_port, &response))
        {
            SI_PROCESS_report_error(SI_PROC_ErrType_error_udp_tx_fail, src_addr, &src_port, 0u, 0u, 0u);
            return FALSE;
        }

        if (FALSE == SI_MESSAGE_invalidate(&response))
        {
            SI_PROCESS_report_error(SI_PROC_ErrType_error_response_invalidate_fail, &response, 0u, 0u, 0u, 0u);
            return FALSE;
        }
        return TRUE;
    }
    else if (TRUE == dispatcher_status.error)
    {
        // FATAL ERROR: Error response can not be sent due to buffering malfunction
        SI_PROCESS_report_error(SI_PROC_ErrType_buffering_malfuntion, &request, 0u, 0u, 0u, 0u);
        return FALSE;
    }
    else
    {
        if (FALSE == dispatcher_status.call_handler)
        {
            // FATAL ERROR: Valid request message requires service handler call
            SI_PROCESS_report_error(SI_PROC_ErrType_service_not_needed, &request, 0u, 0u, 0u, 0u);
            return FALSE;
        }

        // ---- 5) Get requested service
        requested_service = SI_SERVMAN_find_service(request.header.message_id.serviceID, rx_udp_pcb->local_port, request.header.interface_version);
        if (NULLPTR == requested_service)
        {
            error_condition = TRUE;
            (void)SI_PROCESS_construct_header(&request, &response_header, SI_MessageType_ERROR, SI_ReturnCode_UNKNOWN_SERVICE);

            SI_PROCESS_report_error(SI_PROC_ErrType_local_service_not_found, &request, 0u, 0u, 0u, 0u);
        }

        interface_mismatch = (request.header.interface_version != requested_service->interface_version);
        if (interface_mismatch && (FALSE == error_condition))
        {
            error_condition = TRUE;
            (void)SI_PROCESS_construct_header(&request, &response_header, SI_MessageType_ERROR, SI_ReturnCode_WRONG_INTERFACE_VERSION);

            SI_PROCESS_report_error(SI_PROC_ErrType_local_service_not_compatible, &request, 0u, 0u, 0u, 0u);
        }

        // ---- 6) Get requested method
        requested_method = SI_SERVMAN_find_method(requested_service, request.header.message_id.methodID_or_eventID);
        if ((NULLPTR == requested_method) && (FALSE == error_condition))
        {
            error_condition = TRUE;
            (void)SI_PROCESS_construct_header(&request, &response_header, SI_MessageType_ERROR, SI_ReturnCode_UNKNOWN_METHOD);

            SI_PROCESS_report_error(SI_PROC_ErrType_local_service_not_found, &request, 0u, 0u, 0u, 0u);   
        }

        // ---- 7) Call service handler
        if (FALSE == error_condition)
        {
            handler_return_code = requested_method->handler_func(&request, &response);
        }

        // ---- 8) Success -> send response message
        if ((TRUE == dispatcher_status.send_response) && (TRUE == response_possible))
        {
            if (FALSE == error_condition)
            {
                if (FALSE == SI_PROCESS_construct_header(&request, &response_header, SI_MessageType_RESPONSE, handler_return_code))
                {
                    SI_PROCESS_report_error(SI_PROC_ErrType_invalid_handler_retval, &handler_return_code, 0u, 0u, 0u, 0u);
                    return FALSE;
                }
            }

            if (FALSE == SI_MESSAGE_finalize(&response, &response_header, NULLPTR))
            {
                SI_PROCESS_report_error(SI_PROC_ErrType_response_finalize_fail, &response, &response_header, 0u, 0u, 0u);
                return FALSE;
            }

            if (ERR_OK != SomeIP_udp_transmit(rx_udp_pcb, (uint32)src_addr->addr, (uint16)src_port, &response))
            {
                SI_PROCESS_report_error(SI_PROC_ErrType_udp_tx_fail, src_addr, &src_port, 0u, 0u, 0u);
                return FALSE;
            }

            if (FALSE == SI_MESSAGE_invalidate(&response))
            {
                SI_PROCESS_report_error(SI_PROC_ErrType_response_invalidate_fail, &response, 0u, 0u, 0u, 0u);
                return FALSE;
            }
        }

        return TRUE;
    }
}

/* **************************************************** */
/*             Local function definitions               */
/* **************************************************** */

/**
 * Sets response message header based on request message header.
 * Returns TRUE if requested header format is successfully set.
 * @param req: request message, this function assembles the response message header for that message
 * @param resp_header: the response header that this function assembles
 * @param type: Message Type field of resp_header
 * @param code: Return Code field of resp_header
*/
static boolean SI_PROCESS_construct_header(const struct SI_MessageContext* req, struct SI_Header* resp_header, enum SI_MessageType_t type, enum SI_ReturnCode_t code)
{
    *resp_header = req->header;

    if (TRUE == SI_HEADER_check_messageType(type))
    {
        resp_header->message_type = type;
        if (TRUE == SI_HEADER_set_retCode(resp_header, code))
        {
            return TRUE;
        }
    }

    resp_header->message_type = SI_MessageType_ERROR;
    (void)SI_HEADER_set_retCode(resp_header, SI_ReturnCode_NOT_OK);
    return FALSE;
}

static void SI_PROCESS_report_error(enum SI_PROC_ErrType_t type, const void* field0, const void* field1, const void* field2, const void* field3, const void* field4)
{
    switch (type)
    {
        case SI_PROC_ErrType_error_invalid_dispatcher_retval:
        {
            struct SI_DISPATCHER_status* dispatcher_status = (struct SI_DISPATCHER_status*)field0;
            ERH_report_error(ERH_SI_PROCESS_ERROR, type, dispatcher_status->error_message_type, dispatcher_status->error_return_code, 0u, 0u, 0u);
            break;
        }
        case SI_PROC_ErrType_response_finalize_fail:
            /* FALL THROUGH */
        case SI_PROC_ErrType_error_response_finalize_fail:
        {
            struct SI_MessageBuilder* response = (struct SI_MessageBuilder*)field0;
            struct SI_Header* response_header = (struct SI_Header*)field1;
            ERH_report_error(ERH_SI_PROCESS_ERROR, type, response->cursor, response->length, response_header->length, 0u, 0u);
            break;
        }
        case SI_PROC_ErrType_error_udp_tx_fail:
            /* FALL THROUGH */
        case SI_PROC_ErrType_udp_tx_fail:
        {
            const ip_addr_t* src_addr = (const ip_addr_t*)field0;
            uint16* src_port = (uint16 *)field1;
            ERH_report_error(ERH_SI_PROCESS_ERROR, type, (uint32)src_addr->addr, *src_port, 0u, 0u, 0u);
            break;
        }
        case SI_PROC_ErrType_response_invalidate_fail:
            /* FALL THROUGH */
        case SI_PROC_ErrType_error_response_invalidate_fail:
        {
            struct SI_MessageBuilder* response = (struct SI_MessageBuilder*)field0;
            ERH_report_error(ERH_SI_PROCESS_ERROR, type, (uint32)(response->data[0u]), (uint32)(response->data[8u]), 0u, 0u, 0u);
            break;
        }
        case SI_PROC_ErrType_local_method_not_found:
            /* FALL THROUGH */
        case SI_PROC_ErrType_local_service_not_compatible:
            /* FALL THROUGH */
        case SI_PROC_ErrType_local_service_not_found:
            /* FALL THROUGH */
        case SI_PROC_ErrType_service_not_needed:
            /* FALL THROUGH */
        case SI_PROC_ErrType_buffering_malfuntion:
        {
            struct SI_MessageContext* request = (struct SI_MessageContext*)field0;
            ERH_report_error(ERH_SI_PROCESS_ERROR, type, request->header.message_id.serviceID, request->header.message_id.methodID_or_eventID, request->header.request_id.clientID, request->header.request_id.sessionID, 0u);
            break;
        }
        case SI_PROC_ErrType_invalid_handler_retval:
        {
            enum SI_ReturnCode_t* handler_return_code = (enum SI_ReturnCode_t *)field0;
            ERH_report_error(ERH_SI_PROCESS_ERROR, type, *handler_return_code, 0u, 0u, 0u, 0u);
            break;
        }
        default:
        {
            ERH_report_error(ERH_SI_PROCESS_ERROR, type, (uint32)field0, (uint32)field1, (uint32)field2, (uint32)field3, (uint32)field4);
            break;
        }
    }
}

/* END OF SI_PROCESS.C FILE */
