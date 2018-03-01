;
; This code was stolen from the original startup
; code that is built with the compiler. It is not
; the same as the code for that was providided in the
; sample startup.asm. 
;
; 11/16/2002 Sandy Ganz
;

      	CLI                              
     	CLR  R30                         
      	OUT  EECR,R30                    
      	OUT  MCUCR,R30                   
                                                   
   ;DISABLE WATCHDOG                               
    	LDI  R31,0x18                    
      	OUT  WDTCR,R31                   
     	LDI  R31,0x10                    
      	OUT  WDTCR,R31                   
                                                   
   ;CLEAR R2-R14                                   
      	LDI  R24,13                      
      	LDI  R26,2                       
    __CLEAR_REG:                                   
     	ST   X+,R30                      
      	DEC  R24                         
      	BRNE __CLEAR_REG                 
                                                   
   ;CLEAR SRAM                                     
      	LDI  R24,LOW(0x80)               
     	LDI  R26,0x60                    
    __CLEAR_SRAM:                                  
      	ST   X+,R30                      
       	DEC  R24                         
      	BRNE __CLEAR_SRAM    
      	
;
; This part removed...
;      	            
   ;GLOBAL VARIABLES INITIALIZATION                
;       	LDI  R30,LOW(__GLOBAL_INI_TBL*2) 
;       	LDI  R31,HIGH(__GLOBAL_INI_TBL*2)
;    __GLOBAL_INI_NEXT:                             
;       	LPM                              
;       	ADIW R30,1                       
;       	MOV  R1,R0                       
;       	LPM                              
;       	ADIW R30,1                       
;       	MOV  R22,R30                     
;       	MOV  R23,R31                     
;       	MOV  R31,R0                      
;       	MOV  R30,R1                      
;       	SBIW R30,0                       
;       	BREQ __GLOBAL_INI_END            
;       	LPM                              
;       	ADIW R30,1                       
;       	MOV  R26,R0                      
;       	LPM                              
;       	ADIW R30,1                       
;       	MOV  R27,R0                      
;       	LPM                              
;       	ADIW R30,1                       
;       	MOV  R24,R0                      
;       	LPM                              
;       	ADIW R30,1                       
;       	MOV  R25,R0                      
;    __GLOBAL_INI_LOOP:                             
;       	LPM                              
;       	ADIW R30,1                       
;       	ST   X+,R0                       
;       	SBIW R24,1                       
;       	BRNE __GLOBAL_INI_LOOP           
;       	MOV  R30,R22                     
;       	MOV  R31,R23                     
;       	RJMP __GLOBAL_INI_NEXT           
;    __GLOBAL_INI_END:
