#include <stdio.h>
#include <windows.h>


void printBinaryByte(char c);
void printBinaryInt(int in);
void printBinaryShort(short s);

void uppg3(){
	printBinaryByte(9);
	printBinaryShort(9);
	printBinaryInt(9);
}

void printBinaryInt(int in){
	for(int i = 31; i >=0; i--){
		int bit=(in>>i)&0x01;
		printf("%d", bit);
	}
	printf("\n");
}

void printBinaryShort(short s){	
	for(int i = 15; i >=0; i--){
		int bit=(s>>i)&0x01;
		printf("%d", bit);
	}
	printf("\n");
}
void printBinaryByte(char c){
	for(int i = 7; i >=0; i--){
		int bit=(c>>i)&0x01;
		printf("%d", bit);
	}
	printf("\n");
}

void binaryStringToDec(char string[]){
	int newDec=0;
	for (int i=0;i<8;i++){
		if(string[i]=='1'){
			newDec=newDec | (1<<(7-i));
		}
	}
	printf("%d\n", newDec);
}

void clearScreen(){
	for(int i=0; i<80;i++){
		printf("\n");
	}
}

void printPattern(){
	unsigned short left = 0x8000;
	unsigned short right = 0x01;
	
	unsigned int total = ((left<<16) | (right&0xffff));
	unsigned int startpos = 35;
	unsigned int scrollwidth = 60;
	
	
	for (int time=0; time < 240;time++){
		int scrollitr=time % (2*scrollwidth);
		int pos=startpos+scrollwidth-abs(scrollwidth - scrollitr);

		if(time%2 != 0){
			if(right!=0){
				right=right << 1;
			} else {
				right=0x01;
			}
			
			if(left!=0){
				left=left >> 1;
			} else {
				left = 0x8000;
			}
		}
		Sleep(250);
		clearScreen();
		for(int i = 0; i <= pos; i++){
			printf(" ");
		}
		total = ((left<<16) | (right&0xffff));
		printBinaryInt(total);
		int randomjump = pos / 10;
		for(int i = 0; i <randomjump;i++){
			printf("\n");
		}
	}
	
}


int main(int argc, char **argv)
{
	/*
	*char str[] = "00001001";
	*binaryStringToDec(str);
	*unsigned char c = 128;
	*printBinaryByte(c);
	*c=c|0b00101010;
	*printBinaryByte(c);
	*printf("%d\n",c);
	*
	*int tal = 97;
	*printf("%x\n", tal);
	*
	*signed char x = 27;
	*x=x^0xFF;	//1 komplement
	*x++;	//+1 blir 2 komplement
	*printf("XOR \n");
	*printf("%d \n", x);
	*
	*/
	clearScreen();
	printPattern();
	//uppg3();
	return 0;
}
