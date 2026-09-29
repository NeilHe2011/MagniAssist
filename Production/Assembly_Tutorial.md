# This is the assembling tutorial for MagniAssist

All the wiring in this tutorial will follow the wiring image below:
<img width="576" height="399" alt="MagniAssist_WiredDiagram" src="https://github.com/user-attachments/assets/1b5cb148-a427-40b8-9d9d-c9750819a0bb" />

# 3D printing the parts:

Auto orient all the 3D models from the MagniAssist folder and 3D print them as normally (though the magnet attachment will need to stop twice during print to insert the metal piece and magnets). Optionally you can increase the strength of the electronics enclosure by increasing the wall loop and sparse infill density.

For the magnet attachment:
When a visible rectangular gap starts forming in the middle of the CentreMagnetAttachment.stl model stop the 3d printer. Now insert the thin metal piece and continue the print.
Then wait till the magnet holes become visible before stopping the 3d printer again to insert the magnets before continuing the rest of the print.

# Assembly tutorial

To start off we will put together the brain of MagniAssist using the 3D printed electronics enclosure. You can put the modules in the electronics enclosure by simply sliding it in (remember to wire it using the wiring diagram above before you do).
Use the below photos as reference:

<img width="496" height="297" alt="Screenshot 2026-09-11 093433" src="https://github.com/user-attachments/assets/dc055560-2451-4fb3-907e-0f93b8917592" />
<img width="381" height="280" alt="Screenshot 2026-09-11 093446" src="https://github.com/user-attachments/assets/d6bc53a9-da21-4669-a7a7-341c4961c366" />

Next use a soldering iron to press in the threaded inserts in the holes on the corners of the 3d print as shown below.

# Top:
<img width="430" height="218" alt="Screenshot 2026-09-11 133955" src="https://github.com/user-attachments/assets/75a8d124-209f-454b-b0cf-f6dbe11f317c" />

# Bottom:
<img width="471" height="247" alt="Screenshot 2026-09-11 133941" src="https://github.com/user-attachments/assets/d30d86f1-56a4-498f-8ba0-e952ae22b88f" />

Next insert the switch inside this rectangle hole before you solder one end of the switches wire to the + of the charging module and the other to IN+ of the boost convertor:

<img width="293" height="209" alt="Screenshot 2026-09-11 134135" src="https://github.com/user-attachments/assets/bcfd253b-9e8d-47d0-adcb-e392b48e2b3d" />

After soldering on a XT30 connector to the battery wires as well as the BAT+ and BAT- of the charging module put the XT30 connector of the charging module through the rectangle as shown below

<img width="430" height="218" alt="Screenshot 2026-09-11 133955" src="https://github.com/user-attachments/assets/f2a2bf9a-7f42-41b2-918c-b51a7fd2fc47" />



# Bolting in the 3d print covers

Now connect the battery to the charging modules XT30 connector before closing the bottom side by screwing in the magnet attachment cover with bolts.

<img width="612" height="353" alt="Screenshot 2026-09-11 141259" src="https://github.com/user-attachments/assets/a63d9cd0-cc72-4a3f-ac04-0e56cfac91ff" />


After placing in the speakers and the electronics enclosure you can now close the top by screwing in the top cover with bolts.

<img width="564" height="284" alt="Screenshot 2026-09-11 141314" src="https://github.com/user-attachments/assets/cf5934cf-3119-4623-b708-138303770693" />

# Programming MagniAssist

Now that MagniAssist is all assembled all that is left is programming it! You can get creative and program it however you want to your hearts content!
