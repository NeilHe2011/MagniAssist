# MagniAssist
MagniAssist is a pocket sized, magnet mountable, smart speaker.
It features:
- Built in microphone that listens to your commands. e.g. setting timers and asking questions.
- Compact size for portability
- Magnetic back that allows you to snap it on any surface.
- 4 Ohm 3W Bass Speaker for crystal clear noise.

The goal of this project is to reduce daily hassles by creating a hands free smart assistant.

I designed this project for [Stardance!](https://stardance.hackclub.com/home) A program funded by HackClub where you can design/build anything you want and get it funded.

# CAD Model
Everything is designed on Fusion360. The case parts are connected by 8 M2.5 threaded inserts and bolts. The electronics enclosure for the chips/modules of MagniAssist will be removable; that way any upgrades/changes that will be made in the future will be possible.

<img width="887" height="444" alt="Screenshot 2026-09-08 172548" src="https://github.com/user-attachments/assets/2eef40ab-5f8e-44ec-8627-3cc7c19d850d" />
<img width="869" height="446" alt="Screenshot 2026-09-08 173222" src="https://github.com/user-attachments/assets/2f561279-9c8f-4500-a8dc-f5a72d5a2f35" />

# Wired Diagram
Sorry for the messy wiring diagram. Wasn't sure on how to make it look nice and tidy. I've added a legend at the bottom for further clarity and incase that still isn't clear I'm going to further explain it here. 
- First up solder the positive and negative terminals of the battery to the BAT + and BAT - on the charging module, next solder the OUT + and OUT - of the charging module to the IN + and IN - of the boost convertor module.
- Next solder the OUT + and OUT - of the boost convertor module to the 5V and GND on both the ESP32 C3 Supermini and DF Player Mini (Remember to solder on the 5V output option on the boost convertor as well!)
- Connect the Esp32 to the DF Player Mini by soldering Esp32's TX to DF Player Mini's RX and DF Player Mini's TX to Esp32's RX.
- Solder the Spk1 and Spk2 pin to the any of the speaker terminals (doesn't matter which).
- Next up solder the mic chips' SD to GPIO 8 on the Esp32 and WS to GPIO 7 on the Esp32. Now solder mic chip's SCK to GPIO 6, VCC to 3.3V and GND to GND on the esp32.
<img width="576" height="399" alt="MagniAssist_WiredDiagram" src="https://github.com/user-attachments/assets/0ab8d53d-3ae6-472a-9918-179e23aff315" />


# BOM

Here's everything you will need to build MagniAssist (For a BOM more in depth please check out the BOM.md file above) :

- 1x Esp 32 C3 Supermini.
- 1x DF Player Mini. 
- 1x MicroSD Card. 
- 1x 5V 1A Type-c 18650 TP4056 Lithium Battery Charger. 
- 1x 3.7V To 12V Mini DC Boost Converter Board.
- 1x INMP441.
- 1x 1000mAh 1 cell 3.7v lipo battery. 
- 1x 4 Ohm 3W Bass Speaker (27x17x18mm). You will have to use exactly this speaker or one with the same size.
- 1x Switch 
- 8x M2.5 Threaded Inserts 
- 8x M2.5 Bolts
- 15x 6x4mm Neodymium Magnets (Or 30x 6x2mm Neodymium Magnets which are more commonly found on Aliexpress). 


