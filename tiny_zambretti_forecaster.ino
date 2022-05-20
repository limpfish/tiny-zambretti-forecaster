#include "TinyWireM.h"
#include "TinyOzOLEDlimpfish.h"
#include <forcedClimate.h>

#define SUMMER_SWITCH 3

ForcedClimate climateSensor = ForcedClimate(TinyWireM, 0x76);



byte fontcolumn =0;
byte messagei = 0;
byte message_char_index = 0;
char message[30];

// To determine the trend I compare a pressure reading with one taken 3 hours ago. 
// I take a reading roughly every 30 minutes, so that's 7 readings over 3 hours

#define total_logs 7
uint32_t pressure_values[total_logs];
byte pressure_index_tail = 0;
byte pressure_index_head = total_logs-1;
uint32_t  pressure, old_pressure;

// 30 minutes in millis 1800000
#define interval 1800000
unsigned long previousMillis = interval;                                                                                      
unsigned long currentMillis = 0;

// Zambretti stuff...  Based on : http://www.beteljuice.co.uk/zambretti/forecast.html
// Storing strings with progmem, so weirdness follows :
const char string_0[]  PROGMEM = "SETTLED@FINE";
const char string_1[]  PROGMEM = "FINE@WEATHER";
const char string_2[]  PROGMEM = "BECOMING@FINE";
const char string_3[]  PROGMEM = "FINE@BECOMING@LESS@SETTLED";
const char string_4[]  PROGMEM = "FINE@POSSIBLE@SHOWERS";
const char string_5[]  PROGMEM = "FAIRLY@FINE@IMPROVING";
const char string_6[]  PROGMEM = "FAIRLY@FINE@POSSIBLE@SHOWERS";
const char string_7[]  PROGMEM = "FAIRLY@FINE@SHOWERY@LATER";
const char string_8[]  PROGMEM = "SHOWERY@EARLY@IMPROVING";
const char string_9[]  PROGMEM = "CHANGEABLE@MENDING";
const char string_10[] PROGMEM = "FAIRLY@FINE@SHOWERS@LIKELY";
const char string_11[] PROGMEM = "RATHER@UNSETTLED@CLEARING";
const char string_12[] PROGMEM = "UNSETTLED@PROBABLY@IMPROVING";
const char string_13[] PROGMEM = "SHOWERY@BRIGHT@INTERVALS";
const char string_14[] PROGMEM = "SHOWERY@LESS@SETTLED";
const char string_15[] PROGMEM = "CHANGEABLE@SOME@RAIN";
const char string_16[] PROGMEM = "UNSETTLED@FINE@INTERVALS";
const char string_17[] PROGMEM = "UNSETTLED@RAIN@LATER";
const char string_18[] PROGMEM = "UNSETTLED@SOME@RAIN";
const char string_19[] PROGMEM = "MOSTLY@VERY@UNSETTLED";
const char string_20[] PROGMEM = "OCCASIONAL@RAIN@WORSENING";
const char string_21[] PROGMEM = "SOME@RAIN@VERY@UNSETTLED";
const char string_22[] PROGMEM = "RAIN@AT@FREQUENT@INTERVALS";
const char string_23[] PROGMEM = "RAIN@VERY@UNSETTLED";
const char string_24[] PROGMEM = "STORMY@MAY@IMPROVE";
const char string_25[] PROGMEM = "STORMY@MUCH@RAIN";

const char * const string_table[] PROGMEM =   
{   
  string_0,
  string_1,
  string_2,
  string_3,
  string_4,
  string_5,
  string_6,
  string_7,
  string_8,
  string_9,
  string_10,
  string_11,
  string_12,
  string_13,
  string_14,
  string_15,
  string_16,
  string_17,
  string_18,
  string_19,
  string_20,
  string_21,
  string_22,
  string_23,
  string_24,
  string_25
  };

const uint8_t rise_options[]   PROGMEM = {25,25,25,24,24,19,16,12,11, 9, 8, 6, 5, 2, 1, 1, 0, 0, 0, 0, 0, 0}; 
const uint8_t steady_options[] PROGMEM = {25,25,25,25,25,25,23,23,22,18,15,13,10, 4, 1, 1, 0, 0, 0, 0, 0, 0};
const uint8_t fall_options[]   PROGMEM = {25,25,25,25,25,25,25,25,23,23,21,20,17,14, 7, 3, 1, 1, 1, 0, 0, 0}; 

