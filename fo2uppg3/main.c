#include <stdio.h>


/*
char * GetLongestName(char **long_names, int number_of_names)
{
  char * longest_name = 0;
  int most_characters_so_far = 0; 
  // Iterate over names
  for(int i=0; i<number_of_names; i++)
  {
    // Iterate over characters
    int num_characters = 0; 
    while(long_names[i][num_characters] != '\0')
    {
      num_characters += 1; 
    }
    if(num_characters > most_characters_so_far) 
    {
      most_characters_so_far = num_characters;
      longest_name = long_names[i];
    }
  }
  return longest_name;    
}
*/

char* GetLongestName(char** long_names, int number_of_names){
	char* longest_name=0;
	int most_char=0;
	
	for(int i = 0; i < number_of_names;i++){
		int num=0;
		while(long_names[i][num] != 0){
			num=num+1;
		}
		
		if(num > most_char){
			longest_name=long_names[i];
			most_char=num;
		}
	}
	return longest_name;
}



int main(int argc, char **argv)
{
	char *long_names[] = {
    "Alexandrovitj Bulgakowski",
    "Constantine Plumberbatch",
    "Bret",
    "Nathaniel Prescott",
    "Jacqueline Bratwurst"
  };
  int number_of_names = sizeof(long_names)/sizeof(char *);
  char * longest_name = GetLongestName(long_names, number_of_names);
  printf("Longest Name: %s\n", longest_name);
	return 0;
}
