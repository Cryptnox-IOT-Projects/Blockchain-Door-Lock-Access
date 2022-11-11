#include <Arduino.h>
#include <AESLib.h>
#define INPUT_BUFFER_LIMIT (128 + 1)

class Card
{
public:
    Card();
    void sendApdu(uint8_t apdu[], uint16_t apduLength, String commandName, boolean isDecrypt);
    void selectApdu();
    void getCertificates();
    void init();
    void getKeysECC(uint8_t *cardCertResponse, uint8_t *cardCert);
    void getSessionPublicKey(uint8_t *cardCertResponse, uint8_t *cardCertPublicKey, uint8_t *cardCert);
    void aesEncrypt(byte *msg, uint16_t msgLen);
    void openSecureChannel();
    void aes_cbc_encrypt(uint8_t apdu[], uint16_t apduLength, uint8_t data[], uint16_t dataLength, String commandName);
    void VerifyPin();
    void generateKey();
    void LoadSeed();
    void getCardInfo();
    void changePairingKey();
    void changePuk();
    void getPubKey();
    void derive();
    void signWithPin();
    void resetCard();
    void changePin();
    void signatureCheck();
    void checkHistory();
    void checkDoorLockAccess(const char *contractAddr);
};