// barometer range. UK range? : 
#define z_upper  1050
#define z_lower  950
#define z_range (z_upper - z_lower)
// for the sea level adjust, your elevation in metres
#define height_above_sealevel 215


// z_hpa : Sea level adjusted pressure hPa
// z_season: true if Spring/Summer, March - September
// z_trend: 0=steady,1:rise,2:fall

void zambretti_forecast(float z_hpa, bool z_season, byte z_trend)
{ 
    float z_constant = (float(z_range) / 22);
    
    // Northern Hemisphere 
    /*
     * Wind readings.  NOT USED but included here for completeness
     * English windrose cardinal eg. N, NNW, NW etc.
    if (z_wind == "N") {  
      z_hpa += (6.0 / 100) * z_range;  
    } else if (z_wind == "NNE") {  
      z_hpa += (5.0 / 100) * z_range;  
    } else if (z_wind == "NE") {  
      z_hpa += (5.0 / 100) * z_range;  
    } else if (z_wind == "ENE") {  
      z_hpa += (2.0 / 100) * z_range ;  
    } else if (z_wind == "E") {  
      z_hpa -= (0.5 / 100) * z_range;  
    } else if (z_wind == "ESE") {  
      z_hpa -= (2.0 / 100) * z_range;  
    } else if (z_wind == "SE") {  
      z_hpa -= (5.0 / 100) * z_range;  
    } else if (z_wind == "SSE") {  
      z_hpa -= (8.5 / 100) * z_range;  
    } else if (z_wind == "S") {  
      z_hpa -= (12.0 / 100) * z_range;  
    } else if (z_wind == "SSW") {  
      z_hpa -= (10.0 / 100) * z_range;  
    } else if (z_wind == "SW") {  
      z_hpa -= (6.0 / 100) * z_range;  
    } else if (z_wind == "WSW") {  
      z_hpa -= (4.5 / 100) * z_range;  
    } else if (z_wind == "W") {  
      z_hpa -= (3.0 / 100) * z_range;  
    } else if (z_wind == "WNW") {  
      z_hpa -= (0.5 / 100) * z_range;  
    }else if (z_wind == "NW") {  
      z_hpa += (1.5 / 100) * z_range;  
    } else if (z_wind == "NNW") {  
      z_hpa += (3.0 / 100) * z_range;  
    } 
     * 
     */
    // NORTHERN HEMISPHERE Summer?
    if (z_season == true)  
    {   
      if (z_trend == 1)  // rising
      {   
        z_hpa += ((7.0 / 100.0)* z_range);
      } 
      else if (z_trend == 2)  //  falling
      {      
        z_hpa -= ((7.0 / 100.0)* z_range);
      }
    }



/* SOUTHERN HEMISPHERE */

/* Wind readings.  NOT USED but included here for completeness
 * English windrose cardinal eg. N, NNW, NW etc.
 *  if (z_wind == "S") {  
      z_hpa += (6.0 / 100) * z_range;  
    } else if (z_wind == "SSW") {  
      z_hpa += (5.0 / 100) * z_range;  
    } else if (z_wind == "SW") {  
      z_hpa += (5.0 / 100) * z_range;  
    } else if (z_wind == "WSW") {  
      z_hpa += (2.0 / 100) * z_range;  
    } else if (z_wind == "W") {  
      z_hpa -= (0.5 / 100) * z_range;  
    } else if (z_wind == "WNW") {  
      z_hpa -= (2.0 / 100) * z_range;  
    } else if (z_wind == "NW") {  
      z_hpa -= (5.0 / 100) * z_range;  
    } else if (z_wind == "NNW") {  
      z_hpa -= (8.5 / 100) * z_range;  
    } else if (z_wind == "N") {  
      z_hpa -= (12.0 / 100) * z_range;  
    } else if (z_wind == "NNE") {  
      z_hpa -= (10.0 / 100) * z_range; 
    } else if (z_wind == "NE") {  
      z_hpa -= (6.0 / 100) * z_range;  
    } else if (z_wind == "ENE") {  
      z_hpa -= (4.5 / 100) * z_range;  
    } else if (z_wind == "E") {  
      z_hpa -= (3.0 / 100) * z_range;  
    } else if (z_wind == "ESE") {  
      z_hpa -= (0.5 / 100) * z_range;  
    }else if (z_wind == "SE") {  
      z_hpa += (1.5 / 100) * z_range;  
    } else if (z_wind == "SSE") {  
      z_hpa += (3.0 / 100) * z_range;  
    } 
 */
/* SOUTHERN HEMISPHERE Season adjust */
/*
  if (z_season == 0) 
   { 
      if (z_trend == 1) // rising
      {  
        z_hpa += ((7.0 / 100.0) * z_range);  
      } 
      else if (z_trend == 2) 
      {  // falling
        z_hpa -= ((7.0 / 100.0) * z_range); 
      } 
    } 
 */

    
  // limits
  
  if (z_hpa == z_upper) z_hpa = z_upper - 1; 
  byte z_option = floor((z_hpa - z_lower) / z_constant); 


  if (z_option < 0) z_option = 0;  
  if (z_option > 21) z_option = 21;


  // rising 
  if (z_trend == 1) 
  {
    strcpy_P(message, (char*)pgm_read_word(&(string_table[pgm_read_byte(&rise_options[z_option])]))); 
  }
  // falling
  else if (z_trend == 2) 
  {
    strcpy_P(message, (char*)pgm_read_word(&(string_table[pgm_read_byte(&fall_options[z_option])]))); 
  }
  else
  {
    strcpy_P(message, (char*)pgm_read_word(&(string_table[pgm_read_byte(&steady_options[z_option])]))); 
  }

}




