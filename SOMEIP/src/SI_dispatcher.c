/**
 * @file    SI_dispatcher.c
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   "Implements SI_dispatcher.h"
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include "SI_dispatcher.h"

#include "SI_types.h"
#include "SI_service_manager.h"
#include "SI_header.h"
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

static boolean SI_DISPATCHER_header_sintactic_validation( const struct SI_MessageContext* request,
                                                        struct SI_Header* response_header,
                                                        boolean* need_call_handler,
                                                        boolean* need_send_response,
                                                        boolean* needs_resp);
boolean SI_DISPATCHER_request_header_semantic_validation(const struct SI_MessageContext* request,
                                                         struct SI_Header* response_header,
                                                         boolean* need_call_handler,
                                                         boolean* need_send_response,
                                                         boolean* response_header_prepared);
boolean SI_DISPATCHER_get_service(  const struct SI_MessageContext* request,
                                    struct SI_Header* response_header,
                                    boolean* need_call_handler,
                                    boolean* need_send_response,
                                    boolean* response_header_prepared,
                                    struct SI_local_Service** service);
static boolean SI_DISPATCHER_construct_header(  const struct SI_MessageContext* req,
                                                struct SI_Header* resp_header,
                                                enum SI_MessageType_t type,
                                                enum SI_ReturnCode_t code);
static boolean SI_DISPATCHER_expect_response(enum SI_MessageType_t message_type, boolean* needs_resp);
static void SI_DISPATCHER_report_error();

/* **************************************************** */
/*             Global function definitions              */
/* **************************************************** */

/**
 * @param in_request: request message to evaluate
 * @param inout_response_header: in case of error, this will contain information to send
 * @param out_need_call_handler: TRUE if request message requires to invoke the service handler function
 * @param out_need_send_response: TRUE if request message requires a response
 * @param out_response_header_prepared: TRUE if execution failed, response_header contains error info to send
 * @param out_service: reference for the appropriate service, if out_need_call_handler==TRUE then method of this service need to be invoked
 * 
 * @returns TRUE if function execution was done on happy-path only
 */
boolean SI_DISPATCHER_dispatch(const struct SI_MessageContext* in_request,
                               struct SI_Header* inout_response_header,
                               boolean* out_need_call_handler,
                               boolean* out_need_send_response,
                               boolean* out_response_header_prepared,
                               struct SI_local_Service** out_service)
{
    // ---- 0)
    if ((NULLPTR == in_request) || (NULLPTR == inout_response_header) ||
        (NULLPTR == out_need_call_handler) || (NULLPTR == out_need_send_response))
    {
        return FALSE;
    }

    *out_need_call_handler  = FALSE;
    *out_need_send_response = FALSE;
    *out_response_header_prepared = FALSE;
    *out_service = NULLPTR;

    // ---- 1) Sintactic validation
    if (FALSE == SI_DISPATCHER_header_sintactic_validation( in_request,
                                                            inout_response_header,
                                                            out_need_call_handler,
                                                            out_need_send_response,
                                                            out_response_header_prepared))
    {
        SI_DISPATCHER_report_error(SI_DISPATCHER_ErrType_REQ_HDR_SINTACT_VAL, in_request);
        return FALSE;
    }

    // ---- 2) Semantic validation
    if (FALSE == SI_DISPATCHER_request_header_semantic_validation(  in_request,
                                                                    inout_response_header,
                                                                    out_need_call_handler,
                                                                    out_need_send_response,
                                                                    out_response_header_prepared))
    {
        SI_DISPATCHER_report_error(SI_DISPATCHER_ErrType_REQ_HDR_SEMANT_VAL,  in_request);
        return FALSE;
    }

    if ((TRUE == *out_response_header_prepared) && (FALSE == *out_need_call_handler))
    {
        SI_DISPATCHER_report_error(SI_DISPATCHER_ErrType_REQ_HDR_VAL_FAIL_WO_RESP, in_request);
        return FALSE;
    }

    // ---- 3) Decision about sending response
    (void)SI_DISPATCHER_expect_response(in_request->header.message_type, out_need_send_response);

    if (FALSE == *out_need_send_response)
    {
        return TRUE;
    }

    // ---- 4) Service lookup
    if(FALSE == SI_DISPATCHER_get_service(  in_request,
                                            inout_response_header,
                                            out_need_call_handler,
                                            out_need_send_response,
                                            out_response_header_prepared,
                                            out_service))
    {
        return FALSE;
    }

    return TRUE;
}

/* **************************************************** */
/*             Local function definitions               */
/* **************************************************** */

/**
 * @returns TRUE if dispatcher operation is possible
 */
static boolean SI_DISPATCHER_header_sintactic_validation(   const struct SI_MessageContext* request,
                                                            struct SI_Header* response_header,
                                                            boolean* need_call_handler,
                                                            boolean* need_send_response,
                                                            boolean* response_header_prepared)
{
    if ((TRUE == SI_HEADER_validate(&(request->header))) || 
        (FALSE == SI_DISPATCHER_expect_response(request->header.message_type, need_send_response)))
    {
        // request header is valid
        return TRUE;
    }

    if (TRUE == *need_send_response)
    {
        (void)SI_DISPATCHER_construct_header(request, response_header, SI_MessageType_ERROR, SI_ReturnCode_MALFORMED_MESSAGE);
        *response_header_prepared = TRUE;
    }

    // dispatcher operation can continue with prepared response header
    return TRUE;
}

/**
 * @returns TRUE if dispatcher operation is possible
 */
