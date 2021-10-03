#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include "DHT.h"

#define DHTPIN1 12
#define DHTPIN2 11
#define DHTTYPE DHT11 
#define MIN_VOLTAGE 600
#define VREF 5000
#define PIN_LED 13
#define PIN_ANALOG 3
#define MAX_ITERS 20 
#define DEBUG true
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5
     
SoftwareSerial esp8266(1,0);
LiquidCrystal lcd(8,9,4,5,6,7); 
DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);

int lcd_key=0;
int adc_key_in  = 0;
int p = 0, l=0;
int ADC_VALUE;
int ITER;
float VOLTAGE; 
float DUST;
float AVG_DUST;
int ThermistorPin[2] = {1, 2};
int Vo;
float R1 = 10000;
float logR2, R2, Tc[2];
int T;
char aa[5];
String comV1, comV2, comV3, pV1, pV2, pV3;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
double r_check[5] = {0, 0, 0, 0, 0};
int r_check_l = 0;
double s_time = 0;
char inti;
int t=0;

//dust sensor
float computeDust()
{
	digitalWrite(PIN_LED, HIGH);
	delayMicroseconds(280);
	ADC_VALUE = analogRead(PIN_ANALOG);
	digitalWrite(PIN_LED, LOW);
	VOLTAGE = (VREF / 1024.0) * ADC_VALUE * 11;
  
	if (VOLTAGE > MIN_VOLTAGE)
	{
		return (VOLTAGE - MIN_VOLTAGE) * 0.2;
	}
 
	return 0;
}

//lcd
int read_LCD_buttons()
{
	adc_key_in = analogRead(0);

    if (adc_key_in > 1000) return btnNONE; 
    if (adc_key_in < 50)   return btnRIGHT;  
    if (adc_key_in < 195)  return btnUP; 
    if (adc_key_in < 380)  return btnDOWN; 
    if (adc_key_in < 555)  return btnLEFT; 
    if (adc_key_in < 790)  return btnSELECT;   

    return btnNONE;
}

void setup() 
{
	Serial.begin(115200);
	Wire.begin(8);
	Wire.onReceive(receiveEvent);
	Wire.onRequest(requestEvent);
	lcd.begin(16, 2);
  
	pinMode(PIN_LED, OUTPUT);
	digitalWrite(PIN_LED, LOW); 

	dht1.begin();
	dht2.begin(); 
}

//thermistor
void pTER()
{
	comV1="";
    comV1+="id=****&";
  
	for (int i = 0; i<2; i++)
	{
		Vo = analogRead(ThermistorPin[i]);
		R2 = R1 * (1023.0 / (float)Vo - 1.0);
		logR2 = log(R2);
		T = (int)round((100.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2)));
		Tc[i] = T/100 - 273.15;
		itoa(T, aa, 10);
   
		if(i == 0) 
		{
			comV1 += "T1=";
			comV1 += aa;
			comV1 += "&";
		} 
		else if (i ==1) 
		{
			comV1 += "T2=";
			comV1 += aa;    
			comV1 += "&";
		}
	}
	
	pV1=comV1;
}

//dht
void pDHTo()
{
	comV2="";
	T = ((dht1.readTemperature()+273.15)*100);

    itoa(T, aa, 10);
    comV2 += "DTO=";
    comV2 += aa;
    comV2 +="&";
        
	T = (dht1.readHumidity());
  
    itoa(T, aa, 10);
    comV2 += "DHO=";
    comV2 += aa;
    comV2 +="&";
    pV2=comV2;
}

//dht
void pDHTi()
{
	comV3="";
	T = ((dht2.readTemperature()+273.15)*100);

    itoa(T, aa, 10);
    comV3 += "DTI=";
    comV3 += aa;
    comV3 +="&";
        
	T = (dht2.readHumidity());
  
    itoa(T, aa, 10);
    comV3 += "DHI=";
    comV3 += aa;
    comV3 +="&";
}

//dust sensor
void pDUST()
{
    AVG_DUST = 0;
	ITER = 0;
 
	while (ITER < MAX_ITERS)
	{
		DUST = computeDust();
		if (DUST > 0)
		{
			AVG_DUST += DUST;
			ITER++;
			delay(50);
		}     
	} 
   
	AVG_DUST /= MAX_ITERS;
	AVG_DUST *= 100;

	aa[0] = "0";
	itoa(AVG_DUST, aa, 10);
   
	String pom = aa;

	if (pom.length() == 4)
		pom = "0" + pom;
	else if(pom.length() == 6) 
		pom = "00000";

	comV3 += "DUST=" ;
	comV3 +=  pom;  
	pV3=comV3;
}

//lcd
void smart_lcd()
{
	if(millis() - l > 10000)
		pinMode(10,OUTPUT);

	lcd.setCursor(0,0);
	lcd_key = read_LCD_buttons();
   
	if(lcd_key != 5)
	{
		pinMode(10,INPUT);
		l=millis();
		lcd.begin(16, 2); 
   
		switch (lcd_key)
		{
			case 2:
			{
				lcd.clear();
				lcd.print("Temp out|Th");
				lcd.setCursor(0,1);
				lcd.print(Tc[1]);
				lcd.print(" C");
				p++; 
				break;
			}
        
			case 1:
			{
				lcd.clear();
				lcd.print("Temp in|Th");
				lcd.setCursor(0,1);
				lcd.print(Tc[0]);
				lcd.print(" C");
				p++;
				break;
			}
        
			case 4:
			{
				lcd.clear();
				lcd.print("Tmp|Hum in|DHT");
				lcd.setCursor(0,1);
				lcd.print(dht2.readTemperature());
				lcd.print("C ");
				lcd.print("| ");
				lcd.print(dht2.readHumidity());
				lcd.print("%");
				p++;
				break;
			}

			case 3:
			{
				lcd.clear();
				lcd.print("Tmp|Hum out|DHT");
				lcd.setCursor(0,1);
				lcd.print(dht1.readTemperature());
				lcd.print(" C");
				lcd.print("| ");
				lcd.print(dht1.readHumidity());
				lcd.print("%");
				p++;
				break;
			}

			case 0:
			{
				lcd.clear();
				lcd.print("Dust out");
				lcd.setCursor(0,1);
				lcd.print(AVG_DUST/100);
				lcd.print(" ug/m3");
				p=0;
				break;
			}
		} 
	}
}

void loop() 
{
	smart_lcd();
	pTER();
	pDHTo();
	pDHTi();
	pDUST();
}

void receiveEvent()
{
	while(0 < Wire.available()) 
	{
		inti = Wire.read();
	}
}


void requestEvent() 
{
	//send data to nano on request
	switch (inti)
	{
		case '0':
		{
			char b0[27];
			pV1.toCharArray(b0, 27);
			Wire.write(b0,27);
			break;
		}
     
		case '1':
		{
			char b1[18];
			pV2.toCharArray(b1, 18);
			Wire.write(b1,18);
			break;
		}

		case '2':
		{
			char b2[28];
			pV3.toCharArray(b2, 28);
			Wire.write(b2);
			break;
		}
	}
}
