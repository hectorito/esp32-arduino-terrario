#include <ArduinoJson.h>
#include <ArduinoHttpClient.h>
#include <WiFi.h>
#include "time.h"
#include <b64.h>
#include <HttpClient.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
//LIBRERIAS
#include "WiFi.h"
#include "HTTPClient.h"
#include <OneWire.h>
#include "DallasTemperature.h"
#include "DHT.h"
#include "Arduino.h"
#include "Client.h"

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

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
//variables recibidas
String  line          ;
String  Sol_max       ;     
String  Sol_min       ;      
String  Temp_max      ;    
String  Temp_min      ;   
String  Humedad_min   ;
String  Uv_inicio     ;
String  Uv_tiempo     ;
String  Catarata_on   ;
String  Catarata_off  ;
int     Uv            ;
int     FocoTermico   ;
int     PlacaTermica  ;
int     Catarata      ;
int     Auto_sol      ;
int     Auto_terrario ;
int     Auto_humedad  ;
int     Auto_luz;
//variables para enviar se debe de agregar los valores segun estado:
String  Esp_sol   =   "hola" ;
String  Esp_terrario  = "mundo";
String  Esp_humedad  =  "holis";
String  Esp_placatermica= "mundis";
String  Esp_focotermico= "holita";
String  Esp_catarata   = "mundita"; 
String  Esp_uv      = "holamundo"   ;
String  Id_cliente ="";
String  Hora       ="";
String  Razon      ="";
// Valor de estado de los enchufes
int valorplaca = 1;
int valorbombillo = 1;
int valorcascada = 1;
int valoruv = 1;

//variables de entorno
int port = 8000;
int puerto_tcp_http =8000;
int actualizacion = 1;
int value = 0;
//char* ssid       = "hector";
//char* password   = "1111222233";
char* ssid       = "Xinita_sala";
char* password   = "perlanegra";
char* host = "192.168.0.16";
//char* host = "www.google.cl";

char* streamId   = "....................";
char* privateKey = "....................";


//Funciones
void conectar_wifi(char* id, char* pass){
    Serial.println();
    Serial.println();
    IPAddress local_IP(192, 168, 0, 22);
    IPAddress gateway(192, 168, 0, 1);
    IPAddress subnet(255, 255, 255, 0);
    IPAddress primaryDNS(8, 8, 8, 8); //optional
    IPAddress secondaryDNS(8, 8, 4, 4); //optional
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(":");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  };

