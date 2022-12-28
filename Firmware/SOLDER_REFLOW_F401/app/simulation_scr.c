/*
 * simulation_scr.c
 *
 *  Created on: Feb 4, 2021
 *      Author: lukgd
 */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "simulation_scr.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"

static uint8_t * bb_scr_insert_uint(uint8_t *buffer, uint32_t value, uint8_t n_digits, bool spaces)
{
    uint8_t* pbuf = buffer;
    uint8_t *pend;
    uint32_t shifter;
    bool filling = false;
    uint8_t digits[10] = {'0','1','2','3','4','5','6','7','8','9'};

    // count the number of digits
    shifter = value;
    do
    {
        ++pbuf;
        shifter = shifter / 10;
    } while(shifter);

    if(n_digits > 0)
    {
    	int8_t n = n_digits - (pbuf - buffer);
    	while(n-- > 0)
    		pbuf++;
    }

    pend = pbuf;

    // now fill the digits
    do
    {
    	if(filling && spaces)
    		*--pbuf = ' ';
    	else
    		*--pbuf = digits[value % 10];
        value = value / 10;

        filling = value == 0;

    } while(pbuf != buffer);

    *pend = '\0';

    return pend;
}

static uint8_t * bb_scr_insert_int(uint8_t *buffer, int32_t value, uint8_t n_digits, bool spaces)
{
    uint8_t* pbuf = buffer;

    // add signal, invert value and call the unsigned version
    if(value < 0)
    {
        *pbuf++ = '-';
        value *= -1;
    }

    return bb_scr_insert_uint(pbuf, (uint32_t) value, n_digits, spaces);
}

static uint8_t * bb_scr_format_float1d(uint8_t *buf, int32_t f, uint8_t n_digits, bool spaces)
{
	uint8_t *pbuf;
	int32_t ip, fp;

	ip = f / 10;
	fp = f - ip * 10;

	pbuf = bb_scr_insert_int(buf,ip,n_digits,spaces);
	fp =  fp < 0 ? -fp : fp;
	//*pbuf++ = '.';
	pbuf = bb_scr_insert_uint(pbuf,fp,0,false);

	return pbuf;
}

void bb_scr_write_temp(int16_t temp, uint8_t line)
{
	uint8_t buf[23];
	uint8_t *pbuf = buf;
	//SDD1306_Clear_Screen();
	SDD1306_Clear_Line(2);
	pbuf = bb_scr_format_float1d(buf,temp,0,false);
	*pbuf++ = BB_TEMP_UNIT_LABEL[0];
	*pbuf = '\0';
	//SSD1306_Write_Centered_String((uint8_t*)"TEMPERATURA",0);
	SSD1306_Write_Centered_String(buf,3);
}

void simulation_start()
{
	//SSD1306_Write_Centered_String((uint8_t*)"TEMPERATURE",0);
	//bb_scr_write_temp(temp,2);

}
