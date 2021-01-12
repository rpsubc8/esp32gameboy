# ESP32 Gameboy

<center><img src="https://github.com/rpsubc8/esp32gameboy/blob/master/preview/preview.gif"></center>
Gameboy emulator (fork lualiliu) for the TTGO VGA32 board with ESP32 chip.
<ul> 
 <li>Use of vga and keyboard</li>
 <li>Up to 8 cache banks</li>
 <li>PSRAM is not used, it works in ESP32 with 520 KB of RAM (TTGO VGA32)</li></li>
 <li>Use of a single core</li>
 <li>Low-income OSD</li>
 <li>Created project compatible with Arduino IDE and Platform IO</li>
 <li>Sound with the fabgl library</li>
 <li>Support for 8 and 64 color mode</li>
 <li>VGA 200x150 and 320x175</li>
 <li>Emulation of the classic Game Boy</li>
</ul>


<br>
<h1>Requirements</h1>
Required:
 <ul>
  <li>Visual Studio 1.48.1 PLATFORM 2.2.0</li>
  <li>Arduino IDE 1.8.11</li>
  <li>Arduino fabgl 0.9.0 library (included in PLATFORM project)</li>
  <li>Arduino bitluni library 0.3.3 (included in the project)</li>
 </ul>
<center><img src='https://raw.githubusercontent.com/rpsubc8/esp32gameboy/master/preview/ttgovga32v12.jpg'></center> 
<br>


<h1>PlatformIO</h1>
PLATFORM 2.2.0 must be installed from the Visual Studio extensions.
<center><img src='https://raw.githubusercontent.com/rpsubc8/esp32gameboy/master/preview/previewPlatformIOinstall.gif'></center>
Then the working directory <b>gameboy</b> will be selected.
We must modify the file <b>platformio.ini</b> the option <b>upload_port</b> to select the COM port where we have our TTGO VGA32 board.
<center><img src='https://raw.githubusercontent.com/rpsubc8/esp32gameboy/master/preview/previewPlatformIO.gif'></center>
Then we will proceed to compile and upload to the board. No partitions are used, so we must upload all the compiled binary.
Everything is prepared so we don't have to install the bitluni libraries. However, if we need the sound options, we will need to install the libraries of <b>fabgl</b>, only for the Arduino IDE, since for PLATFORM I have left the libraries in the project.


<br>
<h1>Arduino IDE</h1>
The whole project is compatible with the structure of Arduino 1.8.11.
We only have to open the <b>gameboy.ino</b> in the <b>gameboy</b> directory.
<center><img src='https://raw.githubusercontent.com/rpsubc8/esp32gameboy/master/preview/previewArduinoIDEpreferences.gif'></center>
We must install the spressif extensions in the additional card url manager <b>https://dl.espressif.com/dl/package_esp32_index.json</b>
<center><img src='https://raw.githubusercontent.com/rpsubc8/esp32gameboy/master/preview/previewArduinoIDElibrary.gif'></center>
For the normal mode, the project is already prepared, so that no bitluni library is needed. However, if we need the sound support, it is required to install the libraries <b>fabgl 0.9.0</b>.
We can do it from the library manager.
We must deactivate the PSRAM option, and in case of exceeding 1 MB of binary, select 4 MB of partition at the time of uploading. Although the code does not use PSRAM, if the option is active and our ESP32 does not have it, an exception will be generated and restart it in loop mode.


