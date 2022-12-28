/*
 *    Arduino PortentaH7 SSLwebsocket example 
 *    Note: This is not professionally made and is only for Maker activity
 * 
 * 
 *    Research:
 *    https://github.com/janjongboom/mbed-ws/blob/master/source/ws_client_base.h
 *    https://www.amebaiot.com/zh/rtl8195-arduino-api-wifisslclient/
 *    https://os.mbed.com/teams/mbed_example/code/WebSocketClient//file/efa2c147bee1/Websocket.cpp/
 *    example at   https://github.com/ambiot/amb1_arduino/blob/dev/Arduino_package/hardware/libraries/WiFi/examples/WiFiSSLClient/WiFiSSLClient.ino
 *
*/

#include <WiFi.h>
#include <WiFiSSLClient.h>




char ssid[] = "";        // your network SSID (name)
char pass[] = "";                  // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                          // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

char HTTPS_SERVER[] = "4fh42v-8080.preview.csb.app";
char HTTPS_PATH[] = "/";

//    Note: If you open this URL you get to the browser websocket client ran by the websoket server on codesandbox
//    https://4fh42v-8080.preview.csb.app/

// no certificate needed, Arduino has made it already a part of the SSLclient.

WiFiSSLClient client;
unsigned long mySendMillis, myWaitToReadMillis;
int mySendDuration = 10000;   // send test data every x milliseconds
int myWaitToReadDuration = 20;   // delay time before reading data so no conflicts after a write
bool myCanReadNow = true;
bool myOnlyOnce = false;



void mySendTXT(const char *myMessage, int mySize){

// Only for T
  
  // char *myPayload;
   char myPayload[2+4+mySize];
   char myHeader[7];
   int  myHeaderLength = 2;  // must be bigger for sending more data
   char myMask[4];
   // Show input
   Serial.print("myMessage: ");
   Serial.println(myMessage);
   Serial.print("mySize: ");
   Serial.println(mySize);
   Serial.print("sizeof(myMessage)in bytes: ");
   Serial.println(sizeof(myMessage),HEX);
   Serial.print("sizeof(myMessage)in integers: ");
   Serial.println(sizeof(myMessage));
   // Set headers and mask
   //  Hello is    0x48, 0x65, 0x6C, 0x6C, 0x6F,
   // const char msg[] = {0x81, 0x85,    0x01, 0x02, 0x03, 0x04,   0x49, 0x67, 0x6F, 0x68, 0x6E};  // Sends HELLO as TEXT
   myHeader[0]={0x81};   // FIN 80 and TEXT 1




   
  // myHeader[1]={0x85};   // MASKED 80 (must have)  5 size of TEXT to send  presently only set for 5 character sends
  // myHeader[1]= (byte)mySize & 0x7f;   //        & 0xff & 0x80 ;   // 80 = Mask and the size of the text to send
  // myHeader[1]= (char)myHeader[1] & 0x80; 
     myHeader[1]=  mySize | (1 << 7);  // this shows better how to set the 8th bit to declare the next 4 bits are a mask
                                       // also why this only allows 127 characters to be sent
   
   // myMask[0]={0x01};
   // myMask[1]={0x02};
   // myMask[2]={0x03};
   // myMask[3]={0x04};
   for (int k = 0; k < 4; k++) {
      myMask[k]=random(1,99);;
   }
   myPayload[0] = myHeader[0];
   myPayload[1] = myHeader[1];
   myPayload[2] = myMask[0];
   myPayload[3] = myMask[1];
   myPayload[4] = myMask[2];
   myPayload[5] = myMask[3];

   for (int j = 0; j < mySize; j++) {
      myPayload[j+2+4] = myMessage[j] ^ myMask[j % 4];
    }
   // myPayload[6] = myMessage[0]^ myMask[0];
   // myPayload[7] = myMessage[1]^ myMask[1];
   // myPayload[8] = myMessage[2]^ myMask[2];
   // myPayload[9] = myMessage[3]^ myMask[3];
   // myPayload[10]= myMessage[4]^ myMask[0];


   

  // Show outpuyt and send it
   Serial.print("myPayload: ");
   for (int i = 0; i < 2+4+mySize; i++) { 
      Serial.print((char)myPayload[i], HEX);
      Serial.print(",");
   }
   Serial.println();
   Serial.print("myPayload: ");
   Serial.println(myPayload);
   Serial.print("sizeof(myPayload): ");
   Serial.println(sizeof(myPayload));
   client.write((const uint8_t*)myPayload, 2+4+mySize); 
}








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
        
        //  use a Base64 encoder/decoder to make your own Sec-WebSocket-Key
        //  https://www.rapidtables.com/web/tools/base64-decode.html
        client.println("Sec-WebSocket-Key: x3JJHMbDL1EzLkh9GBhXDw==");
        client.println("Sec-WebSocket-Version: 13");
        client.println();
    } else {
        Serial.println("connected to server failed");
    }
    delay(100);

    mySendMillis = millis();
    myWaitToReadMillis = millis();
}

