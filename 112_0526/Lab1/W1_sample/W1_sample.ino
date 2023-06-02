#include <ssl_client.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include "SSD1306.h"
#include <WiFi.h>
#include <HTTPClient.h>


/*OLED 顯示設定*/
///
SSD1306  display();
///


/* 要連接的wifi熱點 */ 
///
const char* ssid ="";
const char* password ="";
///

/* 想透過http抓取的網頁 */
///
const char* url = "";
///

void setup()
{

  display.init();
  display.setFont(ArialMT_Plain_24); 

  
  /* Serial: uart0 : print on putty */
  Serial.begin(115200); 

  /* Wifi 連線 */
  ///
  WiFi.begin();
  ///
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("...");
  }
 
  Serial.print("WiFi connected with IP: ");
  Serial.println(WiFi.localIP());
  
 
}

/* 當setup結束後會開始跑loop() */
 
void loop()
{
  
    HTTPClient http;
    
    http.begin(url);
    int httpCode = http.GET();
    Serial.print("httpCode=");
    Serial.println(httpCode);

    if(httpCode == HTTP_CODE_OK){ // 確認連線是否正常
    
      String payload = http.getString(); // 網頁原始碼內容

      /* 透過ArduinoJson來解析 */
      DynamicJsonDocument doc(payload.length()*2);
      deserializeJson(doc,payload);

      /* 擷取想要看的資訊 */
      ///
      
      const double v  =  ;
      ///
      
      Serial.print("BTC : ");
      Serial.println(v);

      display.clear();
      display.drawString(0,0,"BTC :\n "+String(v));
      display.display();
      
      http.end();     
      
       
    }
    delay(3000);

}
