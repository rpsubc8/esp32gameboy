# ESP32 Gameboy

<center><img src="https://github.com/rpsubc8/esp32gameboy/blob/master/preview/preview.jpg"></center>
Emulador de Gameboy
<ul>
 <li><b>fabgl:</b> sonido, vga, teclado y 4 bancos de cache</li>
 <li><b>vga32:</b> vga, teclado y 8 bancos de cache</li>
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

Hay que comentar y descomentar las opciones de librerias que queremos usar, así como el consumo de RAM.

Se encuentra disponible una versión ya compilada, que puede ser grabada con las esptools de Arduino:

https://github.com/rpsubc8/esp32gameboy/blob/master/arduino/hex/hex.zip

Ejemplo:

<pre>
f:\Users\AppData\Local\Arduino15\packages\esp32\tools\esptool_py\2.6.1/esptool.exe --chip esp32 --port COM44 --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size detect 0xe000 f:\Users\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.4/tools/partitions/boot_app0.bin 0x1000 f:\Users\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.4/tools/sdk/bin/bootloader_qio_80m.bin 0x10000 v:\Temp\arduino_build_211050/gameboy.ino.bin 0x8000 v:\Temp\arduino_build_211050/gameboy.ino.partitions.bin 
</pre>

Los directorios deben ser reemplazadados por los nuestros, así como el puerto COM y la velocidad del mismo.

<br>
<h1>Arduino IDE</h1>
Todo el proyecto es compatible con la estructura de Arduino 1.8.11.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyZXSpectrum/main/preview/previewArduinoIDEpreferences.gif'></center>
Debemos instalar las extensiones de spressif en el gestor de urls adicionales de tarjetas <b>https://dl.espressif.com/dl/package_esp32_index.json</b>
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyZXSpectrum/main/preview/previewArduinoIDElibrary.gif'></center>
Necesitamos las librerias de <b>bitluni 0.3.3</b> y <b>fabgl 0.9.0</b>.
Lo podemos hacer desde el administrador de bibliotecas.
Debemos desactivar la opción de PSRAM, y en caso de superar 1 MB de binario, seleccionar 4 MB de partición a la hora de subir. Aunque el código no use PSRAM, si la opción está activa y nuestro ESP32 no dispone de ella, se generará una excepción y reinicio del mismo en modo bucle.



<br>
<h1>Generar .h</h1>
He creado una tool para generar la estructura de las 10 roms .h:

https://github.com/rpsubc8/esp32gameboy/tree/master/arduino/tools

Los archivos roms .gb deben ser introducidos en el directorio rom, and se debe lanzar:
<pre>
 rom2h roms
</pre>
Todos los .h que se generan, deben ser soltados al proyecto, sobreescribiendo los actuales.
