/**
 * @file    SI_SD_parser.c
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   "Implements SI_SD_parer.h"
 * 
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include "SI_SD_parser.h"

#include "SI_types.h"
#include "SI_endian.h"
#include "SI_message.h"

#include "SI_SD_config.h"
#include "SI_SD_header.h"
#include "SI_SD_payload.h"
#include "SI_SD_message.h"
#include "SI_SD_service_manager.h"
#include "SI_SD_wire.h"
#include "ERH.h"

_Static_assert(SI_SD_SERVICE_OPTIONS_MAX_NUMBER==SI_SD_SERVICE_ENTRIES_MAX_NUMBER, "Fatal error: SI_SD_SERVICE_OPTIONS_MAX_NUMBER s not equals to SI_SD_SERVICE_ENTRIES_MAX_NUMBER, received Service Discovery messages might be unprocessable!");

/* **************************************************** */
/*                       Defines                        */
/* **************************************************** */

/* **************************************************** */
/*               Static global variables                */
/* **************************************************** */

/* **************************************************** */
/*                True global variables                 */
/* **************************************************** */

extern struct SD_received_Service received_service_registry[SI_SD_SERVICE_ENTRIES_MAX_NUMBER];

struct SI_SD_ServiceEntry parsed_entries_pool[SI_SD_SERVICE_ENTRIES_MAX_NUMBER] = {0u};
enum SI_SD_EntryRelatedAction_t parsed_entries_action[SI_SD_SERVICE_ENTRIES_MAX_NUMBER] = {SI_SD_EntryRelatedAction_None};
struct SI_SD_IPv4EndpointOption parsed_options_pool[SI_SD_SERVICE_OPTIONS_MAX_NUMBER] = {0u};
enum SI_SD_OptionRelatedAction_t parsed_options_action[SI_SD_SERVICE_OPTIONS_MAX_NUMBER] = {SI_SD_OptionRelatedAction_None};

/* **************************************************** */
/*                Local type definitions                */
/* **************************************************** */

/* **************************************************** */
/*             Local function declarations              */
/* **************************************************** */

static enum SI_SD_EntryRelatedAction_t SI_SD_PARSER_evaluate_entry(struct SI_SD_ServiceEntry *parsed_entry);
static enum SI_SD_OptionRelatedAction_t SI_SD_PARSER_evaluate_option(struct SI_SD_IPv4EndpointOption *parsed_option);
static void SI_SD_PARSER_fill_received_service_registry(void);

/* **************************************************** */
/*             Global function definitions              */
/* **************************************************** */

/**
 * @note Function fills only raw data fields of SD_Payload. For fully parsed SD_Payload call SI_SD_PARSER_parse_payload().
 */
boolean SI_SD_PARSER_parse_datagram(uint8* udp_payload, uint32 udp_payload_length, struct SD_Header* out_header, struct SD_Payload* out_payload)
{
    if ((NULLPTR == udp_payload) || (NULLPTR == out_header) || (NULLPTR == out_payload) ||
        (SI_SD_CONST_HEADER_LENGTH > udp_payload_length))
    {
        return FALSE;
    }

    SI_SD_WIRE_deserialize_header(udp_payload, out_header);

    if (FALSE == SD_Header_validate(out_header))
    {
        return FALSE;
    }

    if ((out_header->length + SI_CONST_HEADER_PREFIX_LENGTH) != udp_payload_length)
    {
        return FALSE;
    }

    out_payload->data   = udp_payload + (SI_CONST_HEADER_LENGTH + SI_SD_CONST_PREAMBLE_FLAGS_SIZE + SI_SD_CONST_PREAMBLE_RESERVED_SIZE);
    out_payload->length = udp_payload_length - (SI_CONST_HEADER_LENGTH + SI_SD_CONST_PREAMBLE_FLAGS_SIZE + SI_SD_CONST_PREAMBLE_RESERVED_SIZE);

    return TRUE;
}

/**
 * @note Information about the SOME/IP Service Discovery payload format,
 * including the Entries and Options arrays are detailed on the 23. page of the specification.
 */
