/*
 * 	startup.c
 *
 */
 //definitions
#define GPIO_D (volatile unsigned int *) 0x40020C00
#define GPIO_MODER ((volatile unsigned int *) (0x40020c00))
#define GPIO_OTYPER ((volatile unsigned short *) (0x40020c04))
#define GPIO_OSPEEDR ((volatile unsigned int *) (0x40020c08))
#define GPIO_PUPDR ((volatile unsigned int *) (0x40020c0C))
#define GPIO_IDR_LOW ((volatile unsigned char *) (0x40020c10))
#define GPIO_IDR_HIGH ((volatile unsigned char *) (0x40020c11))
#define GPIO_ODR_LOW ((volatile unsigned char *) (0x40020c14))
#define GPIO_ODR_HIGH ((volatile unsigned char *) (0x40020c15))
#define GPIO_BSRR ((volatile unsigned int *) (0x40020c18))
#define GPIO_LCKR ((volatile unsigned short *) (0x40020c1C))
#define GPIO_AFRL ((volatile unsigned int *) (0x40020c20))
#define GPIO_AFRH ((volatile unsigned int *) (0x40020c24))

__attribute__((naked)) __attribute__((section (".start_section")) )
void startup ( void )
{
__asm__ volatile(" LDR R0,=0x2001C000\n");		/* set stack */
__asm__ volatile(" MOV SP,R0\n");
__asm__ volatile(" BL main\n");					/* call main */
__asm__ volatile(".L1: B .L1\n");				/* never return */
}


/*
 * SUMMARY GPIO (Iniz algoritm)
 *	SetPortMODER
 *	if( mode_have_input_pins)
 *		SetPUPDregister
 *	if( mode_has_output_pins)
 *		SetOTYPER
 *		SetOSPEEDR
 *	
 *	 * if( all_port_pins ) // See workbook example 4.3
 *		*GPIO_xxx = ...
 *	else (only_specific_port_pins )
 *	*GPIO &= ~ specific_port_pins //Clear all specific port pins
 *	*GPIO |= port_pins // set according
 */
 
void app_init(void){
	/* * GPIO_D = 0x005555; //port pins b15 to b12 inputs (01) pins b11 to b0 outputs (00)
	*
	* GPIO_OTYPER = 0x0;	//set push pull	
	* GPIO_PUPDR = 0xAAAA0000;	//Set pull up 
	* */
	//Set Port MODER 00 input
	* GPIO_D = 0x55005555; //set pin 11-8 to input rest output
	//Set PUPDR
	* GPIO_PUPDR = 0x00AA0000; //Inputs to push pull
	//Set OTYPER
	* GPIO_OTYPER = 0x0; //All Push pull
	* GPIO_OSPEEDR = 0x55555555; //medium speed(25MHz)
	
}

//help routines

void kbdActivate(unsigned int row){
	switch (row){
		case 0:	//row 1
			*GPIO_ODR_HIGH=0x10;
			break;
		case 1:	//row 2
			*GPIO_ODR_HIGH=0x20;
			break;
		case 2: //row 3
			*GPIO_ODR_HIGH=0x40;
			break;
		case 3:	//row 4
			*GPIO_ODR_HIGH=0x80;
			break;
		default:	//reset all rows
			*GPIO_ODR_HIGH=0x0;
	}
}

int kbdGetCol(){
	unsigned int column=*GPIO_IDR_HIGH;
 	if(column&0x1){
		return 0;
	} else if (column&0x2){
		return 1;
	} else if (column&0x4){
		return 2;
	} else if (column&0x8){
		return 3;
	} else {
		return 4;
	}
}


unsigned char keyb( void){
	unsigned char key[] = {	0x1,0x2,0x3,0xA,
							0x4,0x5,0x6,0xB,
							0x7,0x8,0x9,0xC,
							0xE,0x0,0xF,0xD };
	for(int i=0; i<4;i++){
		kbdActivate(i);
		
		unsigned char col =kbdGetCol();
		kbdActivate(4);		//turn off
		if(col!=4){
			return key[4*i+col];
		}
	}
	return 0xFF;
}

void out7seg( unsigned char c ){
	if(c!=0xff){
		unsigned char segCodes[] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};
		*(volatile unsigned int *) 0x40020C14 = segCodes[c];
	} else {
		* GPIO_ODR_LOW=0x00;
	}
}

//tests
void testGetcol( void ){
	
}



//main
void main(void){
	app_init();
	testGetcol();
	while(1){
		out7seg(keyb());
	}
	return 1;
}


