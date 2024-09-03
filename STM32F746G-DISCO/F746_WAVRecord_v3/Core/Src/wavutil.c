#include "wavutil.h"
#include <string.h>

#define HEADER_BUFFER_SIZE 44

uint8_t pHeaderBuff[HEADER_BUFFER_SIZE];
WAVE_FormatTypeDef WaveFormat;

extern char *msg;
extern FRESULT fres;
extern AUDIO_IN_BufferTypeDef BufferCtl;

/**
 * @brief  Encoder initialization.
 * @param  Freq: Sampling frequency.
 * @param  pHeader: Pointer to the WAV file header to be written.
 * @retval 0 if success, !0 else.
 */
uint32_t WavProcess_EncInit(uint32_t Freq, uint8_t *pHeader)
{
    /* Initialize the encoder structure */
    WaveFormat.SampleRate = Freq;     /* Audio sampling frequency */
    WaveFormat.NbrChannels = 2;       /* Number of channels: 1:Mono or 2:Stereo */
    WaveFormat.BitPerSample = 16;     /* Number of bits per sample (16, 24 or 32) */
    WaveFormat.FileSize = 0x0009C400; /* Total length of useful audio data (payload) */
    WaveFormat.SubChunk1Size = 44;    /* The file header chunk size */
    WaveFormat.ByteRate = (WaveFormat.SampleRate *
                           (WaveFormat.BitPerSample / 8) *
                           WaveFormat.NbrChannels); /* Number of bytes per second  (sample rate * block align)  */
    WaveFormat.BlockAlign = WaveFormat.NbrChannels *
                            (WaveFormat.BitPerSample / 8); /* channels * bits/sample / 8 */

    /* Parse the wav file header and extract required information */
    if (WavProcess_HeaderInit(pHeader, &WaveFormat))
    {
        return 1;
    }
    return 0;
}

/**
 * @brief  Initialize the wave header file
 * @param  pHeader: Header Buffer to be filled
 * @param  pWaveFormatStruct: Pointer to the wave structure to be filled.
 * @retval 0 if passed, !0 if failed.
 */
uint32_t WavProcess_HeaderInit(uint8_t *pHeader, WAVE_FormatTypeDef *pWaveFormatStruct)
{
    /* Write chunkID, must be 'RIFF'  ------------------------------------------*/
    pHeader[0] = 'R';
    pHeader[1] = 'I';
    pHeader[2] = 'F';
    pHeader[3] = 'F';

    /* Write the file length ---------------------------------------------------*/
    /* The sampling time: this value will be written back at the end of the
       recording operation.  Example: 661500 Bytes = 0x000A17FC, byte[7]=0x00, byte[4]=0xFC */
    pHeader[4] = 0x00;
    pHeader[5] = 0xC4;
    pHeader[6] = 0x09;
    pHeader[7] = 0x00;
    /* Write the file format, must be 'WAVE' -----------------------------------*/
    pHeader[8] = 'W';
    pHeader[9] = 'A';
    pHeader[10] = 'V';
    pHeader[11] = 'E';

    /* Write the format chunk, must be'fmt ' -----------------------------------*/
    pHeader[12] = 'f';
    pHeader[13] = 'm';
    pHeader[14] = 't';
    pHeader[15] = ' ';

    /* Write the length of the 'fmt' data, must be 0x10 ------------------------*/
    pHeader[16] = 0x10;
    pHeader[17] = 0x00;
    pHeader[18] = 0x00;
    pHeader[19] = 0x00;

    /* Write the audio format, must be 0x01 (PCM) ------------------------------*/
    pHeader[20] = 0x01;
    pHeader[21] = 0x00;

    /* Write the number of channels, ie. 0x01 (Mono) ---------------------------*/
    pHeader[22] = pWaveFormatStruct->NbrChannels;
    pHeader[23] = 0x00;

    /* Write the Sample Rate in Hz ---------------------------------------------*/
    /* Write Little Endian ie. 8000 = 0x00001F40 => byte[24]=0x40, byte[27]=0x00*/
    pHeader[24] = (uint8_t)((pWaveFormatStruct->SampleRate & 0xFF));
    pHeader[25] = (uint8_t)((pWaveFormatStruct->SampleRate >> 8) & 0xFF);
    pHeader[26] = (uint8_t)((pWaveFormatStruct->SampleRate >> 16) & 0xFF);
    pHeader[27] = (uint8_t)((pWaveFormatStruct->SampleRate >> 24) & 0xFF);

    /* Write the Byte Rate -----------------------------------------------------*/
    pHeader[28] = (uint8_t)((pWaveFormatStruct->ByteRate & 0xFF));
    pHeader[29] = (uint8_t)((pWaveFormatStruct->ByteRate >> 8) & 0xFF);
    pHeader[30] = (uint8_t)((pWaveFormatStruct->ByteRate >> 16) & 0xFF);
    pHeader[31] = (uint8_t)((pWaveFormatStruct->ByteRate >> 24) & 0xFF);

    /* Write the block alignment -----------------------------------------------*/
    pHeader[32] = pWaveFormatStruct->BlockAlign;
    pHeader[33] = 0x00;

    /* Write the number of bits per sample -------------------------------------*/
    pHeader[34] = pWaveFormatStruct->BitPerSample;
    pHeader[35] = 0x00;

    /* Write the Data chunk, must be 'data' ------------------------------------*/
    pHeader[36] = 'd';
    pHeader[37] = 'a';
    pHeader[38] = 't';
    pHeader[39] = 'a';

    /* Write the number of sample data -----------------------------------------*/
    /* This variable will be written back at the end of the recording operation */
    pHeader[40] = 0x00;
    pHeader[41] = 0xC4;
    pHeader[42] = 0x09;
    pHeader[43] = 0x00;

    /* Return 0 if all operations are OK */
    return 0;
}

