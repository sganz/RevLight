/**********************************************************
Project             : RevLight
Version             : 1.0.0.10
Date                : 11/12/2002
Author              : Sandy Ganz
Company             : SGC
Compiler            : CodeVision AVR V1.23.9b
Comments            : Windowing Based Light Controller
Chip type           : AT90S2313
Clock frequency     : 8.000000 MHz
Memory model        : Tiny
Internal SRAM size  : 128
External SRAM size  : 0
Data Stack size     : 64
Optimize            : SPEED Global, Selective Size
External Startup    : YES, GLOBAL VAR INIT HAS BEEN REMOVED
**********************************************************/
         
// 1.0.0.10 06/07/2003 Final code change

#include <90s2313.h>
#include <delay.h>
         
typedef unsigned long int uint32;
typedef unsigned int uint16;
typedef int int16;
typedef unsigned char ubyte;

#define ON  1
#define OFF 0         

/*
* Startup Defaults
*/                

#define START_BRIGHTNESS	3		// Value when in Dimmed mode
#define START_CYLINDERS		8		// V8

#define LAMP_CHECK_DLY		125		// Start up sequence lamp check in MS
#define SWITCH_CHECK_DLY	100		// Stabilize switch reading in MS
#define FLASH_BLINK_DLY		500		// Blink rate for error counts in MS
#define FLASH_SEQUENCE_DLY	2000	// Delay after a flash of a number in MS
#define ERROR_LOOP_DELAY	2000	// Restart rate for error sequence in MS

/*
* Error Blink codes
*/

#define ERROR_RPM_RANGE		2		// Switch Settings for RPM are messed up
#define ERROR_STRAY_CODE	10		// Should never happen, but if it does make it a long sequence

/*
* Defines the OUTPUT ports to the Lamp Driver pins
*/

#define OUTPUT3 PORTD.5		// Optional Output
#define OUTPUT2 PORTD.4		// Red
#define OUTPUT1 PORTD.1		// Green
#define OUTPUT0 PORTD.0		// Yellow
	                            
#define D_INPUT_MASK		0x4c
#define ALL_D_OUTPUT_ON		0xff
#define ALL_D_OUTPUT_OFF	D_INPUT_MASK

/*
* Defines for Port Masking, etc
*/                          

#define OUTPUT_OPTION	(0x20)
#define OUTPUT_RED		(0x10) 	
#define OUTPUT_GREEN	(0x02)
#define OUTPUT_YELLOW	(0x01)

/*
* Optional Input Ports
*/                    

#define INPUT0 PIND.2		// Optional Input 0
#define INPUT1 PIND.3		// Optional Input 1

/*
* Defines the BCD/HEX switch Decoder port pins
*/                                                

#define COLSEL3 PORTB.7
#define COLSEL2 PORTB.6
#define COLSEL1 PORTB.5
#define COLSEL0	PORTB.4
      
/*
* Important! If you write a 0 to an Input port pins
* you will turn OFF the pull up feature.
*/                                      

#define COLSEL_ALL_OFF 0x0f
#define COLSEL_ALL_ON  0xff
                        
/*
* Make the inputs low on the port so no shift of bits needed
*/

#define ROWINPUT3 PINB.3
#define ROWINPUT2 PINB.2
#define ROWINPUT1 PINB.1
#define ROWINPUT0 PINB.0

/*
* Switch set to Position 5 (BCD or HEX Switch), will provide a GND path
* from the Column Select to port pins as follows (TRUE SWITCH) -
*
* PB3 - 1              
* PB2 - 0
* PB1 - 1
* PB0 - 0
*
* Some Switches are INVERTED LOGIC (Same Value of 5) Pins would be as follows -
* 
* PB3 - 0              
* PB2 - 1
* PB1 - 0
* PB0 - 1
* 
* For TRUE Logic switchs, set BCD_SWITCH_TRUE, otherwise comment out for
* use of Inverted logic switches
*/            
                                
#define BCD_SWITCH_TRUE YES

#ifdef BCD_SWITCH_TRUE
#define SWITCHDATA (~PINB & 0x0f)
#else
#define SWITCHDATA (PINB & 0x0f)
#endif

#define BRIGHT_TABLE_CNT	8                                                          
            
