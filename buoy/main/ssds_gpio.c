#include "ssds_global.h"

#define GPIO_TAG "GPIOW"

static const adc_atten_t atten = ADC_ATTEN_DB_0;
// static const adc_unit_t unit = ADC_UNIT_1;
//static esp_adc_cal_characteristics_t *adc_chars;

static uint8_t adc_configured = 0;


esp_err_t init_gpio()
{
	if (adc_configured){
		ESP_LOGW(GPIO_TAG, "ADC already configured");
		return ESP_FAIL;
	}
	adc1_config_width(SSDS_ADC_WIDTH);
  adc1_config_channel_atten(TEMP1_CHANNEL, atten);
  adc1_config_channel_atten(TEMP2_CHANNEL, atten);
  adc1_config_channel_atten(TEMP3_CHANNEL, atten);
  adc1_config_channel_atten(SALINITY_CHANNEL, atten);
  adc1_config_channel_atten(LIGHT_CHANNEL, atten);
  adc1_config_channel_atten(TURBD_CHANNEL, atten);

  //adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
  //esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, SSDS_ADC_WIDTH, DEFAULT_VREF, adc_chars);

  adc_configured = 1;

	return ESP_OK;
}

esp_err_t gather_data(datapoint_t* dp)
{
	uint32_t temp1 = 0;
	uint32_t temp2 = 0;
	uint32_t temp3 = 0;
	uint32_t salin = 0;
	uint32_t light = 0;
	uint32_t turbd = 0;

	for (int i = 0; i < NO_OF_SAMPLES; i++) {
	    temp1 += adc1_get_raw((adc1_channel_t)TEMP1_CHANNEL);
	    temp2 += adc1_get_raw((adc1_channel_t)TEMP2_CHANNEL);
	    temp3 += adc1_get_raw((adc1_channel_t)TEMP3_CHANNEL);
	    salin += adc1_get_raw((adc1_channel_t)SALINITY_CHANNEL);
	    light += adc1_get_raw((adc1_channel_t)LIGHT_CHANNEL);
	    turbd += adc1_get_raw((adc1_channel_t)TURBD_CHANNEL);
	}


	temp1 /= NO_OF_SAMPLES;
	temp2 /= NO_OF_SAMPLES;
	temp3 /= NO_OF_SAMPLES;
	salin /= NO_OF_SAMPLES;
	light /= NO_OF_SAMPLES;
	turbd /= NO_OF_SAMPLES;

	dp->temp1 = temp1;
	dp->temp2 = temp2;
	dp->temp3 = temp3;
	dp->salinity = salin;
	dp->light = light;
	dp->turbidity = turbd;

	ESP_LOGI(GPIO_TAG, "temp1: %d", temp1);
	ESP_LOGI(GPIO_TAG, "temp2: %d", temp2);
	ESP_LOGI(GPIO_TAG, "temp3: %d", temp3);
	ESP_LOGI(GPIO_TAG, "Salinity: %d", salin);
	ESP_LOGI(GPIO_TAG, "Light: %d", light);
	ESP_LOGI(GPIO_TAG, "Turbidity: %d", turbd);

	struct timeval* tv = malloc(sizeof(struct timeval));

	gettimeofday(tv, NULL);
	dp->tv_sec = tv->tv_sec;

	free(tv);


	return ESP_OK;
}