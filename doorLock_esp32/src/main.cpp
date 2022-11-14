#include <Arduino.h>
#include <WiFi.h>

/**************************************************************************/
/*!
    @file     iso14443a_uid.pde
    @author   Adafruit Industries
  @license  BSD (see license.txt)

    This example will attempt to connect to an ISO14443A
    card or tag and retrieve some basic information about it
    that can be used to determine what type of card it is.

    Note that you need the baud rate to be 115200 because we need to print
  out the data and read from the card at the same time!

  This is an example sketch for the Adafruit PN532 NFC/RFID breakout boards
  This library works with the Adafruit NFC breakout
  ----> https://www.adafruit.com/products/364

  Check out the links above for our tutorials and wiring diagrams
  These chips use SPI or I2C to communicate.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

*/
/**************************************************************************/

#include <uECC.h>
#include "AESLib.h"
#include <AES.h>

#include <Crypto.h>
#include <SHA512.h>
#include <Crypto.h>
#include <SHA3.h>
#include <Connection.h>
#include <Card.h>

#define ERC20CONTRACT "<Contract Address>" // Put your ERC20 contract address here
#define TARGETADDRESS "<Wallet Address>"   // put your wallet address here
#define ETHERSCAN_TX "https://goerli.etherscan.io/"
#define INPUT_BUFFER_LIMIT (128 + 1)
#define HASH_SIZE 64
#define card Card()
const byte gpio = 32;

void selectApdu();
void getCertificates();
void init();
void sendApdu(uint8_t apdu[], uint16_t apduLength, String commandName, boolean isDecrypt);
void getKeysECC(uint8_t *cardCertResponse, uint8_t *cardCert);
void getSessionPublicKey(uint8_t *cardCertResponse, uint8_t *cardCertPublicKey, uint8_t *cardCert);
bool hasToken(byte *value, int length);

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
void setup(void)
{
  Serial.begin(115200);
  pinMode(gpio, OUTPUT);
  digitalWrite(gpio, HIGH);
  Connection connection = Connection();
  connection.setUpWifi();
  connection.setUpNFC();
}

void loop(void)
{
  card.selectApdu();
  card.getCertificates();
  card.openSecureChannel();
  card.getPubKey();
  card.checkDoorLockAccess(ERC20CONTRACT);
}
