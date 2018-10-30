#include "MyTimer.h"

/******* PUBLIC FUNCTIONS RELATED WITH SLAVES AND BASIC BOARDS        ************************/


//MyTimer::MyTimer(void (*_timeoutCallback)(uint16_t),uint8_t pin, uint16_t _counter)
MyTimer::MyTimer(uint8_t pinIN, uint8_t pinOUT)
{
  counter 				      = millis();
  //timeoutCallback 	  = _timeoutCallback;
  counterState 			    = 0;
  cnfgEntDig.pinIN			= pinIN;
  cnfgEntDig.pinOUT			= pinOUT;
  pinMode(cnfgEntDig.pinIN,INPUT);
  pinMode(cnfgEntDig.pinOUT,OUTPUT);
}

void MyTimer::set(uint32_t segundos){
	counter 		= segundos;
	counterState 	= 1; //State of the counter: set
}

void MyTimer::clear(){
	counter = millis();
}

void MyTimer::start(){
	//Serial.println(" Start TIMER: "+String(cnfgEntDig.timeA));
	digitalWrite(cnfgEntDig.pinOUT, cnfgEntDig.actS);
	counter = millis()+cnfgEntDig.timeA;
	times -=1;
 	counterState = 2; //State of the counter: running active state
}

void MyTimer::stop(){
	counterState = 0; //State of the counter: stop
}


void MyTimer::update(uint8_t hour,uint8_t minutes){
  switch (cnfgEntDig.mode){
    case MODO_HORARIO: //El relé se activa siguiendo un horario, sin importar el estado del detector de presencia.
    if (checkIfWorkingHours(hour,minutes)){
      digitalWrite(cnfgEntDig.pinOUT, cnfgEntDig.actS);
    }
    else {
      digitalWrite(cnfgEntDig.pinOUT, cnfgEntDig.lockS);
    }
    break;
  }
	if (millis() >= counter){
		if (counterState == 2){
			digitalWrite( cnfgEntDig.pinOUT, cnfgEntDig.lockS); //La salida pasa de activa a NO activa.
			if(cnfgEntDig.timeD >= 0){ //Si hay un tiempo mayor que 0 de NO ACTIVAD se sigue hasta que venza la temporización.
				counter = millis()+cnfgEntDig.timeD;
				counterState = 3; //running No active state
			}
		}
		else if (counterState == 3){ //Se ha terminado un ciclo. 
			if(times > 0) {// se repite un nuevo ciclo si no se ha llegado al límite
				start();
			}
			else{
				stop();
			}
		}
	}	 
}

void MyTimer::cnfgGPIO(cnfgGPIO_t cnfg){
  cnfgEntDig.mode    = cnfg.mode;
  cnfgEntDig.type    = cnfg.type;
  cnfgEntDig.actS    = cnfg.actS;
  cnfgEntDig.lockS   = cnfg.lockS;
  cnfgEntDig.timeA   = cnfg.timeA*1000;
  cnfgEntDig.timeD   = cnfg.timeD*1000;
  cnfgEntDig.times   = cnfg.times;
  cnfgEntDig.fromH   = cnfg.fromH;
  cnfgEntDig.fromM   = cnfg.fromM;
  cnfgEntDig.toH   	= cnfg.toH;
  cnfgEntDig.toM   	= cnfg.toM;
  if((cnfg.pinIN <= 20) && (cnfg.pinIN != cnfgEntDig.pinIN)) {
  	cnfgEntDig.pinIN = cnfg.pinIN;
  	pinMode(cnfgEntDig.pinIN,INPUT);
  }
  if((cnfg.pinOUT <= 20) && (cnfg.pinOUT != cnfgEntDig.pinOUT)) {
  	cnfgEntDig.pinOUT = cnfg.pinOUT;
  	pinMode(cnfgEntDig.pinOUT,OUTPUT);
  }
  
  /*Serial.println(" CONFIG: Type="+String(cnfgEntDig.type)+". act="+String(cnfgEntDig.actS)+". lock="+String(cnfgEntDig.lockS)+". timeup=" + 
  	String(cnfgEntDig.timeA) +". timeDow=" + String(cnfgEntDig.timeD)+". count="+String(cnfgEntDig.times)+
  	". From: "+String(cnfgEntDig.fromH)+":"+String(cnfgEntDig.fromM)+" To: "+String(cnfgEntDig.toH)+":"+String(cnfgEntDig.toM));*/
}