/*
* These may need inversion based on ports drive logic. If these are put into
* eeprom, then no global init needs to be done. As such we can squeeze more
* free space out of the chip.
*/                             
                                   
ubyte eeprom BrightnessMap[BRIGHT_TABLE_CNT]={0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f};

// If we have code space for the lamp check, it is pretty ;-) costs 32 bytes

#define LAMPCHECK 
#define FIXED_CYLINDER_CNT

#ifdef FIXED_CYLINDER_CNT 
/*
* For getting some extra memory by fixing the number of cylinders,
* also safer since not in eeprom
*/
#define NUM_CYLINDERS	(8)
#else
#define NUM_CYLINDERS (eeprom_NumCylinders)
#endif                             
        
/*
*  Global Vars
*
* Note that init of a global will cost table space (Data Area). Startup code
* clears SRAM and R2-R15, so should be clean (0x00)
*/            

register uint16 Ticks;				// Timer1 Ticks
register uint16 SafeTicks;			// only used in main loop
register uint16 Stage1Ticks;		// First Break point (Stage)
register uint16 Stage2Ticks;		// Second Break Point (Overrev)
register uint16 LoopTicks;			// Rolling counter
register ubyte Timer1_Overflow;		// Non Zero when Timer wraps

/*
* For these we don't care if they are registers
*/

ubyte OutputBuffer;					// Output Data to port
uint16 RepeatCnt;	                // Count for Repeat
uint16 Stage1Overlap;				// Overlap Vals            
uint16 Stage2Overlap;
uint16 AvgTicks;

#define AVERAGER

#ifdef AVERAGER
                    
/*
* If the averager is enabled, the tick counts will be
* averaged over the number of main loop cycles specified.
* See notes in code below for info on it.
*/                                       

#define AVG_CYCLES 16			// should be good for most Cylinder counts.

float AvgAccum;
ubyte AvgCnt;
#define REPEAT_WAIT_CNT	6000	// much less since floating point accumulation slows things down

#else

/*
* Number of main loops to wait before acepting a switch. 
* PROCESSOR CLOCK and Main Loop latency dependant.
* This may be too short on much faster a processor then 8mhz
*/

#define REPEAT_WAIT_CNT	60000	// running pretty wide open not much loop delay if no floats
#endif

/*
* These are EEPROM and should be programmed as well when
* the code is downloaded to Flash. 
*/

ubyte eeprom eeprom_Brightness = START_BRIGHTNESS;		// this is EEPROM for the output Brightness

#ifndef FIXED_CYLINDER_CNT
ubyte eeprom eeprom_NumCylinders = START_CYLINDERS;		// number of cylinders for start up
#endif                          
                                   
//#define USE_TICK_TABLE

#ifdef USE_TICK_TABLE

#include "ticktbl.h"

/*
* Start RPM is the 0 item in the table, and not usually 0
* This is the offset to get a switch setting to a table entry.
* for example, if the switches are set to 500RPM (SW3=0, SW2=5) the
* index should be 3
*/                         

#define START_SKIPPED_CNT 2
#define GetCylinderCount()		// Removes from the code, chee-zee
#else

// CPU CLock from the CAVR Project definition, in HZ

#define CPU_CLOCK_HZ   _MCU_CLOCK_FREQUENCY_ 

// Prescale Setting 

#define T1_PRESCALE		8						

#endif	// USE_TICK_TABLE

/*
* For the interrupt routines, we will handle the prolog/epilog
* as the compiler is too generous as we only need to save 
* R30 and the SREG. Also all interrupts should be SPEED optimized
* to prevent any monkey business.
*/                               

#pragma savereg-
#pragma optsize-
                
/*
* Turn off complete register save, just do it manual for the IRQ stuff...
*/                                                                       

interrupt [TIM1_OVF] void timer1_ovf_isr(void)
{
#asm 
    push r30			; Save The R30 Register for 'C' program usage
    in   r30,SREG		; Save the Sreg
    push r30
#endasm

	Timer1_Overflow = 0xff;		// TRUE
	
#asm                                   
    pop r30				; Epilog...backwards
    out SREG,r30        ; Sreg restore
	pop r30				; Now whatever r30 was
#endasm
}

