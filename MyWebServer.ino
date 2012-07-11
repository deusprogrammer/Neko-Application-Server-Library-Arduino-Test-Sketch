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

#include <neko.h>

ApplicationServer appServer;

void* dim(EthernetClient* client, HTTPHeader* header, void* data) {
  // send a standard http response header
  client->println("HTTP/1.1 200 OK");
  client->println("Content-Type: text/html");
  client->println("Connnection: close");
  client->println();
  
  if (!header->fetchQuery("pin"))
    return NULL;
  
  int pin = atoi(header->fetchQuery("pin"));
  int period = 10;
  //float intensity = atof(header->fetchQuery("intensity"));
  
  pinMode(pin, OUTPUT);

  client->println("<html>");
  client->print("Request received to pulse pin ");
  client->print(header->fetchQuery("pin"));
  client->println("<br />");
  client->println("</html>");
              
  delay(1);
  client->stop();
              
  for (float intensity = 0; intensity < 1; intensity += 0.001) {
    delay(period*(1 - intensity));
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
    delay(period*(intensity));
    digitalWrite(pin, LOW);
  }
  
  return NULL;
}

void* pulseLed(EthernetClient* client, HTTPHeader* header, void* data) {
  // send a standard http response header
  client->println("HTTP/1.1 200 OK");
  client->println("Content-Type: text/html");
  client->println("Connnection: close");
  client->println();
  
  if (!header->fetchQuery("pin") || !header->fetchQuery("repeat") || !header->fetchQuery("period") || !header->fetchQuery("low") || !header->fetchQuery("high"))
    return NULL;
  
  int pin = atoi(header->fetchQuery("pin"));
  int repeat = atoi(header->fetchQuery("repeat"));
  int period = atoi(header->fetchQuery("period"));
  float high = atof(header->fetchQuery("high"));
  float low = atof(header->fetchQuery("low"));
  
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
    delay(period*low);
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
    delay(period*high);
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

  appServer.addService(GET, "/pinControl/pulse", pulseLed);
  appServer.addService(GET, "/pinControl/on", ledOn);
  appServer.addService(GET, "/pinControl/off", ledOff);
  appServer.addService(GET, "/pinControl/dim", dim);
}

void loop() {
  appServer.loop();
}



