// Include guard starts here
#ifndef SI_SD_PROCESS_H_
#define SI_SD_PROCESS_H_

/**
 * @file    SI_SD_process.h
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   "Main modul of SOME/IP Service Discovery.
 *           Provides SD Rx handler function."
 * 
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include "lwip/pbuf.h"
#include "lwip/ip_addr.h"
#include "lwip/udp.h"
#include "SI_types.h"

#include "SomeIP_udp.h"
#include "SI_dispatcher.h"
#include "SI_parser.h"
#include "SI_header.h"
#include "SI_service_manager.h"
#include "SI_message.h"

#include "SI_SD_Header.h"
#include "SI_SD_payload.h"

/* **************************************************** */
/*                       Defines                        */
/* **************************************************** */

/* **************************************************** */
/*                  Type definitions                    */
/* **************************************************** */

struct SD_MessageContext
{
    struct SD_Header header;
    struct SD_Payload payload;
    void* cookie; // transport layer cookie
};

/* **************************************************** */
/*               Function declarations                  */
/* **************************************************** */

boolean SI_SD_PROCESS_multicast(struct udp_pcb *rx_udp_pcb, struct pbuf *rx_pbuf, const ip_addr_t *src_addr, u16_t src_port);

// Include guard stops here
#endif /* SI_SD_PROCESS_H_ */