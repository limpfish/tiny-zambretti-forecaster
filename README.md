# Tiny Zambretti Forecaster
Simple weather forecaster using just an ATtiny and a BMP280

![ATtiny zambretti](https://user-images.githubusercontent.com/25790676/169568495-f33bd270-a340-4434-95f6-3f9f036821f9.jpg)


Henry Negretti and Joseph Zambra, renowned scientific instrument makers to Queen Victoria, invented the 'Zambretti Forecaster'. This weird contraption used a series of overlaid discs and operated much like a slide rule. 
With just wind and barometer readings, the 'Zambretti' could produce 90% accurate 12 hour weather forecasts.
This fun little project attempts to squeeze this old 'Zambretti Forecaster' into an 8K ATtiny.



My attempt of the Zambretti algorithm is based on https://github.com/sassoftware/iot-zambretti-weather-forcasting and, mostly, this Javascript implementation http://www.beteljuice.co.uk/zambretti/forecast.html albeit without the wind direction readings!

![wiring diagram](https://user-images.githubusercontent.com/25790676/169569221-613f6375-a9e3-4b54-810e-e8785b88eca8.png)

![ATtiny breadboard](https://user-images.githubusercontent.com/25790676/169568401-5cf4a321-2f9a-41a5-b37d-09acd142ae6e.jpg)

## Libraries used 
I use https://github.com/limpfish/TinyOzOledlimpfish for the OLED display and Forced-BME280 https://github.com/JVKran/Forced-BME280 by Jochem van Kranenburg for the pressure sensor
 