interrupt [TIM1_CAPT] void timer1_capt_isr(void)
/*
* This is called when the timer ICP pin causes an IRQ
*
* If Timer1 has TKO'ed (Overflowed) then it must be reset here
* as this is the only place were you know you have a count.
*
*/
{
#asm 
    push r30			; Save The R30 Register for 'C' program usage
    in   r30,SREG		; Save the Sreg
    push r30
#endasm
    
	/*
	* If coming out of overflow, skip the next read as it 
	* not be a correct sample. Keep Ticks in overflow like
	* value (0) and wait again.
	*/
	
	if(Timer1_Overflow)
	{   
		Ticks = 0;				// transfer to the global
		Timer1_Overflow = 0;	// if overflow, just reset...
	}                       
	else
		Ticks = ICR1;			// transfer to the global

 	TCNT1H = 0;					// Faster then TCNT1 = 0;
	TCNT1L = 0; 
		
#asm                                   
    pop r30				; Epilog...backwards
    out SREG,r30        ; Sreg restore
	pop r30				; Now whatever r30 was
#endasm
}

/*
* Let the compiler handle the prolog/epilog now, as well as let it start
* to optimize for space until otherwise told not to.
*/                                                  

#pragma savereg+
#pragma optsize+

/*
* Regular stack frame back for other functions
*/

void FlashOutputs(ubyte Count)
/*
* Count should be > 0, usually interrupts
* should be off or timing will be off
*/
{
	while(Count--)
	{
	 	PORTD = ALL_D_OUTPUT_ON;	// Turn all on
	 	delay_ms(FLASH_BLINK_DLY);	// wait a few
	 	PORTD = ALL_D_OUTPUT_OFF;	// Turn all off
	 	delay_ms(FLASH_BLINK_DLY);	// wait a few
	}
}

void FatalError(ubyte Error)
/*
* If you are here then blink error code and delay, repeat forever
*/
{                             
	// Kill the interrupts 

#asm("cli")                

	// Start outputs at clean state
	
	while(1)
	{                                           
		FlashOutputs(Error);			// Do the blink code
		delay_ms(ERROR_LOOP_DELAY);		// restart sequence delay
         
 		/*
 		* If a fatal error, allow the user to push the button
 		* to reset, ie, they can readjust the rpm settings
 		* and then push to rest the controller. 
 		* 
 		* The Error Code will flash until the user holds the push
 		* button. Then all lights will come on indication they
 		* should release the button.
 		*/
 		
		if(!INPUT0)
		{
		 	PORTD = ALL_D_OUTPUT_ON;	// Turn all on
			while(!INPUT0);	// Wait for switch to be released	
			WDTCR = 0x08;	// turn on WDT to shortest time, then let it die
			while(1);                                                                         
			
		}
	} 
}
  
#ifndef USE_TICK_TABLE

// If NOT using the table, the user can set the number of cylinders
                                
#ifndef FIXED_CYLINDER_CNT
void GetCylinderCount()
/*
* If not using the table, we can have user cylinder count selection
* Check for input on the push button, if so, then the 
* user is trying to set the number of cylinders. The Button
* MUST BE HELD WHILE THE UNIT IS OFF, THEN POWERED UP
*
* Programming would be like this...
*
* With Button Held in, Light will blink a number of times for the selected cylinders
*
* When the button is released, Flash the count again just to make sure
* the user has it set correctly, then update the eeprom, and continue
*/
{
	// use correct input port
	if(!INPUT0)		// Key Pressed at startup, enter cylinder programming mode
	{        
		FlashOutputs(eeprom_NumCylinders);	// Show number of clinders by blinking
		           
		while(!INPUT0);						// Wait for switch to be released
		
		while(1)
		{
			if(!INPUT0)			   			// Pressed again
			{                                               
				delay_ms(SWITCH_CHECK_DLY); 	// Delay a bit for things to settle
				while(!INPUT0);			   	    // Wait for release                    
				delay_ms(SWITCH_CHECK_DLY); 	// Delay a bit for things to settle

				if(eeprom_NumCylinders == 4 
					|| eeprom_NumCylinders == 6
					|| eeprom_NumCylinders == 8)
//					|| eeprom_NumCylinders == 10
//					|| eeprom_NumCylinders == 12)
				{	                           
				    if(eeprom_NumCylinders == 8)
				    	eeprom_NumCylinders = 4;
	   							    else
						eeprom_NumCylinders += 2;  
				}    	
				else
					eeprom_NumCylinders = 8;			// default incase eeprom not setup

#ifdef NeVeR 

//
// Saves some memory, but not as safe
//
				eeprom_NumCylinders += 2;
				if(eeprom_NumCylinders > 12)
					eeprom_NumCylinders = 4;
#endif
				FlashOutputs(eeprom_NumCylinders);		// Show number of clinders by blinking
			}
		}
	}
}                     
#endif                          

