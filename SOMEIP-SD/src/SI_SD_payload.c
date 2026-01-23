/**
 * @file    SI_SD_payload.c
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   "Implements SI_SD_payload.h"
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include "SI_SD_payload.h"

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

/* **************************************************** */
/*             Local function definitions               */
/* **************************************************** */

boolean SI_SD_PAYLOAD_create_single_entry(enum SD_EntryTypes type,
                                            uint8 index_1st_option_run,
                                            uint8 index_2nd_option_run,
                                            uint8 number_of_options1,
                                            uint8 number_of_options2,
                                            uint16 serviceID,
                                            uint16 instanceID,
                                            uint8 major_version,
                                            uint32 ttl,
                                            uint32 minor_version,
                                            struct SI_SD_ServiceEntry* out_entry)
{
    const boolean invalid_input = (NULLPTR == out_entry);
    const boolean invalid_type = ((SD_EntryTypes_Find != type) && (SD_EntryTypes_Offer != type));

    // ... add additional checks

    if (invalid_input || invalid_type)
    {
        return FALSE;
    }

    out_entry->type = type;
    out_entry->index_1st_option_run = index_1st_option_run;
    out_entry->index_2nd_option_run = index_2nd_option_run;
    out_entry->number_of_options1 = number_of_options1;
    out_entry->number_of_options2 = number_of_options2;
    out_entry->serviceID = serviceID;
    out_entry->instanceID = instanceID;
    out_entry->major_version = major_version;
    out_entry->ttl = ttl;
    out_entry->minor_version = minor_version;

	return TRUE;
}

boolean SI_SD_PAYLOAD_create_option(uint16 length,
                                    enum SD_OptionTypes_t type,
                                    boolean discardable_flag,
                                    uint32 IPv4_address,
                                    uint8 l4_proto,
                                    uint16 port_number,
                                    struct SI_SD_IPv4EndpointOption *out_option)
{
    const boolean invlaid_input = (NULLPTR == out_option);
    const boolean invalid_type = (SD_OptionTypes_IPV4_ENDPOINT != type); // currently the only valid value

    // ... add additional checks

    if (invlaid_input || invalid_type)
    {
        return FALSE;
    }

    out_option->length = length;
    out_option->type = type;
    out_option->discardable_flag = discardable_flag;
    out_option->IPv4_address = IPv4_address;
    out_option->l4_proto = l4_proto;
    out_option->port_number = port_number;

	return TRUE;
}

/* **************************************************** */
/*             Local function definitions               */
/* **************************************************** */

/* END OF SI_SD_PAYLOAD.C FILE */
