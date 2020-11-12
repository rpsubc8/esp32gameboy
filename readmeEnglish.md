# ESP32 Gameboy
<center><img src="https://github.com/rpsubc8/esp32gameboy/blob/master/preview/preview.jpg"></center>
Gameboy emulate ESP32 TTGO VGA32 board.
<ul>
 <li><b>fabgl:</b> sound, vga, keyboard and 4 bank cache</li>
 <li><b>vga32:</b> vga, keyboard and 8 bank cache</li>
</ul> 

<ul>
 <li>Arduino IDE 1.8.11</li>
 <li>TTGO VGA32 ESP32</li>
 <li>https://dl.espressif.com/dl/package_esp32_index.json</li>
 <li>ESP32 espressif Systems 1.0.4</li>
 <li>Libreria fabgl 0.9.0</li>
 <li>Libreria Bitluini ESP32lib 0.3.3</li>
</ul>

<pre>
 #define lib_compile_fabgl
 //#define lib_compile_vga32
 
 //user_min_ram - min ram consume, not bank cache
 #define use_max_ram
</pre>

We have to comment and decomment the library options we want to use, as well as the RAM consumption.

A version is available and it can be climbed with the esptools of the Arduino:

https://github.com/rpsubc8/esp32gameboy/blob/master/arduino/hex/hex.zip

Example:

<pre>
f:\Users\AppData\Local\Arduino15\packages\esp32\tools\esptool_py\2.6.1/esptool.exe --chip esp32 --port COM44 --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size detect 0xe000 f:\Users\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.4/tools/partitions/boot_app0.bin 0x1000 f:\Users\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.4/tools/sdk/bin/bootloader_qio_80m.bin 0x10000 v:\Temp\arduino_build_211050/gameboy.ino.bin 0x8000 v:\Temp\arduino_build_211050/gameboy.ino.partitions.bin 
</pre>

The directories should be replaced by ours, as well as the COM port and speed.



<br>
<h1>Arduino IDE</h1>
The whole project is compatible with the structure of Arduino 1.8.11.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyZXSpectrum/main/preview/previewArduinoIDEpreferences.gif'></center>
We must install the spressif extensions in the additional card url manager <b>https://dl.espressif.com/dl/package_esp32_index.json</b>
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyZXSpectrum/main/preview/previewArduinoIDElibrary.gif'></center>
We need the libraries of <b>bitluni 0.3.3</b> and <b>fabgl 0.9.0</b>.
We can do it from the library manager.
We must deactivate the PSRAM option, and in case of exceeding 1 MB of binary, select 4 MB of partition when uploading. Although the code does not use PSRAM, if the option is active and our ESP32 does not have it, an exception will be generated and restart it in loop mode.



<br>
<h1>Generate .h</h1>
I have created a tool to generate the structure of the 10 roms .h:

https://github.com/rpsubc8/esp32gameboy/tree/master/arduino/tools

The roms .gb files must be entered in the rom directory, and must be launched:
<pre>
 rom2h roms
</pre>
All the .h that are generated, must be released to the project, overwriting the current ones.
