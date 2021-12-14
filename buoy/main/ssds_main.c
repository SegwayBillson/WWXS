/* SSDS Main Driver
 *  This code contains the main driver 
 */

#define MAIN
#include "ssds_global.h"

#include "esp_sleep.h"

#define SSDS_TAG        "WXMAI"

/* Command values */
static uint8_t active_command = 0;
static uint8_t* pubkey;


esp_err_t command_callback(uint8_t cmd)
{
    switch(cmd)
    {
        case BEGIN_RECORDING:
            ESP_LOGI(SSDS_TAG, "Begin recording command recieved");
            active_command = BEGIN_RECORDING;
            break;
        case END_RECORDING:
            ESP_LOGI(SSDS_TAG, "End recording command recieved");
            active_command = END_RECORDING;
            break;
        case RECIEVE_RSA:
            ESP_LOGI(SSDS_TAG, "Recieve RSA command recieved");
            active_command = RECIEVE_RSA;
            break;
        default:
            ESP_LOGW(SSDS_TAG, "Unknown command recieved - %02x", cmd);
            break;
    }

    return ESP_OK;
}

esp_err_t data_callback(uint8_t* data, int size)
{
    ESP_LOGI(SSDS_TAG, "Data callback write size: %d", size);
    switch(active_command)
    {
        // Waiting to recieve current unix time
        case BEGIN_RECORDING:
        {
            if(size > 8)
            {
                ESP_LOGE(SSDS_TAG, "Recieved message of unknown length");
                break;
            }

            struct timeval tv;

            tv.tv_sec = (time_t)data[0] | 
                        (time_t)data[1] << 8 | 
                        (time_t)data[2] << 16 | 
                        (time_t)data[3] << 24;

            ESP_LOGI(SSDS_TAG, "Updated time: %lu", tv.tv_sec);

            if (settimeofday(&tv, NULL) < 0)
            {
                ESP_LOGE(SSDS_TAG, "Time failed to update");
            }

            active_command = 0;
            break;
        }

        // A temporary use of this command to dump all existing datapoints
        case END_RECORDING:
        {
            dp_iterator_t* iterator = get_dp_iterator(0, 0);

            datapoint_t* curr = malloc(sizeof(datapoint_t));

            get_next_dp(iterator, curr);
            while (curr->tv_sec != 0)
            {
                print_datapoint(curr);
                get_next_dp(iterator, curr);
            }

            close_dp_iterator(iterator);
            break;
        }


        // Waiting to recieve a public key
        case RECIEVE_RSA:
        {
            ESP_LOGI(SSDS_TAG, "Recieved RSA public key");
            pubkey = malloc(size);
            memcpy(pubkey, data, size);

            esp_err_t ret = init_encryption(pubkey, size);

            // Generate / start encryption thread

            free(pubkey);

            if (ret != ESP_OK){
                ESP_LOGE(SSDS_TAG, "Encryption error, not sending key back");
            }
            else{
                send_indicate(KEY_READY);
            }

            active_command = 0;

            break;
        }
        default:
        {
            ESP_LOGW(SSDS_TAG, "No active command during write callback");
            break;
        }
    }
    return ESP_OK;
}

esp_err_t connect_callback()
{
    ESP_LOGI(SSDS_TAG, "BLE connect callback");
    pause_thread_timer();
    read_type_flag = READ_NONE;
    return ESP_OK;
}

esp_err_t disconnect_callback()
{
    ESP_LOGI(SSDS_TAG, "BLE disconnect callback");
    end_encryption();
    resume_thread_timer();
    return ESP_OK;
}


void app_main()
{
    ESP_LOGI(SSDS_TAG, "Chip powered up");

    esp_err_t ret = init_storage();
    if (ret != ESP_OK){
        ESP_LOGE(SSDS_TAG, "Storage failed to initialize");
    }


    /* Determining why we woke up */
    esp_sleep_wakeup_cause_t wakeup = esp_sleep_get_wakeup_cause();

    if (wakeup == ESP_SLEEP_WAKEUP_UNDEFINED)
    {
        uint8_t temp_status = get_status();
        ESP_LOGI(SSDS_TAG, "Unknown wakeup source - first boot?");
        ESP_LOGI(SSDS_TAG, "Wakeup status: %u", temp_status);

        /* Most likely a first power on, so we can clear storage */
        reset_storage();

        /* If the board was booted without a wakeup cause, we can assume its idle */
        set_status(STAT_IDLE);
        current_status = STAT_IDLE;
    }
    else
    {
        ESP_LOGI(SSDS_TAG, "Wakeup cause %d", wakeup);
        current_status = get_status();
    }    
    
    ssds_ble_init();

    while (ssds_ble_status != SSDS_BLE_READY);

    ble_register_command_callback(&command_callback);
    ble_register_data_callback(&data_callback);
    ble_register_connect_callback(&connect_callback);
    ble_register_disconnect_callback(&disconnect_callback);
    
    /*
    const uint8_t* point = esp_bt_dev_get_address();
    
    for (int i = 0; i < 6; i++) {
      
      char str[3];
      
      sprintf(str, "%02X", (int)point[i]);
      ESP_LOGI(SSDS_TAG, "%s", str);
      
      if (i < 5){
        ESP_LOGI(SSDS_TAG, "%s", ":");
      }
    }
    */

    ESP_LOGI(SSDS_TAG, "BLE ready to use");
    
    init_gpio();
    
    init_thread_timer();
    
    ESP_LOGI(SSDS_TAG, "Ending main task");
    fflush(stdout);
    vTaskDelete(NULL);
}