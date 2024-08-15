#include <RadioLib.h>
#include <fstream>
#include <string>
#include <iostream>
#include "PiHal.h"
#include <termios.h>
#include <unistd.h>

// Create a new instance of the HAL class
PiHal* hal = new PiHal(0);

// Create the radio module instance
// Pinout corresponds to your SX1262 setup
// NSS pin: WiringPi 10 (GPIO 8)
// DIO1 pin: WiringPi 2 (GPIO 27)
// NRST pin: WiringPi 21 (GPIO 5)
// BUSY pin: WiringPi 0 (GPIO 17)
SX1262 radio = new Module(hal, 10, 2, 21, 0);

int main() {
  //disable terminal buffering for key press detection
  //used for space bar input to advance tests
  struct termios oldt, newt;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);

  //test variables
  float BW[] = {62.5, 125.0, 250.0};
  int SF[] = {7, 9, 11};
  int CR[] = {5, 8};

  int state;
  int test_number = 1;
  std::ifstream file("test_tx.txt");

  //make sure file opening worked
  if (!file.is_open()) {
    printf("Failed to open file!\n");
    return 1;
  }

  //loop through the BW, SF, and CR values
  for (int cr_idx = 0; cr_idx < 2; cr_idx++) {
    for (int sf_idx = 0; sf_idx < 3; sf_idx++) {
      for (int bw_idx = 0; bw_idx < 3; bw_idx++) {

        //reinit radio
        state = radio.begin(915.0, BW[bw_idx], SF[sf_idx], CR[cr_idx], 18, 10, 8, 0.0, false);
        if (state != RADIOLIB_ERR_NONE) {
          printf("Initialization failed, code %d\n", state);
          return 1;
        }

        //show the current test
        printf("\n--- Test #%d: SF=%d, BW=%.1f, CR=%d ---\n", test_number, SF[sf_idx], BW[bw_idx], CR[cr_idx]);

        //read and transmit each line from the file
        std::string line;
        while (std::getline(file, line)) {
          printf("[SX1262] Transmitting: %s\n", line.c_str());
          state = radio.transmit(line.c_str());
          if (state == RADIOLIB_ERR_NONE) {
            printf("Transmission success!\n");
          } else {
            printf("Transmission failed, code %d\n", state);
          }

          hal->delay(500); // half sec delay
        }

        //go to beginning of file
        file.clear();  //clear EOF flag
        file.seekg(0, std::ios::beg);

        //wait for space bar to advance to the next test
        printf("Press space bar to advance to the next test...\n");
        while (true) {
          if (std::cin.peek() == ' ') {
            printf("Advancing to the next test...\n");
            std::cin.get();  //clear the space bar input from the buffer
            break;
          }
          hal->delay(100);
        }

        test_number++;
      }
    }
  }

  //restore terminal settings
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

  file.close();
  return 0;
}