boolean SI_DISPATCHER_request_header_semantic_validation(const struct SI_MessageContext* request,
                                                         struct SI_Header* response_header,
                                                         boolean* need_call_handler,
                                                         boolean* need_send_response,
                                                         boolean* response_header_prepared)
{
    const boolean is_method = SI_HEADER_is_method(request->header.message_id.methodID_or_eventID);
    const boolean is_event  = SI_HEADER_is_event (request->header.message_id.methodID_or_eventID);

    switch (request->header.message_type)
    {
    case SI_MessageType_REQUEST:
    case SI_MessageType_REQUEST_NO_RETURN:
    {
        if (TRUE == is_method)
        {
            // semantic check passed
            *need_call_handler = TRUE;
            return TRUE;
        }

        if (FALSE == SI_DISPATCHER_expect_response(request->header.message_type, need_send_response))
        {
            // message type is corrupted, cant do anything
            return FALSE;
        }

        if ((TRUE == *need_send_response) && (FALSE == *response_header_prepared))
        {
            (void)SI_DISPATCHER_construct_header(request, response_header, SI_MessageType_ERROR, SI_ReturnCode_WRONG_MESSAGE_TYPE);
            *response_header_prepared = TRUE;
        }
        return TRUE;
    }
    case SI_MessageType_NOTIFICATION:
    {
        if (TRUE == is_event)
        {
            // semantic check passed
            *need_call_handler = TRUE;
            return TRUE;
        }

        // It is certain that no response message is needed
        // semantic check failed, cant do anything
        return FALSE;
    }
    default:
    {
        // fatal error: inreachable code
        // semantic check failed, cant do anything
        return FALSE;
    }
    }
}

/**
 * @returns TRUE if dispatcher operation is possible
 */
boolean SI_DISPATCHER_get_service(  const struct SI_MessageContext* request,
                                    struct SI_Header* response_header,
                                    boolean* need_call_handler,
                                    boolean* need_send_response,
                                    boolean* response_header_prepared,
                                    struct SI_local_Service** service)
{
    *service = SI_SERVMAN_find_service(request->header.message_id.serviceID);
    
    if (NULLPTR == *service)
    {
        if (FALSE == *need_send_response)
        {
            SI_DISPATCHER_report_error(SI_DISPATCHER_ErrType_LOC_SER_NOT_FOUND, request);
            return FALSE;
        }
        else
        {
            (void)SI_DISPATCHER_construct_header(request, response_header, SI_MessageType_ERROR, SI_ReturnCode_UNKNOWN_SERVICE);
            *response_header_prepared = TRUE;
            return TRUE;
        }
    }

    if (request->header.interface_version != (*service)->interface_version)
    {
        if (FALSE == *need_send_response)
        {
            SI_DISPATCHER_report_error(SI_DISPATCHER_ErrType_LOC_SER_INTVER_NOT_COMPAT, request);
            return FALSE;
        }
        else
        {
            (void)SI_DISPATCHER_construct_header(request, response_header, SI_MessageType_ERROR, SI_ReturnCode_WRONG_INTERFACE_VERSION);
            *response_header_prepared = TRUE;
            return TRUE;
        }
    }

    if ((request->header.message_id.methodID_or_eventID != (*service)->method->method_id) ||
        (NULLPTR == (*service)->method->handler_func))
    {
        if (FALSE == *need_send_response)
        {
            SI_DISPATCHER_report_error(SI_DISPATCHER_ErrType_LOC_SER_METID_NOT_COMPAT, request);
            return FALSE;
        }
        else
        {
            (void)SI_DISPATCHER_construct_header(request, response_header, SI_MessageType_ERROR, SI_ReturnCode_UNKNOWN_METHOD);
            *response_header_prepared = TRUE;
            return TRUE;
        }
    }

    // service found without any problem
    return TRUE;
}

/**
 * Sets response message header based on request message header.
 * Returns TRUE if requested header format is successfully set.
 * @param req: request message, this function assembles the response message header for that message
 * @param resp_header: the response header that this function assembles
 * @param type: Message Type field of resp_header
 * @param code: Return Code field of resp_header
*/
static boolean SI_DISPATCHER_construct_header(const struct SI_MessageContext* req, struct SI_Header* resp_header, enum SI_MessageType_t type, enum SI_ReturnCode_t code)
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

    // If either check fails, the outcome is always an ERROR type response message
    resp_header->message_type = SI_MessageType_ERROR;
    (void)SI_HEADER_set_retCode(resp_header, SI_ReturnCode_NOT_OK);
    return FALSE;

}

static boolean SI_DISPATCHER_expect_response(enum SI_MessageType_t message_type, boolean* needs_resp)
{
    if ((NULLPTR == needs_resp) || (FALSE == SI_HEADER_check_messageType(message_type)))
    {
        // Any not specified Message Type will be ignored
        return FALSE;
    }

    switch (message_type)
    {
        case SI_MessageType_REQUEST:
        {
            *needs_resp = TRUE;
            return TRUE;
        }
        case SI_MessageType_REQUEST_NO_RETURN:
        case SI_MessageType_NOTIFICATION:
        {
            *needs_resp = FALSE;
            return TRUE;
        }
        default:
        {
            // No need to send response to RESPONSE and ERROR messages
            *needs_resp = FALSE;
            return TRUE;
        }
    }
}

static void SI_DISPATCHER_report_error(enum SI_DISPATCHER_ErrType_t type, const struct SI_MessageContext* in_request)
{
    ERH_report_error(type, ((in_request->header.message_id.serviceID << 16u) | (in_request->header.message_id.methodID_or_eventID)),
                            ((in_request->header.request_id.clientID << 16u) | (in_request->header.request_id.sessionID)),
                            ((in_request->header.message_type << 24u) | (in_request->header.return_code << 16u) | (in_request->header.protocol_version << 8u) | (in_request->header.interface_version)),
                            (in_request->header.length), 0u, 0u);
}

/* END OF SI_DISPATCHER.C FILE */
