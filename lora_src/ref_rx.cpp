#include <RadioLib.h>
#include <iostream>     // For std::cout
#include <vector>       // For std::vector
#include "PiHal_WP.h"
//#include "PiHal_LG.h"

PiHal* hal = new PiHal(0);

// Create the radio module instance
// Pinout corresponds to your SX1262 setup
// NSS pin: WiringPi 10 (GPIO 8)
// DIO1 pin: WiringPi 2 (GPIO 27)
// NRST pin: WiringPi 21 (GPIO 5)
// BUSY pin: WiringPi 0 (GPIO 17)

//testing other groups' pinout with both wiringPi and lgpio
// Pinout corresponds to your SX1262 setup
// NSS pin: WiringPi 10 (GPIO 8)
// DIO1 pin: WiringPi 2 (GPIO 27)
// NRST pin: WiringPi 21 (GPIO 5)
// BUSY pin: WiringPi 0 (GPIO 17)
//SX1262 radio = new Module(hal, 8, 10, 5, 17);

volatile bool receivedFlag = false;
SX1262 radio = new Module(hal, 10, 2, 21, 0);
PhysicalLayer* phy = (PhysicalLayer*)&radio;

//IRQ
void setFlag() {
    receivedFlag = true;
    std::cout << "we received" << std::endl;
}

// the entry point for the program
int main(int argc, char** argv) {
  radio.XTAL = true;

  // initialize just like with Arduino
  printf("[SX1261] Initializing ... ");
  int state = radio.begin();
  if (state != RADIOLIB_ERR_NONE) {
    printf("failed, code %d\n", state);
    return(1);
  }
  printf("success!\n");

  radio.setPacketReceivedAction(setFlag);

  printf("[SX1261] Starting to listen ... ");
  state = radio.startReceive();
  if (state != RADIOLIB_ERR_NONE) {
    printf("failed, code %d\n", state);
    return(1);
  }
  printf("success!\n");

  // loop forever
  int count = 0;
  uint8_t buff[256] = { 0 };
  for(;;) {
    if(receivedFlag) {
      // reset flag
      receivedFlag = false;
      size_t len = radio.getPacketLength();

      int state = radio.readData(buff, len);
      if (state != RADIOLIB_ERR_NONE) {
        printf("Read failed, code %d\n", state);
      } else {
        printf("Data: %s\n", (char*)buff);
      }
    }
  }

  return(0);
}
/*
int main(int argc, char** argv) {
    std::cout << "[SX1262] Initializing ... ";
    //int state = radio.begin(915.0, 250.0, 12, 5, 0x12, 10, 8, 0.0, true);
    int state = radio.beginFSK(915.0, 4.8, 125.0, 467.0, 20.0, 16.0, 0.0, false);
    if (state != RADIOLIB_ERR_NONE) {
        std::cout << "failed, code " << state << std::endl;
        return 1;
    }
    std::cout << "success!" << std::endl;
    radio.setPacketReceivedAction(setFlag);

    //try to rx now..?
    std::cout << "[SX1262] Starting to listen ... ";
    state = radio.startReceive();
    if (state != RADIOLIB_ERR_NONE) {
        std::cout << "failed, code " << state << std::endl;
        return 1;
    }
    std::cout << "success!" << std::endl;

    while (true) {
        if (receivedFlag) {
            //toggle flag
            receivedFlag = false;

            size_t maxLength = 256;
            std::vector<uint8_t> byteArr(maxLength);

            //put data in array
            state = radio.readData(byteArr.data(), maxLength);

            if (state == RADIOLIB_ERR_NONE) {
                //determine actual packet length
                size_t numBytes = radio.getPacketLength();
                
                //serial print packet
                std::cout << "[SX1262] Received packet!" << std::endl;
                std::cout << "[SX1262] Data:\t\t";
                

            } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
                std::cout << "CRC error!" << std::endl;

            } else {
                std::cout << "failed, code " << state << std::endl;
            }
        }
    //could use delay?
    //hal->delay(100);
    }

    return 0;
}
*/
