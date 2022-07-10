/*
 * fsm.h
 *
 *  Created on: Jun 7, 2022
 *      Author: fabio
 */

#ifndef INC_FSM_H_
#define INC_FSM_H_

#include "main.h"
#include "gpio.h"
#include "tim.h"
#include "usart.h"
#include "spi.h"

typedef enum ENUM_STATES {S_Idle = 0, S_Start, S_TurnRight, S_TurnLeft, S_ReadID, S_Auto, S_SkipCross, S_D360, S_SendNack, S_Stop} e_states;
extern e_states state, nextstate;

void idle(void);
void start(void);
void turnRight(void);
void turnLeft(void);
void readID(void);
void Auto(void);
void skipCross(void);
void d360(void);
void sendnack(void);
void stop(void);
void encode_fsm(void);

#endif /* INC_FSM_H_ */
