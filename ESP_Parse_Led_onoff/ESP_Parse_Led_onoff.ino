
#include <ESP8266WiFi.h> //Esp8266 Wifi kütüphanesi
#include <PubSubClient.h> //MQTT için Publisher ve Subscriber Kütüphanesi
#include <OneWire.h> // OneWire kütüphanesini ekliyoruz.

#define wifi_ssid "Ozan iPhone'u"
#define wifi_password ""
#define mqtt_server ""
#define mqtt_user ""
#define mqtt_password ""
#define topic1 "ozann/toptopic"
#define topic2 "ab2017/temperature"
#define topic3 "led" // sub işlemi topic3 üzerinden yapılıyor
#define readSize 6
int i=0;

void callback(char* topic, byte* payload, unsigned int length);
WiFiClient espClient;
PubSubClient client(mqtt_server, 11633, callback, espClient);

// Ledi 12. pine bağlıyoruz
int ledPin = 2; // ESP8266 de D4 ya karşılık geliyor 
int ledPin1 = 0;  // ESP8266 de D3 ya karşılık geliyor 
// Alınan veriyi 0. pine bağlıyoruz
int receivedData = 0; 
int receivedData2 = 0;

void setup() {

  // Ledi bağladığımız 12. pin için çıkış tanımlaması
  pinMode(ledPin, OUTPUT);
  pinMode(ledPin1, OUTPUT);
  //Arduino üzerinde kullandığımız Serial portun Baud Rate 'ini 115200 olarak ayarlıyoruz
  Serial.begin(115200);
  // Wifi kurulumu için oluşturduğumuz fonksiyonu çağırıyoruz
  setup_wifi();
  // Topic3 ten gelen veriyi subscribe ediyor
  client.subscribe(topic3);
  
}

// Alınan veriyi yazmak için callback fonksiyonu tanımlıyoruz.
void callback(char* topic, byte* payload, unsigned int length) {
  char rxBuffer[readSize]; //Gelen datayı okumak için bir Buffer dizisi tanımlıyoruz.
  Serial.println();
  Serial.print("Incoming data: "); //Gelen datayı seri port üzerinden yolla.
  Serial.println(topic);
  for (int i = 0; i < readSize; i++) { //Gelen datayı diziye kaydet.
    if (payload[i] == '$') { //Gelen datayı parse etmek için kullanılan koşul.
      rxBuffer[0] = payload[i+1];
      rxBuffer[1]= payload[i+2];
     receivedData = atoi(rxBuffer);
      }
     if (payload[i] == '>') {
      rxBuffer[0] = payload[i+1];
      rxBuffer[1]= payload[i+2];
      receivedData2 = atoi(rxBuffer);
      //Parse edilmiş gelen datayı seri port üzerinden yolla.
     }
  }
  //Serial.print(rxBuffer);
 //Buffer daki datayı int e çevir.
 Serial.print("receivedData1: ");
 Serial.println(receivedData);
 Serial.println("receivedData2: ");
 Serial.print(receivedData2);

}

void setup_wifi() {  // WIFI ye bağlanmak için oluşturulmuş fonksiyon.
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}

void reconnect() { //Eğer cihaz server a bağlanamazsa sürekli bağlanmayı denemesi için yazılmış fonksiyon.
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("TestMQTT", mqtt_user, mqtt_password)) {
      Serial.println("connected");
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }

}

void pubMQTT(String topic, float topic_val) { //MQTT üzerinden bir datayı yayımlamak için gerekli fonksiyon.
  Serial.print("Newest topic " + topic + " value:");
  Serial.println(String(topic_val).c_str());
  client.publish(topic.c_str(), String(topic_val).c_str(), true);
}

long lastMsg = 0;

int receivedLast = 0;

void loop() { // Sürekli döngü.
  if (!client.connected()) { //Eğer cihaz MQTT yer bağlı değilse bağlanıp okumayı dene.
    reconnect();
    client.subscribe(topic3);
  }

  client.loop();

  //2 seconds
  long now = millis(); 
  if (now - lastMsg > 5000) {
    lastMsg = now;
   
if(receivedData == 1) { digitalWrite(ledPin,HIGH); }
                   else digitalWrite (ledPin,LOW);

if(receivedData2 == 1) { digitalWrite(ledPin1,HIGH); }
                   else digitalWrite (ledPin1,LOW);
  }
}

