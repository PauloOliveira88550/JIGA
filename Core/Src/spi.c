/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    spi.c
  * @brief   This file provides code for the configuration
  *          of the SPI instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "spi.h"

/* USER CODE BEGIN 0 */
void MFRC522_Reset(void);
void MFRC522_ChipSelect(void);
void MFRC522_ChipDeselect(void);
void MFRC522_WriteRegister(uint8_t reg, uint8_t data);
uint8_t MFRC522_ReadRegister(uint8_t reg);
void MFRC522_EnableAntenna(void);
void MFRC522_DisableAntenna(void);
void MFRC522_SetBitMask(uint8_t reg, uint8_t mask);
void MFRC522_ClearBitMask(uint8_t reg, uint8_t mask);
void MFRC522_Halt(void);
MFRC522_Status_t MFRC522_Request(uint8_t request_mode, uint8_t *tag_type);
MFRC522_Status_t MFRC522_ToCard(uint8_t command, uint8_t *in_data, uint8_t in_len,
        uint8_t *out_data, uint16_t *out_len);
MFRC522_Status_t MFRC522_Anticollision(uint8_t *serial_num);
MFRC522_Status_t MFRC522_CalculateCRC(uint8_t *in_data, uint8_t len,
        uint8_t *out_data);
MFRC522_Status_t MFRC522_SelectTag(uint8_t *serial_num, uint8_t *type);
/* USER CODE END 0 */

SPI_HandleTypeDef hspi4;

