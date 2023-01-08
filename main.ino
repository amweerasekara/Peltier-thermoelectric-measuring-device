#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_INA219.h>
#include "FS.h"#include "SD.h"
#include <SPI.h>
#define ONE_WIRE_BUS 4 // Data wire is plugged TO GPIO 4
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SD_CS 5 // Define CS pin for the SD card module
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); // Declaration for an SSD1306
display connected to I2C (SDA, SCL pins)
Adafruit_INA219 ina219;
String dataMessage;
int readingID = 0;
float temp1;
float temp2;
float shuntvoltage;
float busvoltage;
float current_mA;
float loadvoltage_V;
float power_mW;
void setup(){
Serial.begin(115200); // start serial port
sensors.begin();if (! ina219.begin()) {
Serial.println("Failed to find INA219 chip");
while (1) { delay(10); }}
if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
Serial.println(F("SSD1306 allocation failed"));
for(;;);
}
delay(2000);
display.clearDisplay();
display.setTextColor(WHITE);
SD_Card_Initialize();
}
void loop(){
sensors.requestTemperatures(); // Send the command to get temperatures
Serial.print("Temperature for device1: ");
temp1 = sensors.getTempCByIndex(0);
Serial.println(temp1);
Serial.print("Temperature for device12: ");
temp2 = sensors.getTempCByIndex(1);
Serial.println(temp2);
display.clearDisplay();// clear display
// display temperature
display.setTextSize(1);display.setCursor(0,0);
display.print("Temp1: ");
display.setTextSize(1);
display.setCursor(0,10);
display.print(temp1);
display.print(" ");
display.print("`C");
display.setTextSize(1);
display.setCursor(64,0);
display.print("Temp2: ");
display.setTextSize(1);
display.setCursor(64,10);
display.print(temp2);
display.print(" ");
display.print("`C");
shuntvoltage = 0;
busvoltage = 0;
current_mA = 0;
loadvoltage_V = 0;
power_mW = 0;
shuntvoltage = ina219.getShuntVoltage_mV();
busvoltage = ina219.getBusVoltage_V();
current_mA = ina219.getCurrent_mA();
power_mW = ina219.getPower_mW();
loadvoltage_V = busvoltage + (shuntvoltage / 1000);Serial.print("Load Voltage_V : ");
Serial.println(loadvoltage_V);
Serial.print("Current_mA : ");
Serial.println(current_mA);
Serial.print("Power_mW : ");
Serial.println(power_mW);
//display voltage current and power
display.setTextSize(1);
display.setCursor(0,20);
display.print("Load voltage :");
display.print(loadvoltage_V);
display.print(" V");
display.setCursor(0,30);
display.print("Current :");
display.print(current_mA);
display.print(" mA");
display.setCursor(0,40);
display.print("Power :");
display.print(power_mW);
display.print(" mW");
display.display();if(SD.begin(SD_CS)) {
readingID++;
SD_Card_Initialize();
logSDCard();
display.setCursor(0,50);
display.print("Saved");
display.setCursor(40,50);
display.print("Rec_No :");
display.print(readingID);
display.display();
}
else{
display.setCursor(0,50);
display.print("Insert a SD card!");
display.display();
}
delay(10);
}
void logSDCard() { // Write the sensor readings on the SD card
dataMessage = String(readingID) + "," + String(temp1) + "," + String(temp2) + "," +
String(loadvoltage_V) + "," + String(current_mA)+ "," + String(power_mW) + "\r\n";
Serial.print("Save data: ");
Serial.println(dataMessage);
appendFile(SD, "/data.txt", dataMessage.c_str());
}void writeFile(fs::FS &fs, const char * path, const char * message) { // Write to the SD card (DON'T
MODIFY THIS FUNCTION)
Serial.printf("Writing file: %s\n", path);
File file = fs.open(path, FILE_WRITE);
if(!file) {
Serial.println("Failed to open file for writing");
return;
}
if(file.print(message)) {
Serial.println("File written");
} else {
Serial.println("Write failed");
}
file.close();
}
void appendFile(fs::FS &fs, const char * path, const char * message) { // Append data to the SD card
(DON'T MODIFY THIS FUNCTION)
Serial.printf("Appending to file: %s\n", path);
File file = fs.open(path, FILE_APPEND);
if(!file) {
Serial.println("Failed to open file for appending");
return;
}
if(file.print(message)) {
Serial.println("Message appended");} else {
Serial.println("Append failed");
}
file.close();
}
void SD_Card_Initialize(){
SD.begin(SD_CS);
if(!SD.begin(SD_CS)) {
Serial.println("Card Mount Failed");
return;
}
uint8_t cardType = SD.cardType();
if(cardType == CARD_NONE) {
Serial.println("No SD card attached");
return;
}
Serial.println("Initializing SD card...");
if (!SD.begin(SD_CS)) {
Serial.println("ERROR - SD card initialization failed!");
return; // init failed
}
// If the data.txt file doesn't exist
// Create a file on the SD card and write the data labels
File file = SD.open("/data.txt");
if(!file) {
Serial.println("File doens't exist");
Serial.println("Creating file...");writeFile(SD, "/data.txt", "Reading ID, temp1_C, temp2_C, voltage_V, current_mA, power_mW \r\n");
}
else {
Serial.println("File already exists");
}
file.close();
}