void MyTimer::actGPIO(uint8_t value,uint8_t hour,uint8_t minutes){
  //Antes de actuar sobre el relé se comprueba si los modos de funcionamiento se corresponde con la actividad del PIP
	if (checkIfWorkingHours(hour,minutes) && ((cnfgEntDig.mode == MODO_HORARIO_PIP) || (cnfgEntDig.mode == MODO_PIP))) {
		if(cnfgEntDig.actS == value ){
			times = cnfgEntDig.times;
			//Serial.println(" ActGPIO: times = "+String(times)+" INPUT = "+String(value));
			if((cnfgEntDig.timeA>0)&&(times>0)){
				if (digitalRead(cnfgEntDig.pinIN) == cnfgEntDig.actS)
				{
					start();
				}
    	}
    	else{
    		if (digitalRead(cnfgEntDig.pinIN) == cnfgEntDig.actS)
    		{
    				digitalWrite(cnfgEntDig.pinOUT, cnfgEntDig.actS);
    		}
    		else{
    				digitalWrite(cnfgEntDig.pinOUT, cnfgEntDig.lockS);
    		}
    	}
    }
  }	
}

void MyTimer::lockGPIO(){
  digitalWrite(cnfgEntDig.pinOUT, cnfgEntDig.lockS);
}

bool  MyTimer::checkIfWorkingHours(uint8_t hour, uint8_t minute){
	if((cnfgEntDig.fromH == 00)&&(cnfgEntDig.fromM == 00)&&(cnfgEntDig.toH==23)&&(cnfgEntDig.toM == 59)){ //Horario de trabajo todo el día.
		return true;
	}
	else if((cnfgEntDig.fromH == cnfgEntDig.toH)&&(cnfgEntDig.fromM < cnfgEntDig.toM)){ //Horario de trabajo en el mismo día y misma hora
		if((cnfgEntDig.fromH == hour)&&(cnfgEntDig.fromM <= minute)&&(cnfgEntDig.toM > minute)){
			return true;
		}
  }
	else if(cnfgEntDig.fromH < cnfgEntDig.toH){
    if((cnfgEntDig.fromH < hour)&&(hour<cnfgEntDig.toH)){
            return true;
    }
		else if((cnfgEntDig.fromH == hour)&&(cnfgEntDig.fromM<=minute)){
      return true;
    }
    else if((cnfgEntDig.toH == hour)&&(cnfgEntDig.toM>minute)){
      return true;
    }
	}
	//Horario de trabajo final de un día  y principio del día siguiente: Por ejemplo desde: 19:35  del día actual hasta 6:53 del día siguiente.
	else if(cnfgEntDig.toH <cnfgEntDig.fromH){
		if((cnfgEntDig.fromH == hour)&&(cnfgEntDig.fromM <= minute)){
      return true;
    }
    else if((cnfgEntDig.toH == hour)&&(cnfgEntDig.toM > minute)){
      return true;
    }
    else if(((hour <= 23)&&(hour >cnfgEntDig.fromH))||((hour >=0)&&(hour < cnfgEntDig.toH))){
      return true;
    }
	}
	return false;
}

