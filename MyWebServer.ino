/*
  Web Server
 
 A simple web server that shows the value of the analog input pins.
 using an Arduino Wiznet Ethernet shield. 
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)
 
 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 modified 21 Jun 2012
 by Michael Main (added RESTful API)
 
 */

#include <SPI.h>
#include <Ethernet.h>
#include <httpHeader.h>
#include <neko.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(10,0,0,2);

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server(80);
HTTPHeader header;
ApplicationServer appServer;

void* pulseLed(EthernetClient* client, HTTPHeader* header, void* data) {
  // send a standard http response header
  client->println("HTTP/1.1 200 OK");
  client->println("Content-Type: text/html");
  client->println("Connnection: close");
  client->println();
  
  if (!header->fetchQuery("pin") || !header->fetchQuery("repeat") || !header->fetchQuery("period"))
    return NULL;
  
  int pin = atoi(header->fetchQuery("pin"));
  int repeat = atoi(header->fetchQuery("repeat"));
  int period = atoi(header->fetchQuery("period"));
              
  pinMode(pin, OUTPUT);

  client->println("<html>");
  client->print("Request received to pulse pin ");
  client->print(header->fetchQuery("pin"));
  client->print(" ");
  client->print(header->fetchQuery("repeat"));
  client->print(" times.");
  client->println("<br />");
  client->println("</html>");
              
  delay(1);
  client->stop();
              
  for (int i = 0; i < repeat; i++) {
    delay(period/2);
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
    delay(period/2);
    digitalWrite(pin, LOW);
  }
  
  return NULL;
}

void* ledOff(EthernetClient* client, HTTPHeader* header, void* data) {
  // send a standard http response header
  client->println("HTTP/1.1 200 OK");
  client->println("Content-Type: text/html");
  client->println("Connnection: close");
  client->println();
  
  if (!header->fetchQuery("pin"))
    return NULL;
  
  int pin = atoi(header->fetchQuery("pin"));
              
  client->println("<html>");
  client->print("Request received to power off pin ");
  client->print(header->fetchQuery("pin"));
  client->println("<br />");
  client->println("</html>");
  
  delay(1);
  client->stop();
              
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}

void* ledOn(EthernetClient* client, HTTPHeader* header, void* data) {
  // send a standard http response header
  client->println("HTTP/1.1 200 OK");
  client->println("Content-Type: text/html");
  client->println("Connnection: close");
  client->println();
  
  if (!header->fetchQuery("pin"))
    return NULL;
  
  int pin = atoi(header->fetchQuery("pin"));
              
  client->println("<html>");
  client->print("Request received to power on pin ");
  client->print(header->fetchQuery("pin"));
  client->println("<br />");
  client->println("</html>");
  
  delay(1);
  client->stop();
              
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
}

void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
  
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();

  appServer.addService(GET, "/pinControl/pulse", pulseLed);
  appServer.addService(GET, "/pinControl/on", ledOn);
  appServer.addService(GET, "/pinControl/off", ledOff);
}

void loop() {
  WebService* service;
  
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    char line[256];
    int nBytes = 0;
    
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n' && currentLineIsBlank) {
          Serial.print("HEADER: ");
          Serial.println(header.getResource());
          if (header.getContentLength()) {
            int contentLength = header.getContentLength();
            Serial.println("DATA");
            for (int i = 0; i < contentLength; i++) {
              char c = client.read();
              Serial.write(c);
            }
          }

          service = appServer.fetchService(header.getVerb(), header.getResource());
          
          if (service)
            service->callback(&client, &header, NULL);
          else {
            client.println("Unable to find web service you requested");
            delay(1);
            client.stop();
          }
            
          header.reset();
        }
        else if (c == '\n') {
          line[nBytes] = 0;
          header.consumeLine(line);
          nBytes = 0;
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          if (nBytes < 256)
            line[nBytes++] = c;
          currentLineIsBlank = false;
        }
      }
    }
  }
}



