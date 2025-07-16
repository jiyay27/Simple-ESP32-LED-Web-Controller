#include <WiFi.h>

const char* ssid = "S24U";
const char* password = "yayayayaya";

// Define LED pins
const int ledPins[5] = {15, 2, 32, 33, 25};
bool ledStates[5] = {false, false, false, false, false};
bool customState[2] = {false, false};

WiFiServer server(80); 

// Xmas lights mode
void customLED1() {
  // part 1
  for (int r = 0; r < 8; r++) {
    for (int i = 0; i < 5; i++) { 
      digitalWrite(ledPins[i], HIGH);
    }
    delay(300);
    for (int i = 0; i < 5; i++) { 
      digitalWrite(ledPins[i], LOW);
    }
    delay(300);  
  }

  // part 2
  for (int r = 0; r < 8; r++) {
    for (int i = 0; i <= 5; i=i+2) { 
      digitalWrite(ledPins[i], HIGH);
    }
    for (int i = 1; i <= 5; i=i+2) { 
      digitalWrite(ledPins[i], LOW);
    }
    delay(300);
    for (int i = 0; i <= 5; i=i+2) { 
      digitalWrite(ledPins[i], LOW);
    }
    for (int i = 1; i <= 5; i=i+2) { 
      digitalWrite(ledPins[i], HIGH);
    }
    delay(300);
  }

  // part 3
  for (int r = 0; r < 8; r++) {
    for (int i = 0; i < 3; i++) { 
      digitalWrite(ledPins[i], HIGH);
    }
    for (int i = 3; i < 5; i++) { 
      digitalWrite(ledPins[i], LOW);
    }
    delay(200);
    for (int i = 0; i < 3; i++) { 
      digitalWrite(ledPins[i], LOW);
    }
    for (int i = 3; i < 5; i++) { 
      digitalWrite(ledPins[i], HIGH);
    }
    delay(200);
  }

  for (int i = 0; i < 5; i++) { 
    digitalWrite(ledPins[i], LOW);
  }

  // part 4
  for (int r = 0; r < 12; r++) {
    digitalWrite(ledPins[2],HIGH);
    delay(100);
    digitalWrite(ledPins[2],LOW);
    delay(100);
    digitalWrite(ledPins[1],HIGH);
    digitalWrite(ledPins[3],HIGH);
    delay(100);
    digitalWrite(ledPins[1],LOW);
    digitalWrite(ledPins[3],LOW);
    delay(100);
    digitalWrite(ledPins[0],HIGH);
    digitalWrite(ledPins[4],HIGH);
    delay(100);
    digitalWrite(ledPins[0],LOW);
    digitalWrite(ledPins[4],LOW);
    delay(100);
    digitalWrite(ledPins[1],HIGH);
    digitalWrite(ledPins[3],HIGH);
    delay(100);
    digitalWrite(ledPins[1],LOW);
    digitalWrite(ledPins[3],LOW);
    delay(100);
  }

  for (int i = 0; i < 5; i++) { 
    digitalWrite(ledPins[i], HIGH);
  }
}

// Binary counter mode
void customLED2() {
  for (int counter = 0; counter < 32; counter++) {
    for (int i = 0; i < 5; i++) {
      bool bit = (counter >> i) & 1;
      digitalWrite(ledPins[4 - i], bit ? HIGH : LOW);
    }
    delay(300);
  }
}

// general LED off function
void off() {
  for (int i=0;i<5;i++) {
    digitalWrite(ledPins[i], LOW);
  }
}

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < 5; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("Client connected.");
    String request = client.readStringUntil('\r');
    Serial.println("Request: " + request);
    client.flush();

    // Check each LED for ON/OFF commands
    for (int i = 0; i < 5; i++) {
      String onCmd = "/LED" + String(i + 1) + "/ON";
      String offCmd = "/LED" + String(i + 1) + "/OFF";

      if (request.indexOf(onCmd) != -1) {
        ledStates[i] = true;
        digitalWrite(ledPins[i], HIGH);
      }
      if (request.indexOf(offCmd) != -1) {
        ledStates[i] = false;
        off();
      }
    }

    // HTTP response
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();

    // HTML Web Interface
    client.println("<!DOCTYPE html><html><head>");
    client.println("<meta name='viewport' content='width=device-width, initial-scale=1'>");
    client.println("<style>");
    client.println("body { font-family: Arial; text-align: center; padding: 20px; }");
    client.println(".btn { padding: 12px 28px; margin: 8px; font-size: 18px; border: none; border-radius: 8px; }");
    client.println(".on { background-color: green; color: white; }");
    client.println(".off { background-color: red; color: white; }");
    client.println("</style>");
    client.println("</head><body>");

    client.println("<h2>ESP32 Web Control - 5 LEDs</h2>");

    for (int i = 0; i < 5; i++) {
      client.print("<p>LED ");
      client.print(i + 1);
      client.print(" (GPIO ");
      client.print(ledPins[i]);
      client.print("): <strong>");
      client.print(ledStates[i] ? "ON" : "OFF");
      client.println("</strong></p>");

      if (!ledStates[i]) {
        client.print("<a href=\"/LED");
        client.print(i + 1);
        client.println("/ON\"><button class=\"btn on\">Turn ON</button></a>");
      } else {
        client.print("<a href=\"/LED");
        client.print(i + 1);
        client.println("/OFF\"><button class=\"btn off\">Turn OFF</button></a>");
      }

      client.println("<hr>");
    }
    
    String onCust1 = "/PATTERN1/ON";
    String offCust1 = "/PATTERN1/OFF";

    String onCust2 = "/PATTERN2/ON";
    String offCust2 = "/PATTERN2/OFF";

    if (request.indexOf(onCust1) != -1) {
        customState[0] = true;
    }
    if (request.indexOf(offCust1) != -1) {
        customState[0] = false;
        off();
    }
    if (request.indexOf(onCust2) != -1) {
        customState[1] = true;
    }
    if (request.indexOf(offCust2) != -1) {
        customState[1] = false;
        off();
    }

    // --------------------------------------------------- custom 1

    client.print("<p>LED Christmas Mode: ");
    client.print("<strong>");
    client.print(customState[0] ? "ON" : "OFF");
    client.println("</strong></p>");

    if (!customState[0]) {
          client.print("<a href=\"/PATTERN1");
          client.println("/ON\"><button class=\"btn on\">Turn ON</button></a>");
        } 
        else {
          client.print("<a href=\"/PATTERN1");
          client.println("/OFF\"><button class=\"btn off\">Turn OFF</button></a>");
        }

    client.println("<hr>");

    // -------------------------------------------------------------- custom 2

    client.print("<p>LED Binary Counter: ");
    client.print("<strong>");
    client.print(customState[1] ? "ON" : "OFF");
    client.println("</strong></p>");

    if (!customState[1]) {
          client.print("<a href=\"/PATTERN2");
          client.println("/ON\"><button class=\"btn on\">Turn ON</button></a>");
        } 
        else {
          client.print("<a href=\"/PATTERN2");
          client.println("/OFF\"><button class=\"btn off\">Turn OFF</button></a>");
       }
  
    client.println("<hr>");

    client.println("</body></html>");

    if (customState[0]) {
      customLED1(); // call custom function
    }
    
    if (customState[1]) {
      customLED2();  // call custom function
    }

    delay(1);
    client.stop();
    Serial.println("Client disconnected.");
  }
}
