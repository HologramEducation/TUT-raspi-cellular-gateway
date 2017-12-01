#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHT_PIN D3
#define DHT_TYPE DHT11
#define LUM_PIN A0
#define GREEN_LED D1
#define RED_LED D2 

DHT dht(DHT_PIN, DHT_TYPE);

// Update these with values suitable for your network.
const char* ssid = "friendly-raspberry";
const char* password = "hologram";
const char* mqtt_server = "192.168.42.1";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[120];
int msgNum = 0;

void connect_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    if (WiFi.status() == WL_NO_SSID_AVAIL) {
      Serial.println();
      break;
    } else  {
      delay(1000);
      Serial.print(".");
    }
  }

  if(WiFi.status() == WL_CONNECTED) {
    randomSeed(micros());
  
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Network not found, trying again...");
  }
}

void setup_wifi() {
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, HIGH);

  while (WiFi.status() != WL_CONNECTED) {
    connect_wifi();
    delay(2000);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {

    if(WiFi.status() == WL_DISCONNECTED){
      Serial.print("Lost WiFi connection, attempting new connection..");
      setup_wifi();
    }
    
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      digitalWrite(GREEN_LED, HIGH);
      digitalWrite(RED_LED, LOW);
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(BUILTIN_LED, OUTPUT); 
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 10000) {
    lastMsg = now;
    ++msgNum;

    int iLum = analogRead(LUM_PIN); // read photoresistor value (brightness)
    float fHum = dht.readHumidity(); // Read humidity
    float fTemp = dht.readTemperature(true); // Read temperature as Fahrenheit (isFahrenheit = true)

    // Check if any reads failed and exit early (to try again).
    if (isnan(fHum) || isnan(fTemp)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    // convert sensor readings to strings
    char lum[8];
    char temp[8];
    char hum[8];
    dtostrf(iLum, 1, 0, lum);
    dtostrf(fTemp, 1, 2, temp);
    dtostrf(fHum, 1, 2, hum);

    // format data into a JSON string
    snprintf (msg, 120, "{\"msgNum\": %ld, \"temp\": %s, \"hum\": %s, \"lum\": %s}", msgNum, temp, hum, lum);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("environment/studio", msg);
  }
}