#include <Arduino.h>
#include <SPI.h>
#include <PN532_SPI.h>
#include "PN532.h"
#include <Web3.h>
#include <Util.h>
#include <Contract.h>

static Web3 *web3;

static PN532_SPI pn532spi(SPI, 16);
static PN532 nfc(pn532spi);

class Connection
{
public:
    void setUpNFC();
    void setUpWifi();  
};
