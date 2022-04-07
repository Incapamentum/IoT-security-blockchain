// Author: Gustavo Diaz Galeas (Incapamentum)
//
// Last revision: April 7th, 2022

#include <algorithm>
#include <Ed25519.h>
#include <SHA256.h>
#include <SHA512.h>
#include <string.h>

#include "Transactions.h"

// On invocation, sets the server public key to the passed
// key param
Transaction::Transaction(uint8_t key[KEY_LENGTH])
{
    std::copy(key, key + KEY_LENGTH, serverKey_);
}

// Sets temperature and humidity data
void Transaction::setData(uint8_t temp, uint8_t hum)
{
    data_.temperature = temp;
    data_.humidity = hum;
}

// Sets the timestamp_ variable to the value of ts, which is the result
// of hashing the timestamp at the time of generation with the ID of
// the chip
void Transaction::setTimestamp(uint8_t ts[SHA256_SIZE])
{
    std::copy(ts, ts + SHA256_SIZE, timestamp_);
}

// Hashes the timestamp_, serverKey_, and data_ of the block
void Transaction::hashTransaction(unsigned int chipId)
{
    SHA512 h;
    unsigned int ptr = 0;
    unsigned char d_buffer[transaction_size];

    // Packing data to the buffer
    memcpy(d_buffer + ptr, timestamp_, SHA256_SIZE);
    ptr += SHA256_SIZE;
    memcpy(d_buffer + ptr, serverKey_, KEY_LENGTH);
    ptr += KEY_LENGTH;
    memcpy(d_buffer + ptr, &data_, DATA_LENGTH);

    // Setting data to be hashed
    h.update(d_buffer, transaction_size);

    // Hashing
    h.finalize(transactionHash_, SHA512_SIZE);
}

// Signs the hash of the Transaction with the device private key
int Transaction::sign(uint8_t deviceKey[KEY_LENGTH])
{
    Ed25519::sign(signature_, deviceKey, serverKey_, transactionHash_, SHA512_SIZE);
    return 0;
}