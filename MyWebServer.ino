//#include <Event.h>
//#include <Timer.h>

#include <httpHeader.h>

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
 
 */

#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,137,200);

int lastId = -1;
int pin = 7;
int mode = 0;
int pulseDelay = 0;
HTTPHeader header;
//Timer t;

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server(80);

//Function for flashing an LED.
void flashLed() {
  //if (mode == 1)
    //t.pulse(pin, 1000, HIGH);
}

void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
  
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  
  //pinMode(pin, OUTPUT);
  
  //lastId = t.every(pulseDelay, flashLed);
}

void loop() {
  // update timer
  //t.update();
  
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    char line[1024];
    int nBytes = 0;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        //Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connnection: close");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          
          //Determine what webservice is being requested
          if (strcmp(header.getResource(), "/ledBlink") == 0 && header.fetchQuery("repeat") && header.fetchQuery("pin")) {
            pin = atoi(header.fetchQuery("pin"));
            //pulseDelay = atoi(header.fetchQuery("pulseDelay"));
            int repeat = atoi(header.fetchQuery("repeat"));
            
            pinMode(pin, OUTPUT);
            mode = 1;
            //if (lastId >= 0)
              //t.stop(lastId);
            //lastId = t.every(pulseDelay, flashLed);
            
            client.print("Request received to pulse LED on pin ");
            client.print(header.fetchQuery("pin"));
            client.print(" ");
            client.print(header.fetchQuery("repeat"));
            client.print(" times.");
            client.println("<br />");
            client.println("</html>");
            
            // give the web browser time to receive the data
            delay(1);
            // close the connection:
            client.stop();
            
            for (int i = 0; i < repeat; i++) {
              pinMode(pin, OUTPUT);
              digitalWrite(pin, LOW);
              delay(1000);
              digitalWrite(pin, HIGH);
              Serial.println(i);
            }
          }
          else {
            client.println("Unable to find web service you requested");
            // give the web browser time to receive the data
            delay(1);
            // close the connection:
            client.stop();

          }
          
          header.reset();
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          line[nBytes] = 0;
          header.consumeLine(line);
          nBytes = 0;
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          if (nBytes < 1024)
            line[nBytes++] = c;
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
  }
}



