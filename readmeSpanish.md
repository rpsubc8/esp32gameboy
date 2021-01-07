# ESP32 Gameboy

<center><img src="https://github.com/rpsubc8/esp32gameboy/blob/master/preview/preview.jpg"></center>
Emulador de Gameboy (fork lualiliu) para la placa TTGO VGA32 con chip ESP32.
<ul>
 <li><b>fabgl:</b> sonido, vga, teclado y 4 bancos de cache</li>
 <li><b>vga32:</b> vga, teclado y 8 bancos de cache</li>
</ul> 


<br>
<h1>Requerimientos</h1>
Se requiere:
 <ul>
  <li>Visual Studio 1.48.1 PLATFORMIO 2.2.0</li>
  <li>Arduino IDE 1.8.11</li>
  <li>Librería Arduino fabgl 0.9.0 (incluida en proyecto PLATFORMIO)</li>
  <li>Librería Arduino bitluni 0.3.3 (incluida en proyecto)</li>
 </ul>
<center><img src='https://raw.githubusercontent.com/rpsubc8/esp32gameboy/main/preview/ttgovga32v12.jpg'></center> 
<br>


<h1>PlatformIO</h1>
Se debe instalar el PLATFORMIO 2.2.0 desde las extensiones del Visual Studio.
<center><img src='https://raw.githubusercontent.com/rpsubc8/esp32gameboy/main/preview/previewPlatformIOinstall.gif'></center>
Luego se seleccionará el directorio de trabajo <b>TinyCPCEMttgovga32</b>.
Debemos modificar el fichero <b>platformio.ini</b> la opción <b>upload_port</b> para seleccionar el puerto COM donde tenemos nuestra placa TTGO VGA32.
<center><img src='https://raw.githubusercontent.com/rpsubc8/esp32gameboy/main/preview/previewPlatformIO.gif'></center>
Luego procederemos a compilar y subir a la placa. No se usa particiones, así que debemos subir todo el binario compilado.
Está todo preparado para no tener que instalar las librerias de bitluni. Sin embargo, si necesitamos las opciones de sonido, necesitaremos instalar las librerias de <b>fabgl</b>, sólo para el IDE de Arduino, dado que para PLATFORMIO he dejado las librerías en el proyecto.


<br>
<h1>Arduino IDE</h1>
Todo el proyecto es compatible con la estructura de Arduino 1.8.11.
Tan sólo tenemos que abrir el <b>CPCem.ino</b> del directorio <b>CPCem</b>.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyCPC/main/preview/previewArduinoIDEpreferences.gif'></center>
Debemos instalar las extensiones de spressif en el gestor de urls adicionales de tarjetas <b>https://dl.espressif.com/dl/package_esp32_index.json</b>
<center><img src='https://raw.githubusercontent.com/rpsubc8/esp32gameboy/main/preview/previewArduinoIDElibrary.gif'></center>
Para el modo normal, ya está preparado el proyecto, de forma que no se necesita ninguna librería de bitluni. Sin embargo, si necesitamos el soporte de sonido, se requiere instalar las librerias <b>fabgl 0.9.0</b>.
Lo podemos hacer desde el administrador de bibliotecas.
Debemos desactivar la opción de PSRAM, y en caso de superar 1 MB de binario, seleccionar 4 MB de partición a la hora de subir. Aunque el código no use PSRAM, si la opción está activa y nuestro ESP32 no dispone de ella, se generará una excepción y reinicio del mismo en modo bucle.



<br>
<h1>Usabilidad</h1>
Se permiten las siguientes acciones desde el menú (tecla F1):
 <ul>  
  <li>Seleccionar ROM permite elegir el disco.</li>
  <li>Cambiar los milisegundos de polling para teclado</li>
  <li>Velocidad de CPU</li>
  <li>Activar y desactivar el sonido</li>  
  <li>Reinicio del emulador y del chip</li>  
 </ul>
 Se dispone de un OSD básico de bajos recursos, es decir, muy simple, que se visualiza pulsando la tecla <b>F1</b>.
 <center><img src='https://raw.githubusercontent.com/rpsubc8/esp32gameboy/main/preview/previewOSD.gif'></center>
 Los ficheros deben ser convertidos a .h en hexadecimal. Puede usarse la herramienta online:<br>
 <a href='http://tomeko.net/online_tools/file_to_hex.php?lang=en'>http://tomeko.net/online_tools/file_to_hex.php?lang=en</a>
 Para las <b>roms</b> se debe utilizar la herramienta <b>rom2h</b> para generar los .h necesarios.


<br>
<h1>Generar .h</h1>
He creado una tool para generar la estructura de las 10 roms .h:

https://github.com/rpsubc8/esp32gameboy/tree/master/arduino/tools

Los archivos roms .gb deben ser introducidos en el directorio:
<pre>
 input
  roms  
</pre>
Y se debe lanzar el ejecutable <b><rom2h</b>
</pre>
Se generará en el directorio:
<pre>
 output
  dataFlash
   gbrom.h
   roms
</pre>
Todos los .h que se generan, deben ser soltados al proyecto, sobreescribiendo los actuales del directorio <b>dataFlash</b>.
