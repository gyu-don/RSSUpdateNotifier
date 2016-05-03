#include <ESP8266WiFi.h>

#include "config.h"
#include "datetime.h"
extern "C" {
  #include "user_interface.h"
}

DateTime latest;
os_timer_t timer;
bool request_for_update = false;

bool get_latest_datetime(const char* host, int port, const char* url, DateTime& dt)
{
  for(int i=0;i<n_retry;i++){
    WiFiClient client;
    int pubdate_pos;

    if(i){
      delay(d_retry);
      Serial.println("Retry.");
    }
    if(!client.connect(host, port)) {
      Serial.println("connection failed");
      continue;
    }

    Serial.print("Requesting URL: ");
    Serial.print(host);
    if(port != 80){
      Serial.print(String(":") + String(port));
    }
    Serial.println(url);
  
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" + 
                 "Connection: close\r\n\r\n");
    delay(500);
    while(client.available()){
      String line = client.readStringUntil('\n');
      pubdate_pos = line.indexOf("<pubDate>");
      if(pubdate_pos != -1){
        Serial.println("pubDate is found.");
        String date = line.substring(pubdate_pos + 9);
        if(str_to_datetime(date, dt)){
          Serial.println("Succeed. Received DateTime is");
          Serial.println(dt.toString());
          Serial.println();
          return true;
        }
        else{
          Serial.println("malformed or unsupported RSS.");
        }
      }
    }
    Serial.println("pubDate is not found. It may received non-RSS file.");
  }
  Serial.println("Failed.");
  Serial.println();
  return false;
}

void make_true_boolean_flag(void *flag)
{
  *reinterpret_cast<bool*>(flag) = true;
}

void setup()
{
  pinMode(led, OUTPUT);
  pinMode(ack, INPUT_PULLUP);
  Serial.begin(9600);
  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  if(!get_latest_datetime(host, port, url, latest)) latest.year = 0;

  os_timer_setfn(&timer, make_true_boolean_flag, &request_for_update);
  os_timer_arm(&timer, check_interval, true);
}

void loop()
{
  if(digitalRead(ack) == HIGH){
    digitalWrite(led, LOW);
  }
  if(request_for_update){
    request_for_update = false;
    
    DateTime checked;
    bool is_succeed = get_latest_datetime(host, port, url, checked);
    if(is_succeed){
      if(latest.year){  // latest.year is assigned 0 in setup() if get_latest_datetime was failed.
        if(latest < checked){
          latest = checked;
          digitalWrite(led, HIGH);
        }
      }
      else{
        latest = checked;
      }
    }
    else{
      Serial.println("Failed to check RSS.");
    }
  }
}

