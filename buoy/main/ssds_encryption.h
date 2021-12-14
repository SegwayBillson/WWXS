#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_err.h"
#include "mbedtls/aes.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/pk.h"

#define ENC_TAG "ENCRY"

/* Storing initial IV for use in decryption testing */
uint8_t decrypt_iv[16];

/* Temporary workaround for the app, keeping the IV constant over all encryption */
uint8_t const_iv[16];

esp_err_t init_encryption(unsigned char* rsa_pub, uint16_t key_len);
esp_err_t end_encryption();
esp_err_t encrypt_block(unsigned char* input, unsigned char* output, uint16_t buf_len);
esp_err_t decrypt_block(unsigned char* input, unsigned char* output, uint16_t buf_len);


/* Return an encrypted version of the AES key, using the public key
 * provided in init_encryption
 * Requires a 256 byte input array to store the result of this
 * operation in
 */
esp_err_t encrypt_aes ();
