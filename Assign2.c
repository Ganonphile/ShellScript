/*	Assignment 2
 *	Written by: Greg Donaldson
 *	Last Modified: 2/22/13
 */

//Some of these includes are probably not needed, artifacts of different
//ideas I had to kill the child process.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <errno.h>

//Some Function prototypes for modular code.

void Command ();
void Seperate (char*);

int main()
{
  Command(); //Yes, we go straight into the modular code. You will never see main() again.

  return 0;
}

void Command ()
{
  int c;
  char mystring[256]; //A variable to hold the entered command.
  int count = 0;      //A counter for the array.
  int i = 0;          //A counter for all the while loops.

  //Declaring what is inside the char array.

  while (i < 256)
    {
      mystring[i] = '\0';
      i++;
    }
  
  printf("?: ");

  //The following loop takes a character, making sure we aren't at the
  //End of file, adds it to the array, then increments the counter.
  //The loop ends when the user hits enter.

  do{
      c=getchar();
      if (c != EOF)
	{
	  c=tolower(c);
	  mystring[count] = c;
	  count++;
	}
      if (c == '\n')
	{
	  mystring[count-1] = '\0';
	}
  }while (c != '\n');

  //Detect if the user typed exit...

  count = strcmp(mystring, "exit");

  //If so, kill the current process.

  if (count == 0)
  {  
    exit(1);
  }

  Seperate(mystring); //Now we seperate everything into individual words.
}

void Seperate (char * mystring)
{
  int count = 0; 		//To increment the array.
  int countw = 0;		//To increment a different array.
  int temp = 0;			//To increment a two dimensional array.
  int child;	                //For the child process.
  char Dest[2][1000];           //Holding the path for the user.
  int i = 0;                    //A counter for while loops.
  int j = 0;                    //A counter for while loops.
  int Bool[2];                  //A simple array designed to hold a false (0) or true (1) value.
  char **strings;

  strings = (char**)(malloc((100)*sizeof(char*))); //Allocating memory.

  for (i = 0; i < 100; i++)
    {
      strings[i] = (char*)(malloc((100)*sizeof(char))); //Allocating memory.
    }

  //Making sure I don't have garbage data in my arrays.

  while (i < 2)
    {
      while (j < 1000)
	{
	  Dest[i][j] = '\0';
	  j++;
	}
	  Bool[i] = 0;
      i++;
    }

  i = 0;  //Reset counter.
  j = 0;  //Reset counter.

  while (i < 100)
    {
      while (j < 100)
	{
	  strings[i][j] = '\0';
	  j++;
	}
      i++;
    }

  i = 0;   //Reset counter.
  j = 0;   //Reset counter.

  //This loop looks through the first array, running till it hits the end of
  //the command entered by the user. Each character is checked, and should a ' '
  //be reached, that word is finished, a \0 is added to the word to indicate the ending,
  //and the two-dimensional array is incremented to the next word. The loop ends when \0
  //is reached.

  //Added into this while loop was a check in case the user tried to enter the path directly.
  //I may fix this later for the user to specify a path more directly, but for now this checks
  // for the / character, and if it finds it, either breaks the loop (if / is followed by a null
  // terminator), or if the program finds the next characters are bi or usr, then it skips ahead
  // in the string.

  while (mystring[count] != '\0')
    {
      if (mystring[count] == '/' && mystring[count+1] == '\0')
	{
	  break;
	}
      else if (mystring[count] == '/')
	{
	  if (mystring[count+1] == 'u' && mystring[count+2] == 's' && mystring[count+3] == 'r')
	    {
	      count+=5;
	    }
	  if (mystring[count+1] == 'b' && mystring[count+2] == 'i')
	    {
	      count+=5;
	    }
	}
      else if (mystring[count] != ' ')
	{
	  strings[countw][temp] = mystring[count];
	  count++;
	  temp++;
	}
      else
	{
	  count++;
	  countw++;
	  temp = 0;
	  while (mystring[count] != '\0')
	    {
	      strings[countw][temp] = mystring[count];
	      count++;
	      temp++;
	    }
	  break;
	}
    }
  countw++;
  strings[countw] = NULL;

  //These just map the most likely locations to find results for linux commands.

  child = fork(); //Split the processes, assigning the id to the child variable.

  //This if statement just checks to see if the shell is the current process. If not,
  //this process is told to wait and the system switches to the child process.

  if (child > 0)
    {
      wait(NULL);
    }

  //Should the child's id ever be smaller than 0, an error has occured. Otherwise, using
  //the seperated command, when we are the child, execute the command. The commands differ
  //in length depending on the amount of commands entered by the user.

  //Added to the check is a call for the global variable errno . If errno is not 0, the
  //the shell informs the user that the command isn't found, and it breaks the loop.
  
  if (child < 0)
    {
      perror("Fork Error!");
      exit(1);
    }
  if (child == 0)
    {
      while (i < 2)
	{
	  execvp(strings[0],strings);
	      if (errno != 0)
		{
		  Bool[i] = 1;
		}
	      i++;
	}
      if (Bool[0] == 1 && Bool[1] == 1)
	{
	  printf("%s", "?: ");
	  printf("%s", strings[0]);
	  printf("%s \n", " command not found");
	}
      exit(1);
    }

  free(strings);
  
  Command(); //And we head back to let the user input more commands.   
}
