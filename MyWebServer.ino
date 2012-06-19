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
    char line[256];
    int nBytes = 0;
    
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        //Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          if (header.getContentLength()) {
            int contentLength = header.getContentLength();
            Serial.println("DATA");
            for (int i = 0; i < contentLength; i++) {
              char c = client.read();
              Serial.write(c);
            }
          }          
          
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connnection: close");
          client.println();
          client.println("<!DOCTYPE HTML>");
                    
          //Determine what webservice is being requested
          switch (header.getVerb()) {
          case GET:
            if (strcmp(header.getResource(), "/pinControl/pulse") == 0 && header.fetchQuery("repeat") && header.fetchQuery("pin") && header.fetchQuery("period")) {
              pin = atoi(header.fetchQuery("pin"));
              //pulseDelay = atoi(header.fetchQuery("pulseDelay"));
              int repeat = atoi(header.fetchQuery("repeat"));
              int period = atoi(header.fetchQuery("period"));
              
              pinMode(pin, OUTPUT);

              client.println("<html>");
              client.print("Request received to pulse pin ");
              client.print(header.fetchQuery("pin"));
              client.print(" ");
              client.print(header.fetchQuery("repeat"));
              client.print(" times.");
              client.println("<br />");
              client.println("</html>");
              
              delay(1);
              client.stop();
              
              for (int i = 0; i < repeat; i++) {
                delay(period/2);
                pinMode(pin, OUTPUT);
                digitalWrite(pin, HIGH);
                delay(period/2);
                digitalWrite(pin, LOW);
              }
            }
            else if (strcmp(header.getResource(), "/pinControl/on") == 0 && header.fetchQuery("pin")) {
              pin = atoi(header.fetchQuery("pin"));
              
              client.println("<html>");
              client.print("Request received to power on pin ");
              client.print(header.fetchQuery("pin"));
              client.println("<br />");
              client.println("</html>");
              
              pinMode(pin, OUTPUT);
              digitalWrite(pin, HIGH);
            }
            else if (strcmp(header.getResource(), "/pinControl/off") == 0 && header.fetchQuery("pin")) {
              pin = atoi(header.fetchQuery("pin"));
              
              client.println("<html>");
              client.print("Request received to power off pin ");
              client.print(header.fetchQuery("pin"));
              client.println("<br />");
              client.println("</html>");
              
              pinMode(pin, OUTPUT);
              digitalWrite(pin, LOW);
            }
            else {
              client.println("Unable to find web service you requested");
              // give the web browser time to receive the data
              delay(1);
              // close the connection:
              client.stop();
            }
          case POST:
            client.println("Unable to find web service you requested");
            delay(1);
            client.stop();
            break;
          case PUT:
            client.println("Unable to find web service you requested");
            delay(1);
            client.stop();
            break;
          case DELETE:
            client.println("Unable to find web service you requested");
            delay(1);
            client.stop();
            break;
          default:
            client.println("Unable to find web service you requested");
            delay(1);
            client.stop();
            break;
          };
          
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
          if (nBytes < 256)
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