void MyTimer::setCnfg(String rest){
	uint8_t pinIN = cnfgEntDig.pinIN;
	uint8_t pinOUT = cnfgEntDig.pinOUT;
    uint8_t pin  = rest.substring(0,rest.indexOf('#')).toInt() ; //Salida digital que tiene asignada los contadores
    rest = rest.substring(rest.indexOf('#')+1);	
    uint8_t mode  = rest.substring(0,rest.indexOf('#')).toInt() ; //Modo de funcionamiento del dispositivo: MANUAL, HORARIO, HORARIO + PIP, SOLO PIP A CUALQUIER HORA
    rest = rest.substring(rest.indexOf('#')+1);
    uint8_t act = rest.substring(0,rest.indexOf('#')).toInt()==1?HIGH:LOW ; //Valor del pin para el estado ACTIVO
    rest = rest.substring(rest.indexOf('#')+1);
    uint8_t lock = rest.substring(0,rest.indexOf('#')).toInt()==1?HIGH:LOW ;  // Valor del pin para el estado NO ACTIVO
    rest = rest.substring(rest.indexOf('#')+1);
    uint32_t timeUp = rest.substring(0,rest.indexOf('#')).toInt() ; //Tiempo en estado activo. En segundos
    rest = rest.substring(rest.indexOf('#')+1);
    uint32_t timeDw = rest.substring(0,rest.indexOf('#')).toInt() ;//Tiempo en estado NO activo. En segundos
    rest = rest.substring(rest.indexOf('#')+1);
    uint32_t count = rest.substring(0,rest.indexOf('#')).toInt() ;//Número de veces que se repite el ciclo.
    rest = rest.substring(rest.indexOf('#')+1);
    uint8_t fromH = rest.substring(0,rest.indexOf('#')).toInt() ;//Este campo y el siguente configuran A partir de que hora y minuto se ejecuta la acción.
    rest = rest.substring(rest.indexOf('#')+1);
    uint8_t fromM = rest.substring(0,rest.indexOf('#')).toInt() ;//.
    rest = rest.substring(rest.indexOf('#')+1);
    uint8_t toH = rest.substring(0,rest.indexOf('#')).toInt() ;//Este campo y el siguente indican hasta que hora y minutos se ejecuta la acción.
    rest = rest.substring(rest.indexOf('#')+1);
    uint8_t toM = rest.substring(0,rest.indexOf('#')).toInt() ;//
    if (rest.indexOf('#') >= 0){
    	rest = rest.substring(rest.indexOf('#')+1);
    	pinIN = rest.substring(0,rest.indexOf('#')).toInt() ;//Se cambia la entrada digital donde se ubica el detector
     	if(rest.indexOf('#') >=0){
     		rest = rest.substring(rest.indexOf('#')+1);
     		pinOUT = rest.substring(0,rest.indexOf('#')).toInt() ;//Se cambia la salida digital donde se ubica el rele
    	}
    }
     //DEBUGprintln("CAMBIO CONFIG: pin="+String(pin)+". act="+String(act)+". lock="+String(lock)+". timeup=" + String(timeUp) +". timeDow=" + String(timeDw)+". count="+String(count));
    if(pin == cnfgEntDig.pinOUT){
    	cnfgGPIO_t newCfg = {mode,OUTPUT,act,lock,timeUp,timeDw,count,fromH,fromM,toH,toM,pinIN,pinOUT};
    	cnfgGPIO(newCfg);
    }
}

String MyTimer::getCnfg(){
	String tempo = 	String(cnfgEntDig.mode)+'#'+String(cnfgEntDig.type)+'#'+String(cnfgEntDig.actS)+"#"+String(cnfgEntDig.lockS)+"#"+String(cnfgEntDig.timeA/1000)+"#" + String(cnfgEntDig.timeD/1000) +"#" +
					String(cnfgEntDig.times)+"#"+String(cnfgEntDig.fromH)+"#"+String(cnfgEntDig.fromM)+"#"+String(cnfgEntDig.toH)+"#"+String(cnfgEntDig.toM)+"#"+
					String(cnfgEntDig.pinIN)+"#"+String(cnfgEntDig.pinOUT);
	return tempo;
}
