#include <stdio.h>
#include <string.h>

void uppg1(){
	for(int i=0; i <10;i++){
		printf("I am an awesome C programmer\n");
	}
}

void uppg2(){
	char c[] = "I am an awesome C programmer";
	for(int i = strlen(c)-1; i>=0; i--){
		printf("%c", c[i]);
	}
	printf("\n");
}

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

int main(int argc, char **argv)
{
	uppg1();
	uppg2();
	uppg3();
	return 0;
}
