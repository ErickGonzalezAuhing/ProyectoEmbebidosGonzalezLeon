#include <Arduino.h>
#include <ESP32Servo.h>
#include <BluetoothSerial.h>
BluetoothSerial SerialBT;
Servo servo;
const int PinIN1 = 13;
const int PinIN2 = 12;

const int PinIN3 = 14;
const int PinIN4 = 27;

const int PinServo = 32;
bool emparejado = false;
int receivedFlex1; 
int receivedFlex2;


// Funcion para comprobar la conexion a la esp32 master 
void connect_successful(){
  while (!emparejado)
  {
    if (!SerialBT.connected())
    {
      Serial.println("Esperando emparejamiento...");
      delay(1000);
    }

    else
    {
      Serial.println("Emparejado con el maestro");
      emparejado = true;
    }
  }
}

void motores(){
    // Condicional para determinar si los motores DC se encienden segun el sensor de flexion 2
  if (receivedFlex2 <= 1760){
  // MOTOR 1 HORARIO
  digitalWrite (PinIN1, HIGH);
  digitalWrite (PinIN2, LOW);
  // MOTOR 2 HORARIO
  digitalWrite (PinIN3, HIGH);
  digitalWrite (PinIN4, LOW);
  }
  else if(receivedFlex2 > 1760){
  // MOTOR 1 HORARIO
  digitalWrite (PinIN1, LOW);
  digitalWrite (PinIN2, LOW);
  // MOTOR 2 HORARIO
  digitalWrite (PinIN3, LOW);
  digitalWrite (PinIN4, LOW);
  }
}

void sensores(){
  //Condicionales para determinar el movimiento del servomotor segun el sensor de flexion 1
  if((receivedFlex1<=900)&&(receivedFlex1>=700)){
    servo.write(20);
    
  }
  else if((receivedFlex1<700)&&(receivedFlex1>=650)){
    servo.write(60);
    
  }
  else if((receivedFlex1<650)&&(receivedFlex1>=450)){
    servo.write(90);
  
  }
}



void setup() {
  // inicializar la comunicación serial a 9600 bits por segundo:
  // configuramos los pines como salida
  pinMode(PinIN1, OUTPUT);
  pinMode(PinIN2, OUTPUT);
  pinMode(PinIN3, OUTPUT);
  pinMode(PinIN4, OUTPUT);
  servo.attach(PinServo);
  // Configuramos el bluetooh entre maestro y esclavo
  Serial.begin(9600);
  SerialBT.begin("esclavoProyecto");  // Nombre del dispositivo Bluetooth del esclavo
  SerialBT.connect("maestroProyecto"); // Nombre del dispositivo Bluetooth del maestro
  Serial.println("ESP32 esclavo iniciado");
  connect_successful();
}


void loop() {


    if (SerialBT.available() >= 2 * sizeof(int)) {
    // Leer dos enteros

    SerialBT.readBytes(reinterpret_cast<char*>(&receivedFlex1), sizeof(int));
    SerialBT.readBytes(reinterpret_cast<char*>(&receivedFlex2), sizeof(int));

    // Imprimir los enteros recibidos en el Monitor Serie
    Serial.print("Dato entero Flex 1 recibido: ");
    Serial.println(receivedFlex1);
    Serial.print("Dato entero Flex 2 recibido: ");
    Serial.println(receivedFlex2);
  }
  
  sensores();
  motores();
  
}
