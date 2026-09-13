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

Here's everything you will need to build MagniAssist:
- 1x Esp 32 C3 Supermini. [Aliexpress](https://www.aliexpress.com/ssr/300000512/BundleDeals2?spm=a2g0o.productlist.main.1.7356bJzJbJzJ5v&productIds=1005012865945088:12000059561521782&pha_manifest=ssr&_immersiveMode=true&disableNav=YES&sourceName=SEARCHProduct&utparam-url=scene%3Asearch%7Cquery_from%3A%7Cx_object_id%3A1005012865945088%7C_p_origin_prod%3A1005012143614522&pvid=2f450b8d-e709-4b13-9233-d81070c216c6)
- 1x DF Player Mini. [Aliexpress](https://www.aliexpress.com/item/1005006166800318.html?spm=a2g0o.productlist.main.1.3f6clD3ilD3iU1&algo_pvid=923f2b6e-3861-4e08-ab05-1cae28449f6d&algo_exp_id=923f2b6e-3861-4e08-ab05-1cae28449f6d-0&pdp_ext_f=%7B%22order%22%3A%222679%22%2C%22spu_best_type%22%3A%22price%22%2C%22eval%22%3A%221%22%2C%22fromPage%22%3A%22search%22%7D&pdp_npi=6%40dis%21NZD%213.98%211.74%21%21%2115.22%216.65%21%402101e80b17893333279048401e0f79%2112000037327793485%21sea%21NZ%216191201799%21X%211%210%21n_tag%3A-29919%3Bd%3Abd232e9e%3Bm03_new_user%3A-29895%3BpisId%3A5000000216539907&curPageLogUid=SxGtzcp9ICnd&utparam-url=scene%3Asearch%7Cquery_from%3A%7Cx_object_id%3A1005006166800318%7C_p_origin_prod%3A)
- 1x MicroSD Card. [Aliexpress](https://www.aliexpress.com/item/1005007272943317.html?spm=a2g0o.productlist.main.5.7301vmPzvmPzQD&algo_pvid=874a6baa-9b13-4711-bc4f-2fd11effb9bd&algo_exp_id=874a6baa-9b13-4711-bc4f-2fd11effb9bd-4&pdp_ext_f=%7B%22order%22%3A%227168%22%2C%22spu_best_type%22%3A%22price%22%2C%22eval%22%3A%221%22%2C%22fromPage%22%3A%22search%22%7D&pdp_npi=6%40dis%21NZD%2116.99%2113.48%21%21%219.68%217.68%21%402101ca9517893332971487561e0eed%2112000051323260152%21sea%21NZ%216191201799%21X%211%210%21n_tag%3A-29919%3Bd%3Abd232e9e%3Bm03_new_user%3A-29895%3BpisId%3A5000000216539907&curPageLogUid=KAAqwbFr3VaW&utparam-url=scene%3Asearch%7Cquery_from%3A%7Cx_object_id%3A1005007272943317%7C_p_origin_prod%3A)
- 1x 5V 1A Type-c 18650 TP4056 Lithium Battery Charger. [Aliexpress](https://www.aliexpress.com/item/1005006043031985.html?spm=a2g0o.productlist.main.1.4452mWrGmWrGcI&algo_pvid=5ffd7821-ca95-46b5-b80d-2f5b2039f758&algo_exp_id=5ffd7821-ca95-46b5-b80d-2f5b2039f758-0&pdp_ext_f=%7B%22order%22%3A%2244257%22%2C%22spu_best_type%22%3A%22price%22%2C%22eval%22%3A%221%22%2C%22fromPage%22%3A%22search%22%7D&pdp_npi=6%40dis%21NZD%214.09%211.74%21%21%2115.65%216.66%21%402101e7a317893332594326243e1077%2112000035475416223%21sea%21NZ%216191201799%21X%211%210%21n_tag%3A-29919%3Bd%3Abd232e9e%3Bm03_new_user%3A-29895%3BpisId%3A5000000216539907&curPageLogUid=ezPX8Pr2Y3Sx&utparam-url=scene%3Asearch%7Cquery_from%3A%7Cx_object_id%3A1005006043031985%7C_p_origin_prod%3A)
- 1x 3.7V To 12V Mini DC Boost Converter Board. [Aliexpress](https://www.aliexpress.com/item/1005010379187938.html?spm=a2g0o.productlist.main.1.50f9JoQYJoQYW7&algo_pvid=e8396f27-c1c9-41d7-9290-2e2c6130d734&algo_exp_id=e8396f27-c1c9-41d7-9290-2e2c6130d734-0&pdp_ext_f=%7B%22order%22%3A%2212%22%2C%22eval%22%3A%221%22%2C%22fromPage%22%3A%22search%22%7D&pdp_npi=6%40dis%21NZD%218.19%213.85%21%21%2131.31%2114.70%21%4021030dcd17893332155774795e0e00%2112000052210864003%21sea%21NZ%216191201799%21X%211%210%21n_tag%3A-29919%3Bd%3Abd232e9e%3Bm03_new_user%3A-29895%3BpisId%3A5000000217367203&curPageLogUid=QvngS4shbIia&utparam-url=scene%3Asearch%7Cquery_from%3A%7Cx_object_id%3A1005010379187938%7C_p_origin_prod%3A) (Output 5V)
- 1x INMP441. [Aliexpress](https://www.aliexpress.com/item/1005009213592784.html?spm=a2g0o.productlist.main.1.3dda3820tbg4Bx&algo_pvid=a9df6a68-a60a-43e8-853c-c5cf07f8c723&algo_exp_id=a9df6a68-a60a-43e8-853c-c5cf07f8c723-0&pdp_ext_f=%7B%22order%22%3A%223598%22%2C%22eval%22%3A%221%22%2C%22fromPage%22%3A%22search%22%7D&pdp_npi=6%40dis%21NZD%213.67%211.74%21%21%212.09%210.99%21%4021030dcd17893331496702607e0e00%2112000048333792865%21sea%21NZ%216191201799%21X%211%210%21n_tag%3A-29919%3Bd%3Abd232e9e%3Bm03_new_user%3A-29895%3BpisId%3A5000000216539907&curPageLogUid=bAsMqkQvLZPd&utparam-url=scene%3Asearch%7Cquery_from%3A%7Cx_object_id%3A1005009213592784%7C_p_origin_prod%3A)
- 1x 1000mAh 1 cell 3.7v lipo battery. [Aliexpress](https://www.aliexpress.com/item/1005012952293433.html?spm=a2g0o.productlist.main.11.44ee3c3B3c3BWV&algo_pvid=380913ca-43b8-440b-93c2-d60edf363e86&algo_exp_id=380913ca-43b8-440b-93c2-d60edf363e86-10&pdp_ext_f=%7B%22order%22%3A%2228%22%2C%22eval%22%3A%221%22%2C%22fromPage%22%3A%22search%22%7D&pdp_npi=6%40dis%21NZD%2120.65%2112.39%21%21%2178.97%2147.38%21%402101d3fe17893331110291788e0d64%2112000059869398761%21sea%21NZ%216191201799%21X%211%210%21n_tag%3A-29919%3Bd%3Abd232e9e%3Bm03_new_user%3A-29895&curPageLogUid=cc1YTNLWe8Sf&utparam-url=scene%3Asearch%7Cquery_from%3A%7Cx_object_id%3A1005012952293433%7C_p_origin_prod%3A)
- 1x 4 Ohm 3W Bass Speaker (27x17x18mm). You will have to use exactly this speaker or one with the same size - [Aliexpress](https://www.aliexpress.com/item/1005007110234373.html?spm=a2g0o.order_list.order_list_main.30.26a01802C9rLcA#nav-description)
- 1x Switch 
- 8x M2.5 Threaded Inserts
- 8x M2.5 Bolts
- 15x 6x4mm Neodymium Magnets (Or 30x 6x2mm Neodymium Magnets which are more commonly found on Aliexpress)