/* SPI4 init function */
void MX_SPI4_Init(void)
{

  /* USER CODE BEGIN SPI4_Init 0 */

  /* USER CODE END SPI4_Init 0 */

  /* USER CODE BEGIN SPI4_Init 1 */

  /* USER CODE END SPI4_Init 1 */
  hspi4.Instance = SPI4;
  hspi4.Init.Mode = SPI_MODE_MASTER;
  hspi4.Init.Direction = SPI_DIRECTION_2LINES;
  hspi4.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi4.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi4.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi4.Init.NSS = SPI_NSS_SOFT;
  hspi4.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi4.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi4.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi4.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi4.Init.CRCPolynomial = 7;
  hspi4.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi4.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI4_Init 2 */

  /* USER CODE END SPI4_Init 2 */

}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(spiHandle->Instance==SPI4)
  {
  /* USER CODE BEGIN SPI4_MspInit 0 */

  /* USER CODE END SPI4_MspInit 0 */
    /* SPI4 clock enable */
    __HAL_RCC_SPI4_CLK_ENABLE();

    __HAL_RCC_GPIOE_CLK_ENABLE();
    /**SPI4 GPIO Configuration
    PE2     ------> SPI4_SCK
    PE5     ------> SPI4_MISO
    PE6     ------> SPI4_MOSI
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_5|GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI4;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /* USER CODE BEGIN SPI4_MspInit 1 */

  /* USER CODE END SPI4_MspInit 1 */
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{

  if(spiHandle->Instance==SPI4)
  {
  /* USER CODE BEGIN SPI4_MspDeInit 0 */

  /* USER CODE END SPI4_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI4_CLK_DISABLE();

    /**SPI4 GPIO Configuration
    PE2     ------> SPI4_SCK
    PE5     ------> SPI4_MISO
    PE6     ------> SPI4_MOSI
    */
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_2|GPIO_PIN_5|GPIO_PIN_6);

  /* USER CODE BEGIN SPI4_MspDeInit 1 */

  /* USER CODE END SPI4_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
/*
 * ******************************************************************************
 * ******************************************************************************
 * ******************************************************************************
 */
void MFRC522_Init() {
	MFRC522_ChipDeselect();
	MFRC522_Reset();
	//HAL_GPIO_WritePin(GPIOG, GPIO_PIN_2, 1);
	MFRC522_WriteRegister(MFRC522_T_MODE_REG, 0x8D);
	MFRC522_WriteRegister(MFRC522_T_PRESCALER_REG, 0x3E);
	MFRC522_WriteRegister(MFRC522_T_RELOAD_REG_H, 0x03);
	MFRC522_WriteRegister(MFRC522_T_RELOAD_REG_L, 0xE8);
	MFRC522_WriteRegister(MFRC522_RFC_FG_REG, 0x70);
	MFRC522_WriteRegister(MFRC522_TX_ASK_REG, 0x40);
	MFRC522_WriteRegister(MFRC522_MODE_REG, 0x3D);

	MFRC522_EnableAntenna();
}

/* Returns the reader's version number */
uint8_t MFRC522_Version() {
	return MFRC522_ReadRegister(MFRC522_VERSION_REG);
}

/* If card is found, its id and type are returned */
MFRC522_Status_t MFRC522_CheckCard(uint8_t *id, uint8_t *type) {
	MFRC522_Status_t status = MFRC522_Request(MFRC522_PICC_REQ_IDL, id);
	if (status == RFID_OK) { // Detected card
		status = MFRC522_Anticollision(id);
		status = MFRC522_SelectTag(id, type);
	}

	MFRC522_Halt();
	return status;
}

/* Check if two RFID card IDs match */
MFRC522_Status_t MFRC522_CompareIDs(uint8_t *id1, uint8_t *id2){
	for (uint8_t i = 0; i < 4; i++) {
		if (id1[i] != id2[i]) {
			return RFID_ERR;
		}
	}

	return RFID_OK;
}

/* Write a byte to a register */
void MFRC522_WriteRegister(uint8_t reg, uint8_t data) {
	MFRC522_ChipSelect();

	reg = (reg << 1) & 0x7E;

	if (HAL_SPI_Transmit(&hspi4, &reg, 1,
	HAL_MAX_DELAY) != HAL_SPI_ERROR_NONE) {
		//printf("[ERROR]: (WriteRegister) Failed to write address to register\r\n");
		//sendSerialPort("[ERROR]: (WriteRegister) Failed to write address to register\r\n");
	}

	if (HAL_SPI_Transmit(&hspi4, &data, 1,
	HAL_MAX_DELAY) != HAL_SPI_ERROR_NONE) {
		//printf("[ERROR]: (WriteRegister) Failed to write data to register\r\n");
		//sendSerialPort("[ERROR]: (WriteRegister) Failed to write data to register\r\n");
	}

	MFRC522_ChipDeselect();
}

/* Read a byte from a register */
uint8_t MFRC522_ReadRegister(uint8_t reg) {
	uint8_t data = 0x00;
	uint8_t dummy = 0x00;

	MFRC522_ChipSelect();

	reg = (reg << 1) | 0x80;

	if (HAL_SPI_Transmit(&hspi4, &reg, 1,HAL_MAX_DELAY) != HAL_SPI_ERROR_NONE) {
		//printf("[ERROR]: (ReadRegister) Failed to write address to register\r\n");
		//sendSerialPort("[ERROR]: (ReadRegister) Failed to write address to register\r\n");
	}

	if (HAL_SPI_TransmitReceive(&hspi4, &dummy, &data, 1,
	HAL_MAX_DELAY) != HAL_SPI_ERROR_NONE) {
		//printf("[ERROR]: (ReadRegister) Failed to read data from register\r\n");
		//sendSerialPort("[ERROR]: (ReadRegister) Failed to read data from register\r\n");
	}

	MFRC522_ChipDeselect();
	return data;
}

/* Soft reset the reader */
void MFRC522_Reset(void) {
	MFRC522_WriteRegister(MFRC522_COMMAND_REG, MFRC522_COMMAND_SOFT_RESET);
	HAL_Delay(50);
}

/* Enables writing to reader */
void MFRC522_ChipSelect(void) {
	HAL_GPIO_WritePin(MFRC522_PORT_CS, MFRC522_PIN_CS, 0);
}

/* Enables reading from reader */
void MFRC522_ChipDeselect(void) {
	HAL_GPIO_WritePin(MFRC522_PORT_CS, MFRC522_PIN_CS, 1);
}

void MFRC522_SetBitMask(uint8_t reg, uint8_t mask) {
	MFRC522_WriteRegister(reg, MFRC522_ReadRegister(reg) | mask);
}

void MFRC522_ClearBitMask(uint8_t reg, uint8_t mask) {
	MFRC522_WriteRegister(reg, MFRC522_ReadRegister(reg) & (~mask));
}

void MFRC522_EnableAntenna(void) {
	uint8_t status = MFRC522_ReadRegister(MFRC522_TX_CONTROL_REG);
	if (!(status & 0x03)) {
		MFRC522_SetBitMask(MFRC522_TX_CONTROL_REG, 0x03);
	}
}

void MFRC522_DisableAntenna(void) {
	MFRC522_ClearBitMask(MFRC522_TX_CONTROL_REG, 0x03);
}

MFRC522_Status_t MFRC522_Request(uint8_t request_mode, uint8_t *tag_type) {
	uint16_t data;

	MFRC522_WriteRegister(MFRC522_BIT_FRAMING_REG, 0x07);

	tag_type[0] = request_mode;
	MFRC522_Status_t status = MFRC522_ToCard(MFRC522_COMMAND_TRANSCEIVE, tag_type, 1,
	        tag_type, &data);
	if (status == RFID_OK && data != 0x10) {
		//printf("[ERROR]: (Request) Received invalid data from ToCard()\r\n");
		//sendSerialPort("[ERROR]: (Request) Received invalid data from ToCard()\r\n");
		status = RFID_ERR;
	}

	return status;
}

MFRC522_Status_t MFRC522_ToCard(uint8_t command, uint8_t *in_data, uint8_t in_len,
        uint8_t *out_data, uint16_t *out_len) {
	MFRC522_Status_t status = RFID_ERR;
	uint8_t irq_en = 0x00;
	uint8_t wait_irq = 0x00;
	uint8_t last_bits;

	switch (command) {
	case MFRC522_COMMAND_MF_AUTHENT:
		irq_en = 0x12;
		wait_irq = 0x10;
		break;
	case MFRC522_COMMAND_TRANSCEIVE:
		irq_en = 0x77;
		wait_irq = 0x30;
		break;
	default:
		break;
	}

	MFRC522_WriteRegister(MFRC522_COML_EN_REG, (irq_en | 0x80));
	MFRC522_WriteRegister(MFRC522_COMMAND_REG, MFRC522_COMMAND_IDLE); // Stop active commands
	MFRC522_ClearBitMask(MFRC522_COLL_REG, 0x80); // Clear collision register
	MFRC522_WriteRegister(MFRC522_COML_RQ_REG, 0x7F); // Clear interrupt request bits
	MFRC522_SetBitMask(MFRC522_FIFO_LEVEL_REG, 0x80); // Initialize FIFO

	// Write data to FIFO
	for (uint16_t i = 0; i < in_len; i++) {
		MFRC522_WriteRegister(MFRC522_FIFO_DATA_REG, in_data[i]);
	}

	// Execute command
	MFRC522_WriteRegister(MFRC522_COMMAND_REG, command);
	if (command == MFRC522_COMMAND_TRANSCEIVE) {
		MFRC522_SetBitMask(MFRC522_BIT_FRAMING_REG, 0x80);
	}

	// Wait to receive data
	uint8_t n;
	uint16_t i = 36000;
	do {
		n = MFRC522_ReadRegister(MFRC522_COML_RQ_REG);
		i--;
	} while ((i != 0) && !(n & 0x01) && !(n & wait_irq));

	MFRC522_ClearBitMask(MFRC522_BIT_FRAMING_REG, 0x80);

	// Error
	uint8_t error_reg_val = MFRC522_ReadRegister(MFRC522_ERROR_REG);
	if (error_reg_val & 0x13) {
		//printf("[ERROR]: (ToCard) Error register reported error (1)\r\n");
		//sendSerialPort("[ERROR]: (ToCard) Error register reported error (1)\r\n");
		return RFID_ERR;
	}

	// Timeout
	if (i == 0) {
		return RFID_TIMEOUT;
	} else if ((n & 0x01) && !(n & wait_irq)) {
		return RFID_TIMEOUT;
	}

	if (i != 0) {
		if (!(MFRC522_ReadRegister(MFRC522_ERROR_REG) & 0x1B)) {
			status = RFID_OK;

			if (command == MFRC522_COMMAND_TRANSCEIVE) {
				n = MFRC522_ReadRegister(MFRC522_FIFO_LEVEL_REG);
				last_bits = MFRC522_ReadRegister(MFRC522_CONTROL_REG) & 0x07;

				if (!n) {
					n = 1;
				}

				if (last_bits) {
					*out_len = (n - 1) * 8 + last_bits;
				} else {
					*out_len = n * 8;
				}

				if (n > MFRC522_MAX_LEN) {
					n = MFRC522_MAX_LEN;
				}

				// Read the received data from FIFO
				for (uint16_t i = 0; i < n; i++) {
					out_data[i] = MFRC522_ReadRegister(MFRC522_FIFO_DATA_REG);
				}
			}
		} else {
			//printf("[ERROR]: (ToCard) Error register reported error (2)\r\n");
			//sendSerialPort("[ERROR]: (ToCard) Error register reported error (2)\r\n");
			return RFID_ERR;
		}
	}

	// Collision error
	if (error_reg_val & 0x08) {
		//printf("[ERROR]: (ToCard) Detected collision\r\n");
		//sendSerialPort("[ERROR]: (ToCard) Detected collision\r\n");
		return RFID_ERR;
	}

	return status;
}

MFRC522_Status_t MFRC522_Anticollision(uint8_t *serial_num) {
	uint16_t len;
	uint8_t temp = 0;

	MFRC522_WriteRegister(MFRC522_BIT_FRAMING_REG, 0x00);

	serial_num[0] = MFRC522_PICC_ANTICOLLISION;
	serial_num[1] = 0x20;

	MFRC522_Status_t status = MFRC522_ToCard(MFRC522_COMMAND_TRANSCEIVE, serial_num,
	        2, serial_num, &len);
	if (status == RFID_OK) {
		// Check serial number
		for (uint8_t i = 0; i < 4; i++) {
			temp ^= serial_num[i];
			if (temp != serial_num[i]) {
				// No need to print anything
				return RFID_ERR;
			}
		}
	}

	return status;
}

MFRC522_Status_t MFRC522_CalculateCRC(uint8_t *in_data, uint8_t len,
        uint8_t *out_data) {
	MFRC522_ClearBitMask(MFRC522_DIVL_RQ_REG, 0x04); // CRCIrq = 0
	MFRC522_SetBitMask(MFRC522_FIFO_LEVEL_REG, 0x80); // Clear FIFO pointer
	MFRC522_WriteRegister(MFRC522_COMMAND_REG, MFRC522_COMMAND_IDLE); // Stop active commands

	// Write data to FIFO
	for (uint8_t i = 0; i < len; i++) {
		MFRC522_WriteRegister(MFRC522_FIFO_DATA_REG, *(in_data + i));
	}

	MFRC522_WriteRegister(MFRC522_COMMAND_REG, MFRC522_COMMAND_CALC_CRC);

	// Wait for calculation to complete
	uint8_t i = 0xFF;
	uint8_t n;
	do {
		n = MFRC522_ReadRegister(MFRC522_DIVL_RQ_REG);
		i--;
	} while ((i != 0) && !(n & 0x04));

	// Timeout
	if (i == 0) {
		return RFID_TIMEOUT;
	}

	// Save result
	out_data[0] = MFRC522_ReadRegister(MFRC522_CRC_RESULT_REG_L);
	out_data[1] = MFRC522_ReadRegister(MFRC522_CRC_RESULT_REG_H);

	return RFID_OK;
}

MFRC522_Status_t MFRC522_SelectTag(uint8_t *serial_num, uint8_t *type) {
	uint8_t buffer[9];
	uint8_t sak[3] = { 0 };
	uint16_t out_data;

	buffer[0] = MFRC522_PICC_SELECT_TAG;
	buffer[1] = 0x70;

	for (uint8_t i = 0; i < 4; i++) {
		buffer[i + 2] = *(serial_num + i);
	}

	// Calculate CRC
	buffer[6] = buffer[2] ^ buffer[3] ^ buffer[4] ^ buffer[5];
	MFRC522_Status_t status = MFRC522_CalculateCRC(buffer, 7, &buffer[7]);
	if (status != RFID_OK) {
		//printf("[ERROR]: (SelectTag) CalculateCRC returned error (1)\r\n");
		//sendSerialPort("[ERROR]: (SelectTag) CalculateCRC returned error (1)\r\n");
		return status;
	}

	status = MFRC522_ToCard(MFRC522_COMMAND_TRANSCEIVE, buffer, 9, sak, &out_data);
	if (status != RFID_OK) {
		//printf("[ERROR]: (SelectTag) CalculateCRC returned error (2)\r\n");
		//sendSerialPort("[ERROR]: (SelectTag) CalculateCRC returned error (2)\r\n");
	}

	// SAK must be 24 bits (1 byte + CRC)
	if (out_data != 24) {
		//printf("[ERROR]: (SelectTag) SAK mismatch\r\n");
		//sendSerialPort("[ERROR]: (SelectTag) SAK mismatch\r\n");
		return RFID_ERR;
	}

	*type = sak[0];

	return status;
}

void MFRC522_Halt(void) {
	uint8_t buff[4];
	uint16_t len;

	buff[0] = MFRC522_PICC_HALT;
	buff[1] = 0;

	MFRC522_CalculateCRC(buff, 2, &buff[2]);
	MFRC522_ToCard(MFRC522_COMMAND_TRANSCEIVE, buff, 4, buff, &len);
}

/* Converts binary to string hex (%X) */
void MFRC522_PrettyPrint(unsigned char *in, unsigned int size, char **out) {
	char hex_chars[] = "0123456789ABCDEF";

	if (!size) {
		return;
	}

	// Allocate enough space for whole text (4 bytes = 8 chars + '0', 'x' and \0)
	*out = (char*) malloc(size * 2 + 3);
	(*out)[size * 2 + 2] = 0;

	(*out)[0] = '0';
	(*out)[1] = 'x';

	for (uint8_t i = 0; i < size; i++) {
		(*out)[i * 2 + 2] = hex_chars[(in[i] >> 4) & 0x0F];
		(*out)[i * 2 + 3] = hex_chars[in[i] & 0x0F];
	}
}
/* USER CODE END 1 */
