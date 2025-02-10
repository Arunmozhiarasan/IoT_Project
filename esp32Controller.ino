#include <esp_now.h>
#include <WiFi.h>
#include <SPI.h>
#include <LoRa.h>
#include <HardwareSerial.h>

HardwareSerial SerialPort(2);

const int relayPin = 2;
// MAC Addresses of your receivers (ESP8266 nodes)
uint8_t broadcastAddress1[] = {0xC4, 0x5B, 0xBE, 0x63, 0x02, 0xD4}; // Node 1 MAC address
uint8_t broadcastAddress2[] = {0x2C, 0xF4, 0x32, 0x14, 0x74, 0x66}; // Node 2 MAC address
uint8_t broadcastAddress3[] = {0x84, 0xf3, 0xeb, 0x58, 0x1c, 0x0d}; // Node 2 MAC address


typedef struct struct_message {
    char msg[50];
    int pinState;
    bool motion;
    int Soil;
    bool fieldMotor;
    int waterlevel;
    bool tankmotor;
} struct_message;

struct_message outgoingReadings;
struct_message incomingReadings1;
struct_message incomingReadings2;
struct_message incomingReadings3;

bool ismotion;
int soil;
bool fieldmotor;
bool tankmotor;
int waterlevel;
bool pin;
// bool ismotion;
// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("\r\nLast Packet Send Status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Callback when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
    if (memcmp(mac, broadcastAddress1, 6) == 0) {
        memcpy(&incomingReadings1, incomingData, sizeof(incomingReadings1));
        // soil = incomingReadings1.Soil;
        ismotion = incomingReadings1.motion;
        // Serial.print("Received from Node 1 - Soil: ");
        // Serial.println(soil);
        Serial.print("Received from Node 1 -  Motion Status: ");
        Serial.println(ismotion ? "Motion" : "No Motion");
    } else if (memcmp(mac, broadcastAddress2, 6) == 0) {
        memcpy(&incomingReadings2, incomingData, sizeof(incomingReadings2));
        // Process incomingReadings2 data here
        soil=incomingReadings2.Soil;
        Serial.print("Received from Node 2 - Soil: ");
        Serial.println(incomingReadings2.Soil);
    }
    else if (memcmp(mac, broadcastAddress3, 6) == 0) {
        memcpy(&incomingReadings3, incomingData, sizeof(incomingReadings3));
        // Process incomingReadings2 data here
        waterlevel=incomingReadings3.waterlevel;
        Serial.print("Received from Node 3 - water Level: ");
        Serial.println(incomingReadings3.waterlevel);
    }
}

void setup() {
    // Initialize Serial Monitor
    Serial.begin(115200);
    SerialPort.begin(9600, SERIAL_8N1, 16, 17);
    Serial.println("Code start");

    // Set pin D2 as an input
    pinMode(relayPin, OUTPUT);

    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);

    // Initialize ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        while (true) {
            delay(1000);
        }
    }

    // Register peers
    esp_now_peer_info_t peerInfo = {};

    memcpy(peerInfo.peer_addr, broadcastAddress1, 6);
    peerInfo.channel = 0;  // Use default channel
    peerInfo.encrypt = false; // No encryption
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer 1");
        while (true) {
            delay(1000);
        }
    }

    memcpy(peerInfo.peer_addr, broadcastAddress2, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer 2");
        while (true) {
            delay(1000);
        }
    }
    memcpy(peerInfo.peer_addr, broadcastAddress3, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer 3");
        while (true) {
            delay(1000);
        }
    }

    // Register callbacks
    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
    // Prepare the outgoing message
    // delay(2000);
    // pin=digitalRead(relayPin);
    String data=SerialPort.readString();
    Serial.println(data);
    if(data!=""){
      fieldmotor=(data[0]=='0'? false:true);
    tankmotor=(data[1]=='0'? false:true);
    pin=(data[2]=='0'? false:true);
    strcpy(outgoingReadings.msg, "Hello from ESP32");
    outgc:\Users\arunp\Downloads\SparkFun_MAX3010x_Sensor_Library-master.zipoingReadings.pinState = pin; // Example pin state
    outgoingReadings.fieldMotor = fieldmotor;
    outgoingReadings.tankmotor = tankmotor;
    // outgoingReadings.pin = pin;
    // outgoingReadings.isFencing = tankmotor;
    Serial.print("Field Motor : ");
    Serial.println(fieldmotor);
    Serial.print("Tank Motor : ");
    Serial.println(tankmotor);
    String message =("{ \"Soil\":\""+String(soil)+"\",\"Waterlevel\":\""+String(waterlevel)+"\",\"isMotion\":"+bool(ismotion)+"}" );
    SerialPort.println(message);\
    Serial.println(message);
    // Send the message to Node 1
    esp_err_t result1 = esp_now_send(broadcastAddress1, (uint8_t *)&outgoingReadings, sizeof(outgoingReadings));
    Serial.println(result1 == ESP_OK ? "Sent to Node 1" : "Failed to send to Node 1");

    // Send the message to Node 2
    esp_err_t result2 = esp_now_send(broadcastAddress2, (uint8_t *)&outgoingReadings, sizeof(outgoingReadings));
    Serial.println(result2 == ESP_OK ? "Sent to Node 2" : "Failed to send to Node 2");

    esp_err_t result3 = esp_now_send(broadcastAddress3, (uint8_t *)&outgoingReadings, sizeof(outgoingReadings));
    Serial.println(result3 == ESP_OK ? "Sent to Node 3" : "Failed to send to Node 3");
    // Control the relay based on the motor state
    // digitalWrite(relayPin, fieldmotor ? HIGH : LOW);
    }

    
}
