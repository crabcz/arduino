// ethernet
#include <SPI.h>
#include <EthernetENC.h>
#include <PubSubClient.h>

//#define DHTPIN 8  
//#define DHTTYPE DHT11 
#define DEBUG 1 // Debug output to serial console


// debug 
const unsigned int BAUD_RATE=9600;
unsigned long mytime = 0;

// sensor
const unsigned int TRIG_PIN=6; //RX
const unsigned int ECHO_PIN=7;  //TX


//ethernet
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

//char buf[4]; // Buffer to store the sensor value
int updateInterval = 20000; // Interval in milliseconds


IPAddress ip(192, 168, 0, 21); // put your IP fallback here
IPAddress myDns(192, 168, 0, 1); // put your router IP here
IPAddress server(192, 168, 0, 3); // put your MQTT server here

EthernetClient ether;
PubSubClient client(ether);

void reconnect() {
//   Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClientSuperior", "", "")) {
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


float getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);  
  const unsigned long duration= pulseIn(ECHO_PIN, HIGH);
  return duration/29/2;
}

void sensors() {
//        int sensorValue = analogRead(sensorPin);
//        float h = dht.readHumidity();
//        float t = dht.readTemperature();
          float distance = getDistance();

          client.publish("arduino/water/distance", String(distance).c_str(), true);
//        client.publish("home-assistant/sensor02/humidity", String(h).c_str(), true);


//        float e = pzem.energy(i);                                          //energia     
//        float p = pzem.power(i);                                           //potenza 
//        float c = pzem.current(i);                                          //energia     
//        float v = pzem.voltage(i);                                           //potenza
                     
//        client.publish("home-assistant/sensor03/voltage", String(v).c_str(), true);
//        client.publish("home-assistant/sensor03/energy", String(e).c_str(), true);
//        client.publish("home-assistant/sensor03/power", String(p).c_str(), true);
//        client.publish("home-assistant/sensor03/current", String(c).c_str(), true);
         
          #if DEBUG
            Serial.print("Distance value: ");
            Serial.println(distance);
//            Serial.print("HuM value value: ");
//            Serial.println(h);
//            Serial.print("Temp value value: ");
//            Serial.println(t);
//            if(e >= 0.0){ Serial.print(e);Serial.print("Wh; "); }
//            if(p >= 0.0){ Serial.print(p);Serial.print("W; "); }
//            if(c >= 0.0){ Serial.print(c);Serial.print("A; "); }
//            if(v >= 0.0){ Serial.print(v);Serial.print("V; "); }  
            Serial.println();
          #endif   
}


void setup() {
  Serial.begin(BAUD_RATE);
  // start the Ethernet connection:
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
  } else {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  client.setServer(server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if (millis() - mytime > updateInterval) { 
    mytime = millis(); 
    sensors();
  }
}
