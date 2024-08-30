#include <RadioLib.h>
#include <unistd.h>     // For usleep
#include <iostream>     // For std::cout
#include <vector>       // For std::vector
#include <algorithm>    //std::find
#include "PiHal.h"
PiHal* hal = new PiHal(0);

// Create the radio module instance
// NSS pin: WiringPi 10 (GPIO 8)
// DIO1 pin: WiringPi 2 (GPIO 27)
// NRST pin: WiringPi 21 (GPIO 5)
// BUSY pin: WiringPi 0 (GPIO 17)
SX1262 radio = new Module(hal, 10, 2, 21, 0);

volatile bool receivedFlag = false;

// Function to be called when a packet is received
void setFlag() {
    receivedFlag = true;
    std::cout << "we received" << std::endl;
}

int main(int argc, char** argv) {
    // Initialize radio
    std::cout << "[SX1262] Initializing ... ";
    int state = radio.begin(915.0, 250.0, 12, 5, 0x34, 10, 8, 1.6, true);
    if (state != RADIOLIB_ERR_NONE) {
        std::cout << "failed, code " << state << std::endl;
        return 1;
    }
    std::cout << "success!" << std::endl;

    // Set the function that will be called when a new packet is received
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
            // Reset flag
            receivedFlag = false;

            size_t maxLength = 256;
            std::vector<uint8_t> byteArr(maxLength);

            // Read the received data
            state = radio.readData(byteArr.data(), maxLength);

            if (state == RADIOLIB_ERR_NONE) {
                //determine actual packet length
                size_t numBytes = std::find(byteArr.begin(), byteArr.end(), static_cast<unsigned char>('\0')) - byteArr.begin();
                
                //serial print packet
                std::cout << "[SX1262] Received packet!" << std::endl;
                std::cout << "[SX1262] Data:\t\t";
                
                // try to convert might break ...?
                std::string receivedData(byteArr.begin(), byteArr.begin() + numBytes);
                std::cout << receivedData << std::endl;

                // Print RSSI (Received Signal Strength Indicator)
                std::cout << "[SX1262] RSSI:\t\t" << radio.getRSSI() << " dBm" << std::endl;

                // Print SNR (Signal-to-Noise Ratio)
                std::cout << "[SX1262] SNR:\t\t" << radio.getSNR() << " dB" << std::endl;

                // Print frequency error
                std::cout << "[SX1262] Frequency error:\t" << radio.getFrequencyError() << " Hz" << std::endl;

            } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
                // Packet was received, but is malformed
                std::cout << "CRC error!" << std::endl;

            } else {
                // Some other error occurred
                std::cout << "failed, code " << state << std::endl;
            }
        }
    //could use delay?
    }

    return 0;
}
