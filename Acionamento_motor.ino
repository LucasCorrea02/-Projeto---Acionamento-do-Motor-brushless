// --- Inclusão de Biblioteca ---
#include <Servo.h>    
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// --- Mapeamento de Hardware ---
#define   pino_D0  3

// --- Criação Classe Servo ---
Servo esc;                                
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- Variáveis Globais --- 
float rpm;
volatile byte pulsos;
unsigned long timeold;
unsigned int pulsos_por_volta = 10;
int val;

// --- Interrupção ---
void contador()
{
  //Incrementa contador
  pulsos++;
}
// --- Configurações Iniciais ---
void setup()
{
  esc.attach(6);                          
  esc.writeMicroseconds(1000);            
  lcd.begin ();
  pinMode(pino_D0, INPUT);
                                          //Interrupcao 1 - pino digital 3
                                          //Aciona o contador a cada pulso
  attachInterrupt(1, contador, FALLING);
  Serial.begin(115200);
  pulsos = 0;
  rpm = 0;
  timeold = 0; 
}
void loop()
{
  //Atualiza contador a cada segundo
  if (millis() - timeold >= 1000)
  {
                                                   //Desabilita interrupcao durante o calculo
    detachInterrupt(1);
    rpm = ((60 * 1000 / pulsos_por_volta ) / (millis() - timeold) * pulsos);
    timeold = millis();
    pulsos = 0;
    
    //Mostra o valor de RPM no display
    lcd.setBacklight(HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Rotacao: "); 
    lcd.setCursor(1, 1);
    lcd.print(rpm, 0);
    lcd.setCursor(8, 1);
    lcd.print("RPM");  
    delay(500);                                                               
    val= analogRead(A0);                
    Serial.println(val);
    val= map (val, 0,1023, 1000, 2000);     
    esc.writeMicroseconds(val);           
                                                  //Habilita interrupcao
    attachInterrupt(1, contador, FALLING);
  }
  }
