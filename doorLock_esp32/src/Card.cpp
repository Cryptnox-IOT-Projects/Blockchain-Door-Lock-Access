#include <Arduino.h>

#include <uECC.h>
#include "AESLib.h"
#include <AES.h>
#include <Crypto.h>
#include <SHA512.h>
#include <SHA3.h>
#include <Card.h>
#include <Connection.h>
#include <Web3.h>

Crypto crypto(web3);
SHA512 sha512;
uint8_t macKey[32];
uint8_t secret[32];
uint8_t private1[32];
uint8_t public1[64];
bool success;
uint8_t ivKey[16];
AESLib aesLib;
AESLib aesLib_without_padding;
uint8_t aesKey[32];
uint8_t macValue[16];
uint8_t res_cardCert[255];
uint8_t cardCert[64];
uint8_t publicKey[64];
uint8_t cipherDecrypt[2 * INPUT_BUFFER_LIMIT] = {0};

const byte doorLock_gpio = 32;

static int RNG(uint8_t *dest, unsigned size)
{

    while (size)
    {
        uint8_t val = 0;
        for (unsigned i = 0; i < 8; ++i)
        {
            int init = analogRead(35);
            int count = 0;
            while (analogRead(35) == init)
            {
                ++count;
            }

            if (count == 0)
            {
                val = (val << 1) | (init & 0x01);
            }
            else
            {
                val = (val << 1) | (count & 0x01);
            }
        }
        *dest = val;
        ++dest;
        --size;
    }

    return 1;
}

Card::Card()
{
}
void init()
{

    uint8_t public1[64];

    Card().getKeysECC(res_cardCert, cardCert);

    aesLib.set_paddingmode((paddingMode)1);
    byte sampleData[] = {0XE, 0X79, 0X61, 0X6E, 0X6E, 0X61, 0X69, 0X6E, 0X67, 0X77, 0X69, 0X6E, 0X6E, 0X61, 0X69, 0X19, 0X79, 0X61, 0X6E, 0X6E, 0X61, 0X69, 0X6E, 0X67, 0X77, 0X69, 0X6E, 0X32, 0X31, 0X39, 0X34, 0X40, 0X67, 0X6D, 0X61, 0X69, 0X6C, 0X2E, 0X63, 0X6F, 0X6D, 0X30, 0X30, 0X30, 0X30, 0X30, 0X30, 0X30, 0X30, 0X30, 0X30, 0X30, 0X30, 0X30, 0X30, 0X30, 0X30, 0X30, 0X30, 0X30, 0X30, 0X30, 0X43, 0X72, 0X79, 0X70, 0X74, 0X6E, 0X6F, 0X78, 0X20, 0X42, 0X61, 0X73, 0X69, 0X63, 0X20, 0X43, 0X6F, 0X6D, 0X6D, 0X6F, 0X6E, 0X50, 0X61, 0X69, 0X72, 0X69, 0X6E, 0X67, 0X44, 0X61, 0X74, 0X61};
    uint16_t msgLen = sizeof(sampleData);
    int encryptedLength = aesLib.get_cipher_length(sizeof(sampleData));

    Card().aesEncrypt(sampleData, msgLen);
}

void Card::sendApdu(uint8_t apdu[], uint16_t apduLength, String commandName, boolean isDecrypt)
{
    char res_apdu[255];
    uint8_t resApduLength = sizeof(res_apdu);
    success = nfc.inDataExchange(apdu, apduLength, (uint8_t *)res_apdu, &resApduLength);
    if (success)
    {
        nfc.PrintHexChar((uint8_t *)res_apdu, resApduLength);
        memcpy(ivKey, res_apdu, 16);

        if (isDecrypt)
        {
            uint8_t decryptMsg[resApduLength - 16];
            for (int i = 0; i < resApduLength - 16; i++)
            {
                decryptMsg[i] = res_apdu[i + 16];
            }
            uint16_t msgLength = sizeof(decryptMsg);
            uint16_t decryptedLength = aesLib_without_padding.decrypt((byte *)decryptMsg, msgLength, (char *)cipherDecrypt, aesKey, sizeof(aesKey), macValue);
            nfc.PrintHexChar(cipherDecrypt, decryptedLength);
        }
    }
}

