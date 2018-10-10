#include "SdFat.h"
/*
 * This program demonstrates the freeClusterCount() call.
 */
/*
 * SD chip select pin.
 * Default SD chip select is the SPI SS pin.
 */
const uint8_t chipSelect = SS;

#define TEST_FILE "Cluster.test"
// file system
SdFat sd;

// test file
SdFile file;

// Serial output stream
ArduinoOutStream cout(Serial);
//------------------------------------------------------------------------------
void printFreeSpace() {
  cout << F("freeClusterCount() call time: ");
  uint32_t m = micros();
  uint32_t volFree = sd.vol()->freeClusterCount();
  cout << micros() - m << F(" micros\n");
  cout << F("freeClusters: ") <<  volFree << setprecision(3) << endl;
  float fs = 0.000512*volFree*sd.vol()->blocksPerCluster();
  cout << F("freeSpace: ") << fs << F(" MB (MB = 1,000,000 bytes)\n\n");
}
//------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  // Wait for USB Serial 
  while (!Serial) {
    SysCall::yield();
  }

  if (!MAINTAIN_FREE_CLUSTER_COUNT) {
    cout << F("Please edit SdFatConfig.h and set\n");
    cout << F("MAINTAIN_FREE_CLUSTER_COUNT nonzero for\n");
    cout << F("maximum freeClusterCount() performance.\n\n");
  }
  // F stores strings in flash to save RAM
  cout << F("Type any character to start\n");
  while (Serial.read() <= 0) {
    SysCall::yield();
  }

  // initialize the SD card at SPI_HALF_SPEED to avoid bus errors with
  // breadboards.  use SPI_FULL_SPEED for better performance.
  if (!sd.begin(chipSelect, SPI_HALF_SPEED)) {
    sd.initErrorHalt();
  }
  // Insure no TEST_FILE. 
  sd.remove(TEST_FILE);
  
  cout << F("\nFirst call to freeClusterCount scans the FAT.\n\n");
  printFreeSpace();
  
  cout << F("Create and write to ") << TEST_FILE << endl;
  if (!file.open(TEST_FILE, O_WRITE | O_CREAT)) {
    sd.errorHalt(F("Create failed"));
  }
  file.print(F("Cause a cluster to be allocated"));
  file.close();
  
  cout << F("\nSecond freeClusterCount call is faster if\n");
  cout << F("MAINTAIN_FREE_CLUSTER_COUNT is nonzero.\n\n");
  
  printFreeSpace();

  cout << F("Remove ") << TEST_FILE << endl << endl;
  sd.remove(TEST_FILE);
  printFreeSpace();
}
//------------------------------------------------------------------------------
void loop() {}