/*
 * Project: Smart Shopping Trolley using RFID & Arduino Nano
 * Author: Pooja Saraswat & Team
 * Description: Scans item RFID tags, tracks running totals, and provides visual/audio feedback.
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

// PIN CONFIGURATION
#define RX_PIN 2          // Connect to EM18 Tx pin
#define TX_PIN 3          // Not used, but required by SoftwareSerial
#define BUZZER_PIN 12     // Piezo Buzzer Pin
#define GREEN_LED 10      // Scan Success Indicator
#define RED_LED 11        // Scan Error Indicator

// Initialize Hardware Modules
LiquidCrystal_I2C lcd(0x27, 16, 2); // Change address to 0x3F if 0x27 doesn't work
SoftwareSerial rfidSerial(RX_PIN, TX_PIN);

// Global State Variables
float totalBill = 0.0;
int totalItems = 0;
String rfidTagString = "";

void setup() {
  // Initialize Serial Monitors
  Serial.begin(9600);
  rfidSerial.begin(9600);
  
  // Initialize Hardware Interface Pins
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  
  // Ensure components start in safe off-states
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);

  // Initialize LCD Panel and print Welcome Splash Screen
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(" SMART TROLLEY ");
  lcd.setCursor(0, 1);
  lcd.print(" INITIALIZING...");
  delay(2000);
  updateDisplay("Scan an Item", totalBill);
}

void loop() {
  // Read incoming bytes from EM18 Module if an RFID tag enters the field
  if (rfidSerial.available() > 0) {
    char incomingChar = rfidSerial.read();
    
    // Build the 12-character unique RFID string code
    if (incomingChar != '\n' && incomingChar != '\r' && rfidTagString.length() < 12) {
      rfidTagString += incomingChar;
    }
    
    // Once the exact 12-character string tag is processed, evaluate the item data
    if (rfidTagString.length() == 12) {
      Serial.print("Scanned Tag: ");
      Serial.println(rfidTagString);
      
      processScannedTag(rfidTagString);
      rfidTagString = ""; // Clear string container for the next read loop
    }
  }
}

// Function to cross-reference scanned tags against store inventory arrays
void processScannedTag(String tag) {
  
  // NOTE: Replace these placeholder strings with your actual 12-digit RFID Tag IDs!
  if (tag == "120043A1B2C3") { 
    handleValidScan("Milk Carton", 45.00);
  } 
  else if (tag == "120054F6D7E8") {
    handleValidScan("Bread Loaf", 30.00);
  } 
  else if (tag == "120087B3C4A5") {
    handleValidScan("Biscuits", 20.00);
  } 
  else {
    // If an unregistered tag passes through the frame
    handleInvalidScan();
  }
}

// Logic routine executed upon matching a valid item profile
void handleValidScan(String itemName, float price) {
  totalBill += price;
  totalItems++;
  
  // Success Alert triggers: Short high chirp + Green flash
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(BUZZER_PIN, HIGH);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Added: " + itemName);
  lcd.setCursor(0, 1);
  lcd.print("Price: Rs." + String(price, 2));
  
  delay(1500); // Hold message to allow read visibility
  
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  
  updateDisplay("Items: " + String(totalItems), totalBill);
}

// Security logic routine for unexpected or mismatched tag profiles
void handleInvalidScan() {
  // Error Alert triggers: Long continuous tone + Red light flash
  digitalWrite(RED_LED, HIGH);
  digitalWrite(BUZZER_PIN, HIGH);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("⚠️ UNKNOWN ITEM ⚠️");
  lcd.setCursor(0, 1);
  lcd.print(" SCAN FAILED! ");
  
  delay(2000);
  
  digitalWrite(RED_LED, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  
  updateDisplay("Items: " + String(totalItems), totalBill);
}

// Auxiliary display refresh block
void updateDisplay(String headerText, float runningTotal) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(headerText);
  lcd.setCursor(0, 1);
  lcd.print("Total: Rs." + String(runningTotal, 2));
}
