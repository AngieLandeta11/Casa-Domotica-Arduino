#include <ESP32Servo.h>
#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif

//#define RELAY_PIN 21

/*const char* ssid = "MEGACABLE-5G-1BA6";
const char* password = "password";*/

const char* ssid = "POCO";
const char* password = "password";

WiFiServer server(80);

String header;

unsigned long lastTime, timeout = 2000;

///////////// CODIGO ///////////////


const int outputPin1 = 2; // pin de salida recamara
const int outputPin2 = 15; // pin de salida recamara
const int outputPin3 = 19; // pin de salida deck
const int outputPin4 = 16; // pin de salida alberca
const int outputPin5 = 17; // pin de salida alberca
const int outputPin6 = 5; // pin de salida alberca
const int outputPin7 = 18; // pin de salida alberca
//const int servoPin = 22;
Servo servo;
Servo miServo;  
int pinServo=21;
int tiempo=0;


String outputState = "apagado"; //almacenar el estado actual de la salida



void setup() {

  
  Serial.begin(115200);

  pinMode(outputPin1, OUTPUT);
  pinMode(outputPin2, OUTPUT);
  pinMode(outputPin3, OUTPUT);
  pinMode(outputPin4, OUTPUT);
  pinMode(outputPin5, OUTPUT);
  pinMode(outputPin6, OUTPUT);
  pinMode(outputPin7, OUTPUT);
  
  
  digitalWrite(outputPin1, LOW);
  digitalWrite(outputPin2, LOW);
  digitalWrite(outputPin3, LOW);
  digitalWrite(outputPin4, LOW);
  digitalWrite(outputPin5, LOW);
  digitalWrite(outputPin6, LOW);
  digitalWrite(outputPin7, LOW);
  
  
  
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid,password);

  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Dispositivo conectado.");
  Serial.println("Direccion IP: ");
  Serial.print(WiFi.localIP());

  server.begin();
  //relay.begin();
  servo.attach(pinServo, 1500, 2500);
  //myServo.attach(servoPin);
  miServo.attach(22);  

}
int pos = 0;
void loop() {
  
  WiFiClient client = server.available();

  if(client)
  {
    lastTime = millis();
    
    Serial.println("Nuevo cliente");
    String currentLine = "";

    while(client.connected() && millis() - lastTime <= timeout)
    {

      if(client.available())
      {
        
        char c = client.read();
        Serial.write(c);
        header += c;

        if(c == '\n')
        {
          
          if(currentLine.length() == 0)
          {

            ////////// ENCABEZADO HTTP ////////////

            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();


            if (header.indexOf("GET /LuzRecamaraOn") >= 0) {
              Serial.println("GPIO encendido");
              outputState = "encendido";
              digitalWrite(outputPin1, HIGH);
              digitalWrite(outputPin2, HIGH);
            }                         
            else if (header.indexOf("GET /LuzRecamaraOff") >= 0) {
              Serial.println("GPIO apagado");
              outputState = "apagado";
              digitalWrite(outputPin1, LOW);
              digitalWrite(outputPin2, LOW);
            } 

            //deck
            if (header.indexOf("GET /LuzDeckOn") >= 0) {
              Serial.println("GPIO encendido");
              outputState = "encendido";
              digitalWrite(outputPin3, HIGH);
            }                         
            else if (header.indexOf("GET /LuzDeckOff") >= 0) {
              Serial.println("GPIO apagado");
              outputState = "apagado";
              digitalWrite(outputPin3, LOW);
            }

            //alberca
            if (header.indexOf("GET /AlbercaOn") >= 0) {
              Serial.println("GPIO encendido");
              outputState = "encendido";
              digitalWrite(outputPin4, HIGH);
              digitalWrite(outputPin5, HIGH);
              digitalWrite(outputPin6, HIGH);
              digitalWrite(outputPin7, HIGH);
            }                         
            else if (header.indexOf("GET /AlbercaOff") >= 0) {
              Serial.println("GPIO apagado");
              outputState = "apagado";
              digitalWrite(outputPin4, LOW);
              digitalWrite(outputPin5, LOW);
              digitalWrite(outputPin6, LOW);
              digitalWrite(outputPin7, LOW);
            }

            if (header.indexOf("GET /PuertaOn") >= 0) {
              Serial.println("GPIO encendido");
              outputState = "encendido";
              for (pos = 0; pos <= 180; pos += 1) {
                //Movemos el servo a los grados que le entreguemos
                servo.write(pos);
                //Esperamos 15 milisegundos
                delay(15);
              }
            }                         
            else if (header.indexOf("GET /PuertaOff") >= 0) {
              Serial.println("GPIO apagado");
              outputState = "apagado";
              for (pos = 180; pos >= 0; pos -= 1) {
              //Movemos el servo a los grados que le entreguemos
              servo.write(pos);
              //Esperamos 15 milisegundos
              delay(15);
            }
            }

            if (header.indexOf("GET /ElevadorOn") >= 0) {
              Serial.println("GPIO encendido");
              outputState = "encendido";
              miServo.write(0); // Mueve el servomotor a 180 grados
              delay(4500); // Espera 3 segundos
              miServo.write(90); // Mueve el servomotor a 0 grados
              //delay(4500); // Espera 3 segundos
            }                         
            else if (header.indexOf("GET /ElevadorOff") >= 0) {
              Serial.println("GPIO apagado");
              outputState = "apagado";

              miServo.write(180); // Mueve el servomotor a 180 grados
              delay(4500); // Espera 3 segundos
              miServo.write(90); // Mueve el servomotor a 0 grados
              //delay(4500); // Espera 3 segundos
              
            }
            
            

            

            
            

             //////// PAGINA WEB //////////////
             
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("</head>");

            client.println("<body></body>");

            client.println("</html>");

            client.println();
            break;

            
            /////////////////////////////////////
          }
          else
          {
            currentLine = "";
          }
        }
        else if ( c != '\r')
        {
          currentLine += c;
        }
        
        
      }

      
    }

    header = "";
    client.stop();
    Serial.println("Cliente desconectado.");
    Serial.println("");
  }

}
