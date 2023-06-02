#include <ssl_client.h>
#include <WiFiClientSecure.h>
#include <TridentTD_LineNotify.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include "SSD1306.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <string.h>

SSD1306 display(0x3c,21,22);

/* 要連接的wifi熱點 */ 
///
const char* ssid = "Reno";
const char* password =  "a0907863836";
///

/* 想透過http抓取的網頁 */
const char* url = "https://tw.stock.yahoo.com/_td-stock/api/resource/StockServices.stockList;autoRefresh=1659720073465;fields=avgPrice,orderbook;symbols=BTC-USD?bkt=&device=desktop&ecma=modern&feature=ecmaModern,useNewQuoteTabColor&intl=tw&lang=zh-Hant-TW&partner=none&prid=3ec7fd9heqk4r&region=TW&site=finance&tz=Asia/Taipei&ver=1.2.1432&returnMeta=true";

/* Line notify */
char* host = "notify-api.line.me"; // Line Notify API
///
String Linetoken = "tCRd0zMadXVTHz0QToYviYJ2HidektaRKkcipOiujvq";//////填入token
///
String pre_v ="";

void setup()
{

  display.init();
  display.setFont(ArialMT_Plain_24); 

  /* Serial: uart0 : print on putty */
  Serial.begin(115200); 

  /* Wifi 連線 */
  WiFi.begin(ssid, password);
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
      String v = doc["data"][0]["price"];
 
      Serial.print("BTC : ");
      Serial.println(v);

      display.clear();
      display.drawString(0,0,"BTC :\n "+String(v));
      display.display();
      
      http.end();
      
      /* 設定傳送至line */
      WiFiClientSecure line_client;
      line_client.setInsecure();

     ///
     if(v!=pre_v){ // 輸入觸發條件
        pre_v=v;
        String message = "比特幣價格通知: ";

        /* 透過line api 來傳送訊息 */
        LINE.setToken(Linetoken);
        LINE.notify(message+v); // txt 訊息        
        //LINE.notifySticker(); // (txt)&貼圖
        delay(1000);
        
      }
    ///
    }
    delay(5000);
}
