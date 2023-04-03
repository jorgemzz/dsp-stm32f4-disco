/*
 * stm32f4_audio.c
 *
 *  Created on: Apr 1, 2023
 *      Author: jmiranda
 */

#include "stm32f4_audio.h"
#include <stdio.h>

static AUDIO_DrvTypeDef	*pAudioDrv;

/**
  * @brief  Configures the audio peripherals.
  * @param  OutputDevice: OUTPUT_DEVICE_SPEAKER, OUTPUT_DEVICE_HEADPHONE,
  *                       OUTPUT_DEVICE_BOTH or OUTPUT_DEVICE_AUTO .
  * @param  Volume: Initial volume level (from 0 (Mute) to 100 (Max))
  * @param  AudioFreq: Audio frequency used to play the audio stream.
  * @retval AUDIO_OK if correct communication, else wrong communication
  */
uint8_t audio_init(uint16_t OutputDevice, uint8_t Volume, uint32_t AudioFreq){
	uint8_t ret = AUDIO_OK;

	if(ret == AUDIO_OK){
		/* Retieve audio codec identifier */
		if(((cs43l22_drv.ReadID(AUDIO_I2C_ADDRESS)) & CS43L22_ID_MASK) == CS43L22_ID){
			/* Initialize the audio driver structure */
			pAudioDrv = &cs43l22_drv;
		}else{
			ret = AUDIO_ERROR;
		}
	}

	if(ret == AUDIO_OK){
		pAudioDrv->Init(AUDIO_I2C_ADDRESS, OutputDevice, Volume, AudioFreq);
	}

	return AUDIO_OK;
}

/**
  * @brief  Starts playing audio stream from a data buffer for a determined size.
  * @param  pBuffer: Pointer to the buffer
  * @param  Size: Number of audio data BYTES.
  * @retval AUDIO_OK if correct communication, else wrong communication
  */
uint8_t audio_play(uint16_t* pBuffer, uint32_t Size){
	HAL_StatusTypeDef status = HAL_OK;

	/* Call the audio Codec Play function */
	if(pAudioDrv->Play(AUDIO_I2C_ADDRESS, pBuffer, Size) != 0){
		return AUDIO_ERROR;
	}else {
		/* Update the Media layer and enable it for play */
		status = HAL_I2S_Transmit(&hi2s3, pBuffer, Size, HAL_MAX_DELAY);

		/* Return AUDIO_OK when all operations are correctly done */
		if(status == HAL_OK){
			return AUDIO_OK;
		}else{
			return AUDIO_ERROR;
		}
	}
}

/**
  * @brief  Stops audio playing and Power down the Audio Codec.
  * @param  Option: could be one of the following parameters
  *           - CODEC_PDWN_HW: completely shut down the codec (physically).
  *                            Then need to reconfigure the Codec after power on.
  * @retval AUDIO_OK if correct communication, else wrong communication
  */
uint8_t audio_stop(uint32_t Option){
	/* Call Audio Codec Stop function */
	if(pAudioDrv->Stop(AUDIO_I2C_ADDRESS, Option) != 0){
		return AUDIO_ERROR;
	}else{
		if(Option == CODEC_PDWN_HW){
			/* Wait at least 1ms */
			HAL_Delay(1);

			/* Reset the pin */
			HAL_GPIO_WritePin(Audio_RST_GPIO_Port, Audio_RST_Pin, GPIO_PIN_RESET);
		}

		/* Return AUDIO_OK when all operations are correctly done */
		return AUDIO_OK;
	}
}

/**
  * @brief  Initializes Audio low level.
  */
void AUDIO_IO_Init(void){
  /* Power Down the codec */
  HAL_GPIO_WritePin(Audio_RST_GPIO_Port, Audio_RST_Pin, GPIO_PIN_RESET);

  /* Wait for a delay to insure registers erasing */
  HAL_Delay(5);

  /* Power on the codec */
  HAL_GPIO_WritePin(Audio_RST_GPIO_Port, Audio_RST_Pin, GPIO_PIN_SET);

  /* Wait for a delay to insure registers erasing */
  HAL_Delay(5);
}

/**
  * @brief  DeInitializes Audio low level.
  */
void AUDIO_IO_DeInit(void){

}

/**
  * @brief  Writes a single data.
  * @param  Addr: I2C address
  * @param  Reg: Reg address
  * @param  Value: Data to be written
  */
void AUDIO_IO_Write (uint8_t Addr, uint8_t Reg, uint8_t Value){
	HAL_StatusTypeDef status = HAL_OK;

	status = HAL_I2C_Mem_Write(&hi2c1, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, &Value, 1, HAL_MAX_DELAY);

	/* Check the communication status */
	if(status != HAL_OK){
		Error_Handler();
	}
}

/**
  * @brief  Reads a single data.
  * @param  Addr: I2C address
  * @param  Reg: Reg address
  * @retval Data to be read
  */
uint8_t AUDIO_IO_Read(uint8_t Addr, uint8_t Reg){
	HAL_StatusTypeDef status = HAL_OK;
	uint8_t value = 0;

	status = HAL_I2C_Mem_Read(&hi2c1, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, &value, 1, HAL_MAX_DELAY);

	/* Check the communication status */
	if(status != HAL_OK){
		Error_Handler();
	}
	return value;
}
