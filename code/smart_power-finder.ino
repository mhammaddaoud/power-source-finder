// Load Wi-Fi library
#include <ESP8266WiFi.h>

// Replace with your network credentials
const char* ssid     = "SSID";
const char* password = "PWD";
IPAddress ip(192, 168, 0, 150); //set static ip
IPAddress gateway(192, 168, 0, 1); //set getteway
IPAddress subnet(255, 255, 255, 0);//set subnet

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

void setup() {
  Serial.begin(115200);

  pinMode(D5, INPUT);
  pinMode(D6, INPUT);
  // Set outputs to LOW
  digitalWrite(D5, LOW);
  digitalWrite(D6, LOW);

  // Connection to wireless network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  server.begin();
  Serial.println("Server started");

  // Print the IP address in serial monitor. It must be the same we entered above
  Serial.print("Type this address in URL to connect: ");
  Serial.print("http://");
  Serial.println(ip);
  Serial.println("/");
  Serial.println(digitalRead(D5));
  Serial.println(digitalRead(D6));
}

void loop() {
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<meta http-equiv=""refresh"" content=""10"">");
            client.println("<link rel=\"icon\" href=\"data:,\">");

            // Web Page Heading
            client.println("<body><h1>Smart Home</h1>");
            Serial.println(digitalRead(D5));
            Serial.println(digitalRead(D6));

            if ((digitalRead(D5) == HIGH) && (digitalRead(D6) == LOW)) {
              client.println("GOVERNMENT electricity");
              Serial.println("D5 is high gov");
            }
            else if ((digitalRead(D5) == LOW) && (digitalRead(D6) == HIGH)) {
              client.println("GENERATOR electricity");
              Serial.println("D6 high generator");
            }
            else if ((digitalRead(D5) == LOW) && (digitalRead(D6) == LOW)) {
              client.println("NO electricity");
              Serial.println("D5-6 low");
            }
            else if ((digitalRead(D5) == HIGH) && (digitalRead(D6) == HIGH)) {
              client.println("!!EROR!!");
              Serial.println("D5-6 high");
            }

            client.println("</body></html>");

            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
