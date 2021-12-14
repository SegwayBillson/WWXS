
#include "ssds_global.h"

#define STACK_SIZE 2048
StaticTask_t timerTaskBuffer;
StackType_t timerStack[ STACK_SIZE ];
static TaskHandle_t timerTaskHandle = NULL;

static uint8_t timer_active;


#define GPIO_OUTPUT_IO_0    5
#define GPIO_OUTPUT_PIN_SEL  (1ULL<<GPIO_OUTPUT_IO_0)


void timer_isr(void *arg)
{
	TaskHandle_t handle = (TaskHandle_t)arg;
	TIMERG0.int_clr_timers.t0 = 1;
	TIMERG0.hw_timer[0].config.alarm_en = 1;

	gpio_set_level(GPIO_OUTPUT_IO_0, 1);
	if (xTaskResumeFromISR(handle) == pdTRUE){
		portYIELD_FROM_ISR();
	}
}

void data_collection_worker(void *pvParam)
{
	ESP_LOGI(TIMER, "Data collection worker created");
	datapoint_t temp_dp;
	while(1)
	{
		/* Resets the watchdog so we don't get timed out */
		esp_task_wdt_reset();

		ESP_LOGI(TIMER, "Data collection suspending");
		fflush(stdout);
		vTaskSuspend(NULL);

		gpio_set_level(GPIO_OUTPUT_IO_0, 0);
		gather_data(&temp_dp);
		write_dp(&temp_dp);
	}
}


void check_if_day()
{
	/*

	struct timeval tmp_tv;
	tmp_tv.tv_sec = 1555885298;

	settimeofday(&tmp_tv, NULL);
	ESP_LOGI(TIMER, "New time: %lu", tmp_tv.tv_sec);
	*/

	time_t t = time(NULL);
	struct tm lt = {0};

	localtime_r(&t, &lt);
	int local_hour = lt.tm_hour - 7;

	if (local_hour >= 7 && local_hour <= 21){
		ESP_LOGI(TIMER, "Daytime - hour %d", local_hour);
	}
	else{
		ESP_LOGI(TIMER, "Nighttime - hour %d", local_hour);
	}
}

void pause_thread_timer()
{
	if (timer_active) {
		ESP_LOGI(TIMER, "Thread timer paused");
		timer_pause(TIMER_GROUP_0, TIMER_0);
		timer_active = 0;
	}
	else {
		ESP_LOGW(TIMER, "Timer not active");
	}
}

void resume_thread_timer()
{
	if (!timer_active) {

		/* Checking that the time has been initalized */
		struct timeval* tv = malloc(sizeof(struct timeval));
		gettimeofday(tv, NULL);

		if (tv->tv_sec < RTC_THRESHOLD)
		{
			ESP_LOGW(TIMER,"RTC has not been initalized, but continuing anyways");
			ESP_LOGW(TIMER,"RTC given time as %lu", tv->tv_sec);
		}

		free(tv);

		ESP_LOGI(TIMER, "Thread timer resumed");
		timer_start(TIMER_GROUP_0, TIMER_0);
		timer_active = 1;

		set_status(STAT_DAY);
		check_if_day();
	}
	else {
		ESP_LOGW(TIMER, "Timer not paused");
	}
}


void init_thread_timer()
{
	gpio_config_t io_conf;
	//disable interrupt
	io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
	//set as output mode
	io_conf.mode = GPIO_MODE_OUTPUT;
	//bit mask of the pins that you want to set,e.g.GPIO18/19
	io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
	//disable pull-down mode
	io_conf.pull_down_en = 0;
	//disable pull-up mode
	io_conf.pull_up_en = 0;
	//configure GPIO with the given settings
	gpio_config(&io_conf);

	/* Initializing data collection timer */
	
	timer_config_t config = {
		.alarm_en = TIMER_ALARM_EN,
		.counter_en = TIMER_PAUSE,
		.intr_type = TIMER_INTR_LEVEL,
		.counter_dir = TIMER_COUNT_UP,
		.auto_reload = true,
		.divider = TIMER_DIVIDER
	};

	gpio_set_level(GPIO_OUTPUT_IO_0, 0);

	vTaskPrioritySet(NULL, 1);

	timerTaskHandle = xTaskCreateStatic(
		data_collection_worker,
		"COLLECTOR",
		STACK_SIZE,
		( void * ) 1,
		2,
		timerStack,
		&timerTaskBuffer );
	
	ESP_ERROR_CHECK(timer_init(TIMER_GROUP_0, TIMER_0, &config));
	ESP_LOGI(TIMER, "Timer init");

	ESP_ERROR_CHECK(timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0));
	ESP_LOGI(TIMER, "Timer counter set");

	ESP_ERROR_CHECK(timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, TIMER_INTERVAL * TIMER_SCALE));
	ESP_LOGI(TIMER, "Timer alarm set");

	ESP_ERROR_CHECK(timer_enable_intr(TIMER_GROUP_0, TIMER_0));
	ESP_LOGI(TIMER, "Timer interrupt enabled");

	ESP_ERROR_CHECK(timer_isr_register(TIMER_GROUP_0, TIMER_0, &timer_isr,
		(void *)timerTaskHandle, 0, NULL));
	ESP_LOGI(TIMER, "Timer ISR registered");
        
        // Pausing, since we don't have the correct time until the app connects
        pause_thread_timer();
}