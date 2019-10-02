#include <Bounce2.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h> // Requires ArduinoJson V5

Bounce debouncer = Bounce();

// WiFi Parameters
const char* ssid     = "ssid";
const char* password = "password";
const char* nanoleafIP = "192.168.1.xxx";
const char* apiKey = "your nanoleaf api key";
const int   port = 16021; //Don't change the port for Nanoleaf Panels

// Variables n stuff
String scene = "Critical"; // Change this to the name of the scene you want to trigger

int oldcritialstate = HIGH;
String nanoleafSelect;
String last_scene;
WiFiClient client;

// Pin stuff
const short int BUILTIN_LED2 = 16;//GPIO 16
int relayPin = 5; //GPIO 5


void setup() {
  pinMode(relayPin, INPUT_PULLUP);
  pinMode(BUILTIN_LED2, OUTPUT); // Initialize the BUILTIN_LED2 pin as an output
  digitalWrite(BUILTIN_LED2, HIGH); // Turn the LED off by making the voltage HIGH

  // After setting up the button, setup the Bounce instance. We are using a relay, so debouncing is needed.
  debouncer.attach(relayPin);
  debouncer.interval(5); // interval in ms

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting...");
    delay(500);
  }

  // We have to grab the current state of the lights only once at bootup because it takes forever (5-10s
  // for the ESP to receive the return from the request for some reason.
  Serial.println("Getting light status from API...");
  getNanoleaf();
  Serial.print("Current scene is: ");
  Serial.println(String(nanoleafSelect));
  // Flash the onboard LED so we know that we are ready to proceed
  for (int i = 0; i < 5; i++) {
    digitalWrite(BUILTIN_LED2, LOW);
    delay(100);
    digitalWrite(BUILTIN_LED2, HIGH);
    delay(100);
  }
  Serial.println("Ready!");
}


void loop() {

  debouncer.update();
  int criticalstate = debouncer.read();

  if (criticalstate != oldcritialstate) { //if state changed (high to low or low to high)
    if (criticalstate == 0) { //if relay is closed
      getNanoleaf(); // -------------------------------- If your lights are slow to react, comment out this line. 
      Serial.println("Relay is on, changing scene...");
      oldcritialstate = criticalstate;
      putNanoleaf(scene);
    }
  }

  //Restore to original state
  if (criticalstate != oldcritialstate) { //if state changed (high to low or low to high)
    if (criticalstate == 1) { //if relay is open
      Serial.println("Restoring Scene...");
      putNanoleaf(nanoleafSelect);
      oldcritialstate = criticalstate;
    }
  }
}


void putNanoleaf(String scene) {
  const size_t capacity = JSON_OBJECT_SIZE(1);
  DynamicJsonBuffer jsonBuffer(capacity);

  JsonObject& root = jsonBuffer.createObject();
  root["select"] = scene;
  String nanoPUT;
  root.printTo(nanoPUT);
  Serial.print("nanoPUT: ");
  Serial.println(nanoPUT);

  // Send the PUT request
  if (client.connect(nanoleafIP, port)) {
    client.println(String("PUT /api/v1/") + apiKey + "/effects" + " HTTP/1.1");

    client.print("Host: ");
    client.println(nanoleafIP);
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(nanoPUT.length());
    client.println();
    client.println(nanoPUT);
    client.println("Connection: Keep-Alive");
  }
}


void getNanoleaf() {
  if (client.connect(nanoleafIP, port)) {
    client.println(String("GET /api/v1/") + apiKey + " HTTP/1.1");
    client.print("Host: ");
    client.println(nanoleafIP);
    client.println("Connection: Keep-Alive");
    client.println();
    client.findUntil("\"select\":\"", "\0");
    nanoleafSelect = client.readStringUntil('\"');  // if light is on, set variable to true
  }
}
