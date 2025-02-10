#include <ESP8266WiFi.h>
#include <espnow.h>

const int soilSensorPin = A0;

uint8_t broadcastAddress[] = {0xc0, 0x49, 0xef, 0xd3, 0x90, 0x64}; // Replace with ESP32 MAC address

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
struct_message incomingReadings;

bool motor;
int pin;

void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
    Serial.print("Last Packet Send Status: "); 
    Serial.println(sendStatus == 0 ? "Delivery success" : "Delivery fail");
}

void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
    memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
    motor = incomingReadings.fieldMotor;
    Serial.print("motor : ");
    Serial.println(motor);
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    pinMode(D2,OUTPUT);
    if (esp_now_init() != 0) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
    esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 0, NULL, 0);
    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
    int soilMoisture = analogRead(soilSensorPin);
    int moisturePercentage = map(soilMoisture, 1023, 0, 0, 100);

    strcpy(outgoingReadings.msg, "Hello from NodeMCU");
    outgoingReadings.Soil = moisturePercentage;
    outgoingReadings.pinState = pin;
    // outgoingReadings.fieldMotor = motor;

    esp_now_send(broadcastAddress, (uint8_t *) &outgoingReadings, sizeof(outgoingReadings));

    Serial.print("Soil Moisture: ");
    Serial.print(moisturePercentage);
    Serial.println("%");
    if(motor){
      digitalWrite(D2,HIGH);
    }
    else{
      digitalWrite(D2,LOW);
    }
    delay(1000);
}
