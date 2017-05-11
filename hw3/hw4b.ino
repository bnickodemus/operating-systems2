#include <SPI.h>
#include "Microchip_MCP3002.h"

Microchip_MCP3002 adc(4); // use pin 4

void setup()
{
  Serial.begin(115200); // set baud to 115200
  
  // channel 0 on "bottom" (GND)
  // channel 1 on "top" input signal
  adc.setChannel(CHANNEL_0_1);  
}

void loop() 
{
  // get a voltage and print.
  float f = adc.getSample();
  Serial.println(f);

  delay(1); // wait 1 ms (1000 Hz)
  //delay(10); // wait 10 ms (100 Hz)
}
