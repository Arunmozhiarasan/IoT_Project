#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DHTPIN 3           // Digital pin connected to the DHT11
#define DHTTYPE DHT11      // Define the DHT sensor type
#define FLAME_PIN 4        // Digital pin connected to the flame sensor
#define BUZZER_PIN 5       // Digital pin connected to the buzzer
#define LED_PIN 6          // Digital pin connected to the LED
#define MQ7_PIN A0         // Analog pin connected to the MQ-7 sensor

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);  // I2C address may be 0x27 or 0x3F

void setup() {
    Serial.begin(9600);
    dht.begin();
    lcd.init();             // Initialize the LCD
    lcd.backlight();        // Turn on the backlight
    pinMode(FLAME_PIN, INPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    Serial.println("Initializing sensors...");
    lcd.setCursor(0, 0);
    lcd.print("Sensors Initializing");
    delay(2000);  // Wait for LCD to show initialization message
    lcd.clear();
}

void loop() {
    // Read humidity and temperature from DHT11
    float humidity = dht.readHumidity();
    float temperatureC = dht.readTemperature(); // Celsius

    // Display temperature and humidity
    lcd.setCursor(0, 0);
    if (isnan(humidity) || isnan(temperatureC)) {
        Serial.println("Failed to read from DHT sensor!");
        lcd.print("Temp/Humidity Err");
    } else {
        Serial.print("Humidity: ");
        Serial.print(humidity);
        Serial.print(" %\t");
        Serial.print("Temperature: ");
        Serial.print(temperatureC);
        Serial.println(" °C");

        lcd.print("Temp: ");
        lcd.print(temperatureC);
        lcd.print("C ");
        lcd.setCursor(0, 1);
        lcd.print("Hum: ");
        lcd.print(humidity);
        lcd.print("%");
    }

    delay(2000);  // Small delay for readability

    // Read flame sensor value
    int flameState = digitalRead(FLAME_PIN);
    lcd.clear();
    if (flameState == LOW) {
        Serial.println("Danger: Flame detected!");
        digitalWrite(BUZZER_PIN, HIGH); // Turn on the buzzer
        digitalWrite(LED_PIN, HIGH);    // Turn on the LED
        lcd.print("Flame: DETECTED!");
    } else {
        Serial.println("No flame detected.");
        digitalWrite(BUZZER_PIN, LOW);  // Turn off the buzzer
        digitalWrite(LED_PIN, LOW);     // Turn off the LED
        lcd.print("Flame: None");
    }

    delay(2000);  // Small delay for readability

    // Read MQ-7 sensor value
    int mq7Value = analogRead(MQ7_PIN);
    Serial.print("CO Level (MQ-7): ");
    Serial.println(mq7Value);
    lcd.clear();
    lcd.print("CO Level: ");
    lcd.print(mq7Value);

    // Check MQ-7 sensor for dangerous smoke levels (adjust threshold as needed)
    if (mq7Value > 700) {  // Example threshold; adjust based on calibration
        Serial.println("Danger: High CO/smoke level detected!");
        digitalWrite(BUZZER_PIN, HIGH); // Turn on the buzzer
        digitalWrite(LED_PIN, HIGH);    // Turn on the LED
        lcd.setCursor(0, 1);
        lcd.print("High CO Detected!");
    } else {
        digitalWrite(BUZZER_PIN, LOW);  // Turn off the buzzer
        digitalWrite(LED_PIN, LOW);     // Turn off the LED
    }

    delay(2000);  // Wait 2 seconds before the next reading
}

