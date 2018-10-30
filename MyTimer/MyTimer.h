#ifndef MyTimer_h
#define MyTimer_h

// includes files
#include "Arduino.h"

const uint8_t MODO_MANUAL = 00;
const uint8_t MODO_HORARIO = 01;
const uint8_t MODO_HORARIO_PIP = 02;
const uint8_t MODO_PIP = 03;

typedef struct{
  uint8_t mode; //00: Manual, Se activa/desactiva por indicación del usuario. 01: HORARIO. Se activa en el horario establecido. 02. HORARIO PIP. se activa en el horario fijado sólo cuando el PIP detecta presencia. 03. Sigue actividad del PIP.
  uint8_t type; //Configuración: output or input
  uint8_t actS; //Valor para Estado activo:  HIGH ó LOW.
  uint8_t lockS; //Valor Estado NO activo  HIGH ó LOW.
  uint32_t timeA; //Tiempo en MSegundos que deberá estar la salida en el estado activo. si el valor es 0 permanecera en el estado activo hasta que reciba una orden de cambiar de estado
  uint32_t timeD; //Vencido la temporización del estado activo, la salida conmuta al estado no activo por el tiempo, en MILISEGUNDOS, indicado en este campo. Si el valor es cero permanece en el hasta cambiar de estado.
  uint32_t times; //SI los campos timeA y timeD son distintos de cero, la salida repetirá el ciclo de activado desdactivado por el número de veces que indica este campo. Una vez finalizado pasará al estado no activo.
  uint8_t fromH;   //Las anteriores activaciones sólo tendrán lugar desde la hora marcada por from hasta la marcada por to. Si from = 0 y to = 24 las activaciones tendrán lugar las 24 horas.
  uint8_t fromM;
  uint8_t toH;
  uint8_t toM;
  uint8_t pinIN;
  uint8_t pinOUT;
}
cnfgGPIO_t;


class MyTimer
{
  

  public:
    //MyTimer(void (*_timeoutCallback)(uint16_t),uint8_t pin,uint16_t _counter);
    MyTimer(uint8_t pinIN, uint8_t pinOUT);
    void    begin();
    void    set(uint32_t segundos);
    void    clear();
    void    start();
    void    stop();
    void    update(uint8_t hour,uint8_t minutes);
    void    cnfgGPIO(cnfgGPIO_t cnfg);
    void    actGPIO(uint8_t value,uint8_t hour,uint8_t minutes);
    void    lockGPIO();
    String  getCnfg();
    void    setCnfg(String rest);

  private:
    uint32_t      counter;
    uint8_t       times;
   	uint16_t      counterState;
   	void (*timeoutCallback)(uint8_t pin, uint16_t cont);
    cnfgGPIO_t    cnfgEntDig;
    bool  checkIfWorkingHours(uint8_t hour, uint8_t minute);
};


#endif