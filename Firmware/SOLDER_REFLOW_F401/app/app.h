/*
 * app.h
 *
 *  Created on: May 16, 2021
 *      Author: lukgd
 */

#ifndef APP_H_
#define APP_H_

#include <stdbool.h>

void app_started_go(bool start_state);
void app_init(void);
void app_loop(void);
void app_PID(float yk);

#endif /* APP_H_ */
