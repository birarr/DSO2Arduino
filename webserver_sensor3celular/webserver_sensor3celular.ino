/*********
 Trabalho para a disciplina de DSO2
*********/

// Including the ESP8266 WiFi library
#include <ESP8266WiFi.h>
#include <math.h>


// Wi Fi Credentials
const char* ssid = "pocobira";
const char* password = "biragato";

// Web Server on port 80
WiFiServer server(80);

const int B = 4275;               // B value of the thermistor
const int R0 = 100000;            // R0 = 100k
const int pinTempSensor = A0;     // Grove - Temperature Sensor connect to A0


// Runs once on boot
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  // Initializing serial port for debugging purposes
  Serial.begin(115200);
  delay(10);

  // Connecting to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Starting the web server
  server.begin();
  Serial.println("Web server running. Waiting for the ESP IP...");
  delay(10000);
  
  // Printing the ESP IP address
  Serial.println(WiFi.localIP());
}

// Infinity looping
void loop() {
  // Listenning for new clients
  WiFiClient client = server.available();
  
  if (client) {
    Serial.println("New client");
    // bolean to locate when the http request ends
    boolean blank_line = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        
        if (c == '\n' && blank_line) {
     
              // Printing the serial for debugging purposes
              
          
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");
            client.println();

            int a = analogRead(pinTempSensor);

    float R = 1023.0/a-1.0;
    R = R0*R;

    float temperature = 1.0/(log(R/R0)/B+1/298.15)-273.15; // convert to temperature via datasheet
    temperature = temperature - 32;
    temperature = temperature * 0.777;

    Serial.print("temperature = ");
    Serial.println(temperature);

    delay(1000);

     if(temperature > 31) {
  digitalWrite(LED_BUILTIN, LOW);
  
} else {
  digitalWrite(LED_BUILTIN, HIGH);
  
}
            // Web page data
            client.println("");
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            client.println("<head><meta http-equiv=\"refresh\" content=\"5\"></head><body><h1 style=\"color: #4279c7\">DSO2 - Sensor de Temperatura</h1>");
            client.print("<h3>Temperatura atual:<h3>");
            if(temperature < 31)
            {
            client.println("<p style=\"color:rgb(119,221,119)\">"); 
            client.println(temperature);
            client.print(char(176));
            client.println("C"); 
            client.println("</p>"); 
            client.println("Temperatura OK!");
            client.println("</body></html>");   
            }
            else
            {
            client.println("<p style=\"color:rgb(255,0,0)\">");
            client.println(temperature);
            client.print(char(176));
            client.println("C"); 
            client.println("</p>"); 
            client.println("Excesso de temperatura");
            client.println("</body></html>"); 
            }
            break;
        }
        if (c == '\n') {
          // when starts reading a new line
          blank_line = true;
        }
        else if (c != '\r') {
          // when finds a character on the current line
          blank_line = false;
        }
      }
    }  
    // closing the client connection
    delay(1);
    client.stop();
    Serial.println("Client disconnected.");
  }
}
