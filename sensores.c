#include <Arduino.h>
#include <BluetoothSerial.h>
BluetoothSerial SerialBT;
bool emparejado = false;

#define PRO_CPU 0
#define APP_CPU 1
#define NOAFF_CPU tskNO_AFFINITY

#define pinFlex 2
#define pinFlexSecond 4

void TestHwm(char *taskName);
void TaskFlex(void *pvParameters);
void TaskFlexSecond(void *pvParameters);


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
      Serial.println("Emparejado con el esclavo");
      emparejado = true;
    }
  }
}


void setup(){
  xTaskCreatePinnedToCore(TaskFlex,"TaskFlex",4096,NULL,1,NULL,APP_CPU);
  xTaskCreatePinnedToCore(TaskFlexSecond,"TaskFlexSecond",4096,NULL,1,NULL,APP_CPU);
  Serial.begin(9600);
  SerialBT.begin("maestroProyecto", true); // Nombre del dispositivo Bluetooth del maestro el valor de true para activar master
  SerialBT.connect("esclavoProyecto");    // Nombre del dispositivo Bluetooth del esclavo
  Serial.println("ESP32 maestro iniciado");
  //connect_successful();
}

void loop(){}
// Testear tamano de la tarea 
void TestHwm(char *taskName){
  static int stack_hwm, stack_hwm_temp;

  stack_hwm_temp = uxTaskGetStackHighWaterMark(nullptr);
  if(!stack_hwm || (stack_hwm_temp < stack_hwm)){
    stack_hwm=stack_hwm_temp;
    Serial.printf("%s has stack hwm %u\n",taskName,stack_hwm);
  }
} 

// Funcion independiente de la tarea para leer el sensor flex
void TaskFlex(void *pvParameters){
  (void) pvParameters;
  int flexValue;
  Serial.begin(9600);
  connect_successful();
  while(1){
      flexValue = analogRead(pinFlex);
      Serial.print("Sensor flex:");
      Serial.println(flexValue);
      
      SerialBT.write(reinterpret_cast<uint8_t*>(&flexValue), sizeof(int));
      vTaskDelay(1000);
  }
}


// Funcion independiente de la tarea para leer el sensor flex 2
void TaskFlexSecond(void *pvParameters){
  (void) pvParameters;
  int flexValue2;
  Serial.begin(9600);
  connect_successful();
  while(1){
      flexValue2 = analogRead(pinFlexSecond);
      Serial.print("Sensor flex 2:");
      Serial.println(flexValue2);

      SerialBT.write(reinterpret_cast<uint8_t*>(&flexValue2), sizeof(int));
      vTaskDelay(1000);
  }
}
