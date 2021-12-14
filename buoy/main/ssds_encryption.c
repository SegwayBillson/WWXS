#include "ssds_global.h"


/* Encryption thread variables */

#define STACK_SIZE 2048
StaticTask_t encryptTaskBuffer;
StackType_t encryptStack[ STACK_SIZE ];
static TaskHandle_t encryptTaskHandle;


/* Function for the datapoint encryption thread */
void encryption_worker( void* params ) {
  
  dp_iterator_t* data_iterator;
  datapoint_t curr_dp;
  BaseType_t buf_ret;
  esp_err_t enc_ret;
  uint8_t enc_buf[16];
  
  ESP_LOGI(ENC_TAG, "Encryption worker initialized");
  
  /* Constant thread loop */
  while(1){
    
    data_iterator = get_dp_iterator(0,0);
    
    /* Putting the datapoint count into the data buffer */
    buf_ret = xRingbufferSend(data_ring, 
                              &(data_iterator->dp_count), 
                              sizeof(data_iterator->dp_count), 
                              10);
    if (buf_ret == pdFALSE){
      ESP_LOGE(ENC_TAG, "Couldn't write encrypted key to ring buffer");
    }
    
    get_next_dp (data_iterator, &curr_dp);
    
    /* Looping over all datapoints */
    while (curr_dp.tv_sec != 0){
      
      enc_ret = encrypt_block((unsigned char*)&curr_dp, enc_buf, 16);
      if (enc_ret == ESP_FAIL){
        ESP_LOGE(ENC_TAG, "Encryption worker failed to encrypt datapoint");
      }
      
      buf_ret = pdFALSE;
      
      while (buf_ret == pdFALSE){
        
        /* Resets the watchdog so we don't get timed out */
        esp_task_wdt_reset();
        
        /* Putting the datapoint count into the data buffer */
        buf_ret = xRingbufferSend(data_ring, 
                                  &enc_buf, 
                                  16, 
                                  0);
        
        /* Delay 50ms after a failed buffer write */
        if (buf_ret == pdFALSE){
          ESP_LOGW(ENC_TAG, "Encryption buffer full");
          vTaskDelay(200 / portTICK_PERIOD_MS);
        }
        else {
          ESP_LOGI(ENC_TAG, "Encryption worker inserted datapoint");
        }
        
      }
      
      get_next_dp (data_iterator, &curr_dp);
    }
    
    close_dp_iterator(data_iterator);
    
    /* Pause, probably until deletion but sitting idle unless it
     * is called again for some reason
     */
    ESP_LOGI(ENC_TAG, "Encryption worker suspending");
    vTaskSuspend(NULL);
  }
}








/* Encryption engine variables */
static mbedtls_ctr_drbg_context ctr_drbg;
static mbedtls_entropy_context entropy;

/* AES */
static esp_aes_context ctx;
static char *seed_str = "SSDS floating weather station!";

// Initialized as a copy of the initial IV, but modified as encryption occurs.
static unsigned char aes_curr_iv[16];

/* RSA */
static mbedtls_pk_context pk;

static uint8_t encryption_active = 0;



esp_err_t encrypt_block(unsigned char* input, unsigned char* output, uint16_t buf_len)
{	
  int encrypt_length = 16;
  uint8_t iv[16];
  memcpy(iv, const_iv, 16);
  
  /* This is a temporary workaround for the IV apparently not updating on the app, eventually remove please */
  //int ret = esp_aes_crypt_cbc ( &ctx, ESP_AES_ENCRYPT, encrypt_length, aes_curr_iv, input, output );
  int ret = esp_aes_crypt_cbc ( &ctx, ESP_AES_ENCRYPT, encrypt_length, iv, input, output );
  
  if (ret){
    ESP_LOGE(ENC_TAG,"AES Encryption error");
    return ESP_FAIL;
  }
  else{
    return ESP_OK;
  }
}

esp_err_t decrypt_block(unsigned char* input, unsigned char* output, uint16_t buf_len)
{
  uint16_t encrypt_length = buf_len + (16 - (buf_len%16));
  encrypt_length = 16;
  uint8_t iv[16];
  memcpy(iv, const_iv, 16);
  
  //int ret = esp_aes_crypt_cbc ( &ctx, ESP_AES_DECRYPT, encrypt_length, decrypt_iv, input, output );
  int ret = esp_aes_crypt_cbc ( &ctx, ESP_AES_DECRYPT, encrypt_length, iv, input, output );
  
  
  if (ret){
    ESP_LOGE(ENC_TAG,"AES Decryption error");
    return ESP_FAIL;
  }
  else{
    return ESP_OK;
  }
}


