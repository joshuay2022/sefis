#ifndef _SHA_H_
#define _SHA_H_

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * @brief Size of the SHA-256 sum. This times eight is 256 bits.
 */
#define SIZE_OF_SHA_256_HASH 32

/*
 * @brief Size of the chunks used for the calculations.
 *
 * @note This should mostly be ignored by the user, although when using the streaming API, it has an impact for
 * performance. Add chunks whose size is a multiple of this, and you will avoid a lot of superfluous copying in RAM!
 */
#define SIZE_OF_SHA_256_CHUNK 64

/*
 * @brief The opaque SHA-256 type, that should be instantiated when using the streaming API.
 *
 * @note Although the details are exposed here, in order to make instantiation easy, you should refrain from directly
 * accessing the fields, as they may change in the future.
 */
struct Sha_256 {
	uint8_t *hash;
	uint8_t chunk[SIZE_OF_SHA_256_CHUNK];
	uint8_t *chunk_pos;
	size_t space_left;
	size_t total_len;
	uint32_t h[8];
};

/*
 * @brief The simple SHA-256 calculation function.
 * @param hash Hash array, where the result is delivered.
 * @param input Pointer to the data the hash shall be calculated on.
 * @param len Length of the input data, in byte.
 *
 * @note If all of the data you are calculating the hash value on is available in a contiguous buffer in memory, this is
 * the function you should use.
 *
 * @note If either of the passed pointers is NULL, the results are unpredictable.
 */
void calc_sha_256(uint8_t hash[SIZE_OF_SHA_256_HASH], const void *input, size_t len);

/*
 * @brief Initialize a SHA-256 streaming calculation.
 * @param sha_256 A pointer to a SHA-256 structure.
 * @param hash Hash array, where the result will be delivered.
 *
 * @note If all of the data you are calculating the hash value on is not available in a contiguous buffer in memory, this is
 * where you should start. Instantiate a SHA-256 structure, for instance by simply declaring it locally, make your hash
 * buffer available, and invoke this function. Once a SHA-256 hash has been calculated (see further below) a SHA-256
 * structure can be initialized again for the next calculation.
 *
 * @note If either of the passed pointers is NULL, the results are unpredictable.
 */
void sha_256_init(struct Sha_256 *sha_256, uint8_t hash[SIZE_OF_SHA_256_HASH]);

/*
 * @brief Stream more input data for an on-going SHA-256 calculation.
 * @param sha_256 A pointer to a previously initialized SHA-256 structure.
 * @param data Pointer to the data to be added to the calculation.
 * @param len Length of the data to add, in byte.
 *
 * @note This function may be invoked an arbitrary number of times between initialization and closing, but the maximum
 * data length is limited by the SHA-256 algorithm: the total number of bits (i.e. the total number of bytes times
 * eight) must be representable by a 64-bit unsigned integer. While that is not a practical limitation, the results are
 * unpredictable if that limit is exceeded.
 *
 * @note This function may be invoked on empty data (zero length), although that obviously will not add any data.
 *
 * @note If either of the passed pointers is NULL, the results are unpredictable.
 */
void sha_256_write(struct Sha_256 *sha_256, const void *data, size_t len);

/*
 * @brief Conclude a SHA-256 streaming calculation, making the hash value available.
 * @param sha_256 A pointer to a previously initialized SHA-256 structure.
 * @return Pointer to the hash array, where the result is delivered.
 *
 * @note After this function has been invoked, the result is available in the hash buffer that initially was provided. A
 * pointer to the hash value is returned for convenience, but you should feel free to ignore it: it is simply a pointer
 * to the first byte of your initially provided hash array.
 *
 * @note If the passed pointer is NULL, the results are unpredictable.
 *
 * @note Invoking this function for a calculation with no data (the writing function has never been invoked, or it only
 * has been invoked with empty data) is legal. It will calculate the SHA-256 value of the empty string.
 */
uint8_t *sha_256_close(struct Sha_256 *sha_256);

/*
 * @brief The function used for block-sized inner padding
 * @param out A pointer to an output buffer 
 * @param size The size of output buffer
 */
void repeat_ipad(uint8_t *out,const size_t &size);

/*
 * @brief The function used for block-sized outer padding
 * @param out A pointer to an output buffer 
 * @param size The size of output buffer
 */
void repeat_opad(uint8_t *out,const size_t &size);

/*
 * @brief The function used for buffer xor
 * @param x1 A pointer to the first buffer 
 * @param x2 A pointer to the second buffer
 * @param out A pointer to the output buffer
 * @param size The size of the buffer 
 */
void buffer_xor(const uint8_t *x1,const uint8_t *x2,uint8_t *out,const size_t &size);

/*
 * @brief The simple HMAC caculation function
 * @param key A pointer to a key buffer
 * @param key_size Key size
 * @param secret A pointer to a secret buffer
 * @param secret_size Secret size
 * @param out A pointer to an output buffer 
 */
void hmac_sha256(const uint8_t* key,const size_t &key_size,const uint8_t* secret,const size_t &secret_size,uint8_t out[SIZE_OF_SHA_256_HASH]);

/*
 * @brief The function to generate salt
 * @param salt A pointer to an output salt
 * @param size salt size
 */
void generate_salt(uint8_t* salt,const size_t &size);

/*
 * @brief The simple PBKDF2 caculation function
 * @param in The pointer to input buffer
 * @param inSize Input size
 * @param salt A pointer to salt buffer
 * @param saltSize Salt size
 * @param iter Iteration times
 * @param out A pointer to output buffer
 */
int pbkdf2(const uint8_t* in,const size_t &inSize,const uint8_t* salt,const size_t &saltSize,const size_t &iter,uint8_t *out);

/*
 * @brief The simple password verifying function
 * @param usr_password user password
 * @param usr_size user password size
 * @param iter Iteration number
 * @param hashed_password hashed password
 */
bool verify_password(char* usr_password, size_t usr_size, size_t iter,uint8_t* hashed_password);

#ifdef __cplusplus
}
#endif

#endif
