# Ultimate Voice Changer Arduino Library

## What is this?

This is the official Arduino library for the Ultimate Voice Changer Arduino Shield, which is an open source hardware project that uses an MCP3208 ADC and an MCP4921 DAC with SPI communications to provide high quality voice sampling and real-time audio manipulation.

## Shield PCB

Eagle schematics and board layout for an Arduino Uno shield are included in this repository. Boards can also be purchased directly from OSH Park:

* [Ultimate Voice Changer PCB @ OSH Park](https://oshpark.com/shared_projects/ojPzRnSB)

## Component List

* [MCP3204](https://www.mouser.com/ProductDetail/579-MCP3204-CI-P)
* [MCP4921](https://www.mouser.com/ProductDetail/579-MCP4921-E-P)
* [Audio Jack](https://www.mouser.com/ProductDetail/502-35RAPC4BHN2)
* 1K resistor
* 1.5K resistor
* 330 ohm resistor
* 0.1 uF capacitor
* [10 uF electrolytic capacitor (2mm spacing)](https://www.mouser.com/ProductDetail/667-ECA-1HM100)
* [LED T1-3/4 (5mm) RED](https://www.mouser.com/ProductDetail/696-SLX-LX5093ID)
* [36-pin header](https://www.mouser.com/ProductDetail/538-22-28-4363)
* [10K Trimpot](https://www.sparkfun.com/products/9806)

## Example sketches

See the [examples](examples) directory for Dalek / ring modulator examples that mix voice input with a sine wave for robotic voice effects.

## Alternatives

I am no longer actively maintaining this project or providing support and I would suggest looking at the [Adafruit Wave Shield](https://learn.adafruit.com/wave-shield-voice-changer/overview) instead. They even have an example Dalek sketch.