void loop() {
   
    if (myOnlyOnce && (millis() - myWaitToReadMillis) >= myWaitToReadDuration ) {
        myCanReadNow = true;  // after a delay when writing allow reading again 
        myOnlyOnce = false;   // so not flushing the client continuously
        client.flush();       // atttempt to solve random crash
    }
    
    // if there are incoming bytes available
    // from the server, read them and print them
    while (client.available() && myCanReadNow) {
        char c = client.read();
       // Serial.print(c, HEX);  // good to debug
      //  Serial.print(",");
        Serial.print(c);   // show the characters // somehow causing issues after sending data 
    

       if ((millis() - mySendMillis) >= mySendDuration ) {
          mySendMillis = millis();    
          myWaitToReadMillis = millis(); 
          myCanReadNow = false;  // stop the ability to read while writing
          myOnlyOnce = true;
          Serial.println(); 
          Serial.println("Send: Hello"); 
         // const char msg[] = {0x81, 0x85,    0x01, 0x02, 0x03, 0x04,   0x49, 0x67, 0x6F, 0x68, 0x6E};  // Sends HELLO as TEXT
          //client.write((const uint8_t*)msg, strlen(msg)); 
          // testing my solution 
          
         // const char msg[] = "Hello";  // more standard way to send it
          const char msg[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890abcdefghijklmnopqrstuvwxyz~!@#$%^&*()_+";  // more standard way to send it
          //ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890abcdefghijklmnopqrstuvwxyz~!@#$%^&*()_+
          mySendTXT(msg, strlen(msg));
           
          // mySendTXT("Hello", 5);   // this also works
          

            
       } // now send
    }  // while connected



 
    //const char *msg = "Hello world";


    // try binary  

    /*
     * For example, to send "Hello" to server in binary mode, you do:

    flag: 0x82, "Final packet in frame" and "Binary mode"
    mask bit: 1
   len: 6
   mask: 0x11, 0x22, 0x33, 0x44
   payload: "Hello",
   0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x00
   The masked payload is: 0x48^0x11, 0x65^0x22, 0x6C^0x33, 0x6C^0x44, 0x6F^0x11, 0x00^0x22 => 0x59, 0x47, 0x5F, 0x28, 0x7E, 0x22

   The whole stream is: 0x82, 0x86, 0x11, 0x22, 0x33, 0x44, 0x59, 0x47, 0x5F, 0x28, 0x7E, 0x22


    //   FORMAT:          codes -----------MASK ---------------------DATA-----------------------
    //const char msg[] = {0x82, 0x86,      0x11, 0x22, 0x33, 0x44,   0x59, 0x47, 0x5F, 0x28, 0x7E, 0x22};  // that works!
    
    // but why the end BYTE on the original. Try without it. but now the length needs to be 5 so 86 to 85
    
    // Also lets send text not binary so 82 to 81
    // const char msg[] = {0x81, 0x85,    0x11, 0x22, 0x33, 0x44,   0x59, 0x47, 0x5F, 0x28, 0x7E};  // wow that works!
    
    // const char msg[] = {0x81, 0x05,                              0x48, 0x65, 0x6C, 0x6C, 0x6F};  // This died so a mask is always needed!
    // const char msg[] = {0x01, 0x05,                              0x48, 0x65, 0x6C, 0x6C, 0x6F};  // This died so a mask is always needed!
    // const char msg[] = {0x81, 0x85,    0x00, 0x00, 0x00, 0x00,   0x48, 0x65, 0x6C, 0x6C, 0x6F};  // Nope, needs a real mask
    // const char msg[] = {0x81, 0x85,    0x01, 0x01, 0x01, 0x01,   0x49, 0x64, 0x6D, 0x6D, 0x6E};  // Easiest mask, strange results, why were some subtracted?
    // const char msg[] = {0x81, 0x85,    0x01, 0x10, 0x02, 0x11,   0x49, 0x75, 0x6E, 0x7D, 0x6E};  // try worked
    // const char msg[] = {0x81, 0x85,    0x01, 0x02, 0x01, 0x02,   0x49, 0x67, 0x6D, 0x6E, 0x6E};  // This might be best, should make it random  
   // const char msg[] =    {0x81, 0x85,    0x01, 0x02, 0x03, 0x04,   0x49, 0x67, 0x6F, 0x68, 0x6E};  // Probably best to program this 
    //client.write((const uint8_t*)msg, strlen(msg));  


   //client.beginPacket();
   //client.print("Hello");

   //client.endPacket();

   */


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


//void mySendBinary((const uint8_t*)msg, strlen(msg)){
  
  
//}
