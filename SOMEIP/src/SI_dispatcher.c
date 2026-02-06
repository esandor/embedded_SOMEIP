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
#include "SI_servman.h"
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

enum SI_DISP_ErrType_t
{
    SI_DISP_ErrType_sintactic_validation = 0u,
    SI_DISP_ErrType_semantic_interpretation = 1u,
    SI_DISP_ErrType_silence_faliure = 2u
};

/* **************************************************** */
/*             Local function declarations              */
/* **************************************************** */

boolean SI_DISPATCHER_sintactic_validation(const struct SI_MessageContext* request, struct SI_DISPATCHER_status* status);
boolean SI_DISPATCHER_semantic_interpretation(const struct SI_MessageContext* request, struct SI_DISPATCHER_status* status);
static void SI_DISPATCHER_report_error(enum SI_DISP_ErrType_t type, const struct SI_MessageContext* request);

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
boolean SI_DISPATCHER_dispatch(const struct SI_MessageContext* in_request, struct SI_DISPATCHER_status* status)
{
    // ---- 0) Input validation
    if ((NULLPTR == in_request) || (NULLPTR == status))
    {
        return FALSE;
    }

    status->call_handler  = FALSE;
    status->send_response = FALSE;
    status->error = FALSE;
    status->error_message_type = SI_MessageType_RESPONSE;
    status->error_return_code = SI_ReturnCode_OK;

    // ---- 1) Request sintactic validation
    if (FALSE == SI_DISPATCHER_sintactic_validation(in_request, status))
    {
        SI_DISPATCHER_report_error(SI_DISP_ErrType_sintactic_validation, in_request);
        return FALSE;
    }

    // ---- 2) request semantic validation and action determination
    if (FALSE == SI_DISPATCHER_semantic_interpretation(in_request, status))
    {
        SI_DISPATCHER_report_error(SI_DISP_ErrType_semantic_interpretation, in_request);
        return FALSE;
    }

    if ((TRUE == status->error) && (FALSE == status->send_response))
    {
        SI_DISPATCHER_report_error(SI_DISP_ErrType_silence_faliure, in_request);
        return FALSE;
    }

    return TRUE;
}

/* **************************************************** */
/*             Local function definitions               */
/* **************************************************** */

boolean SI_DISPATCHER_sintactic_validation(const struct SI_MessageContext* request, struct SI_DISPATCHER_status* status)
{
    if ((NULLPTR == request) || (NULLPTR == status))
    {
        return FALSE;
    }

    if (FALSE == SI_HEADER_validate(&(request->header)))
    {
        return FALSE;
    }

    return TRUE;
}

boolean SI_DISPATCHER_semantic_interpretation(const struct SI_MessageContext* request, struct SI_DISPATCHER_status* status)
{
    const boolean is_method = SI_HEADER_is_method(request->header.message_id.methodID_or_eventID);
    const boolean is_event  = SI_HEADER_is_event (request->header.message_id.methodID_or_eventID);
    boolean retval = TRUE;

    switch (request->header.message_type)
    {
    case SI_MessageType_REQUEST:
    {
        status->send_response = TRUE;

        if (TRUE == is_method)
        {
            status->call_handler = TRUE;
        }
        else
        {
            status->error = TRUE;
            status->error_message_type = SI_MessageType_ERROR;
            status->error_return_code = SI_ReturnCode_WRONG_MESSAGE_TYPE;
        }
        break;
    }
    case SI_MessageType_ERROR:
        /* FALL TROUGH */
    case SI_MessageType_RESPONSE:
        /* FALL TROUGH */
    case SI_MessageType_REQUEST_NO_RETURN:
    {
        if (TRUE == is_method)
        {
            status->call_handler = TRUE;
        }
        else
        {
            status->error = TRUE;
        }
        break;
    }
    case SI_MessageType_NOTIFICATION:
    {
        if (TRUE == is_event)
        {
            status->call_handler = TRUE;
        }
        else
        {
            status->error = TRUE;
        }
        break;
    }
    default:
    {
        retval = FALSE;
        break;
    }
    }
    return retval;
}

static void SI_DISPATCHER_report_error(enum SI_DISP_ErrType_t type, const struct SI_MessageContext* request)
{
    ERH_report_error(ERH_SI_DISPATCHER_ERROR,
                    type,
                    ((request->header.message_id.serviceID << 16u) | (request->header.message_id.methodID_or_eventID)),
                    ((request->header.request_id.clientID << 16u) | (request->header.request_id.sessionID)),
                    ((request->header.message_type << 24u) | (request->header.return_code << 16u) | (request->header.protocol_version << 8u) | (request->header.interface_version)),
                    (request->header.length), 0u);
}

/* END OF SI_DISPATCHER.C FILE */
