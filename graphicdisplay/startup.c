/*
 * 	startup.c
 *
 */
#include  <stdint.h>
#define SIMULATOR

#define LCD_ON 0x3F // Display on
#define LCD_OFF 0x3E // Display off
#define LCD_SET_ADD 0x40 // Set horizontal coordinate
#define LCD_SET_PAGE 0xB8 // Set vertical coordinate
#define LCD_DISP_START 0xC0 // Start address
#define LCD_BUSY 0x80 // Read busy status


#define PORT_BASE ((volatile uint32_t *) 0x40021000)
// Define word adresses for initialization 
#define portModer ((volatile uint32_t *) 0x40021000)
#define portOtyper ((volatile uint16_t *) 0x40021004)
#define portOspeedr ((volatile uint32_t *) 0x40021008)
#define portPupdr ((volatile uint32_t *) 0x4002100c)
// Define byte-addresses for data and control registers
#define portIdrLow ((volatile uint8_t *) 0x40021010)
#define portIdrHigh ((volatile uint8_t *) 0x40021011)
#define portOdrLow ((volatile uint8_t *) 0x40021014)
#define portOdrHigh ((volatile uint8_t *) 0x40021015)
// Masks for Graphic Display
#define B_E 0x0040
#define B_SELECT 0x0004
#define B_RW 0x0002
#define B_RS 0x0001
#define B_RST 0x0020
#define B_CS2 0x0010
#define B_CS1 0x0008
// Port addresses for systick
#define STK_CTRL ((volatile uint32_t *) 0xE000E010   )
#define STK_LOAD ((volatile uint32_t *) 0xE000E014   )
#define STK_VAL ((volatile uint32_t *) 0xE000E018    )
#define STK_CALIB ((volatile uint32_t *) 0xE000E01C  )
// Bit mask for systick
#define B_MASK 0x0001000
//typedef unsigned char uint8_t; using stdint.h
 
 
 
 
__attribute__((naked)) __attribute__((section (".start_section")) )
void startup ( void )
{
__asm__ volatile(" LDR R0,=0x2001C000\n");		/* set stack */
__asm__ volatile(" MOV SP,R0\n");
__asm__ volatile(" BL main\n");					/* call main */
__asm__ volatile(".L1: B .L1\n");				/* never return */
}

void delay_250ns(){
	*STK_CTRL=0;
	*STK_LOAD=42-1;	//6ns per clock and 250 ns. Remove 1 because count from 0
	*STK_VAL=0;
	*STK_CTRL=5;
	while(*STK_CTRL&B_MASK);
	*STK_CTRL=0;
}