#endif

uint16 RpmToTicks(ubyte bRPM, ubyte bCyl)
/*
* This is COOL, Codevision supports FLOATS!
*
* bRPM in hundreds, as 77 = 7700 RPM, bCyl is the Number of cylinders
* 
* Return : Tickcount for the RPM and Cylinders, or 0 if invalid params
*         
* ---------------------------------------------------------------------
* The basic Tach formula for 4 Stroke engnine with is -
*
* Tach(Hz) = (Firing per Rev) 1/2 * (# Cylinders) * (Engine RPM)/60  
* 
* Period in milliseconds is 1000/Tach(Hz)
*         
* Example - V8 running at 6000 Rpm would have a Tach rate of 400hz 
* which is a period of 2.5ms
* 
* The stratagy here is to calculate the switch settings in the
* number of timer ticks that can happen during a cycle at a given
* RPM. So for the above example we need to have the timer start
* and stop counting in a 2.5ms period, then take that count and
* check for ranges.
*       
* (Most from AVR130 App note)
*
* To get the number of timer ticks you need to know the time for 
* a tick of the timer. 
*
* (CPU Clock MHz)/(Timer 1 Prescaler)
*
* The selected clock is 4Mhz and the Prescale is /64. Other prescale
* values that might work would be /8, but that would limit the range
* for low RPM tracking a bit. /64 give good resolution but /8 give
* much better in the high RPM range where tick counts are low. Messing
* with the combination of clock and prescale can get optimal results for
* both low speed tracking and high speed accuracy
*
* Examples-
*
* 4.00MHz/64 = 16uS Per Clock Tick and a maximum
* Time before the counter overflows of around 1.0485 seconds 
*
* 4.0MHZ/8 = 2uS Per Clock Tick and a maximum 
* Time before the counter overflows of 131.072ms
*                                               
* Some examples at Clock/64
*
* Given some worst case engine speeds, for example a V12 running at 
* 12,000 rpm would have a Tach Frequency of 1200Hz which is a period 
* of 0.000830sec or 830us. This means that we would have a count of
* 52 in the timer at this RPM, that is OK, but not too accurate!
*
* The Next worst case is a 4 Cylinder at idle. A 4 cylinder running
* at 600 RPM would have a LONG time between pulses. This equates to
* 20hz or 50ms. A 4 cylinder at 60RPM would have a period of 500ms so
* Plenty of room for slow speed checking.
*
* Again, jumbling the clock and prescale can give good results
* for a broad range of cylinders and expected RPMs
*/
{
#ifdef USE_TICK_TABLE                                            
/*
* Lookup the TICKS
*/                     
	uint16 idx;

	if(!(bCyl && bRPM))
		return(0);
     
  	// The index is calculated by fixing up the offset 
  	
	idx = bRPM - START_SKIPPED_CNT;
	
	if(idx < 0 || idx >= RPM_TABLE_CNT)	                               
		return(0);
	
	return(RPM_TO_TICKS[idx]);
#else

	/*
	* Calculate the TICKS 
    * float dHz, dMS, dPrescale_ms;
	* 	
    * dPrescale_ms = 1000.0 / (CPU_CLOCK_HZ / T1_PRESCALE);
    * dHz = ((float) bCyl * (float) bRPM) / 60.0;
    *
    * Period of that Freq in Millisecs
    *
    * dMS = 1000.0 / dHz;
    *
    * from the tick period count number of ticks, round up and truncate
    *                                           
    * return(dMS / dPrescale_ms + 0.5);
    *
    * Optimize the formulas of course...
    */
                 
    //uint16 iRPM;

	uint32 lTicks;
        
	if(!(bCyl && bRPM) || bRPM > 128)
		return(0);

	//iRPM = (int) bRPM * 100;

	// formula and rounding
           
	// (1000.0 / (((float) bCyl/2 * (float) bRPM) / 60.0)) / (1000.0 / ((float) CPU_CLOCK_HZ / T1_PRESCALE)) + 0.5;
  	
  	/*
  	* optimized a bit, 1.2 takes into account divide by 2 for 4 cycle engine
	* as well as the fact that RPM is div by 100
  	* saves a bit of space since the compiler will calculate the constant
  	* in the numerator and only have one divide...
  	* The 0.5 is added to move the range to cause the stage to be earlier then
  	* later, so the red light would have error but would be in the direction
  	* of turning on a few RPM early, which is safer.
  	*/

	lTicks = ( ((float) CPU_CLOCK_HZ / T1_PRESCALE ) * 1.20) / ((float) bCyl * (float) bRPM) + 0.5;
	                                  
	// don't convert to uint16 yet, if out of 16bit range, must return 0
	// since it could be too slow (low) of an RPM settting
		         
	if(lTicks > 65535L)
		return(0);
		
	return(lTicks);
#endif
}

