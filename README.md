# RevLight
Windowing Rev Light AVR based Controller code. Written in HP-Infotech CodeVisionAVR C

Initial CAVR Project, 2 Stage RPM Light (windowing).

The Basic operation is this, 2 sets of RPM switches control the Stage Points. The First 2 switches sets the Top stage point and the second 2 switches set the Bottom stage point. These allow a value for a V8 of 200-12800 RPM. Switch settings out of range will produce a blinking display to indicate an error. 

The 2 switches are can be set to various RPM's. The Left most switch controls the value 0-12000 RPM in 1000 RPM increments, and the switch right of that controls the 100 RPM increments. 

Some BCD Switch Examples -

```
SW3 SW2
8   8  = 8800  RPM
0   9  = 900   RPM
a   1  = 10100 RPM
b   9  = 11900 RPM
```
And So on.

Their are 2 pair of switch settings, each sets the RPM for its stage.

* Stage 1 is the Power Band Start
* Stage 2 is the Over Rev Start

So to have you active Power Band Start at 3500RPM and end just before the Redline of 7800 RPM you would set the switches to Sw3=7 Sw2=8 Sw1=3 Sw0=5.

The minimum RPM for a V8 4 stroke will be 200 RPM. Below that you will get an error as indicated by
a blinking display. If you also set invalid values for the switch you will also get a blinking display.

Invalid values that can cause a blinking display are as follows-

- Too Low an RPM setting
- Too High an RPM setting
- Invalid Number on a Switch, ie, setting the 100's digit to a value > 9 (a,b,c,d,e,f are all invalid for the 100's digit.
- Stage 1 RPM > Stage 2 RPM, the power band must be lower then the Redline (Stage 2)
- Stage 2 RPM < Stage 1, SEE ABOVE

To reset the device, you must correct the switch settings and power down the device, then restart
it. The switch settings are only read in on power on.

Outputs are indicated as follows -

* Output 0 - YELLOW, Not inbetween Limits
* Output 1 - GREEN, At Lower or Below Upper RPM
* Output 2 - RED, AT or ABOVE the upper limit
* Output 3 - Diagnostic, Active when on but not able to measure RPM (ie, Cranking)

C Code was written with Codevision AVR C compiler. A companion program is included (EXE) that will help calculate Tic Tables if needed.

[![WTFPL](https://img.shields.io/badge/License-WTFPL-orange.svg)](http://www.wtfpl.net) With one exception that commercial users need to send me a sample, otherwise see terms of WTFPL license.