void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void leer_temp (){
  //SENSOR DE TEMPERATURA 
  Serial.print("Obteniendo Temperaturas.  \n");
  sensors.requestTemperatures();
  Serial.println(".....DONE");
  Serial.print("Sensor 1(*C): ");
  Serial.print(sensors.getTempC(sensor1)); 
  Serial.print('\n');
  Serial.print("Sensor 2(*C): ");
  Serial.print(sensors.getTempC(sensor2)); 
  Serial.print("\n \n");
  //variables de prueba analisis de sensores
  //int res[3] = {0, 0, 0}; //{prom1, prom2, bool}
  int res = 0;
  int sens1 = 11; 
  int sens2 = 22; 
  res = analizar_temp(sens1, sens2);
  if(res == 1){
    Serial.print(" Valor señala anomalia=1, normal=0    es : ");
    Serial.print(res); Serial.print(" \n \n");
  }
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
        Id_cliente ="1";
        Hora       ="00:00:00";
        Razon      ="Temperatura fuera de rango";   
            
        int res = alarma_sensores();
        if(res == 1){
            Id_cliente ="";
            Hora       ="";
            Razon      ="";
            Serial.print("Alarma Temperatura Enviada. \n");
            guardar_temp(sens1, sens2);
            return res;
        } 
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
  Serial.print("Obteniendo Humedad medida. \n");
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int aux = (int)h;
  int aux2 = (int)t;
  Serial.print("Humidity: ");
  Serial.print(aux);
  Serial.print(" %\t Temperature: ");
  Serial.print(aux2);
  Serial.print('\n');
  //variables de prueba analisis de sensor humedad
  int res = 0; int hum_min = 11; int hum_max = 77; 
  //int humedad = (int)h;
  int humedad = 88;
  res = analizar_hum(hum_min, hum_max, humedad);
  if(res == 1){
    Serial.print(" Valor señal.   anomalia=1, normal=0 :  ");
    Serial.print(res); 
    Serial.print(" \n");
    Serial.print("Configurar para ver \n");
  }
  //return res;
}
int analizar_hum(int hum_min, int hum_max, int humedad){
  //Aca debe comparar rango
  int res = 0;
  if(hum_min <= humedad && hum_max >= humedad)
  { guardar_hum(humedad); 
    return res;
    }else{ 
     //notificar emergencia
        Id_cliente ="1";
        Hora       ="12:12:12";
        Razon      ="Humedad fuera de rango";   
            
        int res = alarma_sensores();
        if(res == 1){
            Id_cliente ="";
            Hora       ="";
            Razon      ="";
            Serial.print("Alarma Humedad Enviada. \n");
            guardar_hum(humedad);
            return res;
            //notificar emergencia
            //reportar_datos(humedad);
            }else{
              Serial.print("no se reporto algo fallo");
              return res;
            }
}
}
void guardar_hum(int humedad){
      //datos guardados en un buffer o enviara DB
      int promedio3 = 0;
      //el valor debe ser agregado al buffer
      for(int i=8;i<=0;i--){
          hum_acum_sens1[i+1]=temp_acum_sens1[i];
          promedio3 = promedio3 + hum_acum_sens1[i+1];
        }
       hum_acum_sens1[0]=humedad;
       promedio3+=humedad;
      promedio3=(promedio3/10);
      int  resultado[2] = {promedio3, 1};
      promedio3 = promedio3 + hum_acum_sens1[0];
      //return resultado;
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
      }      
}
int obtener_datos(){
    while (WiFi.status() != WL_CONNECTED) {
          delay(500);
          Serial.print(".");
          return 1;
        }
    Serial.print("Subnet Mask: ");
    Serial.println(WiFi.subnetMask());
    Serial.print("Gateway IP: ");
    Serial.println(WiFi.gatewayIP());
    Serial.print("DNS: ");
    Serial.println(WiFi.dnsIP());

    WiFiClient client;
    if (!client.connect(host, port)) {
        Serial.println("connection failed on HTTPPort");
        return 0;
      }
      String url = "http://192.168.0.16:8000/esp/1";
      Serial.print("Requesting URL: ");
      Serial.println(url);

      // This will send the request to the server
      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" +
                   "Connection: open\r\n\r\n");
      unsigned long timeout = millis();
      while (client.available() == 0) {
          if (millis() - timeout > 5000) {
              Serial.println(">>> Client Timeout !");
              client.stop();
              return 0;
          }
      }
      // Read all the lines of the reply from server and print them to Serial
      while(client.available()) {
          line = client.readStringUntil('\r');
          //Serial.print(line);
          };
      Serial.println();
      Serial.println("Datos recibidos: " + line + "\n");
      return 1;
  }
