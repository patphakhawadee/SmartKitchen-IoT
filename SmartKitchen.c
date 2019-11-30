//SmartHomeA2

void Line_Notify(String message) ;
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <dummy.h>
#include <DHT.h>
#include <DHT_U.h>

//Token line
#define LINE_TOKEN "XiNxB4urqAa32igFDOCWu3wFYscgRnvt2y3OBe4ofBR"
//Token Blynk 
char auth[] = "71686eb35ea0403eb006aae06fb4435e";

String message = "%E0%B9%81%E0%B8%88%E0%B9%89%E0%B8%87%E0%B9%80%E0%B8%95%E0%B8%B7%E0%B8%AD%E0%B8%99%E0%B9%81%E0%B8%81%E0%B9%8A%E0%B8%AA%E0%B8%AB%E0%B8%A3%E0%B8%B7%E0%B8%AD%E0%B8%84%E0%B8%A7%E0%B8%B1%E0%B8%99%E0%B9%80%E0%B8%81%E0%B8%B4%E0%B8%99%E0%B8%81%E0%B8%B3%E0%B8%AB%E0%B8%99%E0%B8%94!!!!!";
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Smartkitchen";
char pass[] = "00000000";

//Type on NodeMCU
int buzzer = D1;
int smokeA0 = A0;
int PIR = D0;
int Led = D3;
int sensorThres = 200;


#include "DHT.h"

#define DHTPIN D2     // what digital pin we're connected to
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
BlynkTimer timer;
DHT dht(DHTPIN, DHTTYPE);

//Show Humidity and Temperature
void myTimerEvent()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) 
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  else
  {
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.println(t);
    Blynk.virtualWrite(V1, h); 
    Blynk.virtualWrite(V2, t);
  }
}

//-----------------------------------------------------------------------------------------
//Connect WIFI on NodeMCU
void setup()
{
  // Debug console
  Serial.begin(115200);

  Blynk.begin(auth, ssid, pass);
  Serial.begin(115200);
  Serial.println("?????????????");
  pinMode(PIR, INPUT);
  pinMode(smokeA0, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(Led, OUTPUT);
  dht.begin();
  timer.setInterval(2000L, myTimerEvent);
  WiFi.mode(WIFI_STA);
  // connect to wifi.
  WiFi.begin(ssid, pass);
  Serial.print("connecting");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

}

//-------------------------------------------------------------------------------------------
void loop()
{
  Blynk.run();
  int analogSensor = analogRead(smokeA0);

  Serial.print("????????????????? : ");
  Serial.println(analogSensor);
 
   if (analogSensor > sensorThres)
  {
   tone(buzzer, 1000, 200); 
   Line_Notify(message);
  }
  else
  {
    noTone(buzzer);
  }
  int value = digitalRead(PIR);
  //if its high or if an any object is detected it will activate the LED and Buzzer
  if (value == HIGH){
    digitalWrite(Led, HIGH);
     Blynk.virtualWrite(V3, Led);
    //digitalWrite(buzzer, HIGH);
  }
  else {
    digitalWrite(Led, LOW);
    //digitalWrite(buzzer, LOW);
  }
  // Checks if it has reached the threshold value
  delay(1000);
  
 
  timer.run(); // Initiates BlynkTimer

}

//-------------------------------------------------------------------
//Connection Line_Notify
void Line_Notify(String message) {
  WiFiClientSecure client;

  if (!client.connect("notify-api.line.me", 443)) {
    Serial.println("connection failed");
    delay(2000);
    return;
  }

  String req = "";
  req += "POST /api/notify HTTP/1.1\r\n";
  req += "Host: notify-api.line.me\r\n";
  req += "Authorization: Bearer " + String(LINE_TOKEN) + "\r\n";
  req += "Cache-Control: no-cache\r\n";
  req += "User-Agent: ESP8266\r\n";
  req += "Content-Type: application/x-www-form-urlencoded\r\n";
  req += "Content-Length: " + String(String("message=" + message).length()) + "\r\n";
  req += "\r\n";
  req += "message=" + message;
  // Serial.println(req);
  client.print(req);

  delay(20);

  // Serial.println("-------------");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
    //Serial.println(line);
  }
  // Serial.println("-------------");
}
