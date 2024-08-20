#include "stm32746g_discovery_audio.h"
#include "fatfs.h"
#include "sdmmc.h"
#include "usart.h"

#define AUDIO_IN_PCM_BUFFER_SIZE 4 * 2304 /* buffer size in half-word */
#define AUDIO_OUT_BUFFER_SIZE 4096
#define AUDIO_FREQ SAI_AUDIO_FREQUENCY_48K
#define MAX_REC_TIME 10
#define REC_SAMPLE_LENGTH (MAX_REC_TIME * AUDIO_FREQ * DEFAULT_AUDIO_IN_CHANNEL_NBR * 2)
#define INPUT_DEVICE INPUT_DEVICE_INPUT_LINE_1
#define OUTPUT_DEVICE OUTPUT_DEVICE_HEADPHONE
#define REC_WAVE_NAME "TEST4.WAV"

typedef enum
{
    AUDIO_ERROR_NONE = 0,
    AUDIO_ERROR_IO,
    AUDIO_ERROR_EOF,
    AUDIO_ERROR_INVALID_VALUE,
} AUDIO_ErrorTypeDef;

typedef enum
{
    BUFFER_EMPTY = 0,
    BUFFER_HALF,
    BUFFER_FULL,
} WR_BUFFER_StateTypeDef;

typedef struct
{
    uint16_t pcm_buff[AUDIO_IN_PCM_BUFFER_SIZE];
    uint32_t pcm_ptr;
    WR_BUFFER_StateTypeDef wr_state;
    uint32_t offset;
    uint32_t fptr;
} AUDIO_IN_BufferTypeDef;

typedef struct
{
    uint8_t buff[AUDIO_OUT_BUFFER_SIZE * 2];
    uint16_t prev_buff[AUDIO_OUT_BUFFER_SIZE];
    WR_BUFFER_StateTypeDef state;
    uint32_t fptr;
} AUDIO_OUT_BufferTypeDef;

typedef struct
{
    uint32_t ChunkID;       /* 0 */
    uint32_t FileSize;      /* 4 */
    uint32_t FileFormat;    /* 8 */
    uint32_t SubChunk1ID;   /* 12 */
    uint32_t SubChunk1Size; /* 16*/
    uint16_t AudioFormat;   /* 20 */
    uint16_t NbrChannels;   /* 22 */
    uint32_t SampleRate;    /* 24 */

    uint32_t ByteRate;      /* 28 */
    uint16_t BlockAlign;    /* 32 */
    uint16_t BitPerSample;  /* 34 */
    uint32_t SubChunk2ID;   /* 36 */
    uint32_t SubChunk2Size; /* 40 */
} WAVE_FormatTypeDef;

uint32_t WavProcess_EncInit(uint32_t Freq, uint8_t *pHeader);
uint32_t WavProcess_HeaderInit(uint8_t *pHeader, WAVE_FormatTypeDef *pWaveFormatStruct);
uint32_t WavProcess_HeaderUpdate(uint8_t *pHeader, WAVE_FormatTypeDef *pWaveFormatStruct);

AUDIO_ErrorTypeDef GetFileInfo(char *file_name, WAVE_FormatTypeDef *info);

AUDIO_ErrorTypeDef Audio_Rec_Start(void);
AUDIO_ErrorTypeDef Audio_Rec_Process(void);

AUDIO_ErrorTypeDef AUDIO_PLAYER_Start(void);
AUDIO_ErrorTypeDef AUDIO_PLAYER_Process(void);
AUDIO_ErrorTypeDef AUDIO_PLAYER_Stop(void);