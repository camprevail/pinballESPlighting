# pinballESPlighting
Controlling open API lighting with an ESP8266 NodeMCU and a Pinball Machine

## Parts List
- One ESP8266 NodeMCU, this one will work. [Amazon Prime Link $8.39](https://www.amazon.com/HiLetgo-Internet-Development-Wireless-Micropython/dp/B010O1G1ES/ref=sr_1_6?keywords=esp-12e&qid=1569633647&s=gateway&sr=8-6)
- 12v Relays - So we don't fry the 3.3v gpio pins on the ESP. [Amazon Prime Link $10 for 10](https://www.amazon.com/gp/product/B07MMNGYDP/ref=ppx_yo_dt_b_asin_title_o02_s00?ie=UTF8&psc=1)
- Micro USB cable - for programming the ESP and optionally powering it in your final setup. 

## Getting Set Up
First off, figure out how you want to control your room lighting. Do you want a certain light on your pinball machine to trigger it? Most of the lights on a pinball machine are constantly flashing, so choose this wisely. If you can access the source code and find a spare output on the io boards, you could program it to trigger on whatever event you want. For TNA, I only used one input on the ESP and tapped the wires on the Beacon, which only lights when the reactor is critical, so this worked out well.
You can find these wire splicers at any hardware store if you want to tap into existing wiring, or you can solder onto terminals. Either way, be prepared to do a little bit of soldering.

<img src="https://mobileimages.lowes.com/product/converted/032076/032076929442.jpg?size=xl" width="200" height="200">

## Wiring Everything Up
Here is the pinout for the 12v relays and the ESP.

- Common pin goes to an ESP ground pin.
- NO goes to a gpio pin on the ESP. I recommend gpio 5 or 4, but don't use 16 since it has a built-in LED attached to it. We will use this later in the code as a status LED.
- Coil 1 and 2 connect to the lamp you are attaching to. Pos and Neg don't matter here.

<img src="https://components101.com/sites/default/files/component_pin/12V-Relay-Pinout.png" width="400" height="211"> <img src="https://pradeepsinghblog.files.wordpress.com/2016/04/nodemcu_pins.png?w=616" width="400" height="337">

This ESP NodeMCU model has a voltage regulator on the Vin pin, so you *could* power it with 12v, but I've seen reports of the regulator getting a little hot with 12v. I power mine with a usb cable from the pinball computer. You could also get a 12v buck converter if you wanted to use the Vin pin and adjust it down to about 6v. 

## The APIs
This is the fun stuff. Did you know the Hue Bridge has a built-in API debug webpage?
Find out the local IP address of your bridge, then type it into a web browser, with /debug/clip.html at the end. You should see a simple GUI. 
Next, create a free [Hue Developers account](https://developers.meethue.com/login/?redirect_to=https%3A%2F%2Fdevelopers.meethue.com%2Fdevelop%2Fhue-api%2F) to access the Hue API documentation and get yourself started. Follow the steps to create an API key, and try some GET and PUT requests in the debug client to control your lights. 

Nanoleaf doesn't have a fancy GUI like the hue bridge, but if you want to control nanoleaf panels, the process is pretty much the same as Hue. Create a dev account [here](https://forum.nanoleaf.me/users/sign_in) and learn how to get an api key. 
You could do this in Python with a few simple lines of code (install the requests lib first)
```
# Hold the power button for 5-7s until the light flashes rapidly, then run this script to get an api key.
# Change the IP to the IP of your nanoleaf panels, but leave the port as 16021. You may find them in your network
# with the alias "IEEE REGISTRATION AUTHORITY"

import requests, json
nanoleaf="http://192.168.1.94:16021/api/v1/new"

r = requests.post(nanoleaf, data=None)
print(r.text)
```