<br>
<h1>Usability</h1>
The following actions are allowed from the menu (F1 key):
 <ul>  
  <li>Select ROM allows to choose the disk.</li>
  <li>Change the milliseconds of keyboard polling</li>
  <li>CPU speed</li>
  <li>Turn the sound on and off</li>
  <li>Emulator and chip restart</li>
 </ul>
 The keys to play are:
 <pre>
  ESP32 Joystick GB Keyboard
  -----------------------------------
   Cursor up - up
   Cursor down - down
   Left - left slider
   Right-handed cursor - right
   a - A
   z - B
   Space bar - Start
   ENTER - SELECT              
 </pre><br>
 There is a basic OSD of low resources, that is to say, very simple, that is visualized by pressing the key <b>F1</b>.
 To move through the OSD, we use the arrow keys, the <b>ENTER</b> to accept and the <b>ESC</b> to cancel.
 <center><img src='https://raw.githubusercontent.com/rpsubc8/esp32gameboy/master/preview/previewOSD.gif'></center>
 Files must be converted to .h in hexadecimal. You can use the online tool:<br>
 <a href='http://tomeko.net/online_tools/file_to_hex.php?lang=en'>http://tomeko.net/online_tools/file_to_hex.php?lang=en</a>
 For the <b>roms</b> you must use the tool <b>rom2h</b> to generate the necessary .h.
 
 
<br>
<h1>Options</h1>
The file <b>gbConfig.h</b> options are selected:
<ul>
 <li><b>use_lib_200x150:</b> 200x150 video mode is used.</li> 
 <li><b>use_lib_320x175:</b> 320x175 video mode is used.</li>
 <li><b>use_lib_vga8colors:</b> Force to use RGB 8 color mode (3 pins). It consumes little ram and is very fast, but only takes 8 colors, compared to 64 in normal mode (6 pins RRGGBB).</li>
 <li><b>use_lib_vga64colors:</b> Force to use RRGGBB 64 colors mode (6 pins). It consumes more ram and is slower, but gets 64 colors (grayscale).</li>
 <li><b>use_lib_vga_low_memory:</b> Experimental mode with low video RAM consumption, but slower.</li>
 <li><b>use_lib_fast_vga:</b> Ultra fast experimental mode, which achieves almost double the video access speed. In 8-color mode, it brings out a classic CGA style palette, and in 64-color mode, grayscale.</li>
 <li><b>use_lib_fast_vgaBLUE8colors:</b> Ultra fast experimental mode for 8-color mode, using blue palette.</li>
 <li><b>use_lib_fast_vgaRED8colors:</b> Ultra fast experimental mode for 8-color mode, using red palette.</li>
 <li><b>use_lib_fabgl_sound:</b> A 3 channel mixer is used in dirty mode. It consumes quite a lot of ram. The fabgl library 0.9.0 is required</li>
 <li><b>use_lib_log_serial:</b> Logs are sent by serial port usb</li>
 <li><b>usb_lib_optimice_checkchange_bankswitch:</b> Only switches banks when they are different, gaining speed.</li>
 <li><b>gb_ms_joy:</b> You must specify the number of milliseconds of polling for the keyboard.</li>
 <li><b>gb_frame_crt_skip:</b> If it is 1 it skips a frame.</li>
 <li><b>use_lib_vga_thread:</b> Experimental multithreading mode, for now it is very slow.</li>
 <li><b>use_min_ram:</b> Does not use any cache bank, saving memory, but being slower.</li>
 <li><b>use_half_ram:</b>Use 4 cache banks, gaining speed, sacrificing memory.</li>
 <li><b>use_max_ram:</b>Use 8 cache banks, gaining speed, sacrificing memory.</li>
 <li><b>use_lib_tinyFont:</b> Uses own routines to draw 6x8 font without using bitluni ones, saving RAM and Flash memory</li>
</ul>


<br>
<h1>Test applications</h1>
We can choose the following games:
<ul>
 <li><b>Long Crownr</b></li>
 <li><b>Retroid</b></li>
</ul>


<br>
<h1>Generate .h</h1>
I have created a tool to generate the structure of the roms in .h:

https://github.com/rpsubc8/esp32gameboy/tree/master/arduino/tools

The roms .gb files must be entered in the directory:
<pre>
 input
  roms  
</pre>
And you must launch the executable <b>rom2h</b>
</pre><br>
It will be generated in the directory:
<pre>
 output
  dataFlash
   gbrom.h
   roms
</pre>
All the .h that are generated, must be released to the project, overwriting the current ones from the <b>dataFlash</b> directory.