/**
 * @brief  Initialize the wave header file
 * @param  pHeader: Header Buffer to be filled
 * @param  pWaveFormatStruct: Pointer to the wave structure to be filled.
 * @retval 0 if passed, !0 if failed.
 */
uint32_t WavProcess_HeaderUpdate(uint8_t *pHeader, WAVE_FormatTypeDef *pWaveFormatStruct)
{
    /* Write the file length ---------------------------------------------------*/
    /* The sampling time: this value will be written back at the end of the
       recording operation.  Example: 661500 Bytes = 0x000A17FC, byte[7]=0x00, byte[4]=0xFC */
    pHeader[4] = (uint8_t)(BufferCtl.fptr);
    pHeader[5] = (uint8_t)(BufferCtl.fptr >> 8);
    pHeader[6] = (uint8_t)(BufferCtl.fptr >> 16);
    pHeader[7] = (uint8_t)(BufferCtl.fptr >> 24);
    /* Write the number of sample data -----------------------------------------*/
    /* This variable will be written back at the end of the recording operation */
    BufferCtl.fptr -= 44;
    pHeader[40] = (uint8_t)(BufferCtl.fptr);
    pHeader[41] = (uint8_t)(BufferCtl.fptr >> 8);
    pHeader[42] = (uint8_t)(BufferCtl.fptr >> 16);
    pHeader[43] = (uint8_t)(BufferCtl.fptr >> 24);

    /* Return 0 if all operations are OK */
    return 0;
}

/**
 * @brief  Starts Audio streaming.
 * @param  None
 * @retval Audio error
 */
