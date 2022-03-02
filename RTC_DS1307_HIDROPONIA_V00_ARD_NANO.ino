/*********************************************************
 * Projeto.: Relogio e timer usando o DS1307             *
 *-------------------------------------------------------*
 * SUMARIO DO PROJETO.:                                  *
 * UTILIZACAO TIMER EM HIDROPONIA OPERACAO:              *
 * 1) BOMBA ON 15 - OFF 15 MINUTOS - DAS 8:00 AS 16:59   *
 * 2) BOMBA ON 15 - OFF 60 MINUTOS - DAS 17:00 AS 7:59   *
 * ------------------------------------------------------*
 * CLIENTE.:   Prototipo                                 *
 * AUTOR  .:   Renato Correa                             *
 * DATA   .:   20220222                                  *
 * Placa  .:   ARDUINO NANO                              *
 * Arquivo.:   RTC_DS1307_HIDROPONIA_V00_ARD_NANO.ino    *
 * Versoes.:                                             *
 * VER  |DESCRICAO                            | DATA     *
 *  0.00|INICIAL                              | 20220222 *
 *                                                       *
 *                                                       *
 * Portas utilizadas (SAIDAS).:                          *
 * Pino 8  = Rele                                        *
 *                                                       *
 * Portas utilizadas (ENTRADAS).:                        *
 * NAO USA                                               *
 *                                                       * 
 * OBS.:                                                 *
 *                                                       *
 *                                                       *
 *                                                       *
 *                                                       *
 *********************************************************/


#include "Wire.h"
#define DS1307_ADDRESS 0x68
#define relay 8              // Aciona o rele D8 = 8 -> Logica invertida 0 = Ativa, 1 = Desativa
#define Interval 1000        // Temporizacao para o loop

// Intervalos que define o tempo em que bomba vai funcionar em minutos
#define intervalo_ON_dia  300000
#define intervalo_OFF_dia  300000

#define intervalo_ON_noite  300000
#define intervalo_OFF_noite   1800000

byte zero = 0x00;

// Editar as horas liga e desliga abaixo para programar o liga e desliga
unsigned int intIniDia = 800;     //Inicio da hora do dia 
unsigned int intFimDia = 1700;  //Fim da hora do dia 

unsigned int intFim = 0;

unsigned int minuto = 0;
unsigned int hora = 0;

// Conversao de hora minuto para um numero inteiro
// Ativa timer se tiver nos horarios
void timerAtive()
{
  String strHora = "";
  String strMinuto = "";
  String fimStr = "";
  
  if(minuto < 10)
  {
      strMinuto = String(minuto);
      strMinuto = "0" + strMinuto;
  }
  else
  {
    strMinuto = String(minuto);
  }
  
  if(hora < 10)
  {
      strHora = String(hora);
      strHora = "0" + strHora;
  }
  else
  {
    strHora = String(hora);
  }

  fimStr = strHora + strMinuto;

  // Convertido para integer
  intFim = fimStr.toInt();
  Serial.print("intFim: ");
  Serial.println(intFim);
  
  //Faz a comparacao para verficar se esta no tempo
  // VERFICA SE ATIVA A SAIDA OU NAO

  Serial.println("\n*** CICLO ***");
  
  if((intFim >= intIniDia && intFim <= intFimDia) || (intFim >= 2400)) 
  {
    printDate();
    Serial.print("PERIODO DIA: ");
    // Ligando a bomba (15 minutos)
    Serial.println("Relay -> ON");
    digitalWrite(relay, LOW);
    delay(intervalo_ON_noite);

    printDate();
    Serial.print("PERIODO DIA: ");
    // Desligando a bomba (15 minutos)
    Serial.println("Relay -> OFF");
    digitalWrite(relay, HIGH);
    delay(intervalo_OFF_dia);
    
  }
  else // Entra aqui quando nao esta no intervalo acima ou seja 0:00 as 7:59 e 18:01 as 23:59
  {
    printDate();
    Serial.print("PERIODO NOITE: ");
    // Ligando a bomba (15 minutos)
    Serial.println("Relay -> ON");
    digitalWrite(relay, LOW);
    delay(intervalo_ON_noite);

    printDate();
    Serial.print("PERIODO NOITE: ");
    // Desligando a bomba (60 minutos)
    Serial.println("Relay -> OFF");
    digitalWrite(relay, HIGH);
    delay(intervalo_OFF_noite);
  }
}



void setup(){
  Wire.begin();
  Serial.begin(9600);
  delay(1000);
  pinMode(relay, OUTPUT);
  //setDateTime();            //Necessário configurar na função "setDateTime()"
  digitalWrite(relay,HIGH);
  Serial.println("** START - by RLC VER 00 - Hidroponia **");
  
  Serial.println("[Parametros]");
  Serial.print("Inicio do dia(hhmm): ");
  Serial.println(intIniDia);

  Serial.print("Fim do dia(hhmm): ");
  Serial.println(intFimDia);

  
  Serial.println("** Ini Relay **");
  delay(1000);
  iniRelay();
}

void loop(){
  printDate();
  
  // Verificar se o tempo esta no tempo para ativar/desativar o timer
  timerAtive();
  delay(Interval);
}

void iniRelay()
{
  delay(100);
  int cont;
  for(cont=0;cont < 2;cont++)
  {
    digitalWrite(relay,LOW);
    delay(1000);
    digitalWrite(relay,HIGH);
    delay(1000);
  }
}

void setDateTime(){

  byte segundo =      0;  //0-59
  byte minuto =       4;  //0-59
  byte hora =         11;  //0-23
  byte diasemana =    4;  //1-7
  byte dia =          2;  //1-31
  byte mes =          3; //1-12
  byte ano  =         22; //0-99

  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero); 

  Wire.write(decToBcd(segundo));
  Wire.write(decToBcd(minuto));
  Wire.write(decToBcd(hora));
  Wire.write(decToBcd(diasemana));
  Wire.write(decToBcd(dia));
  Wire.write(decToBcd(mes));
  Wire.write(decToBcd(ano));

  Wire.write(zero); 

  Wire.endTransmission();

}

byte decToBcd(byte val){
// Conversão de decimal para binário
  return ( (val/10*16) + (val%10) );
}

byte bcdToDec(byte val)  {
// Conversão de binário para decimal
  return ( (val/16*10) + (val%16) );
}

void printDate(){

  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero);
  Wire.endTransmission();

  Wire.requestFrom(DS1307_ADDRESS, 7);

  int segundo = bcdToDec(Wire.read());
  minuto = bcdToDec(Wire.read());
  hora = bcdToDec(Wire.read() & 0b111111);    //Formato 24 horas
  int diasemana = bcdToDec(Wire.read());             //0-6 -> Domingo - Sábado
  int dia = bcdToDec(Wire.read());
  int mes = bcdToDec(Wire.read());
  int ano = bcdToDec(Wire.read());
  
  

  //Exibe a data e hora. Ex.:   3/12/13 19:00:00
  Serial.print(dia);
  Serial.print("/");
  Serial.print(mes);
  Serial.print("/");
  Serial.print(ano);
  Serial.print(" ");
  Serial.print(hora);
  Serial.print(":");
  Serial.print(minuto);
  Serial.print(":");
  Serial.println(segundo);

  

}
