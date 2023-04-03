/*
 * stm32f4_audio.h
 *
 *  Created on: Apr 1, 2023
 *      Author: jmiranda
 */

#ifndef INC_STM32F4_AUDIO_H_
#define INC_STM32F4_AUDIO_H_

#include "cs43l22.h"
#include "main.h"

#define AUDIO_I2C_ADDRESS               0x94

/* Audio status definition */
#define AUDIO_OK                        0
#define AUDIO_ERROR                     1
#define AUDIO_TIMEOUT                   2

uint8_t audio_init(uint16_t OutputDevice, uint8_t Volume, uint32_t AudioFreq);
uint8_t audio_play(uint16_t* pBuffer, uint32_t Size);
uint8_t audio_stop(uint32_t Option);

#endif /* INC_STM32F4_AUDIO_H_ */
