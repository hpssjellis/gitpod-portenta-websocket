//    https://www.amebaiot.com/zh/rtl8195-arduino-api-wifisslclient/

//    example at   https://github.com/ambiot/amb1_arduino/blob/dev/Arduino_package/hardware/libraries/WiFi/examples/WiFiSSLClient/WiFiSSLClient.ino


#include <WiFi.h>
#include <WiFiSSLClient.h>


char ssid[] = "";        // your network SSID (name)
char pass[] = "";       // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                   // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

// from the gitpod dynamically loaded server
//char HTTPS_SERVER[] = "8080-hpssjellis-gitpodporten-rwr8c7utrkv.ws-us79.gitpod.io";
char HTTPS_SERVER[] = "8080-hpssjellis-gitpodporten-rwr8c7utrkv.ws-us79.gitpod.io";
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
        client.println("Sec-WebSocket-Protocol: chat, superchat");
        client.println("Sec-WebSocket-Version: 13");
        client.println("Origin: 192.168.137.210");
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
        Serial.write(c);
    }

    if ((millis() - myStore) >= 10000 ) {
      myStore = millis();
      Serial.println("Send: D somehow"); 
     // client.print((char)0x00); // Start of message frame
     // client.print("D");    // Message payload
     // client.print((char)0xff); // End of message frame
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
