

#include <ArduinoJson.h>
#include <IFTTTMaker.h>

/******************************************************************************
PIR_Motion_Detector_Example.ino
Example sketch for SparkFun's PIR Motion Detector
  (https://www.sparkfun.com/products/13285)
Jim Lindblom @ SparkFun Electronics
May 2, 2016

The PIR motion sensor has a three-pin JST connector terminating it. Connect
the wire colors like this:
- Black: D2 - signal output (pulled up internally)
- White: GND
- Red: 5V

Connect an LED to pin 13 (if your Arduino doesn't already have an LED there).

Whenever the PIR sensor detects movement, it'll write the alarm pin LOW.

Development environment specifics:
Arduino 1.6.7
******************************************************************************/
const int MOTION_PIN = 2; // Pin connected to motion detector
const int LED_PIN = 6; // LED pin - active-high

//#include <IFTTTWebhook.h>
#include <ArduinoHttpClient.h>
#include <WiFi101.h>
#include "arduino_secrets.h"

//////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;


// Name of the server we want to connect to
const char kHostname[] = "maker.ifttt.com";
// Path to download (this is the bit after the hostname in the URL
// that you want to download
const char kPath[] = "/trigger/hallway_movement/with/key/ENTER KEY HERE";
const char kPath_off[] = "/trigger/hallway_clear/with/key/ENTER KEY HERE";

// Number of milliseconds to wait without receiving any data before we give up
const int kNetworkTimeout = 30*1000;
// Number of milliseconds to wait if no data is available before trying again
const int kNetworkDelay = 1000;

WiFiClient c;
HttpClient http(c, kHostname);

WiFiClient wifi;
int status = WL_IDLE_STATUS;
String response;
int statusCode = 0;

void setup() 

{
 Serial.begin(9600);
 while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
  }

  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
 
  // The PIR sensor's output signal is an open-collector, 
  // so a pull-up resistor is required:
  pinMode(MOTION_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  Serial.println("Motion detection active...");
}

void loop() 
{
                                    
  int proximity = digitalRead(MOTION_PIN);
  if (proximity == HIGH) // If the sensor's output goes low, motion is detected
  {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("Motion detected!");
    Serial.println("making POST request...");

{
  int err =0;
  
  err = http.get(kPath);
  if (err == 0)
  {
    Serial.println("startedRequest ok");

    err = http.responseStatusCode();
    if (err >= 0)
    {
      Serial.print("Got status code: ");
      Serial.println(err);

      // Usually you'd check that the response code is 200 or a
      // similar "success" code (200-299) before carrying on,
      // but we'll print out whatever response we get

      // If you are interesting in the response headers, you
      // can read them here:
      //while(http.headerAvailable())
      //{
      //  String headerName = http.readHeaderName();
      //  String headerValue = http.readHeaderValue();
      //}

      int bodyLen = http.contentLength();
      //Serial.print("Content length is: ");
      Serial.println(bodyLen);
      Serial.println();
      Serial.println("Body returned follows:");
    
      // Now we've got to the body, so we can print it out
      unsigned long timeoutStart = millis();
      char c;
      // Whilst we haven't timed out & haven't reached the end of the body
      while ( (http.connected() || http.available()) &&
             (!http.endOfBodyReached()) &&
             ((millis() - timeoutStart) < kNetworkTimeout) )
      {
          if (http.available())
          {
              c = http.read();
              // Print out this character
              Serial.print(c);
             
              // We read something, reset the timeout counter
              timeoutStart = millis();
          }
          else
          {
              // We haven't got any data, so let's pause to allow some to
              // arrive
              delay(kNetworkDelay);
          }
      }
    }
    else
    {    
      Serial.print("Getting response failed: ");
      Serial.println(err);
    }
  }
  else
  {
    Serial.print("Connect failed: ");
    Serial.println(err);
  }
  Serial.println();
  Serial.println("Leaving ON for 30 seconds");
  delay(30000);
  http.get(kPath_off);
  Serial.println();
  Serial.println("Motion no longer detected... Switching off light");
  http.stop();

} 


  Serial.println();
  Serial.println("Wait two seconds before reading sensor again...");
  Serial.println();
  delay(2000);
  Serial.println("Motion detection active...");
  }
  else
  {
    digitalWrite(LED_PIN, LOW);
  }
}

