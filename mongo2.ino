#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include <HardwareSerial.h>

HardwareSerial SerialPort(2);

const char* ssid = "Praveen";
const char* password = "praveenb";
const char* serverUrl = "https://backend-black-tau-90.vercel.app/update";
const char* serverUrl2 = "https://backend-black-tau-90.vercel.app/datas";
void setup() {s
  Serial.begin(115200);
  SerialPort.begin(9600, SERIAL_8N1, 16, 17);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  // float temperature = getTemperature(); // Function to get temperature reading

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    HTTPClient http1;
    http1.begin(serverUrl2);
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");
    http1.addHeader("Content-Type", "application/json");
    // Create JSON payload
    // Serial.println(jsonPayload);
    http1.POST("");
    String data=http1.getString();
    Serial.println(data);
    StaticJsonDocument<200> doc;
    deserializeJson(doc, data);
    bool isFencing = doc["isFencing"];
    bool fieldMotor = doc["FieldMotor"];
    bool waterTankMotor = doc["WaterTankMotor"];
    String motordata=String(fieldMotor)+String(waterTankMotor)+String(isFencing);
    SerialPort.println(motordata);
    // Serial.println( motordata[0]=='0' ? "t":"f");
    String str=SerialPort.readString();
    Serial.println(str);
    http.POST(str);
    // if (httpCode > 0) {
    //   String payload = http.getString();
    //   // Serial.println(payload);
    // } else {
    //   Serial.println("Error sending temperature data");
    // }
    http.end();
  }
  // delay(6000); // Send temperature data every minute
}