esp_err_t init_encryption(unsigned char* rsa_pub, uint16_t key_len)
{
  ESP_LOGE(ENC_TAG, "Encryption initializing");
  int ret = 0;
  size_t olen = 0;
  
  if (encryption_active) {
    ESP_LOGE(ENC_TAG, "Encryption double initialized");
    return ESP_FAIL;
  }
  
  /* Initializing random number generators */
  mbedtls_entropy_init( &entropy );
  mbedtls_ctr_drbg_init( &ctr_drbg );
  
  if( ( ret = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy,
    (unsigned char *) seed_str, strlen( seed_str ) ) ) != 0 )
  {
    ESP_LOGE(ENC_TAG, "mbedtls_ctr_drbg_init returned -0x%04x\n", -ret );
    return ESP_FAIL;
  }
  
  
  /* Generating the 32 byte AES key */
  if( ( ret = mbedtls_ctr_drbg_random( &ctr_drbg, aes_key, 32 ) ) != 0 )
  {
    ESP_LOGE(ENC_TAG, "mbedtls_ctr_drbg_random returned -0x%04x\n", -ret );
    return ESP_FAIL;
  }
  
  /* Generating the 16 byte IV */
  if( ( ret = mbedtls_ctr_drbg_random( &ctr_drbg, aes_iv, 16 ) ) != 0 )
  {
    ESP_LOGE(ENC_TAG, "mbedtls_ctr_drbg_random returned -0x%04x\n", -ret );
    return ESP_FAIL;
  }
  
  
  /* Copying the initial IV for use */
  memcpy(aes_curr_iv, aes_iv, 16);
  
  /* Copying the initial IV for use in decryption if we want later */
  memcpy(decrypt_iv, aes_iv, 16);
  
  memcpy(const_iv, aes_iv, 16);
  
  ESP_LOGE(ENC_TAG, "AES key and IV generated");
  
  esp_aes_init ( &ctx );
  
  esp_err_t err = esp_aes_setkey(&ctx, aes_key, (unsigned)256);
  
  ESP_ERROR_CHECK(err);
  
  /* Loading public key */
  mbedtls_pk_init (&pk);
  
  if( (ret = mbedtls_pk_parse_public_key ( &pk, rsa_pub, key_len )) != 0 )
  {
    ESP_LOGE(ENC_TAG, "mbedtls_pk_parse_public_keyfile returned -0x%04x\n", -ret );
    return ESP_FAIL;
  }
  
  /* Encrypting AES key */
  if( ( ret = mbedtls_pk_encrypt( &pk, (unsigned char*)aes_key, 32,
    encrypted_key, &olen, 256,
    mbedtls_ctr_drbg_random, &ctr_drbg ) ) != 0 )
  {
    ESP_LOGE(ENC_TAG, "mbedtls_pk_encrypt returned -0x%04x\n", -ret );
    return ESP_FAIL;
  }
  
  
  /* Putting encrypted key and IV into the parameter buffer */
  
  BaseType_t buf_ret;
  
  buf_ret = xRingbufferSend(data_ring, 
                            encrypted_key, 
                            ENCRYPTED_KEY_LEN, 
                            10);
  if (buf_ret == pdFALSE){
    ESP_LOGE(ENC_TAG, "Couldn't write encrypted key to ring buffer");
  }
  
  buf_ret = xRingbufferSend(data_ring, 
                            aes_iv, 
                            sizeof(aes_iv), 
                            10);
  if (buf_ret == pdFALSE){
    ESP_LOGE(ENC_TAG, "Couldn't write encrypted key to ring buffer");
  }
  
  // Generate encryption thread
  
  
  encryptTaskHandle = xTaskCreateStatic(
    encryption_worker,
    "ENCRYPTER",
    STACK_SIZE,
    ( void * ) 1,
                                        2,
                                        encryptStack,
                                        &encryptTaskBuffer );
  
  
  
  
  encryption_active = 1;
  
  return ESP_OK;
}


esp_err_t end_encryption()
{
  if (!encryption_active)
  {
    ESP_LOGW(ENC_TAG, "Encryption never initialized, cannot end");
    return ESP_FAIL;
  }
  
  esp_aes_free ( &ctx );
  
  mbedtls_ctr_drbg_free(&ctr_drbg);
  mbedtls_entropy_free(&entropy);
  mbedtls_pk_free(&pk);
  
  /* Clean up encryption task handle */
  if (encryptTaskHandle != NULL){
    vTaskDelete(encryptTaskHandle);
    ESP_LOGW(ENC_TAG, "Encryption thread deleted");
  }
  
  encryption_active = 0;
  
  return ESP_OK;
}