boolean SI_SD_PARSER_parse_payload(struct SD_Payload* in_payload)
{
    uint32 i = 0u;
    uint8* entries_array = NULLPTR;
    uint8* entries = NULLPTR;
    uint32 entries_length = 0u;
    uint32 entries_numof = 0u;
    uint8* options_array = NULLPTR;
    uint8* options = NULLPTR;
    uint32 options_length = 0u;
    uint32 options_numof = 0u;
    enum SI_SD_EntryRelatedAction_t entry_action = SI_SD_EntryRelatedAction_None;
    struct SI_SD_ServiceEntry parsed_entry = {0u};
    enum SI_SD_OptionRelatedAction_t option_action = SI_SD_OptionRelatedAction_None;
    struct SI_SD_IPv4EndpointOption parsed_option = {0u};
    uint8* entries_element = NULLPTR;
    uint8* option_element = NULLPTR;

    if (NULLPTR == in_payload)
    {
        return FALSE;
    }

    entries_array = in_payload->data;
    entries = (entries_array + SI_SD_CONST_ENTRIES_ARRAY_LENGTH_SIZE);
    entries_length = be_get_u32(entries_array);
    entries_numof = (entries_length >> SI_SD_CONST_ENTRY_ARRAY_DIV_RATIO);

    options_array = entries_array + SI_SD_CONST_ENTRIES_ARRAY_LENGTH_SIZE + entries_length;
    options = (options_array + SI_SD_CONST_OPTIONS_ARRAY_LENGTH_SIZE);
    options_length = be_get_u32(options_array);
    options_numof = (options_length / SI_SD_CONST_OPTION_ARRAY_SIZE);

    if (SI_SD_SERVICE_ENTRIES_MAX_NUMBER < entries_numof)
    {
        entries_numof = SI_SD_SERVICE_ENTRIES_MAX_NUMBER;
    }

    if (SI_SD_SERVICE_OPTIONS_MAX_NUMBER < options_numof)
    {
        options_numof = SI_SD_SERVICE_OPTIONS_MAX_NUMBER;
    }

    for (i = 0u; i < entries_numof; i++)
    {
        entries_element = &(entries[i*SI_SD_CONST_ENTRY_ARRAY_SIZE]);

        SI_SD_WIRE_deserialize_ServiceEntry(entries_element, &parsed_entry);

        entry_action = SI_SD_PARSER_evaluate_entry(&parsed_entry);

        parsed_entries_pool[i] = parsed_entry;
        parsed_entries_action[i] = entry_action;
    }

    for (i = 0u; i < options_numof; i++)
    {
        option_element = &(options[i*SI_SD_CONST_OPTION_ARRAY_SIZE]);

        // note: in case of other option type, parsing will be different
        SI_SD_WIRE_deserialize_IPv4EndpointOption(option_element, &parsed_option);

        if (SD_OptionTypes_IPV4_ENDPOINT != parsed_option.type)
        {
            ERH_report_error(ERH_OPT_TYPE_INVALID, 0u, 0u, 0u, 0u, 0u, 0u);
            return FALSE;
        }

        option_action = SI_SD_PARSER_evaluate_option(&parsed_option);

        parsed_options_pool[i] = parsed_option;
        parsed_options_action[i] = option_action;
    }

    SI_SD_PARSER_fill_received_service_registry();

    return TRUE;
}

/* **************************************************** */
/*             Local function definitions               */
/* **************************************************** */

static boolean SI_SD_PARSER_find_registered_entry(struct SI_SD_ServiceEntry *parsed_entry)
{
    struct SD_remote_Service* registered_service = NULLPTR;

    registered_service = SI_SD_PROVIDER_lookup_service(parsed_entry->serviceID,
                                                       parsed_entry->instanceID,
                                                       parsed_entry->major_version);

    if (NULLPTR != registered_service)
    {
        return TRUE;
    }
    return FALSE;
}

static enum SI_SD_EntryRelatedAction_t SI_SD_PARSER_evaluate_entry(struct SI_SD_ServiceEntry *parsed_entry)
{
    const boolean invalid_parameter = (NULLPTR == parsed_entry);
    const boolean invalid_entry = ((SD_EntryTypes_Find != parsed_entry->type) && (SD_EntryTypes_Offer != parsed_entry->type));

    if (invalid_parameter || invalid_entry)
    {
        return SI_SD_EntryRelatedAction_None;
    }

    if (SD_EntryTypes_Find == parsed_entry->type)
    {
        // Received message asks for service information
        return SI_SD_EntryRelatedAction_Provide;
    }
    else if (TRUE == SI_SD_PARSER_find_registered_entry(parsed_entry))
    {
        // Received message is an offer and we already know of it
        if (0u == parsed_entry->ttl)
        {
            return SI_SD_EntryRelatedAction_Delete;
        }
        else
        {
            return SI_SD_EntryRelatedAction_Update;
        }
    }
    else
    {
        // Received message is an offer and we never heard of it
        return SI_SD_EntryRelatedAction_Save;
    }

    // Unreachable code
    return SI_SD_EntryRelatedAction_None;
}

static enum SI_SD_OptionRelatedAction_t SI_SD_PARSER_evaluate_option(struct SI_SD_IPv4EndpointOption *parsed_option)
{
    const boolean invalid_parameter = (NULLPTR == parsed_option);
    const boolean invalid_option = (SD_OptionTypes_IPV4_ENDPOINT != parsed_option->type);

    if (invalid_parameter || invalid_option)
    {
        return SI_SD_OptionRelatedAction_None;
    }
    return SI_SD_OptionRelatedAction_Save;
}

static void SI_SD_PARSER_fill_received_service_registry(void)
{
    uint32 i = 0u;

    // note: SI_SD_SERVICE_ENTRIES_MAX_NUMBER must equal to SI_SD_SERVICE_OPTIONS_MAX_NUMBER
    for (i = 0u; i < SI_SD_SERVICE_ENTRIES_MAX_NUMBER; i++)
    {
        if ((SI_SD_EntryRelatedAction_None != parsed_entries_action[i]) || (SI_SD_OptionRelatedAction_None != parsed_options_action[i]))
        {
            received_service_registry[i].entry = parsed_entries_pool[i];
            received_service_registry[i].entry_action = parsed_entries_action[i];
            received_service_registry[i].option = parsed_options_pool[i];
            received_service_registry[i].option_action = parsed_options_action[i];
            received_service_registry[i].used = TRUE;
        }
        else
        {
            received_service_registry[i].used = FALSE;
        }
    }
}

/* END OF SI_SD_PARSER.C FILE */
