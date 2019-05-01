# Si4463-CW-Beacon
A simple CW beacon forked from Alexander Sholohov RA9YER 

Please refer to https://github.com/alexander-sholohov/si4463-beacon for infos about using WDS from SiLabs to change tx frequency.
This is a slightly modified version, i've just added a pair of features that IZ5MMH Stefano, a friend of mine, asked for his proper beacon. Now arduino goes to sleep in the inactivity period, and put to sleep the Si4463 breakout module too.
So i've added a Sleep() function "stolen" from nickgammon website to put arduino in sleep mode for 8 seconds, the maximum you can reach without add more code lines, and i've connected the Si4463 SDN pin to D2, to have the possibility to raise it up.
When you raise SDN at HIGH logical level the chip goes in ShutDowN mode, draining microampere, a must for a standalone beacon to leave ontop of some hill. As added bonus everything is restarted as the first time, and this help in avoiding possible locks or "software failures", after the tx is done the system goes in sleep mode, and after 8 seconds everything is raised up and resetted.
To avoid problems and use of logic level shifter, the entire system will be powered @3.3V, this comes in handy for a lot of things, and with only two AA batteries you can have a lot of hours of activity, this are the measured current drain of the entire system, i've used 2 NiMH batteries so @2.8V :

sleep mode     4mA
wakeup         8mA
tx in warmup  ~20mA
tx "keydown"  ~47mA

This is the schematic

and this is my realization, nothing of professional as you can see, but fully functional :-), Stefano think to leave the beacon ontop a near hill, 630m asl, with battery and a small solar panel to charge it....just for fun, we don't expect a lot from such tiny transmitter, it's just an experiment :-)
