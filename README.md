# Smart Weather Station

Arduino and server software for weather condition station. Project allows to measure temperature, humidity and air pollution, display current values on LCD screen and save data in cloud.

### Project implementation
<table>
    <tr>
        <td align="center"><img src="https://github.com/jakubx6/smart_weather/blob/main/img/inside.jpg" width=200></td>
        <td align="center"><img  src="https://github.com/jakubx6/smart_weather/blob/main/img/outside.jpg" width=200></td>
    </tr>
    <tr>
        <td style="text-align: center">Inside logic platform with sensors and LCD.</a></td>
        <td style="text-align: center">Outside module with sensors.</a></td>
    </tr>
</table>

### Sensors and modules

Boards:
- Arduino UNO
- Arduino NANO
- ESP 01S

Shields:
- DFRobot LCD Keypad
- ESP01 Adapter

Sensors:
- DHT11
- Thermistor 10 kΩ
- GP2Y1010AU0F Optical Dust Sensor

### Website data diplay
<table>
    <tr>
        <td align="center"><img src="https://github.com/jakubx6/smart_weather/blob/main/img/website.jpg" width=200></td>
    </tr>
    <tr>
        <td style="text-align: center"><a href="http://sympozjonlab.fun:4200/graph?minutes=15">Check Website</a></td>
    </tr>
</table>

### Project description
The project uses low-cost modules available on the Internet. There are two Arduino boards. UNO handling communication with sensors, calculation of correct values of measured conditions, transmission of values on demand to the NANO via I2C interface and display condition values on LCD screen. NANO is set to receive data from UNO and ESP module operation, like sending data to the Cloud. Internet service is responsible for receive data, stored it and displaying on the website. There are three webservices:
- http://sympozjonlab.fun:4200/results
- http://sympozjonlab.fun:4200/average?minutes=15
- http://sympozjonlab.fun:4200/graph?minutes=15

*parameter 'minutes' is responsible for agregate data in range 1-60 minutes*

### License
Take what you want and have fun ;)