void Setup(void)
/*
* Sets up ports, reads switchs, etc. Allows the compiler to optimize
* this for space, since this doesn't much matter, make it small as possible.
*/      
{                    
	ubyte HexSwitchH, HexSwitchL;
	ubyte bRPM;
	            
	/*
	* Port B (8 Bit Port) - Input/Output Ports initialization
	*
	* Pin Definitions :
	* PB7 - Column Select 3 as OUTPUT (MSB)
	* PB6 - Column Select 2 as OUTPUT
	* PB5 - Column Select 1 as OUTPUT
	* PB4 - Column Select 0 as OUTPUT (LSB)
	* PB3 - Encoder Bit 3 as Input (MSB)
	* PB2 - Encoder Bit 2 as Input
	* PB1 - Encoder Bit 1 as Input
	* PB0 - Encoder Bit 0 as Input (LSB)
	*
	* All outputs start at 0, inputs NOT pulled up.
	*/
  
	PORTB=0x0f;		
	DDRB=0xF0;

	/*
	* Port D (7 Bit Port) - Input/Output Ports initialization
	* 
	* Pin Definitions :
	* PD7 - Not Used
	* PD6 - Input, ICP Pin for Timer1
	* PD5 - Output 3
	* PD4 - Output 2
	* PD3 - Input, Possible Interrupt 1
	* PD2 - Input, Possible Interrupt 0
	* PD1 - Output 1
	* PD0 - Output 0
	*  
	* All outputs at 0, all inputs are pulled up. MSBIT not used
	*/
    
 	PORTD=0x4C;		// Binary 0100 1100 
	DDRD=0x33;		// Binary 0011 0011
	
	/*
	* Timer/Counter 0 initialization
	* Set to not used, i.e., system clock and stopped
	*/

	TCCR0=0x00;
	TCNT0=0x00;
                           
	/*
	* Timer/Counter 1 initialization -
	* 
	* Clock source 	: System Clock
	* Clock value 	: Timer 1  Clock/8
	* Mode 			: CTC top=OCR1A
	* OC1 output 	: Discon.
	* Noise Canceler: Off
	*
	* Input Capture on Falling Edge (ICP Pin on PD6)
	*
	* Note : It might be a nice feature to adjust the Prescale value
	* on some combinaions of Cylinder and CPU Clock. I.e., if the
	* user has selected something < 4 Cylinder then change the clock to
	* /64. This allows lower end RPM not to be too high.
	*/
	
	TCCR1A=0x00;		// no output for compare mode
//	TCCR1B=0x02;		// Binary 0000 0010 Falling Edge ICP, CTC and Clk/8 
	TCCR1B=0x82;		// Binary 1000 0010 Noise Cancel, Falling Edge ICP, CTC and Clk/8 
	TCNT1H=0x00;		// Clear Timer counts
	TCNT1L=0x00;
	OCR1H=0x00;			// Clear Compare register
	OCR1L=0x00;

	/*
	* External Interrupt(s) initialization
	* INT0 : Off
	* INT1 : Off
	* 
	* Currently not used for interrupts, but could be as they are inputs
	*/
	
	GIMSK=0x00;			// Interrupt controll
	MCUCR=0x00;			// Edge settings

	/*
	* Timer(s)/Counter(s) Interrupt(s) initialization
	* 
	* Set up for Interrupts on the following events
	*
	* ICP - Input Capture
	* Timer 1 Overflow
	*/
	
	TIFR=0x88;	// Clear Timer 1 pending overflow and capture events (just to be safe)
	TIMSK=0x88;	// Binary 1000 1000 

	/*
	* Analog Comparator initialization
	*
	* Analog Comparator : Off
	* Analog Comparator Input Capture by Timer/Counter 1 : Off
	* Analog Comparator Output : Off
	*/

	ACSR=0x80;

	/*
	* Check for USER change of cylinder count, does nothing
	* if using the table, since using the table implies the number
	* of cylinders
	*/
	
	
#ifndef FIXED_CYLINDER_CNT	
	GetCylinderCount();   
#endif	
	                                  
	// Lamp Check (Nice rolling sequence) Cost is about 32 bytes after optimizer
	
#ifdef LAMPCHECK                              
	OUTPUT0 = ON;
	delay_ms(LAMP_CHECK_DLY);
	OUTPUT1 = ON;
	delay_ms(LAMP_CHECK_DLY);
	OUTPUT0 = OFF;
	delay_ms(LAMP_CHECK_DLY);
	OUTPUT2 = ON;
	delay_ms(LAMP_CHECK_DLY);
	OUTPUT1 = OFF;
	delay_ms(LAMP_CHECK_DLY);
	OUTPUT3 = ON;
	delay_ms(LAMP_CHECK_DLY);
	OUTPUT2 = OFF;
	delay_ms(LAMP_CHECK_DLY);
	OUTPUT3 = OFF;
	delay_ms(LAMP_CHECK_DLY);
#endif                        

	/*
	* Read the BCD switches, this is done only at power on
	*
	* The hardware is connected to potentially 4 bcd switches
	* with the column select (an output) for the LSB at PB4 to MSB at PB7.
	* The Rows are a HEX NIBBLE which can be read in LSB at PB0 to MSB at PB3.
	*/                                       
	         
	PORTB = COLSEL_ALL_ON;			// Turn off all selects just to be sure...

	COLSEL3 = OFF;					// Turn on the Most significat Switch first
	delay_ms(SWITCH_CHECK_DLY); 	// Delay a bit for things to settle
	HexSwitchH = SWITCHDATA;      	// Now Get the lower 4 bits
	COLSEL3 = ON;					// Turn on the Most significat Switch first

	COLSEL2 = OFF;
	delay_ms(SWITCH_CHECK_DLY);
	HexSwitchL = SWITCHDATA;
	COLSEL2 = ON;

	// Debug Switch test
//--------------------->	            
//	HexSwitchH = 0x07;	// Set for 7800 Limit
//	HexSwitchL = 0x08;
//<---------------------
	// calc RPM, and then Ticks for Stage2 (Rev Limit)
	
	bRPM = 10 * HexSwitchH + HexSwitchL;	// Get into RPM/100

	Stage2Ticks = RpmToTicks(bRPM, NUM_CYLINDERS);

	/*
	* This calculates a ~100RPM Tick count based on the next Lower RPM Setting                 
	* so the Stage2 indicator can come in a little early so you can use it
	* for knowing you are about to REDLINE. Slight chance for
	* error if the bRPM is at the end of the table. Not very likely...But be
	* aware ;-)
	*/
	
	Stage2Overlap = RpmToTicks(--bRPM, NUM_CYLINDERS);

	/*
	* If messed up, just use the Stage1Ticks for the transition,
	* this will yeild no overlap, but will catch a rare error
	* and still let it run without overlap
	*/
	
	if(!Stage2Overlap)
		Stage2Overlap = Stage2Ticks;

    // Get Stage 1 Settings                                               
	
	COLSEL1 = OFF;
	delay_ms(SWITCH_CHECK_DLY);
	HexSwitchH =  SWITCHDATA;
	COLSEL1 = ON;      

	COLSEL0 = OFF;					// Now the Least Siginificant
	delay_ms(SWITCH_CHECK_DLY);
	HexSwitchL =  SWITCHDATA;    
	COLSEL0 = ON;

// Debug Switch test
//--------------------->	            
//	HexSwitchH = 0x03;
//	HexSwitchL = 0x05;
//<---------------------
	
	// calc the RPM for the stage 1 (staging)
		
	bRPM = 10 * HexSwitchH + HexSwitchL;
            
	Stage1Ticks = RpmToTicks(bRPM, NUM_CYLINDERS);

	/*
	* This calculates a ~100RPM Tick count based on the next Lower RPM Setting                 
	* so the Stage1 indicator can come in a little early so you can use it
	* for knowing you are at the bottom of the powerband. Slight chance for
	* error if the bRPM is at the end of the table. Not very likely...But be
	* aware ;-)
	*/
	
	Stage1Overlap = RpmToTicks(--bRPM, NUM_CYLINDERS);
	
	/*
	* If messed up, just use the Stage1Ticks for the transition,
	* this will yeild no overlap, but will catch a rare error
	* and still let it run without overlap
	*/
	
	if(!Stage1Overlap)
		Stage1Overlap = Stage1Ticks;
		
	/*
	* Check for more sane values, or fatal error the user
	* RPMToTicks Must return 0 for out of Range RPM Values
	*/
	                                                    
	if(Stage1Ticks < Stage2Ticks || !Stage1Ticks || !Stage2Ticks)
		FatalError(ERROR_RPM_RANGE);
		
}