AUDIO_ErrorTypeDef Audio_Rec_Start(void)
{
    uint32_t byteswritten = 0;
    msg = "Start Writing Header...\n";
    HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), 0xFFF);
    if (f_open(&SDFile, REC_WAVE_NAME, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
    {
        msg = "File Open ERROR\n";
        HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), 0xFFF);
        Error_Handler();
    }
    else
    {
        WavProcess_EncInit(AUDIO_FREQ, pHeaderBuff); // store header into buffer

        fres = f_write(&SDFile, pHeaderBuff, HEADER_BUFFER_SIZE, (void *)&byteswritten); // write header file
        if ((byteswritten == 0) || (fres != FR_OK))
        {
            msg = "Write File ERROR\n";
            HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), 0xFFF);
            Error_Handler();
        }
        else
        {
            if (byteswritten != 0)
            {
                BSP_AUDIO_IN_InitEx(INPUT_DEVICE, AUDIO_FREQ, DEFAULT_AUDIO_IN_BIT_RESOLUTION, DEFAULT_AUDIO_IN_CHANNEL_NBR);
                // BSP_AUDIO_IN_SetVolume(100);
                // BSP_AUDIO_IN_OUT_Init(INPUT_DEVICE, OUTPUT_DEVICE_HEADPHONE1, AUDIO_FREQ, DEFAULT_AUDIO_IN_BIT_RESOLUTION, DEFAULT_AUDIO_IN_CHANNEL_NBR);
                BSP_AUDIO_IN_SetVolume(50);
                BSP_AUDIO_IN_Record((uint16_t *)&BufferCtl.pcm_buff[0], AUDIO_IN_PCM_BUFFER_SIZE);
                BufferCtl.fptr = byteswritten;
                BufferCtl.pcm_ptr = 0;
                BufferCtl.offset = 0;
                BufferCtl.wr_state = BUFFER_EMPTY;
                return AUDIO_ERROR_NONE;
            }
        }
    }
    return AUDIO_ERROR_IO;
}

/**
 * @brief  Manages Audio process.
 * @param  None
 * @retval Audio error
 */
AUDIO_ErrorTypeDef Audio_Rec_Process(void)
{
    uint32_t byteswritten = 0;

    /* Check if there are Data to write to USB Key */
    if (BufferCtl.wr_state == BUFFER_FULL)
    {
        msg = "Writing Buffer to File\n";
        HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), 0xFFF);

        /* write buffer in file */
        if (f_write(&SDFile, (uint8_t *)(BufferCtl.pcm_buff + BufferCtl.offset),
                    AUDIO_IN_PCM_BUFFER_SIZE,
                    (void *)&byteswritten) != FR_OK)
        {
            return AUDIO_ERROR_IO;
        }
        BufferCtl.fptr += byteswritten;
        BufferCtl.wr_state = BUFFER_EMPTY;
    }
    return AUDIO_ERROR_NONE;
}

/**
 * @brief  Calculates the remaining file size and new position of the pointer.
 * @param  None
 * @retval None
 */
void BSP_AUDIO_IN_TransferComplete_CallBack(void)
{
    BufferCtl.pcm_ptr += AUDIO_IN_PCM_BUFFER_SIZE / 2;
    if (BufferCtl.pcm_ptr == AUDIO_IN_PCM_BUFFER_SIZE / 2)
    {
        BufferCtl.wr_state = BUFFER_FULL;
        BufferCtl.offset = 0;
    }

    if (BufferCtl.pcm_ptr >= AUDIO_IN_PCM_BUFFER_SIZE)
    {
        BufferCtl.wr_state = BUFFER_FULL;
        BufferCtl.offset = AUDIO_IN_PCM_BUFFER_SIZE / 2;
        BufferCtl.pcm_ptr = 0;
    }

    msg = "Transfer Complete\n";
    HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
}

/**
 * @brief  Manages the DMA Half Transfer complete interrupt.
 * @param  None
 * @retval None
 */
void BSP_AUDIO_IN_HalfTransfer_CallBack(void)
{
    BufferCtl.pcm_ptr += AUDIO_IN_PCM_BUFFER_SIZE / 2;
    if (BufferCtl.pcm_ptr == AUDIO_IN_PCM_BUFFER_SIZE / 2)
    {
        BufferCtl.wr_state = BUFFER_FULL;
        BufferCtl.offset = 0;
    }

    if (BufferCtl.pcm_ptr >= AUDIO_IN_PCM_BUFFER_SIZE)
    {
        BufferCtl.wr_state = BUFFER_FULL;
        BufferCtl.offset = AUDIO_IN_PCM_BUFFER_SIZE / 2;
        BufferCtl.pcm_ptr = 0;
    }

    msg = "Half Transfer\n";
    HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
}