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

//variables de acumulacion
int temp_acum_sens1[10]={1,1,1,1,1,1,1,1,1,1};
int temp_acum_sens2[10]={1,1,1,1,1,1,1,1,1,1};
int temp_acum_sens3[10]={1,1,1,1,1,1,1,1,1,1};

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

//el orden de las funciones importa??!!%&$

void leer_temp (){
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
  //variables de prueba analisis de sensores
  //int res[3] = {0, 0, 0}; //{prom1, prom2, bool}
  int res = 0;
  int sens1 = 11; 
  int sens2 = 22; 
  res = analizar_temp(sens1, sens2);
  //analiza res 1 = ok;
  //return res;  
}
int analizar_temp(int sens1, int sens2){
  //Aca debe preguntar el rango en la API
  if(sens1 <50 && sens1 > 20 && sens2 < 50 && sens2 >20)
       { 
        int res = 0;
        res = guardar_temp(sens1, sens2);
        return res;}
   else{
        //notificar emergencia
        reportar_evento();
        //reportar_datos(sens1, sens2);
        //guardar_temp(sens1, sens2);
        int res = 0; 
        return res;
      }
}
void guardar_temp(int sens1, int sens2){
      int promedio1 = 0;
      int promedio2 = 0;
      //el valor debe ser agregado al buffer
      for(int i=0;i<9;i++)
      {
        temp_acum_sens1[i+1]=temp_acum_sens1[i];
        temp_acum_sens2[i+1]=temp_acum_sens2[i];
        promedio1=promedio1+temp_acum_sens1[i+1];
        promedio2=promedio2+temp_acum_sens2[i+1];
        }
      temp_acum_sens1[0]=sens1;
      temp_acum_sens2[0]=sens2;
      promedio1=promedio1+temp_acum_sens1[0];
      promedio2=promedio2+temp_acum_sens2[0];
      promedio1=(promedio1/10);
      promedio2=(promedio2/10);
      int  resultado[2] = { promedio1, promedio2 };
      //return resultado;
    }

void leer_hum (){
   //SENSOR DE HUMEDAD
  Serial.print("SENSOR DE HUMEDAD Y TEMPERATURA");
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t Temperature: ");
  Serial.print(t);
  Serial.print('\n');
  //variables de prueba analisis de sensor humedad
  //int res = 0; int hum_min = 11; int hum_max = 77; int humedad = 70;
  //res = analizar_hum (hum_min, hum_max, humedad);
  //analiza return 1 = ok;
}
int analizar_hum(int hum_min, int hum_max, int humedad){
  //Aca debe comparar rango
  if(hum_min <= humedad && hum_max >= humedad)
  { guardar_hum(humedad); int res = 1; return res;}
  else{ 
  //notificar emergencia
  //reportar_datos(humedad);
  int guardado[2] = {0, 0}; // {promedio, bool}
  guardar_hum(humedad);
  int res = 0; return res;}
}
void guardar_hum(int humedad){
      //datos guardados en un buffer o enviara DB
      int promedio3 = 0;
      //el valor debe ser agregado al buffer
      for(int i=0;i<9;i++)
      {
        temp_acum_sens3[i+1]=temp_acum_sens3[i];
        promedio3=promedio3+temp_acum_sens3[i+1];
        }
      temp_acum_sens3[0]=humedad;
      promedio3=promedio3+temp_acum_sens3[0];
      promedio3=(promedio3/10);
      int  resultado[2] = {promedio3, 1};
      //return resultado;
  }
  

void reportar_datos(){}
void solicitar_datos(){}

//definir cada evento de forma unitaria
void reportar_evento(){}
void gestionar_enchufes(){
  
     if(valorplaca == 0 && valorbombillo == 0 && valorcascada == 0 && valoruv == 0 ){
      digitalWrite(entradaplaca, HIGH);
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
     if(valorplaca == 1 && valorbombillo == 0 && valorcascada == 0 && valoruv == 0 ){
      digitalWrite(entradaplaca, LOW);
      digitalWrite(entradabombillo, HIGH);
      digitalWrite(entradacascada, HIGH);
      digitalWrite(entradauv, HIGH);
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
      leer_temp ();
      leer_hum ();
      //if(tiempo_revision()) {
       // reportar_datos();
        //si en la respuesta sí hay cambios
        //if(haycambios){
        //  solicitar_datos();
        //  variables toman nuevos valores;
        //  informar si es necesario y salir
        //}
        //else{
        //delay(10000); // luego ... salir...
        //}
      }      
}

void setup() {
  Serial.begin(115200);
  sensors.begin();
  dht.begin();

  //Cada relay debe ser NC estado inicial low
  //
  pinMode(entradaplaca, OUTPUT);
  digitalWrite(entradaplaca, LOW);
  
  pinMode(entradabombillo, OUTPUT);
  digitalWrite(entradabombillo, LOW);
  
  pinMode(entradacascada, OUTPUT);
  digitalWrite(entradacascada, LOW);
  
   pinMode(entradauv, OUTPUT);
  digitalWrite(entradauv, LOW);  
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

leer_temp();
       delay(2000);



leer_hum();

       delay(2000);
} 
