
//    https://www.amebaiot.com/zh/rtl8195-arduino-api-wifisslclient/

//    example at   https://github.com/ambiot/amb1_arduino/blob/dev/Arduino_package/hardware/libraries/WiFi/examples/WiFiSSLClient/WiFiSSLClient.ino


#include <WiFi.h>
#include <WiFiSSLClient.h>
#include "debug.h"



char ssid[] = "";        // your network SSID (name)
char pass[] = "";       // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                   // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

char HTTPS_SERVER[] = "4fh42v-8080.preview.csb.app";   // use a dynamic codesandbox this one may still be active
char HTTPS_PATH[] = "/";

// no certificate needed, Arduino has made it already a part of the SSLclient.

WiFiSSLClient client;
unsigned long myStore;



void setup() {
    //Initialize serial and wait for port to open
    Serial.begin(115200);




    // attempt to connect to Wifi network
    while (status != WL_CONNECTED) {
        Serial.print("\r\n Attempting to connect to SSID: ");
        Serial.println(ssid);
        // Connect to WPA/WPA2 network. Change this line if using open or WEP network
        status = WiFi.begin(ssid,pass);

        // wait for connection
        delay(5000);
    }
    Serial.println("Connected to wifi");
    printWifiStatus();

    Serial.println("\nStarting connection to server...");
 // client.setRootCA((unsigned char*)rootCABuff);  // already done by arduino
    // if a connection is formed, report back via serial
    if (client.connect(HTTPS_SERVER, 443)) {
        Serial.println("connected to server");
        // Make a HTTP request:
        client.print("GET ");
        client.print(HTTPS_PATH);
        client.println(" HTTP/1.1");
        client.print("Host: ");
        client.println(HTTPS_SERVER);
        client.println("Upgrade: websocket");
        client.println("Connection: Upgrade");
        client.println("Sec-WebSocket-Key: x3JJHMbDL1EzLkh9GBhXDw==");
       // client.println("Sec-WebSocket-Protocol: chat, superchat");
        client.println("Sec-WebSocket-Version: 13");
        client.println();
    } else {
        Serial.println("connected to server failed");
    }
    delay(100);

    myStore = millis();
}

void loop() {
   
    // if there are incoming bytes available
    // from the server, read them and print them
    while (client.available()) {
        char c = client.read();
        Serial.print(c, HEX);  // good to debug
        Serial.print(",");
       // Serial.print(c);   // show the characters // somehow causing issues 
    }

    if ((millis() - myStore) >= 20000 ) {
      myStore = millis();
      Serial.println(); 
      Serial.println("Send: Hello"); 
 
    //const char *msg = "Hello world";


    // try binary  

    /*
     * For example, to send "Hello" to server in binary mode, you do:

    flag: 0x82, "Final packet in frame" and "Binary mode"
    mask bit: 1
   len: 6
   mask: 0x11, 0x22, 0x33, 0x44
   payload: "Hello", 0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x00
   The masked payload is: 0x48^0x11, 0x65^0x22, 0x6C^0x33, 0x6C^0x44, 0x6F^0x11, 0x00^0x22 => 0x59, 0x47, 0x5F, 0x28, 0x7E, 0x22

   The whole stream is: 0x82, 0x86, 0x11, 0x22, 0x33, 0x44, 0x59, 0x47, 0x5F, 0x28, 0x7E, 0x22
     */

     //FORMAT:          codes -----------MASK ---------------------DATA-----------------------
    //const char msg[] = {0x82, 0x86,      0x11, 0x22, 0x33, 0x44,   0x59, 0x47, 0x5F, 0x28, 0x7E, 0x22};  // that works!
    
    // but why the end BYTE on the original. Try without it. but now the length needs to be 5 so 86 to 85
    // Also lets send text not binary so 82 to 81
    //const char msg[] = {0x81, 0x85,    0x11, 0x22, 0x33, 0x44,   0x59, 0x47, 0x5F, 0x28, 0x7E};  // wow that works!
    
    //const char msg[] = {0x81, 0x05,                              0x48, 0x65, 0x6C, 0x6C, 0x6F};  // This died so a mask is always needed!
    const char msg[] = {0x81, 0x85,    0x00, 0x00, 0x00, 0x00,   0x48, 0x65, 0x6C, 0x6C, 0x6F};  // testing mask 0,0,0,0 WOW!

    
    client.write((const uint8_t*)msg, strlen(msg));  


   //client.beginPacket();
   //client.print("Hello");

   //client.endPacket();



    }


    // if the server's disconnected, stop the client
    if (!client.connected()) {
        Serial.println();
        Serial.println("disconnecting from server.");
        client.stop();

        // do nothing
        while (true);
    }
}

void printWifiStatus() {
    // print the SSID of the network you're attached to:
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // print your WiFi shield's IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);

    // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.print(rssi);
    Serial.println(" dBm");
}
