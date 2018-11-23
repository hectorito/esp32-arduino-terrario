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
int hum_acum_sens1[10]={1,1,1,1,1,1,1,1,1,1};

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

//variables de entorno
int actualizacion = 0;
int value = 0;

char* ssid     = "Xinita_sala";
char* password = "perlanegra";

const char* host = "google.com";
const char* streamId   = "....................";
const char* privateKey = "....................";


//el orden de las funciones importa??!!%&$
//definir cada evento de forma unitaria depende del tipo de emergencia
void conectar_wifi(char* id, char* pass){
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  };
void reportar_evento_temp(){};
void reportar_evento_hum(){};
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
        guardar_temp(sens1, sens2);
        return res;}
   else{
        //notificar emergencia
        reportar_evento_temp();
        
        guardar_temp(sens1, sens2);
        int res = 1;//indica parametro fuera de limites 
        return res;
      }
}
void guardar_temp(int sens1, int sens2){
      int promedio1 = 0;
      int promedio2 = 0;
      //el valor debe ser agregado al buffer
      for(int i=8;i<=0;i--){
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
      for(int i=8;i<=0;i--){
          hum_acum_sens1[i+1]=temp_acum_sens1[i];
          promedio3 = promedio3 + hum_acum_sens1[i+1];
        }
      promedio3=(promedio3/10);
      int  resultado[2] = {promedio3, 1};
      promedio3 = promedio3 + hum_acum_sens1[0];
      //return resultado;
  }
  

int reportar_datos(){
  //esta funcion debe enviar a API 
  //recibir si hay o no hactualizaciones
  //set var actualizacion segun JSON
  actualizacion = 0;
  if(actualizacion == 0){
      int act =0;
      return act;
      }
   else{
      int act = 1;
      return act;
      }
}
void solicitar_datos(){}

void actualizar_esp32(){
      void solicitar_datos();
      //debe traer un JSON con los datos.
      //Asignar los nuevos valores a las variables (desde el JSON)
      valorplaca = 1;
      valorbombillo = 1;
      valorcascada = 1;
      valoruv = 1;
      //cambiar el estado de los toma corriente
      gestionar_enchufes(valorplaca, valorbombillo, valorcascada, valoruv);  
}

void gestionar_enchufes( int valorplaca, int valorbombillo, int valorcascada, int valoruv){//debe recibir el estado de los enchufes.
  
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

  conectar_wifi( ssid, password);
  
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
      ++value;
      
      Serial.print("connecting to API on: ");
      Serial.println(host);
      delay(1000); 
      
      leer_temp();
      delay(2000);
      
      leer_hum();       
      delay(2000);
      
      //automaticamente va por los datos
      //hay que cargarlos y cambiar de estado
      delay(1000);
      actualizacion = reportar_datos();
      delay(2000);
      if(actualizacion == 1){
        actualizar_esp32();
        };
        
      //podemos cambiar las credenciales de wifi y cargar nuevamente la coneccion
      //conectar_wifi(id, pass);

      // Use WiFiClient class to create TCP connections
      WiFiClient client;
      int httpPort = 80; //el puerto debe ser el adecuado para llegar a la API     creo 8080
      if (!client.connect(host, httpPort)) {
          Serial.println("connection failed");
          return;
      }
      // We now create a URI for the request
      String url = "/input/";
      url += streamId;
      url += "?private_key=";
      url += privateKey;
      url += "&value=";
      url += value;
  
      Serial.print("Requesting URL: ");
      Serial.println(url);

      // This will send the request to the server
      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" +
                   "Connection: close\r\n\r\n");
      unsigned long timeout = millis();
      while (client.available() == 0) {
          if (millis() - timeout > 5000) {
              Serial.println(">>> Client Timeout !");
              client.stop();
              return;
          }
      }

      // Read all the lines of the reply from server and print them to Serial
      while(client.available()) {
          String line = client.readStringUntil('\r');
          Serial.print(line);
      }
  
      Serial.println();
      Serial.println("closing connection");
    
} 