int alarma_sensores(){
        Serial.print("Reportando anomalia en temperarura o humedad.  \n");
        while (WiFi.status() != WL_CONNECTED) {
              delay(500);
              Serial.print(".");
        }
        Serial.print("DNS: ");
        Serial.println(WiFi.dnsIP());
        
        WiFiClient client;  

        if (!client.connect(host, port)) {
          Serial.println("connection failed on WiFiClient ");
          return 0;
        }
        String url = "http://192.168.0.16:8000/alarma/1";
  
        Serial.print("Requesting URL: ");
        Serial.println(url);
        // This will send the POST request to the server
        client.print(String("POST ") + url + " HTTP/1.1\r\n" +
                     "Host: " + host + "\r\n" +
                      String("id_cliente:")   + Id_cliente      +
                      String("hora:")         + Hora           +
                      String("razon:")        + Razon         +
                      "Connection: open\r\n\r\n");
                      
          unsigned long timeout = millis();
          while (client.available() == 0) {
              if (millis() - timeout > 5000) {
                  Serial.println(">>> Client Timeout !");
                  client.stop();
                  return 0;
              }
          }
          String response;
            // Read all the lines of the reply from server and print them to Serial
          while(client.available()) {
              response = client.readStringUntil('\r');
              Serial.print(response);
              //Aqui asignar los valores que vienen en el json a las variables globales
              }; 
            Serial.print(" \n");
            Serial.print("Alarma informada correctamente. \n");
            return 1;
};
int reportar_datos(){

        while (WiFi.status() != WL_CONNECTED) {
              delay(500);
              Serial.print(".");
        }
        Serial.print("Subnet Mask: ");
        Serial.println(WiFi.subnetMask());
        Serial.print("Gateway IP: ");
        Serial.println(WiFi.gatewayIP());
        Serial.print("DNS: ");
        Serial.println(WiFi.dnsIP());
        
        WiFiClient client;  

        if (!client.connect(host, port)) {
          Serial.println("connection failed on HTTPPort");
          return 0;
        }
        String url = "http://192.168.0.16:8000/esp/1";
  
        Serial.print("Requesting URL: ");
        Serial.println(url);
        // This will send the PUT request to the server
        client.print(String("PUT ") + url + " HTTP/1.1\r\n" +
                     "Host: " + host + "\r\n" +
                      String("esp_sol:")          + Esp_sol      +
                      String("esp_terrario:")     + Esp_terrario +
                      String("esp_humedad:")      + Esp_humedad  +
                      String("esp_placatermica:") + Esp_placatermica +
                      String("esp_focotermico:")  + Esp_focotermico  +
                      String("esp_catarata:")     + Esp_catarata  +
                      String("esp_uv:")           + Esp_uv +
                      "Connection: open\r\n\r\n");
                      
          unsigned long timeout = millis();
          while (client.available() == 0) {
              if (millis() - timeout > 5000) {
                  Serial.println(">>> Client Timeout !");
                  client.stop();
                  return 0;
              }
          }
          String response;
            // Read all the lines of the reply from server and print them to Serial
          while(client.available()) {
              response = client.readStringUntil('\r');
              Serial.print(response);
              //Aqui asignar los valores que vienen en el json a las variables globales
              }; 
            Serial.print(" \n");
            Serial.print("Api disponible: \n");
            Serial.println(" esta respuesta pertenece al POST \n  \n");
            return 1;
  };



void actualizar_esp32(){
     
      String jsonmensaje;
      int i_line =1;
      int i_json =0;
      while(i_line < line.length()){ //escapar los "
            if(line[i_line] == '"'){
                  jsonmensaje+=(char)92;
                  i_json++;
                  jsonmensaje+= line[i_line];
                  i_json++;
            }else{
                  jsonmensaje+= line[i_line];
                  i_json++;
            }
            i_line++;
      }
      //Serial.println(jsonmensaje);
      Serial.print("En Funcion ActualizarEsp. \n");
      Serial.print("Aqui falta pasar el mensaje correcto formado con las variables. \n");
      String jsonmensaje2 = "{\"sol_max\":\"40\",\"sol_min\":\"20\",\"temp_max\":\"30\",\"temp_min\":\"20\",\"humedad_min\":\"35\",\"uv_inicio\":\"8\",\"uv_tiempo\":\"22\",\"catarata_on\":\"1\",\"catarata_off\":\"3\",\"uv\":\"1\",\"focotermico\":\"1\",\"placatermica\":\"0\",\"catarata\":\"0\",\"auto_sol\":\"1\",\"auto_terrario\":\"1\",\"auto_humedad\":\"1\",\"auto_luz\":\"1\"}";
      //Serial.print(jsonmensaje2);
      DynamicJsonBuffer jsonBuffer;
      JsonObject& parsed = jsonBuffer.parseObject(jsonmensaje2); //Parse message
      if(!parsed.success()){
          Serial.print("Falló parseObject()");
      }else{
      Sol_max       = parsed["sol_max"].as<String>();      
      Sol_min       = parsed["sol_min"].as<String>();      
      Temp_max      = parsed["temp_max"].as<String>();    
      Temp_min      = parsed["temp_min "].as<String>();   
      Humedad_min   = parsed["humedad_min"].as<String>();
      Uv_inicio     = parsed["uv_inicio"].as<String>();
      Uv_tiempo     = parsed["uv_tiempo"].as<String>();
      Catarata_on   = parsed["catarata_on"].as<String>();
      Catarata_off  = parsed["catarata_off"].as<String>();
      Uv            = parsed["uv"].as<int>();
      FocoTermico   = parsed["focoTermico "].as<int>();
      PlacaTermica  = parsed["placaTermica"].as<int>();
      Catarata      = parsed["catarata"].as<int>();
      Auto_sol      = parsed["auto_sol"].as<int>();
      Auto_terrario = parsed["auto_terrario"].as<int>();
      Auto_humedad  = parsed["auto_humedad"].as<int>();
      Auto_luz      = parsed["auto_luz"].as<int>();
      Serial.print("El JSON a sido analizado por parseObject()  \n");
      }
      Serial.print("ejemplo de lectura: Auto_luz es : ");
      Serial.print(Auto_luz);
      Serial.print( "\n");
      //cambiar el estado de los toma corriente
      gestionar_enchufes(valorplaca, valorbombillo, valorcascada, valoruv); 
      actualizacion = 0;    //retornar actualizacion para reviar si paso bien
      Serial.print("No toma valores recibidos::::  Saliendo de Actualización de ESP·  \n   \n   \n"); 
}     



