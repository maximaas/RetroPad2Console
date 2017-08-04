# RetroPad2Console

for SNES & Saturn pads using on original xbox and gamecube.

Copyright (C) 2015,2017 Maximaas<br/>
Version 1.0 - completed on 2015.12.09

<H2>About</H2>
Original Xbox and Nintendo Gamecube(Europe version) are good emulator platforms with RGB video support, which makes it possible to play retro games on a RGB capable CRT display. But with weird controller scheme, the default Xbox and GC controller are disasters for most retro games. So I came up with the idea of making an adapter to let me use SNES and Saturn pads on these consoles. Saturn pad have six buttons on the right, it's perfect for MegaDrive and 2D-Fighting games while SNES pad is for NES/SNES/GBP and other games.

<H2>References</H2>
Before I start, I googled a lot of resources to help me accomplish this project (see blow). Greate thanks to all these authors.<br/><br/>

http://www.int03.co.uk/crema/hardware/gamecube/gc-control.htm<br/>
Gamecube controlle protocol explanation.

http://www.raphnet.net/electronique/x2wii/index_en.php<br/>
N64/Snes/Nes controller to gamecube/Wii conversion project

https://github.com/brownan/Gamecube-N64-Controller<br/>
I directly used the gamecube communication code from this project with just a little modification. It helps me greatly, without this I will never done my work.

http://www.brunofreitas.com/node/41<br/>
A very powerful adapter, I directly used the Xbox pad communication code from this project or I will never got my project work on original xbox.

<H2>Pictures</H2>

![image](https://github.com/maximaas/RetroPad2Console/blob/master/pics/photo/IMG_8322.JPG)
![image](https://github.com/maximaas/RetroPad2Console/blob/master/pics/photo/IMG_8313.JPG)<br/>
I bought several SNES to PC adapters to make use of its case and snes female connector.

![image](https://github.com/maximaas/RetroPad2Console/blob/master/pics/photo/IMG_8314.JPG)<br/>
My work uses USB connector as the output in order to adapt both Xbox and GC.

![image](https://github.com/maximaas/RetroPad2Console/blob/master/pics/photo/IMG_8318.JPG)<br/>
Interface cable. Female USB connector to Xbox and GC controller interface.

![image](https://github.com/maximaas/RetroPad2Console/blob/master/pics/photo/IMG_8319.JPG)
![image](https://github.com/maximaas/RetroPad2Console/blob/master/pics/photo/IMG_8320.JPG)<br/>
The PCB I designed with the size fits with the SNES to PC adapter case. And a bit of hand craft work should be done to accommodate the Saturn and USB connectors.

<H2>Build the Adapter</H2>
This adapter is actually a minimum Arduino based on ATMEGA8-16PU. It receives the SNES/Saturn pad signals then interprete and send them to the target according to Xbox or GC protocol.

![image](https://github.com/maximaas/RetroPad2Console/blob/master/pics/photo/IMG_8323.JPG)
The eagle files of the hardware part are included. The PCB have both ICSP and Serial interfaces, bootloader and firmware can be programed via them.

The software part is standard Arduino sketch codes, can be compiled and uploaded with Arduino IDE.

BTW, I'm using Sloeber IDE(http://eclipse.baeyens.it/) to help keeping my Arduino projects well organized.

<H2>Interface cable wiring</H2>

<H2>Known Issues</H2>
<ul>
<li>The turning on of the LED on the adapter means the handshaking process is over and the adapter is funtional. Unlike the Gamecube, the Xbox will not do the handshaking process with the controller until the power on video clip is finished. When cold-plugged to the Xbox the adapter will be funtional after the console started.</li>
<br/>
<li>The adapter can not work with Game Boy Interface (a replacement to the official Gameboy Player application) because of a different handshaking protocol. The ATMEGA8-16PU only have 8KB memory, adding any fix codes will cause it reachs it's physical limit.</li>
</ul>
