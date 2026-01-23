/**
 * @file    SI_ERH.c
 * @author  Erdei Sándor (sandorerdei21@gmail.com)
 * @date
 * @brief   "Implements ERH.h"
 */

/* **************************************************** */
/*                      Includes                        */
/* **************************************************** */

#include "ERH.h"

/* **************************************************** */
/*                       Defines                        */
/* **************************************************** */

/* **************************************************** */
/*               Static global variables                */
/* **************************************************** */

static struct ERH_Error ERH_error_buffer[ERH_SIZE_OF_ERH_BUFFER] = {0u};

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
 
void ERH_report_error(uint8 type, uint32 field0, uint32 field1, uint32 field2, uint32 field3, uint32 field4, uint32 field5)
{
    uint8 i = 0u;

    for (i = 0u; i < ERH_SIZE_OF_ERH_BUFFER; i++)
    {
        if (FALSE == ERH_error_buffer[i].valid)
        {
            ERH_error_buffer[i].type = type;
            ERH_error_buffer[i].valid = TRUE;
            ERH_error_buffer[i].field0 = field0;
            ERH_error_buffer[i].field1 = field1;
            ERH_error_buffer[i].field2 = field2;
            ERH_error_buffer[i].field3 = field3;
            ERH_error_buffer[i].field4 = field4;
            ERH_error_buffer[i].field5 = field5;
            break;
        }
    }
}

/* **************************************************** */
/*             Local function definitions               */
/* **************************************************** */

/* END OF SI_HEADER.C FILE */
