/*
 * This sketch is a simple write/read benchmark.
 */
#include "application.h"
#include <SdFat.h>
#include <SdFatUtil.h>

#define FILE_SIZE_MB 5
#define FILE_SIZE (1000000UL*FILE_SIZE_MB)
#define BUF_SIZE 100

// SOFTWARE SPI pin configuration - modify as required
// The default pins are the same as HARDWARE SPI
const uint8_t chipSelect = A2;    // Also used for HARDWARE SPI setup
const uint8_t mosiPin = A5;
const uint8_t misoPin = A4;
const uint8_t clockPin = A3;

uint8_t buf[BUF_SIZE];

Sd2Card card;
SdVolume volume;
SdFile root;
SdFile file;

void error(const char* str)
{
  Serial1.print("error: ");
  Serial1.println(str);
  if (card.errorCode()) {
    Serial1.print("SD error: ");
    Serial1.print(card.errorCode(), HEX);
    Serial1.print(',');
    Serial1.println(card.errorData(), HEX);
  }
  while(1) {
    SPARK_WLAN_Loop();
  };
}

void setup() {
  Serial1.begin(115200);
  while (!Serial1.available()) SPARK_WLAN_Loop();
  while (Serial1.available()) Serial1.read();

  // initialize the SD card at SPI_FULL_SPEED for best performance.
  // try SPI_HALF_SPEED if bus errors occur.
  // Initialize HARDWARE SPI with user defined chipSelect
  if (!card.init(SPI_FULL_SPEED, chipSelect)) error("card.init failed");

  // Initialize SOFTWARE SPI
  //if (!card.init(mosiPin, misoPin, clockPin, chipSelect)) error("card.init failed");

  // initialize a FAT volume
  if (!volume.init(&card)) error("volume.init failed!");

  Serial1.print("Type is FAT");
  Serial1.println(volume.fatType(), DEC);

  if (!root.openRoot(&volume)) error("openRoot failed");
}

void loop() {
  uint32_t t;
  double r;

  Serial1.println("Type any character to start");
  while (!Serial1.available()) SPARK_WLAN_Loop();
  while (Serial1.available()) Serial1.read();

  // open or create file - truncate existing file.
  if (!file.open(&root, "BENCH.DAT", O_CREAT | O_TRUNC | O_RDWR)) {
    error("open failed");
  }
  // fill buf with known data
  for (uint16_t i = 0; i < (BUF_SIZE-2); i++) {
    buf[i] = 'A' + (i % 26);
  }
  buf[BUF_SIZE-2] = '\r';
  buf[BUF_SIZE-1] = '\n';
  Serial1.print("File size ");
  Serial1.print(FILE_SIZE_MB);
  Serial1.println(" MB");
  uint32_t n = FILE_SIZE/sizeof(buf);
  Serial1.println("Starting write test.  Please wait up to a minute");
  // do write test
  t = millis();
  for (uint32_t i = 0; i < n; i++) {
    if (file.write(buf, sizeof(buf)) != sizeof(buf)) {
      error("write failed");
    }
  }
  t = millis() - t;
  file.sync();
  r = (double)file.fileSize()/t;
  Serial1.print("Write ");
  Serial1.print(r);
  Serial1.println(" kB/sec");
  Serial1.println();
  Serial1.println("Starting read test.  Please wait up to a minute");
  // do read test
  file.rewind();
  t = millis();
  for (uint32_t i = 0; i < n; i++) {
    if (file.read(buf, sizeof(buf)) != sizeof(buf)) {
      error("read failed");
    }
  }
  t = millis() - t;
  r = (double)file.fileSize()/t;
  Serial1.print("Read ");
  Serial1.print(r);
  Serial1.println(" kB/sec");
  Serial1.println("Done");
  file.close();
}