/*
* Now back to SPEED optimization. This prevent any odd
* code from being generated in the main loop.
*/                         
            
#pragma optsize-

void main(void)
{
	Setup();	

    /*
    * Current Output Connections -
    *
    * OUTPUT0 = YELLOW, RPM < Stage1RPM
    * OUTPUT1 = GREEN, RPM >= Stage1RPM and RPM < Stage2RPM
    * OUTPUT2 = RED, RPM >= Stage2RPM
    * OUTPUT3 = RPM TOO SLOW
    *
    * Remember that the period (number of ticks) gets larger when
    * the RPM gets slower, so these compairs will seem backwards
    */
                              
	while(1)
	{
        /*
        * This operation must be indivisible as it is 2 bytes 
        * and could get split up by an interrupt. Total cost
        * for this operation is 4 instructions (1us) of potential
        * interrupt delay which is about 1/2 a tick if /8
        * on the prescaler. This could be factored in, but
        * just not that much of an error if IRQ's are off
        * and the input event happens. At this point I'm not
        * even sure if the data is not set up in the ICR as accurate
        * just has not notified the CPU, in which case no added error.
        */
        
        #asm("cli")                
		SafeTicks = Ticks; 
		#asm("sei")
                            
#ifdef AVERAGER
		
		/*
		* Average the ticks, the trick is that we are out of code
		* space, but since we have already are using floating point
		* use it again since we would have to do this in a long, and
		* that would bring in additional code. It is a tad slower but
		* this is not a problem. Delays too long will make the 
		* display lazy, and too short would make this worthless. 
		* This should not be a huge effect on the PWM brightness,
		* but who knows...
		* 
		* Could make AVG_CYCLES read the number of cyls and
		* then multiply by 2 to get a full rev, so the average
		* would always be a complete set of all cylinders firing.
		*/ 

		AvgCnt++;
		AvgAccum += SafeTicks;
		
		if(AvgCnt == AVG_CYCLES)
		{
			SafeTicks = AvgTicks = AvgAccum / AVG_CYCLES;
			AvgCnt = 0;
			AvgAccum = 0;
		}
		else   
		{  
			// may want to pad operations here
			// to make loop consistant, as the 
			// floating divide might be a long
			// operation in terms of this loop...
			// But will change the PWM delay values more
			//AvgAccum / AVG_CYCLES;	// dummy operation, make sure not optimized out ;-)
			
			SafeTicks = AvgTicks;	// Just keep using the average
		}
		
#endif
                                       
        OutputBuffer = ALL_D_OUTPUT_OFF;
	 	if(Timer1_Overflow || !SafeTicks)
		{                               
			/*
			* Must check for Overflow first as it would indicate
			* that the Ticks is not a true value. If the Ticks is
			* Zero (0) then this is still in an overflow condition
			* waiting for a good ranged Ticks. This can be during
			* cranking or other slow speed points
			*/

    		OutputBuffer = OUTPUT_OPTION;		// only the option output
		}
		else       
		{           
   			// RPM lower then Stage RPM

    	  	if(SafeTicks > Stage1Ticks)
      		{       
	   	    	OutputBuffer = OUTPUT_YELLOW;	// Light YELLOW
    	   	}                                        
			
			/*
			* Now, see if we are in the power band, or just approaching it.
			* This is done by seeing if we have a few ticks less
			* by around 100 RPM then the Stage1Tick setting. This allows also 
			* keeps the Yellow light setting on too so an overlap between
			* the two can be seen so that when approaching the stage1 Rpm
			* you will see both Yellow and Green. As soon as the RPM 
			* Range is totally in the range of < Stage1Ticks and > Stage2Ticks
			* Only the Green light will be on. If the green light is on
			* and we are coming up to the red line (Stage2Overlap) then
			* turn on the RED light to indicate we are about to make the
			* transition to overrev!
			*/			

    		if(SafeTicks < Stage1Overlap && SafeTicks > Stage2Ticks)
    		{           
  				OutputBuffer |= OUTPUT_GREEN;	// Light GREEN
	
				/*
				* If getting close to the Redline, blip
				* the Red on to start to indicate close to
				* the edge...
				*/
				
	    		if(SafeTicks <= Stage2Overlap)
	    			OutputBuffer |= OUTPUT_RED;
	   		} 

    		/*
    		* Under any condition if SafeTicks <= Stage2Ticks, this
    		* is an overrev condition. 
    		*/
    		
   		    if(SafeTicks <= Stage2Ticks)
       		{
				OutputBuffer = OUTPUT_RED;	// RED - Your Overreving!
   		    }	              

		}          

   		if(!INPUT0)	// Pulled Up, so False is active
   		{                        
   			/*
   			* Press and HOLD Switch must be press for the wait cycle, then it 
   			* will auto advance to the lower brightness level. The RED Output
   			* is on during the Key press so you can check the brightness
   			* of the Lamp. The eeprom_Brightness variable is stored in EEPROM 
   			* so it will come back on with that setting when the device
   			* is next powered up. What a nice thing!
   			*/                                          
    			
   			RepeatCnt++;
    		  
   			if(RepeatCnt == REPEAT_WAIT_CNT)
   			{	    
   				/*
   				* Only adjust the dimmer if Input1 is active, when
   				* not in dimmer mode, just do a Lamp Test
   				*/                                       
  				
   				if(!INPUT1)
   				{
   					if(eeprom_Brightness >= BRIGHT_TABLE_CNT - 1)
   						eeprom_Brightness = 0;
   					else
   						eeprom_Brightness++;
   				}
    				         
  				RepeatCnt=0;
   			}	
  	            
			/*
			* If not dimmer mode, then we can flash all outputs, then
			* cause a reset as soon as the button is released. This is
			* usefull since you can now change the RPM settings, press the
			* button (if not in dimer mode) and reset the controller without
			* having to power down.
			*/
  				
			if(INPUT1) 
			{	
				OutputBuffer = ALL_D_OUTPUT_ON;
	
				// If Brightness OFF, and Switch Pushed, RESET
				// OutputBuffer = OUTPUT_RED;		// Turn On RED for testing...
	            
				while(!INPUT0);	// Wait for switch to be released, or will come up in cyl select state	
				WDTCR = 0x08;	// turn on WDT to shortest time, then let it RESET
				while(1);                                                                         
			}
   		} 
   		else
   		{         
   			// Decay if not pressed
   			
   			if(RepeatCnt)
   				RepeatCnt--;
   		}                             
    		 
   		/*
   		* If Input 1 is active, then use the PWM to dim
   		* the outputs. This input would be typically
   		* attached to the dash lights, any typically
   		* should not be pulled up so when the lights
   		* are off (no 12Volts) the PWM is not used
   		*/

   		if(!INPUT1)
   		{
    		// PWM the outputs by the loop speed and brightness map ratio
    		
   			if(LoopTicks & BrightnessMap[eeprom_Brightness])
   				PORTD = ALL_D_OUTPUT_OFF;   // all outputs off
   			else           
   				PORTD = OutputBuffer | D_INPUT_MASK;		// Turn On bits that need it
   		}
   		else
			PORTD = OutputBuffer | D_INPUT_MASK;			// Turn on 100%
  		
		LoopTicks++;						// General Rolling Loop counter for PWM
	}//while
    	
	FatalError(ERROR_STRAY_CODE);			// Should not ever get here...
}//main
                                                        
