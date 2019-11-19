#include <SFE_BMP180.h>
#include <Wire.h>
#include <DHT.h>
#include <Wire.h>
#include <SPI.h>
#include <SoftwareSerial.h>
SoftwareSerial ESPSerial(10, 11); // RX, TX

//Se declara una instancia de la librería
SFE_BMP180 pressure;

// Definimos el pin digital donde se conecta el sensor
#define DHTPIN 2

// Dependiendo del tipo de sensor
#define DHTTYPE DHT11

// Inicializamos el sensor DHT11
DHT dht(DHTPIN, DHTTYPE);

//Se declaran las variables. Es necesario tomar en cuenta una presión inicial
//esta será la presión que se tome en cuenta en el cálculo de la diferencia de altura
double PresionBase;

//Leeremos presión y temperatura. Calcularemos la diferencia de altura
double Presion = 0;
double Altura = 0;
double Temperatura = 0;
char status;
int analogValue;
bool digitalValue;
const size_t LengthData = 5;
float data[LengthData];
int ESP_Status, ESPenvio_Error, i1;
float h;
float t;
double PresionNivelMar=1013.25;
double conv;

void setup() {
  //iniciamos el proceso de comunicacion serial
  Serial.begin(9600);
  ESPSerial.begin(9600);
  Serial.println("INICIANDO");
  
  // Comenzamos el sensor DHT
  dht.begin();
  
  //Se inicia el sensor y se hace una lectura inicial
  SensorStart();
}

void loop() {
   
  //Se hace lectura del sensor
  ReadSensor(); //SENSOR BMP180 y lecturas de otros sensores
  impresion(); //muestra los datos en monitor serial 
  lluvia();// hace lectura de sensor de lluvia 
   // Crear array de datos para enviar al mñodulo ESP8266-01
  data[0] = Temperatura;
  data[1] = h;
  data[2] = Presion;
  data[3] = Altura;
  data[4] = digitalValue;
  envioWIFI();
  delay(10000);
     //Cada 10 segundos hará una nueva lectura
}

void lluvia() {
  // Entrada analogica
  analogValue = analogRead(0); // leemos A0 del sensor
  if (analogValue < 300)
    Serial.println("Lluvia Intensa");
  else if (analogValue < 500)
    Serial.println("Lluvia Moderada");
  else
    Serial.println("Lluvia no detectada");

  // Entrada digital pin 4 arduino
  digitalValue = digitalRead(4);
  if (digitalValue == HIGH)
    Serial.println(">>>>> NO LLUVIA!");
  else
    Serial.println(">>>>> LLUVIA!");
}

void SensorStart() {
  //Secuencia de inicio del sensores y deteccion de algun error en lectura
  if (pressure.begin())
    Serial.println("medicion iniciada success");
  else
  {
    Serial.println("sensores en mantenimiento fail (disconnected?)\n\n");
    while (1);
  }
  //Se inicia la lectura de temperatura
  status = pressure.startTemperature();
  if (status != 0)  {
    delay(status);
    //Se lee una temperatura inicial
    status = pressure.getTemperature(Temperatura);
    if (status != 0)    {
      //Se inicia la lectura de presiones
      status = pressure.startPressure(3);
      if (status != 0)
      {
        delay(status);
        //Se lee la presión inicial incidente sobre el sensor en la primera ejecución
        status = pressure.getPressure(PresionBase, Temperatura);
      }
    }
  }
}
void ReadSensor() {
  

 // Comprobamos si ha habido algún error en la lectura dth 11

 // Leemos la humedad relativa
   h = dht.readHumidity();
  // Leemos la temperatura en grados centígrados (por defecto)
   t = dht.readTemperature();
  
  if (isnan(h) || isnan(t)) {
    Serial.println("Error obteniendo los datos del sensor DHT11");
    return;
  }
  
  //En este método se hacen las lecturas de presión y temperatura y se calcula la altura
  //Se inicia la lectura de temperatura
  
  status = pressure.startTemperature();
  if (status != 0)
  {
    delay(status);
    //Se realiza la lectura de temperatura
    status = pressure.getTemperature(Temperatura);
    if (status != 0)
    {
      //Se inicia la lectura de presión
      status = pressure.startPressure(3);
      if (status != 0)
      {
        delay(status);
        
        //Se lleva a cabo la lectura de presión,</span>
        //considerando la temperatura que afecta el desempeño del sensor</span>
        
        status = pressure.getPressure(Presion, Temperatura);
        if (status != 0)
        {
          //Cálculo de la altura en base a la presión leída en el Setup
          Altura = pressure.altitude(Presion,PresionNivelMar);
        }
        else Serial.println("Error en la lectura de presion\n");
      }
      else Serial.println("Error iniciando la lectura de presion\n");
    }
    else Serial.println("Error en la lectura de temperatura\n");
  }
  else Serial.println("Error iniciando la lectura de temperatura\n");
}


// variables a imprimir

void impresion(){

   Serial.println("//////////////");
  Serial.println("DATOS DTH11");
  Serial.print("Humedad: ");
  Serial.print(h);
  Serial.println(" %\t");
  Serial.print("Temperatura: ");
  Serial.print(t);
  Serial.println(" *C ");
  Serial.print("/////////////");
  Serial.println("              ");
  Serial.println("              ");
  Serial.println(" SENSOR BMP180");
  Serial.print("Temperatura: ");
  Serial.print(Temperatura);
  Serial.println(" grados C");
  Serial.print("Presion: ");
  Serial.print(Presion);
  Serial.println(" milibares");
  Serial.print("Altura relativa: ");
  Serial.print(Altura);
  Serial.println(" m.s.n.m");
  Serial.println("              ");
  Serial.println("/////////////");
  Serial.println("PLUVIOMETRO");
  
  
  
  
  }
// aqui se envian las variables al modulo wifi
  void envioWIFI() {
  ESP_Status = 404;
  delay(1000);

  ////////////// ENVIAR y RECIBIR ESP8266-01 //////////////
  ESPSerial.write((byte*)data, LengthData * sizeof(data[0]));  // Enviar array de datos por SoftwareSerial.h
  delay(7000);
  if (ESPSerial.available()) {
    ESP_Status = ESPSerial.parseInt();  // Leer canal SoftwareSerial.h
    if (ESP_Status != 200) ESPenvio_Error++;
    else ESPenvio_Error = 0;
  }
  
  ////////////// ENVIAR y RECIBIR ESP8266-01 //////////////
}
