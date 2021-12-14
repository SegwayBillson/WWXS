/* Global declarations for use across the
 * SSDS software
 */

#ifdef MAIN
	#define EXTERN
	#define INIT(val) = val
#else
	#define EXTERN extern
	#define INIT(val)
#endif

#ifndef __SSDS_GLOBAL
#define __SSDS_GLOBAL

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/ringbuf.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_err.h"
#include "driver/timer.h"
#include "esp_task_wdt.h"
#include "esp32/clk.h"
#include "ssds_storage.h"
#include "ssds_bluetooth.h"
#include "ssds_encryption.h"
#include "ssds_gpio.h"
#include "ssds_timer.h"

#endif

/* System status */
#define STAT_READ_READY 0
#define STAT_IDLE 			1
#define STAT_DAY 				2
#define STAT_NIGHT 			3
#define STAT_PROCESS 		4

/* This value is used for quick checks during the day
 * when memory is kept powered on
 * When in doubt, default to using the value stored in NVS
 */
EXTERN uint8_t current_status INIT(255);


/* Command values */
#define RECIEVE_RSA         0x01
#define BEGIN_RECORDING     0x02
#define END_RECORDING       0x03

/* BLE Read states */
#define DP_COUNT 			10
#define KEY_READY			9
#define IV_READY 			8
#define DATA_READY		7
#define READ_NONE			0


/* BLE */
EXTERN uint8_t read_type_flag INIT(0);
EXTERN uint8_t ssds_ble_status INIT(0);

/* AES */
#define ENCRYPTED_KEY_LEN 256

EXTERN unsigned char aes_key[32];
EXTERN unsigned char aes_iv[16];
EXTERN uint8_t encrypted_key[ENCRYPTED_KEY_LEN];


/* Ring buffers */
#define PARAM_BUF_SIZE 1025

RingbufHandle_t data_ring;


#ifndef configASSERT
	#define configASSERT( x )
	#define configASSERT_DEFINED 0
#else
	#define configASSERT_DEFINED 1
#endif
