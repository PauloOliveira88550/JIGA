/*
 * fsm.c
 *
 *  Created on: Jun 7, 2022
 *      Author: fabio
 */
#include "fsm.h"

#define TAG1 0x21B
#define TAG2 0x207
#define TAG3 0x178
#define TAG4 0x144
#define TAG5 0XD3
#define TAG6 0xBD

e_states state, nextstate;

uint8_t AllowedCardID[4];
uint8_t CardID[4];
uint8_t type;
char *result;
int status;
char msg[32];
int pwm_turnl = 0;
int pwm_turnr = 0;
int delay_turn = 0;
uint8_t path = 'A';
char path_flag = 0;
uint8_t value = 255;

int sum_card = 0;

void (*state_process [])(void) = {
															idle,
															start,
															turnRight,
															turnLeft,
															readID,
															Auto,
															skipCross,
															d360,
															sendnack,
															stop};


void idle(void){

    y_ant = 0;
	e_ant = 0;
	sum_e = 0 ;
	sum_e_bkp = 0;
	u_d = 0;
	u_d_ant = 0;
	//path_flag = 0;

	if(path_flag == 0){
		if(test_flag){
			path = inbuffer[0];
			test_flag = 0;
			path_flag = 1;
		}

	}
	else if((test_flag==1) && (path_flag==1)){
		if(inbuffer[0] == 49)
			nextstate = S_Start;

		test_flag=0;

	}
	else if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13))
		nextstate = S_Start;
	else
		nextstate=S_Idle;
}

void start(void){

	aut = 1;

	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, (40 * 53999)/100);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, (40 * 53999)/100);



	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);


	//
    HAL_TIM_Base_Start_IT(&htim3);


	nextstate = S_Auto;


}

void turnRight(void){
	aut = 0;
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, (pwm_turnl * 53999)/100);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, (pwm_turnr * 53999)/100);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 0);

	HAL_Delay(delay_turn);
	nextstate = S_Auto;

}
void turnLeft(void){
	aut = 0;
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, (pwm_turnl * 53999)/100);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, (pwm_turnr * 53999)/100);

	HAL_Delay(delay_turn);
	nextstate = S_Auto;



}

void readID(void){
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 0);

	status = MFRC522_CheckCard(CardID, &type);

	if (status == RFID_OK) {
		sum_card = CardID[0]+CardID[1]+CardID[2]+CardID[3];
		switch(sum_card){
			case TAG1:
				if(path == 'B'){
					nextstate = S_TurnRight;
					pwm_turnl = 80;
					pwm_turnr = 40;
					delay_turn = 950;

				}
				else if(path == 'C'){
					nextstate = S_TurnLeft;
					pwm_turnl = 40;
					pwm_turnr = 80;
					delay_turn = 1000;
				}

				break;

			case TAG2:
				nextstate = S_D360;

				break;

			case TAG3:
				if(path == 'A'){
					nextstate = S_TurnLeft;
					pwm_turnl = 40;
					pwm_turnr = 80;
					delay_turn = 1000;
				}
				else if(path == 'C'){
					nextstate = S_SkipCross;
				}

				break;

			case TAG4:
				if(path == 'A'){
					nextstate = S_TurnRight;
					pwm_turnl = 80;
					pwm_turnr = 40;
					delay_turn = 950;
				}
				else if(path == 'B'){
					nextstate = S_SkipCross;
				}

				break;

			case TAG5:
				nextstate = S_D360;

				break;

			case TAG6:
				nextstate = S_D360;

				break;

			default:
				nextstate = S_ReadID;
				break;
		}

	}else if(inbuffer[0]==51){
		nextstate = S_D360;
		inbuffer[0] = '\0';
	}
	else
		nextstate = S_ReadID;


}
void Auto(void){
	aut = 1;

	if(HCSR04_Read(HCSR04_SENSOR1)<=20){
		aut = 0;
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 0);

		HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);


		while(HCSR04_Read(HCSR04_SENSOR1)<=20){

			    htim4.Instance->CCR1 = value;  // vary the duty cycle
		}
		HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);
		nextstate = S_Auto;

	}

	else if(inbuffer[0]==50){
		nextstate=S_Stop;
		test_flag = 0;
	}

	else if((sensor[0] == 1)&&(sensor[1] == 1)&&(sensor[2] == 1)&&(sensor[3] == 0)&&(sensor[4] == 0)){
		nextstate=S_TurnLeft;
		pwm_turnl = 50;
		pwm_turnr = 70;
		delay_turn = 900;
		aut = 0;
	}
	else if((sensor[0] == 0)&&(sensor[1] == 0)&&(sensor[2] == 1)&&(sensor[3] == 1)&&(sensor[4] == 1)){
		nextstate=S_TurnRight;
		pwm_turnl = 70;
		pwm_turnr = 50;
		delay_turn = 900;
		aut = 0;
	}
	else if((sensor[0] == 1)&&(sensor[1] == 1)&&(sensor[2] == 1)&&(sensor[3] == 1)&&(sensor[4] == 1)){
		nextstate=S_ReadID;
		aut = 0;
	}
	else
		nextstate = S_Auto;


}
void skipCross(void){
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, (33 * 53999)/100);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, (33 * 53999)/100);
	HAL_Delay(500);
	nextstate = S_Auto;

}

void d360(void){
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, (50 * 53999)/100);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, (50 * 53999)/100);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 0);
	HAL_Delay(3600);
	nextstate = S_Stop;


}
void sendnack(void){

	nextstate=S_Stop;

}
void stop(void){

	aut = 0;
	path_flag = 0;

	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_4);

	nextstate = S_Idle;


}

void encode_fsm(void){

	state_process[state]();
}


