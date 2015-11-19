/*
  SD card test

 This example shows how use the utility libraries on which the'
 SD library is based in order to get info about your SD card.
 Very useful for testing a card when you're not sure whether its working or not.

 The circuit:
  * SD card attached to SPI bus as follows:
  Refer to "libraries/SdFat/Sd2Card_config.h"

 created  28 Mar 2011
 by Limor Fried
 modified 16 Mar 2011
 by Tom Igoe
 modified for Maple(STM32 micros)/libmaple
 17 Mar 2012
 by dinau
 */
 // include the SD library:

#include "application.h"
#include "SD.h"

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;

// SOFTWARE SPI pin configuration - modify as required
// The default pins are the same as HARDWARE SPI
const uint8_t chipSelect = A2;    // Also used for HARDWARE SPI setup
const uint8_t mosiPin = A5;
const uint8_t misoPin = A4;
const uint8_t clockPin = A3;


void setup()
{
  Serial1.begin(115200);
  while (!Serial1.available());

  Serial1.print("\nInitializing SD card...");

  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  // Initialize HARDWARE SPI with user defined chipSelect
  if (!card.init(SPI_HALF_SPEED, chipSelect)) {

  // Initialize SOFTWARE SPI (uncomment and comment out above line to use)
//  if (!card.init(mosiPin, misoPin, clockPin, chipSelect)) {
    Serial1.println("initialization failed. Things to check:");
    Serial1.println("* is a card is inserted?");
    Serial1.println("* Is your wiring correct?");
    Serial1.println("* did you change the chipSelect pin to match your shield or module?");
    return;
  } else {
   Serial1.println("Wiring is correct and a card is present.");
  }

  // print the type of card
  Serial1.print("\nCard type: ");
  switch(card.type()) {
    case SD_CARD_TYPE_SD1:
      Serial1.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial1.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial1.println("SDHC");
      break;
    default:
      Serial1.println("Unknown");
  }

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card)) {
    Serial1.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    return;
  }

  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  Serial1.print("\nVolume type is FAT");
  Serial1.println(volume.fatType(), DEC);
  Serial1.println();

  volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  volumesize *= 512;                         // SD card blocks are always 512 bytes
  Serial1.print("Volume size (bytes): ");
  Serial1.println(volumesize);
  Serial1.print("Volume size (Kbytes): ");
  volumesize /= 1024;
  Serial1.println(volumesize);
  Serial1.print("Volume size (Mbytes): ");
  volumesize /= 1024;
  Serial1.println(volumesize);

  Serial1.println("\nFiles found on the card (name, date and size in bytes): ");
  root.openRoot(volume);

  // list all files in the card with date and size
  root.ls(LS_R | LS_DATE | LS_SIZE);
}

void loop(void) {

}
