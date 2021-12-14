#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "esp_err.h"

//All ESP32s run their ADC at 1.1V by default, but this can vary by chip
#define DEFAULT_VREF    1100

#define NO_OF_SAMPLES   64          			//Multisampling
#define SSDS_ADC_WIDTH	ADC_WIDTH_BIT_12 

#define TEMP1_CHANNEL 		ADC_CHANNEL_0
#define TEMP2_CHANNEL 		ADC_CHANNEL_1
#define TEMP3_CHANNEL 		ADC_CHANNEL_2
#define SALINITY_CHANNEL 	ADC_CHANNEL_3
#define LIGHT_CHANNEL 		ADC_CHANNEL_4
#define TURBD_CHANNEL			ADC_CHANNEL_5



esp_err_t init_gpio();

esp_err_t gather_data(datapoint_t* dp);
