#include <Wire.h>
#include <SoftwareSerial.h>

#define DEBUG true

SoftwareSerial esp8266(1,0);

String comV1, comV2, comV3, comV4;
double s_time=millis();
char c;

String sendData(String command, const int timeout, boolean debug)
{
	String response = "";
        
    esp8266.print(command);
        
    long int time = millis();
        
    while( (time+timeout) > millis())
    {
        while(esp8266.available())
        {
            char c = esp8266.read();
            response+=c;
        }  
    }
        
    if(debug)
    {
        Serial.print(response);
    }
        
    return response;
}

void esp_reset()
{
	pinMode(2, OUTPUT);
    delay(1000);
    pinMode(2, INPUT);
}

void setup() 
{
	Wire.begin();
	esp8266.begin(115200);
  
	pinMode(2, INPUT);
	digitalWrite(2,LOW);

  
    sendData("AT\r\n",2000,DEBUG);
    sendData("AT+RST\r\n",2000,DEBUG);
    sendData("AT+CWMODE=1\r\n",2000,DEBUG);
    sendData("AT+CWJAP=\"*****\",\"*****\"\r\n",6000,DEBUG); //SSID, PASSWORD

	esp_reset();
}

void loop() 
{
	if(abs(millis()-s_time) >=  600000)
	{
		esp_reset();
		delay(2000);
	}
    
	if(abs(millis()-s_time) >=  30000)
	{
		comV1=""; 
		Wire.beginTransmission(8);
		Wire.write("0");
		Wire.endTransmission(); 
		Wire.requestFrom(8, 26); 
  
		while (Wire.available()) 
		{
			c = Wire.read();
			comV1+=c;       
		}

		if (comV1.length() != 26)
			continue;
		delay(1000);

		comV2="";
		Wire.beginTransmission(8);
		Wire.write("1");
		Wire.endTransmission();
		Wire.requestFrom(8, 17); 
	
		while (Wire.available()) 
		{
			c = Wire.read();
			comV2+=c;       
		}

		if (comV2.length() != 17)
			continue;
		delay(1000);

		comV3="";
		Wire.beginTransmission(8);
		Wire.write("2");
		Wire.endTransmission();
		Wire.requestFrom(8, 27); 
		
		while (Wire.available()) 
		{
			c = Wire.read();
			comV3+=c;       
		}

		if (comV3.length() != 27)
			continue;

		comV4="";
		comV4=comV1+comV2+comV3;
		comV4+="\r\n";

		sendData("AT+CIPSTART=\"TCP\",\"*****\",**\r\n",3000,DEBUG); //IP, PORT
		delay(500);
		sendData("AT+CIPSEND=100\r\n",1000,DEBUG);
		delay(500);
		sendData("POST /arduino HTTP/1.1\nContent-type: application/xml\nHost: *****:**\nContent-Length: 70\n\r\n",2000,DEBUG); //IP:PORT
		sendData("AT+CIPSEND=72\r\n",1000,DEBUG);
		delay(500);
		sendData(comV4,1000,DEBUG);
		delay(2000);
  
		s_time=millis();
	}
	
	delay(100);
}