void setup() {

  OzOled.init();
  OzOled.setDeactivateScroll();

  climateSensor.begin();

  pinMode(SUMMER_SWITCH, INPUT_PULLUP);
}


void loop() 
{
  // Setup scroll...
  OzOled.setDeactivateScroll(); 
  OzOled.setCursorXYPixel(0,0);
  OzOled.sendData(0); // blank first column for wrap
  OzOled.setCursorXYPixel(0,1);
  OzOled.sendData(0); // blank first column for wrap
  OzOled.setCursorXYPixel(126,0);
  OzOled.sendBiggerCharData(message[messagei],fontcolumn); // top half
  OzOled.setCursorXYPixel(126,1);
  OzOled.sendBiggerCharData(message[messagei],fontcolumn+12); // lower half
  fontcolumn++; fontcolumn%=12; // column

  if (fontcolumn==0)   // increment character index?
  {
    messagei++;
    messagei%=strlen(message);//20;//message.length(); // roll over
  }

  OzOled.scrollLeftPixel(0,1,8); // Perform hardware scroll
  OzOled.setDeactivateScroll();  // End... moved 1 pixel?

  delay(10); 

  currentMillis = millis();

  // time for reading?
  if ( (currentMillis - previousMillis) >= interval) 
  {
    previousMillis = currentMillis;
    
    // pressure at sea level conversion... optimise?
    pressure = (((climateSensor.getPressure(true) * 100.0)/pow((1-((float)(height_above_sealevel))/44330), 5.255))/100.0);
    
    pressure_index_head++; pressure_index_head%=total_logs; 
    pressure_index_tail++; pressure_index_tail%=total_logs;
    pressure_values[ pressure_index_head ] = pressure;
    old_pressure = pressure_values[pressure_index_tail];
    byte zambretti_trend = 0;
    
    // significat difference between readings?
    if (  abs( float(old_pressure/100) - float(pressure/100)) > 1.6  ) 
    {
      if ( pressure >  old_pressure ) zambretti_trend = 1; // rise
      if ( pressure <  old_pressure ) zambretti_trend = 2; // fall
    }

    zambretti_forecast(pressure/100,digitalRead(SUMMER_SWITCH),zambretti_trend);

    // reset scroll indexes
    messagei=0;
    fontcolumn=0;

    //pad with forecast with space
    while ( strlen(message)< 29 ) strcat(message, "@");

  }


}