void Card::selectApdu()
{

    success = nfc.inListPassiveTarget();
    if (success)
    {

        uint8_t selectApdu[] = {0x00, 0xA4, 0x04, 0x00, 0x07, 0xA0, 0x00, 0x00, 0x10, 0x00, 0x01, 0x12};
        sendApdu(selectApdu, sizeof(selectApdu), "Select", false);
    }
}

void Card::getCertificates()
{

    uint8_t menuApdu[] = {0x80, 0xF7, 0x00, 0x01, 0x00};                                                     // menu Cert
                                                                                                             // uint8_t cardCertApdu[] = {0x80, 0xF8, 0x01, 0x00, 0x10, 0x53, 0x0C, 0x18, 0x4B, 0x89, 0xE1, 0x02, 0x84, 0x63, 0x02, 0x18, 0x4B, 0x08, 0xE1, 0x02, 0xF7}; // card Cert
    uint8_t cardCertApdu[] = {0x80, 0xF8, 0x00, 0x00, 0x08, 0x53, 0x0C, 0x18, 0x4B, 0x89, 0xE1, 0x02, 0x84}; // card Cert
                                                                                                             // sendApdu(menuApdu, sizeof(menuApdu), "GET Manufacture Certificate", false);
    uint8_t resCardApduLength = sizeof(res_cardCert);
    success = nfc.inDataExchange(cardCertApdu, sizeof(cardCertApdu), res_cardCert, &resCardApduLength);
    if (success)
    {
        nfc.PrintHex(res_cardCert, resCardApduLength);
    }
}

void Card::getKeysECC(uint8_t *cardCertResponse, uint8_t *cardCert)
{
    uECC_set_rng(&RNG);
    const struct uECC_Curve_t *curve = uECC_secp256r1();
    uint8_t cardCertPublicKey[64];
    bool eccSharedSuccess;
    bool makeKeySuccess;

    makeKeySuccess = uECC_make_key(public1, private1, curve);

    uint8_t public2[64];

    getSessionPublicKey(cardCertResponse, cardCertPublicKey, cardCert);

    eccSharedSuccess = uECC_shared_secret(cardCertPublicKey, private1, secret, curve);
}

void Card::getSessionPublicKey(uint8_t *cardCertResponse, uint8_t *cardCertPublicKey, uint8_t *cardCert)
{

    String respBuffer;
    for (int i = 10; i < 74; i++)
    {
        if (cardCertResponse[i] < 0x10)
            respBuffer = respBuffer + "0";

        respBuffer = respBuffer + String(cardCertResponse[i], HEX) + " ";

        cardCertPublicKey[i - 10] = cardCertResponse[i], HEX;
        cardCert[i - 10] = cardCertResponse[i], HEX;
        memcpy(cardCert, cardCertPublicKey, sizeof(cardCertPublicKey));
    }
}

void Card::aesEncrypt(byte *msg, uint16_t msgLen)
{
    unsigned char ciphertext[2 * INPUT_BUFFER_LIMIT] = {0};
    byte iv_key[N_BLOCK] = {0X8B, 0XA5, 0XC9, 0X66, 0XD4, 0X8E, 0XF5, 0X3C, 0X9D, 0X6D, 0X88, 0XE9, 0X92, 0XBA, 0X74, 0X61};
    byte aes_key[] = {0XDD, 0X7D, 0X64, 0XC2, 0XDC, 0X2E, 0X9E, 0X13, 0X6E, 0XED, 0X6E, 0X75, 0X93, 0XE2, 0X48, 0X7E, 0X4A, 0XB, 0X47, 0X52, 0X7, 0X35, 0X8A, 0X85, 0XBE, 0X3E, 0X9E, 0X21, 0XB4, 0X7A, 0XA7, 0X5B};

    uint16_t cipherLength;

    cipherLength = aesLib.encrypt(msg, msgLen, (char *)ciphertext, secret, sizeof(secret), iv_key);

    byte enc_iv[N_BLOCK] = {0X8B, 0XA5, 0XC9, 0X66, 0XD4, 0X8E, 0XF5, 0X3C, 0X9D, 0X6D, 0X88, 0XE9, 0X92, 0XBA, 0X74, 0X61};
    uint8_t init[] = {0x80, 0xFE, 0x00, 0x00, static_cast<uint8_t>(82 + cipherLength), 0x41, 0x04};

    int sizeOfInit = sizeof(init) + sizeof(public1) + sizeof(iv_key) + cipherLength;
    uint8_t initApdu[sizeOfInit];

    uint8_t *bufout = initApdu;
    memcpy(bufout, init, sizeof(init));
    bufout += sizeof(init);
    memcpy(bufout, public1, sizeof(public1));
    bufout += sizeof(public1);
    memcpy(bufout, enc_iv, sizeof(enc_iv));
    bufout += sizeof(enc_iv);
    memcpy(bufout, ciphertext, cipherLength);

    uint8_t res_init[255];
    uint8_t initResLength = sizeof(res_init);

    success = nfc.inDataExchange(initApdu, sizeof(initApdu), res_init, &initResLength);
}

