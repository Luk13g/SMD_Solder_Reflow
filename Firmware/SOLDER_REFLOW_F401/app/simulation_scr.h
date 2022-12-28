/*
 * simulation_scr.h
 *
 *  Created on: Feb 4, 2021
 *      Author: lukgd
 */

#ifndef SIMULATION_SCR_H_
#define SIMULATION_SCR_H_

#define BB_TEMP_UNIT_LABEL          "C"

void bb_scr_write_temp(int16_t temp, uint8_t line);
void simulation_start(void);

#endif /* SIMULATION_SCR_H_ */
