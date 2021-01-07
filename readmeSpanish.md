# ESP32 Gameboy

<center><img src="https://github.com/rpsubc8/esp32gameboy/blob/master/preview/preview.jpg"></center>
Emulador de Gameboy (fork lualiliu) para la placa TTGO VGA32 con chip ESP32.
<ul> 
 <li>Uso de vga y teclado</li>
 <li>Hasta 8 bancos de cache</li>
 <li>No se usa PSRAM, funcionando en ESP32 de 520 KB de RAM (TTGO VGA32)</li>
 <li>Uso de un sólo core</li>
 <li>OSD de bajos recursos</li>
 <li>Creado proyecto compatible con Arduino IDE y Platform IO</li>
 <li>Sonido con la libreria fabgl</li>
 <li>Soporte para modo 8 y 64 colores.</li>
 <li>VGA 200x150 y 320x175</li>
 <li>Emulación de la Game Boy clásica</li>
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
 Las teclas para jugar son:
 <pre>
  Teclado ESP32        Joystick GB
  -----------------------------------
   Cursor arriba     - arriba
   Cursor abajo      - abajo
   Cursor izquierda  - izquierda
   Cursor derecha    - derecha
   a                 - A
   z                 - B
   Barra espaciadora - Start
   ENTER             - SELECT              
 </pre><br>
 Se dispone de un OSD básico de bajos recursos, es decir, muy simple, que se visualiza pulsando la tecla <b>F1</b>.
 <center><img src='https://raw.githubusercontent.com/rpsubc8/esp32gameboy/main/preview/previewOSD.gif'></center>
 Los ficheros deben ser convertidos a .h en hexadecimal. Puede usarse la herramienta online:<br>
 <a href='http://tomeko.net/online_tools/file_to_hex.php?lang=en'>http://tomeko.net/online_tools/file_to_hex.php?lang=en</a>
 Para las <b>roms</b> se debe utilizar la herramienta <b>rom2h</b> para generar los .h necesarios.


<br>
<h1>Opciones</h1>
El archivo <b>gbConfig.h</b> se seleccionan las opciones:
<ul>
 <li><b>use_lib_200x150:</b> Se usa modo de vídeo 200x150.</li> 
 <li><b>use_lib_320x175:</b> Se usa modo de vídeo 320x175.</li>  
 <li><b>use_lib_vga8colors:</b> Obliga a usar RGB modo de 8 colores (3 pines). Consume poca ram y es muy rápido, pero sólo saca 8 colores, frente a los 64 del modo normal (6 pines RRGGBB).</li>
 <li><b>use_lib_vga64colors:</b> Obliga a usar RRGGBB modo de 64 colores (6 pines). Consume más ram y es más lento, pero saca 64 colores (escala de grises).</li>
 <li><b>use_lib_vga_low_memory:</b> Modo experimental de bajo consumo de RAM de video, pero más lento.</li> 
 <li><b>use_lib_fast_vga:</b> Modo experimental ultra rápido, que se consigue casi el doble de velocidad de acceso a video. En el modo de 8 colores, saca una paleta estilo CGA clásica, y en el modo 64 colores, escala de grises.</li>
 <li><b>use_lib_fast_vgaBLUE8colors:</b> Modo experimental ultra rápido para el modo de 8 colores, usando paleta azul.</li>
 <li><b>use_lib_fast_vgaRED8colors:</b> Modo experimental ultra rápido para el modo de 8 colores, usando paleta roja.</li>
 <li><b>use_lib_fabgl_sound:</b> Se utiliza un mezclador de 3 canales en modo dirty. Consume bastante ram. Se requiere la librería fabgl 0.9.0</li>
 <li><b>use_lib_log_serial:</b> Se envian logs por puerto serie usb</li>
 <li><b>usb_lib_optimice_checkchange_bankswitch:</b> Sólo conmuta bancos cuando son distintos, ganando velocidad.</li> 
 <li><b>gb_ms_joy:</b> Se debe especificar el número de milisegundos de polling para el teclado.</li>
 <li><b>gb_frame_crt_skip:</b> Si es 1 se salta un frame.</li>
 <li><b>use_lib_vga_thread:</b> Modo multihilo experimental, por ahora es muy lento.</li>
 <li><b>use_min_ram:</b> No usa ningún banco de cache, ahorrando memoria, pero siendo más lento.</li>
 <li><b>use_half_ram:</b> Usa 4 bancos de cache, ganando velocidad, sacrificando memoria.</li>
 <li><b>use_max_ram:</b> Usa 8 bancos de cache, ganando velocidad, sacrificando memoria.</li>
</ul>


<br>
<h1>Aplicaciones Test</h1>
Podemos elegir los siguientes juegos:
<ul>
 <li><b>Last Crownr</b></li>
 <li><b>Retroid</b></li> 
</ul>

<br>
<h1>Generar .h</h1>
He creado una tool para generar la estructura de las roms en .h:

https://github.com/rpsubc8/esp32gameboy/tree/master/arduino/tools

Los archivos roms .gb deben ser introducidos en el directorio:
<pre>
 input
  roms  
</pre>
Y se debe lanzar el ejecutable <b>rom2h</b>
</pre><br>
Se generará en el directorio:
<pre>
 output
  dataFlash
   gbrom.h
   roms
</pre>
Todos los .h que se generan, deben ser soltados al proyecto, sobreescribiendo los actuales del directorio <b>dataFlash</b>.
