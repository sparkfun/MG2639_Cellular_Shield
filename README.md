SparkFun MG2639 Cellular Shield
=========================

[![SparkFun MG2639 Cellular Shield](https://github.com/sparkfun/SparkFun-MG2639-Cellular-Shield/blob/88eb285879021d54828d13c734865cc6500bc2bf/Board%20Layout.png)](https://raw.githubusercontent.com/sparkfun/SparkFun-MG2639-Cellular-Shield/88eb285879021d54828d13c734865cc6500bc2bf/Board%20Layout.png?token=117102__eyJzY29wZSI6IlJhd0Jsb2I6c3BhcmtmdW4vU3BhcmtGdW4tTUcyNjM5LUNlbGx1bGFyLVNoaWVsZC84OGViMjg1ODc5MDIxZDU0ODI4ZDEzYzczNDg2NWNjNjUwMGJjMmJmL0JvYXJkIExheW91dC5wbmciLCJleHBpcmVzIjoxNDEzMjA3ODExfQ%3D%3D--2cf12303dcbe55c20f8917d72f3ada013677ca8e)

The SparkFun MG2639 Cellular shield is an Arduino compatible shield that gives the user access to the MG2639_V3 GSM+GPS module. The module is capable of accquiring a GPS location as well as communication with GSM networks all over the world. Phonecalls can be made, texts sent, and data pushed to and read from web pages. All the supporting circuitry is provided including translation from 2.8V of the module to a user selectable 3.3V or 5V.

The main interface is two software serial ports.

Power can be provided either from the Arduino board's VIN or from an external battery such as a lithium polymer.

Repository Contents
-------------------

* **firmware** - Contains the various example sketches to check network registration, send text messages, and post to the [SparkFun data channel](http://data.sparkfun.com).
* **hardware** - Schematic and PCB layout for the shield.
* **datasheets** - The AT command set, hardware guide, and various other datasheets.
* **library** - An eagle library containing the various custom footprints for this shield. Specifically the MG2639 footprint.

License Information
-------------------
The hardware is released under [CERN Open Hardware License 1.2](http://www.ohwr.org/attachments/2388/cern_ohl_v_1_2.txt).
The code is beerware; if you see me (or any other SparkFun employee) at the local, and you've found our code helpful, please buy us a round!

Distributed as-is; no warranty is given.