void delay_500ns(){
	delay_250ns();
	delay_250ns();
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

//Choose graphic display and set bits that are 1 in x
static void graphic_ctrl_bit_set( uint8_t x){
	uint8_t c = *portOdrLow;
	c &= ~B_SELECT;		//set B_SELECT to 0
	c |= (~B_SELECT & x);	//set bits in x that are 1  to 1 in c, make sure B_SELECT stays 0
	*portOdrLow = c;
}

//choose graphic display and reset bits that are 1 in x
static void graphic_ctrl_bit_clear ( uint8_t x){
	uint8_t c =*portOdrLow;
	c &= ~B_SELECT;	//set B_SELECT to 0
	c &= ~x; //set all that are 1 in x to 0 in c
	*portOdrLow=c;
}

//configure CS-signals
static void select_controller( uint8_t controller ){
	switch ( controller ){
		case 0x0: 
			graphic_ctrl_bit_clear(B_CS1);
			graphic_ctrl_bit_clear(B_CS2);
			break;
		case B_CS1:
			graphic_ctrl_bit_set(B_CS1);
			graphic_ctrl_bit_clear(B_CS2);
			break;
		case B_CS2:
			graphic_ctrl_bit_clear(B_CS1);
			graphic_ctrl_bit_set(B_CS2);
			break;
		case (B_CS1 | B_CS2):
			graphic_ctrl_bit_set(B_CS1);
			graphic_ctrl_bit_set(B_CS2);
			break;
	}
}

static void graphic_wait_ready(void){
	uint8_t c;
	graphic_ctrl_bit_clear(B_E);
	*portModer= 0x00005555; //bit 15-8 inpyts 7-0 outputs
	graphic_ctrl_bit_clear(B_RS);
	graphic_ctrl_bit_set(B_RW);
	delay_500ns();
	
	while(1){
		graphic_ctrl_bit_set(B_E);
		delay_500ns();
		c = * portIdrHigh & LCD_BUSY;
		graphic_ctrl_bit_clear(B_E);
		delay_500ns();
		if(c==0){
			break;
		}
	}
	graphic_ctrl_bit_set(B_E);	//not on lecture but in book
	*portModer= 0x55555555; // all to outputs
}

static uint8_t graphic_read(uint8_t controller){
	uint8_t rv;
	graphic_ctrl_bit_clear(B_E);
	*portModer= 0x00005555; //bit 15-8 inpyts 7-0 outputs;
	graphic_ctrl_bit_set(B_RS); //TODO can do this and next togeter
	graphic_ctrl_bit_set(B_RW);
	select_controller(controller);
	delay_500ns();
	graphic_ctrl_bit_set(B_E);
	delay_500ns();
	rv=*portIdrHigh;
	graphic_ctrl_bit_clear(B_E);
	*portModer= 0x55555555;	//all to outputs
	if(controller == B_CS1){
		select_controller(B_CS1);
		graphic_wait_ready();
	} else if (controller == B_CS2){
		select_controller(B_CS2);
		graphic_wait_ready();
	}
	return rv;
}

static uint8_t graphic_read_data(uint8_t controller){
	graphic_read(controller); //data sheet says first read is nonsense 
	return graphic_read(controller);
}

static void graphic_write(uint8_t value, uint8_t controller){
	*portOdrHigh = value;
	select_controller(controller);
	delay_500ns();
	graphic_ctrl_bit_set(B_E);
	delay_500ns();
	graphic_ctrl_bit_clear(B_E);
	if(controller&B_CS1){
		select_controller(B_CS1);
		graphic_wait_ready();
	} else if ( controller&B_CS2){
		select_controller(B_CS2);
		graphic_wait_ready();
	}
	*portOdrHigh = 0;	//not on lecture but in book
	graphic_ctrl_bit_set(B_E);
	select_controller(0);
}

static void graphic_write_command(uint8_t command, uint8_t controller){
	graphic_ctrl_bit_clear(B_E);
	select_controller(controller);
	graphic_ctrl_bit_clear(B_RS);
	graphic_ctrl_bit_clear(B_RW);
	graphic_write(command, controller);
}

static void graphic_write_data(uint8_t data,uint8_t controller){
	graphic_ctrl_bit_clear(B_E);
	select_controller(controller);
	graphic_ctrl_bit_set(B_RS);
	graphic_ctrl_bit_clear(B_RW);
	graphic_write(data, controller);
}



void init_app( void ){
	*portModer= 0x55555555;	//all to outputs
}

void graphic_initalize( void ){
	graphic_ctrl_bit_set(B_E);
	for(int i=0;i<10;i++){
		delay_micro();
	}
	graphic_ctrl_bit_clear(B_CS1);
	graphic_ctrl_bit_clear(B_CS2);
	graphic_ctrl_bit_clear(B_RST);
	graphic_ctrl_bit_clear(B_E);
	delay_milli(30);
	graphic_ctrl_bit_set(B_RST);
	graphic_write_command(LCD_OFF,B_CS1|B_CS2);
	graphic_write_command(LCD_ON, B_CS1|B_CS2); //Toggle display
	graphic_write_command(LCD_DISP_START,B_CS1|B_CS2); //start=0
	graphic_write_command(LCD_SET_ADD,B_CS1|B_CS2); //add=0
	graphic_write_command(LCD_SET_PAGE,B_CS1|B_CS2); //page=0
	select_controller(0); //Deactivate both CS-Signals
}

void graphic_clear_screen( void ){
	for(int page=0; page<8;page++){
		graphic_write_command(LCD_SET_PAGE | page,B_CS1|B_CS2);
		graphic_write_command(LCD_SET_ADD | 0,B_CS1|B_CS2);
		for(int add=0; add<64;add++){
			graphic_write_data(0,B_CS1|B_CS2);
		}
	}
}

void pixel(uint32_t x, uint32_t y, uint32_t set ){	//TODO: to wide types? as specification.. TODO: use sensible start counting on 0....
	if( (x <1)||(y<1) || (x>128)||(y>64) ){
		return;	//do nothing TODO: would prefer crashing? 
	}
	//Create mask
	uint8_t mask;
	uint32_t index =(y-1)/8;	//irrational y-1..
	switch((y-1)%8){
		case 0:
			mask = 1;
			break;
		case 1:
			mask=2;
			break;
		case 2:
			mask=4;
			break;
		case 3:
			mask=8;
			break;
		case 4:
			mask=0x10;
			break;
		case 5:
			mask=0x20;
			break;
		case 6:
			mask=0x40;
			break;
		case 7:
			mask=0x80;
			break;
	}
	if(set==0){
		mask=~mask; //bitwise invert, bit should be 0!
	}
	//decide physcial coordinates and choose control circuit
	uint8_t controller;
	uint32_t x_fysisk;
	if(x>64){
		controller=B_CS2;
		x_fysisk=x-65;
	} else {
		controller=B_CS1;
		x_fysisk=x-1;	//stupid correction for irrationial input
	}
	
	graphic_write_command(LCD_SET_ADD | x_fysisk, controller);
	graphic_write_command(LCD_SET_PAGE|index, controller);
	uint8_t temp = graphic_read_data(controller);
	graphic_write_command(LCD_SET_ADD|x_fysisk, controller);	//because of auto increment address //TODO investigate
	if(set==1){
		mask = mask|temp;
	} else {
		mask = mask & temp;
	}
	graphic_write_data(mask, controller);
}

void main(void){
	uint32_t i;
	init_app();
	graphic_initalize();
#ifndef SIMULATOR
	graphic_clear_screen();
#endif
	for( i =0; i< 128;i++){	//draw a horizontal line
		pixel(i,10,1);
	}
	for(i=0; i < 64;i++){	//draw a veritcal line
		pixel(10,i,1);
	}
	delay_milli(500);
	for(i=0;i<128;i++){	//erase horizontal line
		pixel(i,10,0);
	}
	for(i=0; i < 64;i++){	//erase veritcal line
		pixel(10,i,0);
	}
}

/*
 * Task 5.14
void main(void){
	init_app();
	graphic_initalize();
#ifndef SIMULATOR
	graphic_clear_screen();
#endif
	graphic_write_command(LCD_SET_ADD|10,B_CS1|B_CS2);
	graphic_write_command(LCD_SET_PAGE| 1, B_CS1|B_CS2);
	graphic_write_data(0xff,B_CS1|B_CS2);
}
 */

