/*
* RPM Table for the following Parameters - 
*
* 	Cylinders   : 8
* 	Cycle       : 4
* 	Clock (MHz) : 8
* 	Prescaler   : 8
*
* 	Start RPM   : 300
* 	End   RPM   : 12800
* 	Step  RPM   : 100
*
* T1 Tick Period     (ms) = 0.00100000004749745
* Overflow period T1 (ms) = 65.536003112793
*
* typedef unsigned int uint16;
*/                             

   
#define START_RPM		300
#define END_RPM			12800
#define RPM_TABLE_CNT	126			// Items in RPM table
#define T1_PRESCALE		8			// Timer 1 Prescale value
#define CPU_CLOCK_HZ	8000000.0	// CPU Clock in Hz (FLOAT)
   
const uint16 RPM_TO_TICKS[RPM_TABLE_CNT]={
	50000,	// (Hz 20.0) Set RPM 300  Actual RPM 300.0, Delta 0.0
	37500,	// (Hz 26.7) Set RPM 400  Actual RPM 400.0, Delta 0.0
	30000,	// (Hz 33.3) Set RPM 500  Actual RPM 500.0, Delta 0.0
	25000,	// (Hz 40.0) Set RPM 600  Actual RPM 600.0, Delta 0.0
	21429,	// (Hz 46.7) Set RPM 700  Actual RPM 700.0, Delta 0.0
	18750,	// (Hz 53.3) Set RPM 800  Actual RPM 800.0, Delta 0.0
	16667,	// (Hz 60.0) Set RPM 900  Actual RPM 900.0, Delta 0.0
	15000,	// (Hz 66.7) Set RPM 1000  Actual RPM 1000.0, Delta 0.0
	13636,	// (Hz 73.3) Set RPM 1100  Actual RPM 1100.0, Delta 0.0
	12500,	// (Hz 80.0) Set RPM 1200  Actual RPM 1200.0, Delta 0.0
	11538,	// (Hz 86.7) Set RPM 1300  Actual RPM 1300.1, Delta 0.1
	10714,	// (Hz 93.3) Set RPM 1400  Actual RPM 1400.0, Delta 0.0
	10000,	// (Hz 100.0) Set RPM 1500  Actual RPM 1500.0, Delta 0.0
	9375,	// (Hz 106.7) Set RPM 1600  Actual RPM 1600.0, Delta 0.0
	8824,	// (Hz 113.3) Set RPM 1700  Actual RPM 1699.9, Delta -0.1
	8333,	// (Hz 120.0) Set RPM 1800  Actual RPM 1800.1, Delta 0.1
	7895,	// (Hz 126.7) Set RPM 1900  Actual RPM 1899.9, Delta -0.1
	7500,	// (Hz 133.3) Set RPM 2000  Actual RPM 2000.0, Delta 0.0
	7143,	// (Hz 140.0) Set RPM 2100  Actual RPM 2100.0, Delta 0.0
	6818,	// (Hz 146.7) Set RPM 2200  Actual RPM 2200.1, Delta 0.1
	6522,	// (Hz 153.3) Set RPM 2300  Actual RPM 2299.9, Delta -0.1
	6250,	// (Hz 160.0) Set RPM 2400  Actual RPM 2400.0, Delta 0.0
	6000,	// (Hz 166.7) Set RPM 2500  Actual RPM 2500.0, Delta 0.0
	5769,	// (Hz 173.3) Set RPM 2600  Actual RPM 2600.1, Delta 0.1
	5556,	// (Hz 180.0) Set RPM 2700  Actual RPM 2699.8, Delta -0.2
	5357,	// (Hz 186.7) Set RPM 2800  Actual RPM 2800.1, Delta 0.1
	5172,	// (Hz 193.3) Set RPM 2900  Actual RPM 2900.2, Delta 0.2
	5000,	// (Hz 200.0) Set RPM 3000  Actual RPM 3000.0, Delta 0.0
	4839,	// (Hz 206.7) Set RPM 3100  Actual RPM 3099.8, Delta -0.2
	4688,	// (Hz 213.3) Set RPM 3200  Actual RPM 3199.7, Delta -0.3
	4545,	// (Hz 220.0) Set RPM 3300  Actual RPM 3300.3, Delta 0.3
	4412,	// (Hz 226.7) Set RPM 3400  Actual RPM 3399.8, Delta -0.2
	4286,	// (Hz 233.3) Set RPM 3500  Actual RPM 3499.8, Delta -0.2
	4167,	// (Hz 240.0) Set RPM 3600  Actual RPM 3599.7, Delta -0.3
	4054,	// (Hz 246.7) Set RPM 3700  Actual RPM 3700.0, Delta 0.0
	3947,	// (Hz 253.3) Set RPM 3800  Actual RPM 3800.4, Delta 0.4
	3846,	// (Hz 260.0) Set RPM 3900  Actual RPM 3900.2, Delta 0.2
	3750,	// (Hz 266.7) Set RPM 4000  Actual RPM 4000.0, Delta 0.0
	3659,	// (Hz 273.3) Set RPM 4100  Actual RPM 4099.5, Delta -0.5
	3571,	// (Hz 280.0) Set RPM 4200  Actual RPM 4200.5, Delta 0.5
	3488,	// (Hz 286.7) Set RPM 4300  Actual RPM 4300.5, Delta 0.5
	3409,	// (Hz 293.3) Set RPM 4400  Actual RPM 4400.1, Delta 0.1
	3333,	// (Hz 300.0) Set RPM 4500  Actual RPM 4500.4, Delta 0.4
	3261,	// (Hz 306.7) Set RPM 4600  Actual RPM 4599.8, Delta -0.2
	3191,	// (Hz 313.3) Set RPM 4700  Actual RPM 4700.7, Delta 0.7
	3125,	// (Hz 320.0) Set RPM 4800  Actual RPM 4800.0, Delta 0.0
	3061,	// (Hz 326.7) Set RPM 4900  Actual RPM 4900.4, Delta 0.4
	3000,	// (Hz 333.3) Set RPM 5000  Actual RPM 5000.0, Delta 0.0
	2941,	// (Hz 340.0) Set RPM 5100  Actual RPM 5100.3, Delta 0.3
	2885,	// (Hz 346.7) Set RPM 5200  Actual RPM 5199.3, Delta -0.7
	2830,	// (Hz 353.3) Set RPM 5300  Actual RPM 5300.4, Delta 0.4
	2778,	// (Hz 360.0) Set RPM 5400  Actual RPM 5399.6, Delta -0.4
	2727,	// (Hz 366.7) Set RPM 5500  Actual RPM 5500.5, Delta 0.5
	2679,	// (Hz 373.3) Set RPM 5600  Actual RPM 5599.1, Delta -0.9
	2632,	// (Hz 380.0) Set RPM 5700  Actual RPM 5699.1, Delta -0.9
	2586,	// (Hz 386.7) Set RPM 5800  Actual RPM 5800.5, Delta 0.5
	2542,	// (Hz 393.3) Set RPM 5900  Actual RPM 5900.9, Delta 0.9
	2500,	// (Hz 400.0) Set RPM 6000  Actual RPM 6000.0, Delta 0.0
	2459,	// (Hz 406.7) Set RPM 6100  Actual RPM 6100.0, Delta 0.0
	2419,	// (Hz 413.3) Set RPM 6200  Actual RPM 6200.9, Delta 0.9
	2381,	// (Hz 420.0) Set RPM 6300  Actual RPM 6299.9, Delta -0.1
	2344,	// (Hz 426.7) Set RPM 6400  Actual RPM 6399.3, Delta -0.7
	2308,	// (Hz 433.3) Set RPM 6500  Actual RPM 6499.1, Delta -0.9
	2273,	// (Hz 440.0) Set RPM 6600  Actual RPM 6599.2, Delta -0.8
	2239,	// (Hz 446.7) Set RPM 6700  Actual RPM 6699.4, Delta -0.6
	2206,	// (Hz 453.3) Set RPM 6800  Actual RPM 6799.6, Delta -0.4
	2174,	// (Hz 460.0) Set RPM 6900  Actual RPM 6899.7, Delta -0.3
	2143,	// (Hz 466.7) Set RPM 7000  Actual RPM 6999.5, Delta -0.5
	2113,	// (Hz 473.3) Set RPM 7100  Actual RPM 7098.9, Delta -1.1
	2083,	// (Hz 480.0) Set RPM 7200  Actual RPM 7201.2, Delta 1.2
	2055,	// (Hz 486.7) Set RPM 7300  Actual RPM 7299.3, Delta -0.7
	2027,	// (Hz 493.3) Set RPM 7400  Actual RPM 7400.1, Delta 0.1
	2000,	// (Hz 500.0) Set RPM 7500  Actual RPM 7500.0, Delta 0.0
	1974,	// (Hz 506.7) Set RPM 7600  Actual RPM 7598.8, Delta -1.2
	1948,	// (Hz 513.3) Set RPM 7700  Actual RPM 7700.2, Delta 0.2
	1923,	// (Hz 520.0) Set RPM 7800  Actual RPM 7800.3, Delta 0.3
	1899,	// (Hz 526.7) Set RPM 7900  Actual RPM 7898.9, Delta -1.1
	1875,	// (Hz 533.3) Set RPM 8000  Actual RPM 8000.0, Delta 0.0
	1852,	// (Hz 540.0) Set RPM 8100  Actual RPM 8099.4, Delta -0.6
	1829,	// (Hz 546.7) Set RPM 8200  Actual RPM 8201.2, Delta 1.2
	1807,	// (Hz 553.3) Set RPM 8300  Actual RPM 8301.1, Delta 1.1
	1786,	// (Hz 560.0) Set RPM 8400  Actual RPM 8398.7, Delta -1.3
	1765,	// (Hz 566.7) Set RPM 8500  Actual RPM 8498.6, Delta -1.4
	1744,	// (Hz 573.3) Set RPM 8600  Actual RPM 8600.9, Delta 0.9
	1724,	// (Hz 580.0) Set RPM 8700  Actual RPM 8700.7, Delta 0.7
	1705,	// (Hz 586.7) Set RPM 8800  Actual RPM 8797.7, Delta -2.3
	1685,	// (Hz 593.3) Set RPM 8900  Actual RPM 8902.1, Delta 2.1
	1667,	// (Hz 600.0) Set RPM 9000  Actual RPM 8998.2, Delta -1.8
	1648,	// (Hz 606.7) Set RPM 9100  Actual RPM 9101.9, Delta 1.9
	1630,	// (Hz 613.3) Set RPM 9200  Actual RPM 9202.5, Delta 2.5
	1613,	// (Hz 620.0) Set RPM 9300  Actual RPM 9299.4, Delta -0.6
	1596,	// (Hz 626.7) Set RPM 9400  Actual RPM 9398.5, Delta -1.5
	1579,	// (Hz 633.3) Set RPM 9500  Actual RPM 9499.7, Delta -0.3
	1562,	// (Hz 640.0) Set RPM 9600  Actual RPM 9603.1, Delta 3.1
	1546,	// (Hz 646.7) Set RPM 9700  Actual RPM 9702.5, Delta 2.5
	1531,	// (Hz 653.3) Set RPM 9800  Actual RPM 9797.5, Delta -2.5
	1515,	// (Hz 660.0) Set RPM 9900  Actual RPM 9901.0, Delta 1.0
	1500,	// (Hz 666.7) Set RPM 10000  Actual RPM 10000.0, Delta 0.0
	1485,	// (Hz 673.3) Set RPM 10100  Actual RPM 10101.0, Delta 1.0
	1471,	// (Hz 680.0) Set RPM 10200  Actual RPM 10197.1, Delta -2.9
	1456,	// (Hz 686.7) Set RPM 10300  Actual RPM 10302.2, Delta 2.2
	1442,	// (Hz 693.3) Set RPM 10400  Actual RPM 10402.2, Delta 2.2
	1429,	// (Hz 700.0) Set RPM 10500  Actual RPM 10496.8, Delta -3.2
	1415,	// (Hz 706.7) Set RPM 10600  Actual RPM 10600.7, Delta 0.7
	1402,	// (Hz 713.3) Set RPM 10700  Actual RPM 10699.0, Delta -1.0
	1389,	// (Hz 720.0) Set RPM 10800  Actual RPM 10799.1, Delta -0.9
	1376,	// (Hz 726.7) Set RPM 10900  Actual RPM 10901.2, Delta 1.2
	1364,	// (Hz 733.3) Set RPM 11000  Actual RPM 10997.1, Delta -2.9
	1351,	// (Hz 740.0) Set RPM 11100  Actual RPM 11102.9, Delta 2.9
	1339,	// (Hz 746.7) Set RPM 11200  Actual RPM 11202.4, Delta 2.4
	1327,	// (Hz 753.3) Set RPM 11300  Actual RPM 11303.7, Delta 3.7
	1316,	// (Hz 760.0) Set RPM 11400  Actual RPM 11398.2, Delta -1.8
	1304,	// (Hz 766.7) Set RPM 11500  Actual RPM 11503.1, Delta 3.1
	1293,	// (Hz 773.3) Set RPM 11600  Actual RPM 11600.9, Delta 0.9
	1282,	// (Hz 780.0) Set RPM 11700  Actual RPM 11700.5, Delta 0.5
	1271,	// (Hz 786.7) Set RPM 11800  Actual RPM 11801.7, Delta 1.7
	1261,	// (Hz 793.3) Set RPM 11900  Actual RPM 11895.3, Delta -4.7
	1250,	// (Hz 800.0) Set RPM 12000  Actual RPM 12000.0, Delta 0.0
	1240,	// (Hz 806.7) Set RPM 12100  Actual RPM 12096.8, Delta -3.2
	1230,	// (Hz 813.3) Set RPM 12200  Actual RPM 12195.1, Delta -4.9
	1220,	// (Hz 820.0) Set RPM 12300  Actual RPM 12295.1, Delta -4.9
	1210,	// (Hz 826.7) Set RPM 12400  Actual RPM 12396.7, Delta -3.3
	1200,	// (Hz 833.3) Set RPM 12500  Actual RPM 12500.0, Delta 0.0
	1190,	// (Hz 840.0) Set RPM 12600  Actual RPM 12605.0, Delta 5.0
	1181,	// (Hz 846.7) Set RPM 12700  Actual RPM 12701.1, Delta 1.1
	1172 	// (Hz 853.3) Set RPM 12800  Actual RPM 12798.6, Delta -1.4
};
