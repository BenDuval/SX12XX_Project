#include <iostream>
#include <fstream>
#include <RadioLib.h>
#include "PiHal.h"
#include <termios.h>
#include <unistd.h>

// Create a new instance of the HAL class
PiHal* hal = new PiHal(0);

// Create the radio module instance
// NSS pin: WiringPi 10 (GPIO 8)
// DIO1 pin: WiringPi 2 (GPIO 27)
// NRST pin: WiringPi 21 (GPIO 5)
// BUSY pin: WiringPi 0 (GPIO 17)
SX1262 radio = new Module(hal, 10, 2, 21, 0);

int main() {
  //test params
  float BW[] = {62.5, 125.0, 250.0};
  int SF[] = {7, 9, 11};
  int CR[] = {5, 8};

  int state;
  int packet_count;
  std::ofstream outfile;
  
  //space bar for test advancing
  struct termios oldt, newt;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);

  //loop through the BW, SF, and CR values
  for (int cr_idx = 0; cr_idx < 2; cr_idx++) {
    for (int sf_idx = 0; sf_idx < 3; sf_idx++) {
      for (int bw_idx = 0; bw_idx < 3; bw_idx++) {

        //make file name
        std::string filename = "rx_data_" + std::to_string(SF[sf_idx]) + "_" + std::to_string((int)BW[bw_idx]) + "_" + std::to_string(CR[cr_idx]) + ".txt";
        outfile.open(filename);
        if (!outfile.is_open()) {
          printf("Failed to open file %s for writing\n", filename.c_str());
          return 1;
        }

        //init with current values
        printf("[SX1262] Initializing ... \n");
        state = radio.begin(915.0, BW[bw_idx], SF[sf_idx], CR[cr_idx], 18, 10, 8, 0.0, false);
        if (state != RADIOLIB_ERR_NONE) {
          printf("Initialization failed, code %d\n", state);
          return 1;
        }
        printf("Initialization success for SF=%d, BW=%.1f, CR=%d\n", SF[sf_idx], BW[bw_idx], CR[cr_idx]);

        //start receiving
        printf("[SX1262] Starting receiver ... ");
        state = radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF);
        if (state != RADIOLIB_ERR_NONE) {
          printf("Start receive failed, code %d\n", state);
          return 1;
        }
        printf("Receiver started!\n");

        packet_count = 0;

        //main waiting loop -> space breaks us out onto next file
        while (true) {
          //check if interrupt -> packet rcv
          if (radio.getIrqStatus() & RADIOLIB_SX126X_IRQ_RX_DONE) {
            //readbuffer
            uint8_t data[64];
            size_t len = sizeof(data);
            state = radio.readData(data, len);
            if (state == RADIOLIB_ERR_NONE) {
              //success
              packet_count++;
              printf("Received packet #%d: %s\n", packet_count, data);

              //write out to file
              outfile << "Packet #" << packet_count << ": " << data << std::endl;
            } else {
              //failed, not sure why this would happen
              printf("Failed to read data, code %d\n", state);
            }

            //restart rcvr
            state = radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF);
            if (state != RADIOLIB_ERR_NONE) {
              printf("Restart receive failed, code %d\n", state);
              break;
            }
          }

          //check if space bar is pressed to advance to the next test
          if (std::cin.peek() == ' ') {
            printf("Advancing to next test...\n");
            break;
          }

          //smaller delay than tx delay
          hal->delay(100);
        }

        //test done, show results in terminal
        outfile.close();
        printf("Total packets received for SF=%d, BW=%.1f, CR=%d: %d\n", SF[sf_idx], BW[bw_idx], CR[cr_idx], packet_count);

      }
    }
  }

  //restore terminal settings
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

  return 0;
}
