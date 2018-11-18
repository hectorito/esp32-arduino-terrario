#include <b64.h>
#include <HttpClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <WiFiServer.h>



//LIBRERIAS
#include "WiFi.h"
#include "HTTPClient.h"
#include <OneWire.h>
#include "DallasTemperature.h"
#include "DHT.h"


//ONE WIRE - pin 15
#define ONE_WIRE_BUS 15
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
//OneWire ds(15); //para obtener direcciones

//sensor de humedad
#define DHTTYPE DHT21
const int DHTPin = 19;
DHT dht(DHTPin, DHTTYPE); //le paso pin y tipo

//direcciones de one wire
DeviceAddress sensor1 = { 0x28, 0x1A, 0x98, 0xA2, 0x8, 0x0, 0x0, 0x88 };
DeviceAddress sensor2 = { 0x28, 0x21, 0xC5, 0xA3, 0x8, 0x0, 0x0, 0x6B };

//variables de estado
int [][]

//variables de entrada enchufes (numero del pin GPIO)
int entradaplaca = 9;
int entradabombillo = 8;
int entradacascada = 7;
int entradauv = 6;

// Valor de estado de los enchufes
int valorplaca = 1;
int valorbombillo = 1;
int valorcascada = 1;
int valoruv = 1;

void setup() {
  Serial.begin(115200);
  sensors.begin();
  dht.begin();

  //Cada relay debe ser NC estado inicial low
  //
  pinMode(entradaplaca, OUTPUT);
  digitalWrite(entradaplaca, HIGH);
  
  pinMode(entradabombillo, OUTPUT);
  digitalWrite(entradabombillo, HIGH);
  
  pinMode(entradacascada, OUTPUT);
  digitalWrite(entradacascada, HIGH);
  
   pinMode(entradauv, OUTPUT);
  digitalWrite(entradauv, HIGH);  
  /*
  Serial.println("hola ESP32");
  Serial.print("comenzaremos a conectar a wifi");
  Serial.print("");
  //WIFI station mode
  WiFi.mode(WIFI_AP_STA);
  //Comienza SmartConfig
  WiFi.beginSmartConfig();
  //mensaje de espera del paquete del celular
  Serial.println("Esperando ip/password del celular ...");
  while(!WiFi.smartConfigDone()){
    delay(500);
    Serial.print(".");
    }
   Serial.println("");
   Serial.println("Llego ip/password del celular!");

   //esperando q wifi se conecte
   Serial.println("conectando ESP32 al WiFi...");
   while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
    }
  Serial.println("WIFI CONNECTADO");
  Serial.print("IP Adress: ");
  Serial.println(WiFi.localIP());
  */
  }

