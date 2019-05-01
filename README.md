# Si4463 UHF CW Beacon
A simple CW beacon forked from Alexander Sholohov RA9YER 

Please refer to [Alexander Sholohov GitHub](https://github.com/alexander-sholohov/si4463-beacon) for infos about using WDS from SiLabs to change tx frequency. 
This is a slightly modified version of his original code, i've just added a pair of features that IZ5MMH Stefano, a friend of mine, asked for his proper beacon. Now arduino goes to sleep in the inactivity period, and put to sleep the Si4463 breakout module too.
So i've added a Sleep() function "stolen" from nickgammon website to put arduino in sleep mode for 8 seconds, the maximum you can reach without add more code lines, and i've connected the Si4463 SDN pin to D2, to have the possibility to raise it up.
When you raise SDN at HIGH logical level the chip goes in ShutDowN mode, draining microampere, a must for a standalone beacon to leave ontop of some hill. As added bonus everything is restarted as the first time, and this help in avoiding possible locks or "software failures", after the tx is done the system goes in sleep mode, and after 8 seconds everything is raised up and resetted.
If you wanna change the morse timing you must edit the "src/morse/morse.cpp" file, exactly here:

```javascript
// configurable params
#define UNITSIZE_IN_MS (150)
#define WARMUP_PAUSE_IN_MS (3000)
#define ENDOFTEXT_PAUSE_IN_MS (3000)
#define LONG_RETRY_PAUSE_IN_MS (1000)
#define LONG_CARRIER_DURATION_IN_MS (4000)
```

UNITSIZE_IN_MS is the duration of a dot, the program automatically calculate the lenght of a line, the other parameters are self explanatory, only WARMUP_PAUSE_IN_MS need an explanation. Changing this you change the period where the Si4463 is in tx mode but without actually transmitting anything, and is used for a kind of "warmup of the valves" :-) The effect on the drift isn't so great as you can expect, the real problem in this board is the cheap crystal oscillator who is greatly inaccurate and suffer a lot the temperature changes, if you have the possibility change it with a tcxo smd quartz, you can buy it with a bunch of cents....or use the putty to thermically stabilize it as much as you can as i've done :-))

To avoid problems and use of logic level shifter, **the entire system will be powered @3.3V**, this comes in handy for a lot of things, and with only two AA batteries you can have a lot of hours of activity, this are the measured current drain of the entire system, i've used 2 NiMH batteries so @2.8V :

```javascript
sleep mode     4mA
wakeup         8mA
tx "warmup"   ~20mA
tx "keydown"  ~47mA
```

**!WARNING!** Powering Arduino from his 5V pin bypass the voltage regulator, so ensure to use the proper voltage here, and once more **!WARNING!** the Si4463 module ISN'T 5V tolerant, so use only 3.3V to power it up, i've choose to use 3.3V for both the boards, Arduino doesn't complain about this :-) you can read a lot of interesting things about that question on the great [Nick Gammon Forum](http://www.gammon.com.au/welcome.html), an invaluable source of infos for the makers...

This is the schematic:

![thebeacon](https://github.com/IU5HKU/Si4463-CW-Beacon/blob/master/img/layout.jpg)

and this is my realization, nothing of professional as you can see, but fully functional :-), Stefano think to leave the beacon ontop a near hill, 630m asl, with battery and a small solar panel to charge it....just for fun, we don't expect a lot from such tiny transmitter, it's just an experiment :-)
To obtain a better stability and avoid as much as possibile the frequency drift, i've put some putty over the breakout board, the same used for the glass on the old wooden windows, cheap and dirty, but it does his job quite well.

![thebeacon](https://github.com/IU5HKU/Si4463-CW-Beacon/blob/master/img/Beacon.jpg)
