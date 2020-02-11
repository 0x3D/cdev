/*
 * 	startup.c
 *
 */
#define PORT_BASE ((volatile unsigned int *) 0x40021000)
// Definera word-adresser för initieringar
#define portModer ((volatile unsigned int *) 0x40021000)
#define portOtyper ((volatile unsigned short *) 0x40021004)
#define portOspeedr ((volatile unsigned int *) 0x40021008)
#define portPupdr ((volatile unsigned int *) 0x4002100c)
// Definer byte-adresser för data och styrregister
#define portIdrLow ((volatile unsigned char *) 0x40021010)
#define portIdrHigh ((volatile unsigned char *) 0x40021011)
#define portOdrLow ((volatile unsigned char *) 0x40021014)
#define portOdrHigh ((volatile unsigned char *) 0x40021015)
// Masks for bits
#define B_E 0x0040
#define B_SELECT 0x0004
#define B_RW 0x0002
#define B_RS 0x0001
//masks for systick
#define STK_CTRL ((volatile unsigned int *) 0xE000E010   )
#define STK_LOAD ((volatile unsigned int *) 0xE000E014   )
#define STK_VAL ((volatile unsigned int *) 0xE000E018    )
#define STK_CALIB ((volatile unsigned int *) 0xE000E01C  )
#define B_MASK 0x0001000




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



// adressera ASCII-display och ettställ de bitar som är 1 i x
void ascii_ctrl_bit_set( unsigned char x ){
	unsigned char c = *portOdrLow;
	* portOdrLow = (B_SELECT| x | c);
}

// adressera ASCII-display och nollställ de bitar som är 1 i x
void ascii_ctrl_bit_clear( unsigned char x ){
	unsigned char c = *portOdrLow;
	c = (c & ~x);
	* portOdrLow = (B_SELECT | c);
}

void ascii_write_controller(unsigned char command){
	//Delay 40ns
	ascii_ctrl_bit_set(B_E);
	* portOdrHigh = command;
	ascii_ctrl_bit_clear(B_E); //lecture notes places this here
	delay_250ns();
	//Delay 10ns
}

void ascii_write_cmd(unsigned char command){
	ascii_ctrl_bit_clear(B_RS);
	ascii_ctrl_bit_clear(B_RW);
	ascii_write_controller(command);
}

void ascii_write_data(unsigned char data){
	ascii_ctrl_bit_set(B_RS);
	ascii_ctrl_bit_clear(B_RW);
	ascii_write_controller(data);
}

unsigned char ascii_read_controller( void ){
	unsigned char rv;
	ascii_ctrl_bit_set(B_E);
	delay_250ns();
	delay_250ns();
	rv = * portIdrHigh;
	ascii_ctrl_bit_clear(B_E);
	return rv;
}

unsigned char ascii_read_status( void ){
	unsigned char rv;
	* portModer = 0x00005555; // bit 15-8 inputs
	ascii_ctrl_bit_set(B_RW); //order according to lecture
	ascii_ctrl_bit_clear(B_RS);
	rv = ascii_read_controller();
	* portModer = 0x55555555; // all outputs
	return rv;
}

unsigned char ascii_read_data( void ){
	* portModer = 0x00005555; // bit 15-8 inputs
	ascii_ctrl_bit_set(B_RS);
	ascii_ctrl_bit_set(B_RW);
	unsigned char rv = ascii_read_controller();
	* portModer = 0x55555555; // all outputs
	return rv;
}

void ascii_init( void ){
	//2 lines, 5x8 char
	while( (ascii_read_status() & 0x80) == 0x80);
	delay_micro(8);
	ascii_write_cmd(0b00111000);
	delay_micro(39);
	
	//light display, light cursor, constant viewing
	while( (ascii_read_status() & 0x80) == 0x80);
	delay_micro(8);
	ascii_write_cmd(0b00001111);
	delay_micro(39);
	
	//Clear Display
	while( (ascii_read_status() & 0x80) == 0x80);
	delay_micro(8);
	ascii_write_cmd(1);
	delay_milli(2);
	
	//adress using increment, no shift
	while( (ascii_read_status() & 0x80) == 0x80);
	delay_micro(8);
	ascii_write_cmd(0b00000110);
	delay_micro(39);
	
}

void ascii_gotoxy(int x, int y){
	/* gotoxy(row, column) row=[1..20], column = [1,2]
	 * address = row-1
	 * if column = 2; address = adress +0x40
	 * ascii_write_cmd ( 0x80| address)
	 */
	 char address= x-1;
	 if (y==2){
		 address= address + 0x40;
	 }
	 ascii_write_cmd(0x80|address);
}

void ascii_write_char( unsigned char c){
	/* wait until status flag is 0
	 * dealy 8 µs
	 * ascii_write_data(char)
	 * delay 43 µs
	 */
	while( (ascii_read_status() & 0x80) == 0x80);
	delay_micro(8);
	ascii_write_data(c);
	delay_micro(43);
}

void init_app( void ){
	* portModer = 0x55555555; // all outputs
}

void main(void){
	char *s;
	char test1[] = "Alfanumerisk ";
	char test2[] = "Display - test";
	init_app();
	ascii_init();
	ascii_gotoxy(1,1);
	s=test1;
	while(*s){
		ascii_write_char(*s++);
	}
	ascii_gotoxy(1,2);
	s = test2;
	while(*s){
		ascii_write_char(*s++);
	}
	return 0;
}

