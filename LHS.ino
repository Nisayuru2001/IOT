#define BLYNK_TEMPLATE_ID "TMPL66pcc14L9"
#define BLYNK_TEMPLATE_NAME "Lecture Hall Air and Light Control System"
#define BLYNK_AUTH_TOKEN "Nf06c2c7ihoffRCt8SdJCo99usit-jXb"

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>  
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "iphone";  // Enter your Wi-Fi SSID
char pass[] = "12345678";  // Enter your Wi-Fi password
#define DHTPIN D4      // DHT sensor pin
#define DHTTYPE DHT11   // DHT sensor type
DHT dht(DHTPIN, DHTTYPE);

int relaypin = D1;
int smokeA0 = A0;

BlynkTimer timer;

void sendSensor() {
  int Gas = analogRead(smokeA0); // Read analog value from pin A0

  Blynk.virtualWrite(V3, Gas); // Send raw analog value to Blynk
  Serial.print("Gas Value: ");
  Serial.println(Gas);

  if (Gas > 400) {
    Blynk.logEvent("gas_alert", "Gas Leakage Detected");
  }
}

void setup() {
  Serial.begin(115200); // or change to desired baud rate
  Serial.println("Starting...");

  int retryCount = 0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to Wi-Fi...");
    WiFi.begin(ssid, pass);
    delay(2000); // Wait for connection
    retryCount++;
    if (retryCount > 5) {
      Serial.println("Max retries exceeded. Check Wi-Fi credentials.");
      break;
    }
  }

  Blynk.begin(auth, ssid, pass);

  dht.begin();
  pinMode(relaypin, OUTPUT);
  pinMode(smokeA0, INPUT);

  timer.setInterval(2500L, sendSensor);
}

BLYNK_WRITE(V2) {  // Function to receive commands from Blynk app to control LED
  int relayState = param.asInt();  // Get the value from the Blynk app (0 or 1)
  digitalWrite(relaypin, relayState); // Turn the relay on or off based on the value received
}

void loop() {
  if (Blynk.connected()) {
    Blynk.run();
  } else {
    Serial.println("Lost connection to Blynk server. Reconnecting...");
    if (Blynk.connect() == true) {
      Serial.println("Reconnected to Blynk server.");
    } else {
      Serial.println("Failed to reconnect to Blynk server. Check network connection.");
    }
  }

  // Read sensor data
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  
  // Check if sensor reading is valid
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    Serial.print("Humidity: ");
    Serial.println(humidity);
    Serial.print("Temperature: ");
    Serial.println(temperature);
    return;
  }

  // Print sensor data
  Serial.print("Humidity: ");
  Serial.println(humidity);
  Serial.print("Temperature: ");
  Serial.println(temperature);

  // Send sensor data to Blynk app
  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidity);

  // Call the sendSensor function to update gas data
  sendSensor();

  delay(2000); // Delay between sensor readings and Blynk updates
}
