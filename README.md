# Ultimate Voice Changer Arduino Library

## What is this?

This is the official Arduino library for the Ultimate Voice Changer Arduino Shield, which is an open source hardware project that uses an MCP3208 ADC and an MCP4921 DAC with SPI communications to provide high quality voice sampling and real-time audio manipulation.

## Hardware

Eagle schematics and board layout for an Arduino Uno shield are included in this repository. Boards can be purchased directly from OSH Park:

<a href="https://oshpark.com/shared_projects/ojPzRnSB"><img src="https://oshpark.com/assets/badge-5b7ec47045b78aef6eb9d83b3bac6b1920de805e9a0c227658eac6e19a045b9c.png" alt="Order from OSH Park"></img></a>

## Component List

* [MCP3204](https://www.mouser.com/ProductDetail/579-MCP3204-CI-P)
* [MCP4921](https://www.mouser.com/ProductDetail/579-MCP4921-E-P)
* [Audio Jack](https://www.mouser.com/ProductDetail/502-35RAPC4BHN2)
* 1K resistor
* 1.5K resistor
* 330 resistor
* 0.1 uF capacitor
* [10 uF electrolytic capacitor (2mm spacing)](https://www.mouser.com/ProductDetail/667-ECA-1HM100)
* [LED T1-3/4 (5mm) RED](https://www.mouser.com/ProductDetail/696-SLX-LX5093ID)
* [36-pin header](https://www.mouser.com/ProductDetail/538-22-28-4363)
* [10K Trimpot](https://www.sparkfun.com/products/9806)

## Example sketches

Currently, there is a single sketch available for a ring modulator effect.

## Alternatives

I am no longer actively maintaining this project or providing support and I would suggest looking at the [Adafruit Wave Shield](https://learn.adafruit.com/wave-shield-voice-changer/overview) instead. They even have an example Dalek sketch.

