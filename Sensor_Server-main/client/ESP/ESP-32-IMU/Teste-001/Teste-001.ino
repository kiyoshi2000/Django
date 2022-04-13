
//Para funcionar no ESP-32
#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
//Comunicao I2C com o sensor
#include <Wire.h>
 
//Direcao I2C da IMU
#define MPU 0x68

//MPU-9250  valores em inteiros de 16 bits
//Valores RAW
int16_t AcX, AcY, AcZ, GyX, GyY, GyZ;


//Objetivo da Classe String para receber o valor Int
//String valor;
//valor = String(AcX);

//Configuracao da Wifi
String IP_ADDRESS = "192.168.1.65"; 
const char* SSID = "ARAKI_2.4G";
const char* NETWORK_PASSWORD = "08051968";

//Cadastrar o sensor
String SENSOR_NAME = "sensor-1"; //sensor username
String SENSOR_PASSWORD = "IMU-0001"; //sensor password


void setup() {
  
  Serial.begin(115200); //Inicia a comunicao serial para verificar o envio dos dados pelo serial monitor
  //Comunicao e teste do Wi-fi
  WiFi.begin(SSID, NETWORK_PASSWORD);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) { 
    delay(1000);
    Serial.print(".");
  }
  
  Wire.begin(21,22); //39 GPIO22 SCL / 42 GPIO21 SDA Para o ESP-32  //D2(GPIO4)=SDA / D1(GPIO5)=SCL Para o ESP-8266
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  
}


void loop() {
    
    Wire.beginTransmission(MPU);
    Wire.write(0x3B); //Pedir o registro 0x3B - corresponde do AcX
    Wire.endTransmission(false);
    Wire.requestFrom(MPU,6,true);   //A partir do 0x3B, pede 6 registros HIGH e LOW
    AcX=Wire.read()<<8|Wire.read(); //Cada valor ocupa 2 registros
    AcY=Wire.read()<<8|Wire.read();
    AcZ=Wire.read()<<8|Wire.read();
    //Enviar valores ao server
    //Se tiver varios sensores basta usar o sendValue varias vezes com diferentes sensores names e passwords
    sendValue(SENSOR_NAME, SENSOR_PASSWORD, String(AcX));
    //delay(10); //Delay de 4 segundos para melhor visualizacao
}


void sendValue(String sensorName, String sensorPassword, String value) {
    //Check o status da conexao WiFi 
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin("http://"+IP_ADDRESS+":8000/iotmonitor/readings");
        http.addHeader("content-type", "application/x-www-form-urlencoded");

        String body = "sensor=" + sensorName + "&password=" + sensorPassword; //Autenticacao
        body += "&value="  + value; //sensor information

        Serial.println(body); //Mostra http request no  Serial Monitor

        int httpCode = http.POST(body); //Envia o request para server

        String response = http.getString(); //Obtem a resposta do server

        http.end(); //close connection
        Serial.println("##[RESPONSE]## ==> " + response);
        Serial.println(AcX);
        delay(10);
        
    } else {
        Serial.println("WiFi is not connected");
    }
    
}