void Card::openSecureChannel()
{

    const struct uECC_Curve_t *curve = uECC_secp256r1();
    uint8_t private_opc[32];
    uint8_t public_opc[64];
    bool eccSuccess;

    eccSuccess = uECC_make_key(public_opc, private_opc, curve);
    if (eccSuccess)
    {

        uint8_t opc[] = {0x80, 0x10, 0x00, 0x00, 0x41, 0x04};

        int size_opc = sizeof(opc) + 64;
        uint8_t opcApdu[size_opc];

        uint8_t *bufout_opc = opcApdu;
        memcpy(bufout_opc, opc, sizeof(opc));
        bufout_opc += sizeof(opc);
        memcpy(bufout_opc, public_opc, sizeof(public_opc));

        uint8_t res_opc[255];
        uint8_t opcResLength = sizeof(res_opc);

        success = nfc.inDataExchange(opcApdu, sizeof(opcApdu), res_opc, &opcResLength);
        if (success)
        {

            uint8_t salt[32];

            memcpy(salt, res_opc, sizeof(salt));

            uint8_t pairingSecret[] = {0X43, 0X72, 0X79, 0X70, 0X74, 0X6E, 0X6F, 0X78, 0X20, 0X42, 0X61, 0X73, 0X69, 0X63, 0X20, 0X43, 0X6F, 0X6D, 0X6D, 0X6F, 0X6E, 0X50, 0X61, 0X69, 0X72, 0X69, 0X6E, 0X67, 0X44, 0X61, 0X74, 0X61};
            uint8_t shared_secret_opc[32];

            uint8_t cardSessionPubKey[64];

            getSessionPublicKey(res_cardCert, cardSessionPubKey, cardCert);

            eccSuccess = uECC_shared_secret(cardSessionPubKey, private_opc, shared_secret_opc, curve);
            if (eccSuccess)
            {

                uint8_t sizeOfSecret = sizeof(shared_secret_opc) + sizeof(pairingSecret) + sizeof(salt);
                uint8_t secret_opc[sizeOfSecret];
                uint8_t *bufout_opc = secret_opc;
                memcpy(bufout_opc, shared_secret_opc, sizeof(shared_secret_opc));
                bufout_opc += sizeof(shared_secret_opc);
                memcpy(bufout_opc, pairingSecret, sizeof(pairingSecret));
                bufout_opc += sizeof(pairingSecret);
                memcpy(bufout_opc, salt, sizeof(salt));

                uint8_t result[64];
                Hash *hash = &sha512;

                hash->reset();
                hash->update(secret_opc, sizeof(secret_opc));

                hash->finalize(result, sizeof(result));

                memcpy(aesKey, result, 32);

                memcpy(macKey, result + 32, 32);

                uint8_t iv_opc[N_BLOCK] = {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
                uint8_t MAC_iv[N_BLOCK] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
                uint8_t RNG_data[] = {0X7, 0X72, 0X30, 0XB, 0XDC, 0X82, 0X58, 0XEC, 0X32, 0X59, 0XCE, 0X38, 0X69, 0X24, 0X1B, 0X59, 0XFB, 0X10, 0X7B, 0X92, 0X10, 0XF2, 0X6E, 0X1F, 0X5E, 0X37, 0X66, 0X6A, 0XC6, 0X55, 0XB5, 0XEF};
                uint8_t AEStest[] = {0X17, 0XAB, 0XF2, 0XAF, 0X5E, 0X19, 0X58, 0X7A, 0X8D, 0X4C, 0X9C, 0XC9, 0X22, 0X6F, 0X80, 0X77, 0X56, 0X2, 0X14, 0X7A, 0X8D, 0X58, 0X6C, 0X46, 0X5F, 0XE4, 0XDB, 0X2F, 0X14, 0XD7, 0X20, 0XBE};
                uint8_t MACtest[] = {0XFE, 0XF7, 0X97, 0X3B, 0XF0, 0X33, 0XAD, 0X12, 0XC1, 0X3E, 0X5B, 0X94, 0X59, 0X82, 0X26, 0X80, 0XC0, 0XD6, 0XCB, 0X2, 0X3C, 0X36, 0X4, 0XD5, 0X2D, 0X3E, 0X5A, 0XF6, 0X7B, 0XFF, 0X1E, 0XDF};

                char ciphertextOPC[2 * INPUT_BUFFER_LIMIT] = {0};
                int paddedLength = aesLib.get_cipher_length(sizeof(RNG_data));

                uint16_t cipherLength = aesLib.encrypt((byte *)RNG_data, sizeof(RNG_data), ciphertextOPC, aesKey, sizeof(aesKey), iv_opc);

                uint8_t opcApduHeader[] = {0x80, 0x11, 0x00, 0x00, static_cast<uint8_t>(cipherLength + 16)};
                uint8_t MAC_apduHeader[] = {0x80, 0x11, 0x00, 0x00, static_cast<uint8_t>(cipherLength + 16), 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

                uint8_t MAC_data_length = sizeof(MAC_apduHeader) + cipherLength;
                uint8_t MAC_data[MAC_data_length];
                uint8_t *buffMAC_data = MAC_data;

                memcpy(buffMAC_data, MAC_apduHeader, sizeof(MAC_apduHeader));
                buffMAC_data += sizeof(MAC_apduHeader);
                memcpy(buffMAC_data, ciphertextOPC, cipherLength);

                char ciphertextMACLong[2 * INPUT_BUFFER_LIMIT] = {0};
                uint16_t encryptedLengthMAC = aesLib_without_padding.encrypt((byte *)MAC_data, MAC_data_length, ciphertextMACLong, macKey, sizeof(macKey), MAC_iv);

                uint8_t MACpaddedLength = aesLib_without_padding.get_cipher_length(MAC_data_length);

                uint8_t MAC_value[16];

                uint8_t firstSliceLength = encryptedLengthMAC - 16;

                for (int i = firstSliceLength; i < encryptedLengthMAC; i++)
                {
                    MAC_value[i - firstSliceLength] = ciphertextMACLong[i];
                }

                uint8_t apduOpcLength = sizeof(opcApduHeader) + sizeof(MAC_value) + cipherLength;
                uint8_t sendApduOpc[apduOpcLength];
                uint8_t *buff_send_apdu = sendApduOpc;

                memcpy(buff_send_apdu, opcApduHeader, sizeof(opcApduHeader));
                buff_send_apdu += sizeof(opcApduHeader);
                memcpy(buff_send_apdu, MAC_value, sizeof(MAC_value));
                buff_send_apdu += sizeof(MAC_value);
                memcpy(buff_send_apdu, ciphertextOPC, cipherLength);

                uint8_t res_send_opc[255];
                uint8_t sendOpcResLength = sizeof(res_send_opc);

                success = nfc.inDataExchange(sendApduOpc, sizeof(sendApduOpc), res_send_opc, &sendOpcResLength);

                if (success)
                {
                    memcpy(ivKey, res_send_opc, 16);
                }
            }
        }
    }
}

void Card::aes_cbc_encrypt(uint8_t apdu[], uint16_t apduLength, uint8_t data[], uint16_t dataLength, String commandName)
{
    char encryptedData[2 * INPUT_BUFFER_LIMIT] = {0};
    uint16_t encryptedLength = aesLib.encrypt((byte *)data, dataLength, encryptedData, aesKey, sizeof(aesKey), ivKey);
    uint8_t macApdu[] = {static_cast<uint8_t>(encryptedLength + 16), 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint16_t macDataLength = apduLength + sizeof(macApdu) + encryptedLength;
    uint8_t macData[macDataLength];
    uint8_t *buffMacData = macData;
    memcpy(buffMacData, apdu, apduLength);
    buffMacData += apduLength;
    memcpy(buffMacData, macApdu, sizeof(macApdu));
    buffMacData += sizeof(macApdu);
    memcpy(buffMacData, encryptedData, encryptedLength);

    char macEncryptedData[2 * INPUT_BUFFER_LIMIT] = {0};
    uint8_t macIv[N_BLOCK] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint16_t macEncryptedLength = aesLib_without_padding.encrypt((byte *)macData, macDataLength, macEncryptedData, macKey, sizeof(macKey), macIv);

    uint8_t firstSliceEncryptedLength = macEncryptedLength - 16;

    for (int i = firstSliceEncryptedLength; i < macEncryptedLength; i++)
    {
        macValue[i - firstSliceEncryptedLength] = macEncryptedData[i];
    }

    uint8_t lengthValue[] = {static_cast<uint8_t>(encryptedLength + 16)};
    uint16_t sendApduLength = apduLength + sizeof(lengthValue) + sizeof(macValue) + encryptedLength;

    uint8_t apduCommand[sendApduLength];
    uint8_t *buffApdu = apduCommand;
    memcpy(buffApdu, apdu, apduLength);
    buffApdu += apduLength;
    memcpy(buffApdu, lengthValue, sizeof(lengthValue));
    buffApdu += sizeof(lengthValue);
    memcpy(buffApdu, macValue, sizeof(macValue));
    buffApdu += sizeof(macValue);
    memcpy(buffApdu, encryptedData, encryptedLength);

    sendApdu(apduCommand, sendApduLength, commandName, true);
}

void Card::getPubKey()
{
    uint8_t data[] = {};
    uint8_t apdu[] = {0x80, 0xC2, 0x00, 0x01};
    aes_cbc_encrypt(apdu, sizeof(apdu), data, sizeof(data), "get pub key ");
}

void Card::LoadSeed()
{
    uint8_t data[] = {0X2, 0X47, 0XB4, 0XE8, 0X7F, 0XE8, 0X27, 0X5B, 0XFE, 0X70, 0XD2, 0XA9, 0X81, 0XC4, 0XB1, 0X34, 0XC5, 0X39, 0X79, 0XF, 0XB4, 0X78, 0X58, 0XB0, 0XCC, 0XF9, 0X1, 0XAA, 0X60, 0X94, 0X60, 0XB5, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30};
    uint8_t apdu[] = {0x80, 0xD0, 0x03, 0x00};
    Card().aes_cbc_encrypt(apdu, sizeof(apdu), data, sizeof(data), "Load Seed ");
}

void Card::VerifyPin()
{
    uint8_t data[] = {0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30};
    uint8_t apdu[] = {0x80, 0x20, 0x00, 0x00};
    Card().aes_cbc_encrypt(apdu, sizeof(apdu), data, sizeof(data), "Verify Pin ");
}

void Card::generateKey()
{
    uint8_t data[] = {0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30};
    uint8_t apdu[] = {0x80, 0xD4, 0x00, 0x00};
    Card().aes_cbc_encrypt(apdu, sizeof(apdu), data, sizeof(data), "GENERATE KEY ");
}

void Card::changePin()
{
    uint8_t data[] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38};
    uint8_t apdu[] = {0x80, 0x21, 0x00, 0x00};
    Card().aes_cbc_encrypt(apdu, sizeof(apdu), data, sizeof(data), "Change Pin");
}

void Card::derive()
{
    uint8_t data[] = {0X80, 0X0, 0X0, 0X2C, 0X80, 0X0, 0X0, 0X3C, 0X80, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0, 0X0};
    uint8_t apdu[] = {0x80, 0xD1, 0x08, 0x00};
    Card().aes_cbc_encrypt(apdu, sizeof(apdu), data, sizeof(data), "derive ");
}

void Card::getCardInfo()
{
    uint8_t data[] = {0};
    uint8_t apdu[] = {0x80, 0xFA, 0x00, 0x00};
    Card().aes_cbc_encrypt(apdu, sizeof(apdu), data, sizeof(data), "card info ");
}

void Card::signWithPin()
{
    uint8_t data[] = {0X2, 0X47, 0XB4, 0XE8, 0X7F, 0XE8, 0X27, 0X5B, 0XFE, 0X70, 0XD2, 0XA9, 0X81, 0XC4, 0XB1, 0X34, 0XC5, 0X39, 0X79, 0XF, 0XB4, 0X78, 0X58, 0XB0, 0XCC, 0XF9, 0X1, 0XAA, 0X60, 0X94, 0X60, 0XB5, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30};
    uint8_t apdu[] = {0x80, 0xC0, 0x00, 0x00};
    Card().aes_cbc_encrypt(apdu, sizeof(apdu), data, sizeof(data), "Sign with Pin ");
}

void Card::changePairingKey()
{
    uint8_t data[] = {0X43, 0X72, 0X79, 0X70, 0X74, 0X6E, 0X6F, 0X78, 0X20, 0X42, 0X61, 0X73, 0X69, 0X63, 0X20, 0X43, 0X6F, 0X6D, 0X6D, 0X6F, 0X6E, 0X50, 0X61, 0X69, 0X72, 0X69, 0X6E, 0X67, 0X44, 0X61, 0X74, 0X61, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30};
    uint8_t apdu[] = {0x80, 0xDA, 0x00, 0x00};
    Card().aes_cbc_encrypt(apdu, sizeof(apdu), data, sizeof(data), "Change Pairing Key");
}

void Card::resetCard()
{
    uint8_t data[] = {0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30};
    uint8_t apdu[] = {0x80, 0xFD, 0x00, 0x00};
    Card().aes_cbc_encrypt(apdu, sizeof(apdu), data, sizeof(data), "Reset Card");
}

void Card::changePuk()
{
    uint8_t data[] = {0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30};
    uint8_t apdu[] = {0x80, 0x21, 0x01, 0x00};
    Card().aes_cbc_encrypt(apdu, sizeof(apdu), data, sizeof(data), "Change Puk");
}

void Card::checkHistory()
{
    uint8_t data[] = {0x53, 0x0C, 0x18, 0x4B, 0x89, 0xE1, 0x02, 0x84, 0X7, 0X72, 0X30, 0XB, 0XDC, 0X82, 0X58, 0XEC};
    uint8_t apdu[] = {0x80, 0xF8, 0x01, 0x00};
    Card().aes_cbc_encrypt(apdu, sizeof(apdu), data, sizeof(data), "signature check");
}

void Card::checkDoorLockAccess(const char *contractAddr)
{
    uint8_t address[32];
    uint8_t publicKey[64];

    for (int i = 0; i < sizeof(publicKey); i++)
    {
        publicKey[i] = cipherDecrypt[i + 1];
    }

    crypto.Keccak256(publicKey, sizeof(publicKey), address);

    uint8_t realAddress[20];

    for (int i = 12; i < sizeof(address); i++)
    {
        realAddress[i - 12] = address[i];
    }

    std::ostringstream os;
    os << "0x";
    for (int i : realAddress)
    {
        if (i < 16)
        {
            os << std::hex << 0;
            os << std::hex << i;
        }
        else
        {
            os << std::hex << i;
        }
    }

    std::string pubKeyStr(os.str());
    string addressToCheck = pubKeyStr.c_str();
    pinMode(doorLock_gpio, OUTPUT);
    digitalWrite(doorLock_gpio, HIGH);
    web3 = new Web3(GOERLI_ID);

    Contract contract(web3, contractAddr);
    uint256_t tokenValue = 1;
    string param = contract.SetupContractData("checkDoorAccess(address,uint256)", &addressToCheck, tokenValue);
    Serial.println("calling CheckDoorAccess function from smartContract");
    string result = contract.ViewCall(&param);
    Serial.println(result.c_str());

    int checkTokenIndex = result.length() - 3;

    char checkOwner = result.at(checkTokenIndex);

    if (checkOwner != '0')
    {
        Serial.println("Door open");
        digitalWrite(doorLock_gpio, LOW);
        delay(5000);
        digitalWrite(doorLock_gpio, HIGH);
    }
    else
    {
        Serial.println("Door close");
        digitalWrite(doorLock_gpio, HIGH);
    }
}