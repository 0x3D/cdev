/*
 * 	startup.c
 *
 */
#define SIMULATOR                                                     
#define GPIO_E ((volatile unsigned int *) 0x40021000      )
#define GPIO_ODR_LOW ((volatile unsigned char *) (0x40021014))
#define GPIO_ODR_HIGH ((volatile unsigned char *) (0x4001015))
#define STK_CTRL ((volatile unsigned int *) 0xE000E010   )
#define STK_LOAD ((volatile unsigned int *) 0xE000E014   )
#define STK_VAL ((volatile unsigned int *) 0xE000E018    )
#define STK_CALIB ((volatile unsigned int *) 0xE000E01C  )
#define B_MASK 0x0001000
 
 
 
 
__attribute__((naked)) __attribute__((section (".start_section")) )
void startup ( void ){
__asm__ volatile(" LDR R0,=0x2001C000\n");		/* set stack */
__asm__ volatile(" MOV SP,R0\n");
__asm__ volatile(" BL main\n");					/* call main */
__asm__ volatile(".L1: B .L1\n");				/* never return */
}

void init_app(){
	*GPIO_E =0x55555555; 
}

void delay_250ns(){
	*STK_CTRL=0;
	*STK_LOAD=42-1;	//6ns per clock and 250 ns. Remove 1 because count from 0
	*STK_VAL=0;
	*STK_CTRL=5;
	while(*STK_CTRL&B_MASK);
	*STK_CTRL=0;
}

void delay_micro(){
	for(int i=0;i<4;i++){
		delay_250ns;
	}
}

void delay_milli(int ms){
	#ifdef	SIMULATOR
	ms = ms/1000;
	ms++;
	#endif
	for(int i=0;i<1000*ms;i++){
		delay_micro();
	}
}

void main(void){
	init_app();
	while(1){
		*GPIO_ODR_LOW=0; //Bargraph 0
		delay_milli(500);
		*GPIO_ODR_LOW=0xff;	//bargraph 0xff
		delay_milli(500);
	}
}


