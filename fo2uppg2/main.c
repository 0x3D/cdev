#include <stdio.h>

char* GetSecondString(char string[]){
	//int length = sizeof(*string)/sizeof(string[0]);			Dont work in c, pointer! 
	for(int i=0; string[i] != 0; i++){
		if(string[i]=='|'){
			return &string[i+1];
		}
	}
	return (char*) -1;	//crash if wrong
}

int main(int argc, char **argv)
{
	char string[] = "This is the first string | This is the second string.";
	printf("The second string is: %s ", GetSecondString(string));
	return 0;
}
