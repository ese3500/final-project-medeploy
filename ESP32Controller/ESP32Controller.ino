 // AARON ESP32 MAC ADDRESS:
 // EC:62:60:1D:11:20
 // DAILING ESP32 MAC ADDRESS:
 // CC:DB:A7:10:8E:10

#include <esp_now.h>
#include <WiFi.h>
#include <WebServer.h>

//#define ssid "Keterrer IOT"
//#define password "theCl0ud"

// REPLACE WITH THE MAC Address of your receiver 
uint8_t broadcastAddress[] = {0xEC,0x62,0x60,0x1D,0x11,0x20};

// Variable to store if sending data was successful
String success;

//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
    char msg[100];
    //int state;
    int p1;
    int p2;
    int p3;
    int door;
} struct_message;

typedef struct struct_message2 {
    char msg[100];
    //int state;
    int pain;
    int help;
    int bpm;
} struct_message2;

struct_message outgoing;

struct_message2 incoming;
String in;

esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  //Serial.print("\r\nLast Packet Send Status:\t");
  //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incoming, incomingData, sizeof(incoming));

  //Serial.print("Bytes received: ");
  //Serial.println(len);
  //in = incoming.msg;
  Serial.println("INCOMING MESSAGE");
  Serial.print("Pain Level: ");
  Serial.println(incoming.pain);
  Serial.print("Help Level: ");
  Serial.println(incoming.help);
  Serial.print("BPM: ");
  Serial.println(incoming.bpm);
  Serial.println();
}
 
void setup() {
  Serial.begin(115200);
  //WiFi.begin(ssid, password);
  pinMode(5, INPUT); // high for forward
  pinMode(18, INPUT); // high for backward
  pinMode(12, INPUT);
  pinMode(14, INPUT); // open door
  // dont move when both low


  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {
  /*if (digitalRead(12) == HIGH && digitalRead(18) == LOW && digitalRead(13) == LOW) {
    outgoing.state = 1; //forward
  } else if (digitalRead(12) == LOW && digitalRead(18) == HIGH && digitalRead(13) == LOW) {
    outgoing.state = 2; //backward
  }else if (digitalRead(12) == HIGH && digitalRead(18) == HIGH && digitalRead(13) == LOW) {
    outgoing.state = 3; //left
  } else if (digitalRead(12) == HIGH && digitalRead(18) == LOW && digitalRead(13) == HIGH) {
    outgoing.state = 4; //right
  } else {
    outgoing.state = 5; //neutral
  }
  Serial.println(outgoing.state);*/
  if (digitalRead(12) == HIGH) {
    outgoing.p1 = 1;
  } else {
    outgoing.p1 = 0;
  }
  if (digitalRead(18) == HIGH) {
    outgoing.p2 = 1;
  } else {
    outgoing.p2 = 0;
  }
  if (digitalRead(13) == HIGH) {
    outgoing.p3 = 1;
  } else {
    outgoing.p3 = 0;
  }
  if (digitalRead(14) == HIGH) {
    outgoing.door = 1;
  } else {
    outgoing.door = 0;
  }
  //Serial.println(outgoing.p1);
  //Serial.println(outgoing.p2);
  //Serial.println(outgoing.p3);
  //Serial.println();
  strcpy(outgoing.msg, "This is the controller");
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &outgoing, sizeof(outgoing));
  
  if (result == ESP_OK) {
    //Serial.println("Sent with success");
  }
  else {
    //Serial.println("Error sending the data");
  }
}
