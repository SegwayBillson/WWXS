#ifndef __SSDS_BLUETOOTH__
#define __SSDS_BLUETOOTH__

#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "esp_bt_device.h"



/* Function definitions */


/* Intializes the bluetooth controller for use in the SSDS station.
 */
void ssds_ble_init();

/* Deinitialize the bluetooth to prepare for deep sleep
 */
void ssds_ble_deinit();

/* When the command characteristic is written to with a valid command,
 * the function cb will be called with the command as an argument
 */
void ble_register_command_callback(esp_err_t (*cb)(uint8_t));


void ble_register_data_callback(esp_err_t (*cb)(uint8_t*, int));

void ble_register_connect_callback(esp_err_t (*cb)());

void ble_register_disconnect_callback(esp_err_t (*cb)());

void send_indicate(uint8_t flag);



/* Global variables */

/* Used to detect when the bluetooth controller is fully ready
 * Can begin to use BLE when ssds_ble_status == SSDS_BLE_READY
 */
extern uint8_t ssds_ble_status;
#define ctrl_char_ready         (1 << 0)
#define data_char_ready         (1 << 1)

#define SSDS_BLE_READY (ctrl_char_ready | data_char_ready)


/* Global defines */

#define BT_TAG "BLULE"

#define PREPARE_BUF_MAX_SIZE 1024

#define SSDS_APP_ID 0
#define GATTS_HANDLE_NUM 8

#define GATT_CTRL_CHAR_UUID 0xABC0
#define GATT_DATA_CHAR_UUID 0xABC1

#define GATT_CTRL_CHAR_DESC_UUID 0xCBA0
#define GATT_DATA_CHAR_DESC_UUID 0xCBA1

#define DEVICE_NAME "SSDS Test"

#define GATTS_CHAR_VAL_LEN_MAX 0x40




/* Global structures */

typedef struct {
    uint8_t                 *prepare_buf;
    int                     prepare_len;
} prepare_type_env_t;

struct gatts_profile {
    uint16_t gatts_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_handle;
    esp_gatt_srvc_id_t service_id;
    uint16_t ctrl_char_handle;
    esp_bt_uuid_t ctrl_char_uuid;
    uint16_t data_char_handle;
    esp_bt_uuid_t data_char_uuid;
    uint16_t ctrl_descr_handle;
    esp_bt_uuid_t ctrl_descr_uuid;
    uint16_t data_descr_handle;
    esp_bt_uuid_t data_descr_uuid;
};


#endif