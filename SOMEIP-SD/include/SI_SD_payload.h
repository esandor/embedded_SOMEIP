// Include guard starts here
#ifndef SI_SD_PAYLOAD_H_
#define SI_SD_PAYLOAD_H_

/**
 * @file    SI_SD_payload.h
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   "Interface for handling SOME/IP Service Discovery payload, provides check functions for easy handling.
 * 
 *           The modul is implemented with major simplifications:
 *           The modul only handles Service (type 1) entries and only IPv4 Endpoint type options.
 *           EventGroup entries and options with other types are not supported."
 * 
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include "SI_types.h"
#include "SI_header.h"
#include "SI_SD_config.h"
#include "SI_SD_header.h"

/* **************************************************** */
/*                       Defines                        */
/* **************************************************** */

/* **************************************************** */
/*                  Type definitions                    */
/* **************************************************** */

enum SI_SD_EntryRelatedAction_t
{
    SI_SD_EntryRelatedAction_None = 0x00u,
    SI_SD_EntryRelatedAction_Save = 0x01u,
    SI_SD_EntryRelatedAction_Update = 0x02u,
    SI_SD_EntryRelatedAction_Delete = 0x03u,
    SI_SD_EntryRelatedAction_Provide = 0x04u
};

enum SI_SD_OptionRelatedAction_t
{
    SI_SD_OptionRelatedAction_None = 0x00u,
    SI_SD_OptionRelatedAction_Save = 0x01u,
    SI_SD_OptionRelatedAction_Update = 0x02u,
    SI_SD_OptionRelatedAction_Delete = 0x03u,
    SI_SD_OptionRelatedAction_Provide = 0x04u
};

enum SD_EntryTypes
{
    SD_EntryTypes_Find =  ((uint8)0x00u),
    SD_EntryTypes_Offer = ((uint8)0x01u),  /* Offer (ttl>0) / StopOffer (ttl=0) */
};

struct SI_SD_ServiceEntry
{
    enum SD_EntryTypes type;
    uint8 index_1st_option_run;
    uint8 index_2nd_option_run;
    uint8 number_of_options1;
    uint8 number_of_options2;
    uint16 serviceID;
    uint16 instanceID;
    uint8 major_version;
    uint32 ttl;
    uint32 minor_version;
};

enum SD_OptionTypes_t
{
    SD_OptionTypes_IPV4_ENDPOINT     = 0x04u,
    SD_OptionTypes_IPV4_MC           = 0x14u,
    SD_OptionTypes_IPV4_SD_Endpoint  = 0x24u
};

struct SI_SD_IPv4EndpointOption
{
    uint16 length;
    enum SD_OptionTypes_t type;
    boolean discardable_flag;
    uint32 IPv4_address;
    uint8 l4_proto;
    uint16 port_number;
};

struct SD_Payload
{
    /* raw byte array */
    uint8* data;
    uint32 length;

    /* parsed structures */
    uint32 entries_length;
    struct SI_SD_ServiceEntry entries[SI_SD_SERVICE_ENTRIES_MAX_NUMBER];
    enum SI_SD_EntryRelatedAction_t entry_action[SI_SD_SERVICE_ENTRIES_MAX_NUMBER];
    boolean enty_validity[SI_SD_SERVICE_ENTRIES_MAX_NUMBER];

    uint32 options_length;
    struct SI_SD_IPv4EndpointOption options[SI_SD_SERVICE_OPTIONS_MAX_NUMBER];
    enum SI_SD_OptionRelatedAction_t option_action[SI_SD_SERVICE_OPTIONS_MAX_NUMBER];
    boolean option_validity[SI_SD_SERVICE_OPTIONS_MAX_NUMBER];
};

/* **************************************************** */
/*               Function declarations                  */
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
                                            struct SI_SD_ServiceEntry* out_entry);
boolean SI_SD_PAYLOAD_create_option(uint16 length,
                                    enum SD_OptionTypes_t type,
                                    boolean discardable_flag,
                                    uint32 IPv4_address,
                                    uint8 l4_proto,
                                    uint16 port_number,
                                    struct SI_SD_IPv4EndpointOption *out_option);

// Include guard stops here
#endif // SI_SD_PAYLOAD_H_
