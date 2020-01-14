
#if defined(ESP8266) 

#include <ESP8266WiFi.h> //ESP8266WiFi.h .- ESP8266 Core WiFi Library
#else
#include < WiFi.h >  //WiFi.h .- ESP32 Core WiFi Library
#endif
 
#include <Adafruit_AMG88xx.h>
#if defined(ESP8266)
 
#include <ESP8266WebServer.h> //ESP8266WebServer.h .- Servidor web local utilizado para servir el portal de configuración
 
#else
 
#include < WebServer.h > //WebServer.h .- Servidor DNS local utilizado para redireccionar todas las solicitudes al portal de configuración (https://github.com/zhouhan0126/DNSServer---esp32)
#endif
 
#include <DNSServer.h>//DNSServer.h .- Local WebServer usado para servir el portal de configuración (https://github.com/zhouhan0126/DNSServer---esp32)
#include <WiFiManager.h> //WiFiManager.h .- WiFi Configuration Magic (https://github.com/zhouhan0126/DNSServer---esp32) >> https://github.com/zhouhan0126/DNSServer---esp32 (ORIGINAL)

const int PIN_AP = 2; // pulsador para volver al modo AP

//dATOS RELEVANTES dE API
#include <ESP8266HTTPUpdateServer.h>

ESP8266WebServer webServer(80); //port webServer
ESP8266HTTPUpdateServer httpUpdateServer;

Adafruit_AMG88xx amg;

float pixels[AMG88xx_PIXEL_ARRAY_SIZE]; //ARRAY DE AMG8833
 
 
void configModeCallback (WiFiManager *myWiFiManager) {
 Serial.println("Modo de configuración ingresado");
 Serial.println(WiFi.softAPIP());
 
 Serial.println(myWiFiManager->getConfigPortalSSID());
}
 
//flag for saving data
bool shouldSaveConfig = false;
 
// En https://github.com/tzapu/WiFiManager
//callback notifying us of the need to save config
 
void saveConfigCallback () {
 Serial.println("Debería guardar la configuración");
 shouldSaveConfig = true;
}
 
void setup() {
 Serial.begin(9600);
 pinMode(PIN_AP, INPUT);
 //declaración de objeto wifiManager
 WiFiManager wifiManager;
 
 // utilizando ese comando, como las configuraciones se apagarán en la memoria
 // en caso de que la redacción se conecte automáticamente, ella é apagada.
 // wifiManager.resetSettings();
 
 //devolución de llamada para cuando entra en el modo de configuración AP
 wifiManager.setAPCallback(configModeCallback);
 //devolución de llamada cuando se conecta a una red, es decir, cuando pasa a trabajar en modo EST
 wifiManager.setSaveConfigCallback(saveConfigCallback);
 
 //crea una red de nombre ESP_AP con pass 12345678
 wifiManager.autoConnect("Makeit_Lab_Api", "arduino1234");

 if ( digitalRead(PIN_AP) == HIGH ) {
  Serial.println("reajustar"); //resetear intenta abrir el portal
  if(!wifiManager.startConfigPortal("conect AP", "arduino1234") ){
   Serial.println("No se pudo conectar");
   delay(2000);
   ESP.restart();
   delay(1000);
  }
  Serial.println("conectado ESP_AP!!!");
 }

 //CONFIGURACION CALLBACK

 httpUpdateServer.setup(&webServer);

    webServer.on("/all", HTTP_GET, []() {
      String json = {};
      webServer.sendHeader("Access-Control-Allow-Origin", "*");
      webServer.sendHeader("Access-Control-Allow-Methods", "GET,OPTIONS");
      webServer.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
      webServer.send(200, "text/json", json);
    });
    

    // CONFIGURACION DE CAMARA
    bool status;

    status = amg.begin();
    if (!status) {
        Serial.println("Check wiring!");
        while (1);
    }

    Serial.println("-- CAMERA Test --");

    Serial.println();
}
 
void loop() {
 
 //WiFiManager wifiManager;
 //si el botón se ha presionado
 /*if ( digitalRead(PIN_AP) == HIGH ) {
  Serial.println("reajustar"); //resetear intenta abrir el portal
  if(!wifiManager.startConfigPortal("Makeit_Lab_Api", "arduino1234") ){
   Serial.println("No se pudo conectar");
   delay(2000);
   ESP.restart();
   delay(1000);
  }
  Serial.println("conectado ESP_AP!!!");
 }*/

 //  dnsServer.processNextRequest();
    //  webSocketsServer.loop();
    webServer.handleClient();

  static bool hasConnected = false;
    if (WiFi.status() != WL_CONNECTED) {
      //      Serial.printf("Connecting to %s\n", ssid);
      hasConnected = false;
    }
    else if (!hasConnected) {
      hasConnected = true;
      Serial.print("Connected! Open http://");
      Serial.print(WiFi.localIP());
      Serial.println(" in your browser");
    }
}