void setup(){
  Serial.begin(115200);
  sensors.begin();
  dht.begin();
  
  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  conectar_wifi( ssid, password);
  //WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");
  
  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  //disconnect WiFi as it's no longer needed
  //WiFi.disconnect(true);
  //WiFi.mode(WIFI_OFF);

  //Cada relay debe ser NC estado inicial low
  //
  //pinMode(entradaplaca, OUTPUT);
  //digitalWrite(entradaplaca, LOW);
  
  //pinMode(entradabombillo, OUTPUT);
  //digitalWrite(entradabombillo, LOW);
  
  //pinMode(entradacascada, OUTPUT);
  //digitalWrite(entradacascada, LOW);
  
   //pinMode(entradauv, OUTPUT);
   //digitalWrite(entradauv, LOW);  
}
 
void loop(){
      Serial.print(" \n \n \n");
      Serial.print("Begin****************************************** \n");
      printLocalTime();
      ++value;
      leer_temp();
      delay(1000);

      leer_hum();       
      delay(1000);
      
      Serial.print( " \n \n");
      Serial.print("Obtener datos (paso1 get).  \n");
      Serial.print( "Al encender debe actualizar. estado actualizacion necesaria (0=no, 1=si). \n");
      Serial.print("Pedir datos: \n");
      //debe llegar un valor que indica si es necesario actualizar.
      // .   No esta llegando
       actualizacion = obtener_datos(); //asigna Line (json resibido)
       Serial.print(" \n \n");
       Serial.print( "Revision por Actualización (0=no, 1=si). \n");
       Serial.print("Actualizacion Requerida = ");
       Serial.print(actualizacion);
       Serial.print(" \n \n");
       delay(1000);
       
       Serial.print("Inicio  revision de actualizacion:  \n  (dos)   \n"); 
       if(!actualizacion){
        Serial.print(" Valor es cero --> conservar estado. \n");
        }else{
              Serial.println("Valor es 1 cambiar estado de artefactos");
              Serial.print("Actualizar ESP32 (paso2 ).   \n \n");
              actualizar_esp32();
              };
        Serial.print("Iniciando a reportar datos desde la ESP·\n   (paso 3) \n");     
        int reportado = reportar_datos();
        Serial.print(" Los valores se han enviado (0=no, 10=si) : " );
        Serial.print(reportado);
        Serial.print(" \n \n");
        Serial.print("End*********************************************** \n \n");   
        //este chip falla ocasionalmente, buscar forma de reiniciar el sistema. eso soluciona mucho.
        // ESP.restart();     
        
}