void loop() {  
  /*
  Serial.print("DIRECCIONES");
  byte i;
  byte addr[8];
  
  if (!ds.search(addr)) {
    Serial.println(" No more addresses.");
    Serial.print('\n');
    Serial.println();
    ds.reset_search();
    delay(250);
    return;
  }
  Serial.print(" ROM =");
  for (i = 0; i < 8; i++) {
    Serial.write(' ');
    Serial.print(addr[i], HEX);
  }
    delay(1000);
  */

  //SENSOR DE TEMPERATURA 
  Serial.print("ROBTENIENDO TEMPERATURAS...");
  sensors.requestTemperatures();
  Serial.println("DONE");
  
  Serial.print("Sensor 1(*C): ");
  Serial.print(sensors.getTempC(sensor1)); 
  Serial.print('\n');
  Serial.print("Sensor 2(*C): ");
  Serial.print(sensors.getTempC(sensor2)); 
  Serial.print('\n');

  //SENSOR DE HUMEDAD
  Serial.print("SENSOR DE HUMEDAD Y TEMPERATURA");
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t Temperature: ");
  Serial.print(t);
  Serial.print('\n');
  
 
  delay(2000);

     if(valorplaca == 0 && valorbombillo == 0 && valorcascada == 0 && valoruv == 0 ){
      digitalWrite(entradaplaca, HIGH);
      digitalWrite(entradabombillo, HIGH);
      digitalWrite(entradacascada, HIGH);
      digitalWrite(entradauv, HIGH);
      delay(10000); // luego consulta estados..
    } 
     if(valorplaca == 1 && valorbombillo == 0 && valorcascada == 0 && valoruv == 0 ){
      digitalWrite(entradaplaca, LOW);
      digitalWrite(entradabombillo, HIGH);
      digitalWrite(entradacascada, HIGH);
      digitalWrite(entradauv, HIGH);
      delay(10000); // luego consulta estados..
    }
     if(valorplaca == 0 && valorbombillo == 1 && valorcascada == 0 && valoruv == 0 ){
      digitalWrite(entradaplaca, HIGH);
      digitalWrite(entradabombillo, LOW);
      digitalWrite(entradacascada, HIGH);
      digitalWrite(entradauv, HIGH);
      delay(10000); // luego consulta estados..
    }
     if(valorplaca == 0 && valorbombillo == 0 && valorcascada == 1 && valoruv == 0 ){
      digitalWrite(entradaplaca, HIGH);
      digitalWrite(entradabombillo, HIGH);
      digitalWrite(entradacascada, LOW);
      digitalWrite(entradauv, HIGH);
      delay(10000); // luego consulta estados..
    }
     if(valorplaca == 0 && valorbombillo == 0 && valorcascada == 0 && valoruv == 1 ){
      digitalWrite(entradaplaca, HIGH);
      digitalWrite(entradabombillo, HIGH);
      digitalWrite(entradacascada, HIGH);
      digitalWrite(entradauv, LOW);
      delay(10000); // luego consulta estados..
    }
     if(valorplaca == 1 && valorbombillo == 0 && valorcascada == 0 && valoruv == 1 ){
      digitalWrite(entradaplaca, LOW);
      digitalWrite(entradabombillo, HIGH);
      digitalWrite(entradacascada, HIGH);
      digitalWrite(entradauv, LOW);
      delay(10000); // luego consulta estados..
    }
     if(valorplaca == 1 && valorbombillo == 0 && valorcascada == 1 && valoruv == 0 ){
      digitalWrite(entradaplaca, LOW);
      digitalWrite(entradabombillo, HIGH);
      digitalWrite(entradacascada, LOW);
      digitalWrite(entradauv, HIGH);
      delay(10000); // luego consulta estados..
    }
     if(valorplaca == 1 && valorbombillo == 1 && valorcascada == 0 && valoruv == 0 ){
      digitalWrite(entradaplaca, LOW);
      digitalWrite(entradabombillo, LOW);
      digitalWrite(entradacascada, HIGH);
      digitalWrite(entradauv, HIGH);
      delay(10000); // luego consulta estados..
    }
     if(valorplaca == 1 && valorbombillo == 0 && valorcascada == 0 && valoruv == 0 ){
      digitalWrite(entradaplaca, LOW);
      digitalWrite(entradabombillo, HIGH);
      digitalWrite(entradacascada, HIGH);
      digitalWrite(entradauv, HIGH);
      delay(10000); // luego consulta estados..
    }
     if(valorplaca == 0 && valorbombillo == 1 && valorcascada == 0 && valoruv == 1 ){
      digitalWrite(entradaplaca, HIGH);
      digitalWrite(entradabombillo, LOW);
      digitalWrite(entradacascada, LOW);
      digitalWrite(entradauv, HIGH);
      delay(10000); // luego consulta estados...
    }
     if(valorplaca == 0 && valorbombillo == 1 && valorcascada == 1 && valoruv == 0 ){
      digitalWrite(entradaplaca, HIGH);
      digitalWrite(entradabombillo, LOW);
      digitalWrite(entradacascada, LOW);
      digitalWrite(entradauv, HIGH);
      delay(10000); // luego consulta estados...
    }
     if(valorplaca == 0 && valorbombillo == 0 && valorcascada == 1 && valoruv == 1 ){
      digitalWrite(entradaplaca, HIGH);
      digitalWrite(entradabombillo, HIGH);
      digitalWrite(entradacascada, LOW);
      digitalWrite(entradauv, LOW);
      delay(10000); // luego consulta estados...
    }
     if(valorplaca == 1 && valorbombillo == 1 && valorcascada == 1 && valoruv == 0 ){
      digitalWrite(entradaplaca, LOW);
      digitalWrite(entradabombillo, LOW);
      digitalWrite(entradacascada, LOW);
      digitalWrite(entradauv, HIGH);
      delay(10000); // luego consulta estados...
    }
     if(valorplaca == 0 && valorbombillo == 1 && valorcascada == 1 && valoruv == 1 ){
      // SOLO LA PLACA ENCENDIDA 
      digitalWrite(entradaplaca, HIGH);
      digitalWrite(entradabombillo, LOW);
      digitalWrite(entradacascada, LOW);
      digitalWrite(entradauv, LOW);
      delay(10000);
      digitalWrite(entradaplaca, LOW);
      delay(10000);
    }
     if(valorplaca == 1 && valorbombillo == 0 && valorcascada == 1 && valoruv == 1 ){
      digitalWrite(entradaplaca, LOW);
      digitalWrite(entradabombillo, HIGH);
      digitalWrite(entradacascada, LOW);
      digitalWrite(entradauv, LOW);
      delay(10000); // luego consulta estados...
    }
     if(valorplaca == 1 && valorbombillo == 1 && valorcascada == 0 && valoruv == 1 ){
     digitalWrite(entradaplaca, LOW);
      digitalWrite(entradabombillo, LOW);
      digitalWrite(entradacascada, HIGH);
      digitalWrite(entradauv, LOW);
      delay(10000); // luego consulta estados...
    }            
     if(valorplaca == 1 && valorbombillo == 1 && valorcascada == 1 && valoruv == 1 ){
      digitalWrite(entradaplaca, LOW);
      digitalWrite(entradabombillo, LOW);
      digitalWrite(entradacascada, LOW);
      digitalWrite(entradauv, LOW);
      leer_temp();
      leer_hum();
      if(tiempo_revision()){
        reportar_datos();
      //si en la respuesta sí hay cambios
      //if(haycambios){
      //  solicitar_datos();
      //  variables toman nuevos valores;
      //  informar si es necesario y salir
      //}
      //else{
      //delay(10000); // luego consulta estados...
      //}
    }  
    
}

void leer_temp(){
  guardar_temp();  
  }
void leer_hum(){
  guardar_hum();
  }
void guardar_temp(){
  
  }
}
void guardar_hum(){}
void reportar_datos(){}
void solicitar_datos(){}
void reportar_evento(){}
boolean tiempo_revision(){
  //si el tiempo de revisar se cumple 
  //variables de tiempo comparar
  }
