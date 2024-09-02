#include <RadioLib.h>
#include <chrono> // For timestamping
#include "PiHal_WP.h"
//#include "PiHal_LG.h"

PiHal* hal = new PiHal(0);

//wiringPi module instance, use with PiHal_WP.h
// Create the radio module instance
// NSS pin: WiringPi 21 (GPIO 5)
// DIO1 pin: WiringPi 22 (GPIO 6)
// NRST pin: WiringPi 25 (GPIO 26)
// BUSY pin: WiringPi 27 (GPIO 16)

//testing other groups' pinout with both wiringPi and lgpio
// Create the radio module instance
// NSS pin: WiringPi 21 (GPIO 5)
// DIO1 pin: WiringPi 22 (GPIO 6)
// NRST pin: WiringPi 25 (GPIO 26)
// BUSY pin: WiringPi 27 (GPIO 16)
//SX1262 radio = new Module(hal, 5, 6, 26, 16);
SX1262 radio = new Module(hal, 21, 22, 25, 27);
PhysicalLayer* phy = (PhysicalLayer*)&radio;

int main() {
    radio.XTAL = true;
    printf("[SX1262] Initializing ... ");
    //int state = radio.beginFSK(915.0, 4.8, 125.0, 467.0, 20.0, 16.0, 0.0, false);
    //int state = radio.begin(915.0, 250.0, 12, 5, 0x12, 10, 8, 0.0, true);
    int state = radio.begin();
    if (state != RADIOLIB_ERR_NONE) {
        printf("Initialization failed, code %d\n", state);
        return 1;
    }
    printf("Initialization success!\n");

    int count = 0;

    while (true) {
        //getcurrent time 
        auto now = std::chrono::high_resolution_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

        //print the timestamp and packet number
        printf("[SX1262] Transmitting packet #%d at time %lld ms ... ", count, timestamp);

        //create a packet with the timestamp and message
        char str[64];
        sprintf(str, "Timestamp: %lld, Hello World! #%d", timestamp, count++);

        //send the packet
        state = radio.transmit(str);
        if (state == RADIOLIB_ERR_NONE) {
            printf("success!\n");

            //print the effective data rate for the transmitted packet
            float effectiveDataRate = radio.getDataRate();
            printf("Effective Data Rate: %.2f bps\n", effectiveDataRate);
        } else {
            printf("failed, code %d\n", state);
        }

        //don't want to overwhelm receiver
        hal->delay(1000);
    }

    return 0;
}
