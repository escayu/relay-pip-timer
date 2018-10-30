/*
  MyPip&Rele

  Arranca un temporizador de x milisegundos. Cuando el temporizador venza se 
  actua sobre un rele para cerrar o abrir un circuito (apagar/encender luz).
  El temporizador se reinicia cada que el PIP detecte presencia activando el rele
*/
#include <Temporizador.h>

// constants won't change. They're used here to set pin numbers:
const int pipPin  =   2;     // the number of the PIP pin
const int relePin =   3;     // the number of the Rele pin
const int ledPin  =  13;      // the number of the LED pin
const uint32_t alarm = 120000; // cada vez que el PIP se activa reiniciamos el temporizador por 2 minutos
uint8_t  pipState = LOW;
Temporizador count(&activarRele);

void setup() {
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(pipPin, INPUT);
  pinMode(relePin, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  // read the state of the pushbutton value:
  pipState = digitalRead(pipPin);
  count.loop();
  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (pipState == HIGH) {
    Serial.println("Reinicia contador");
    // turn LED on:
    digitalWrite(ledPin, HIGH);
    digitalWrite(relePin, HIGH);
    count.set(alarm);
  } else {
    // turn LED off:
    digitalWrite(ledPin, LOW);
  }
  delay(20);
}

void activarRele(){
  Serial.println("activaRele: Se apaga la luz");
  digitalWrite(ledPin, LOW);
  digitalWrite(relePin, LOW);  
}

