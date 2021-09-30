# PiDGC
Raspberry Pi based Digital Gauge Cluster by Josh Ellis / josh@ellis.sh

*** Please Note: This library is designed for, and without heavy modification, will not work on anything other than 
the Raspberry Pi 4B running a 2021 version of RaspiOS (in my case `2021-05-07-raspios-buster-armhf-lite.img`). There are
files compiled specifically for this architecture. Additionally, this is hardcoded to support a 1280x480 display. It 
will look weird on other displays. ***

Setup steps on Buster Lite:
- raspi-config
  - System Options
    - Boot / Auto Login
      - Select "Console Autologin"
    - Network at Boot
      - Select "No," do not wait for network at boot
  - Interface Options
    - I2C
      - Enable I2C
  - Advanced Options
    - GL Driver
      - Select "GL (Fake KMS)"
- Follow ALL STEPS in `files/PICAN3_UGA_10.pdf` to setup/enable the PiCAN3. You must enable the CANBUS and RTC.
- Follow instructions on https://www.raspberrypi.org/forums/viewtopic.php?t=24679#p227301 to set the proper resolution. This should be 1280x480.
- Export environment variables needed for Qt:
  - `export QT_QPA_PLATFORM=eglfs`
  - `export QT_QPA_PLATFORM_PLUGIN_PATH=/usr/local/qt5.15/plugins/platforms`
  - `export LD_LIBRARY_PATH=/usr/local/qt5.15/lib`
- Install dependencies needed for PiDGC:
  - `sudo apt-get install libgles2 libpulse-dev libdouble-conversion-dev libegl1-mesa libmtdev-dev libinput-tools default-jdk`
  - `sudo apt-get install libts-dev libxkbcommon-x11-0 libwebpdemux2 libgstreamer-plugins-base1.0-0 gstreamer1.0-gl gstreamer1.0-plugins-bad`
- Add `pi` user to render group. This is required for good performance.
  - `sudo gpasswd -a pi render`
- Copy the `files/qt5.15.tar.gz` archive to your Raspberry Pi 4, and then decompress it into `/usr/local/qt5.15`
- Make PiDGC folder and copy over all files:
  - `mkdir ~/pidgc`
  - Copy `files/display` into ~/pidgc folder
  - Copy `files/config.ini` into ~/pidgc folder
  - Copy `files/images` folder and all files it contains into ~/pidgc folder
  - Copy `files/hw-interface.jar` into ~/pidgc folder
- Edit `/boot/config.txt`. Append:
~~~
initial_turbo=60
disable_splash=1
boot_delay=0
~~~
- Append `quiet fastboot` to `/boot/cmdline.txt`
- Append contents of `files/network_interfaces` to file `/etc/network/interfaces`
- Copy contents of `linux/pidgc-display.service` to `/etc/systemd/system/pidgc-display.service`
- Copy contents of `linux/pidgc-hw-interface.service` to `/etc/systemd/system/pidgc-hw-interface.service`
- Enable both services
  - `sudo systemctl enable pidgc-display.service`
  - `sudo systemctl enable pidgc-hw-interface.service`
- Bring up CAN:
  - `sudo /sbin/ip link set can0 up type can bitrate 500000`
- Add user `pi` to dialout group:
  - `sudo adduser pi dialout`
- Disable unused services once everything is done. This decreases boot time substantially:
  - sudo systemctl disable ssh
  - sudo systemctl disable hciuart 
  - sudo systemctl disable nmbd # If you have samba installed
  - sudo systemctl disable smbd # If you have samba installed
  - sudo systemctl disable systemd-timesyncd
  - sudo systemctl disable wpa_supplicant
  - sudo systemctl disable rpi-eeprom-update
  - sudo systemctl disable raspi-config
  - sudo systemctl disable networking
  - sudo systemctl disable dhcpcd

Now when you reboot the system should start the display and be up and running!