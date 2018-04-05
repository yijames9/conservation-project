#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
//#include <signal.h>

#define FILE_DIR_LENGTH 64
#define ID_LENGTH 12 /*length of id is 12*/
//#define NO_DEVICES_A 1
//#define NO_DEVICES_B 1
//#define NO_DEVICE_TOT 2
#define NO_DEVICE_TOT 5



void initdeviceid(char devices[NO_DEVICE_TOT][FILE_DIR_LENGTH + 2])//, int count[2]) //POSSIBLY ADD COUNT TO MAKE IT SO THIS FILE GETS EDITTED LESS
{
  int i = 0;
  static const char filename[] = "/home/pi/CurrDraft/w1-ids.txt"; //w1-ids; test.txt 
  FILE *file = fopen ( filename, "r" );
  if ( file != NULL )
  {
    char * line = (char*) malloc((ID_LENGTH+2) * sizeof(char)); //dynamic so memory can be freed
    while ( fgets ( line , ID_LENGTH+2, file ) != NULL ) //add one for newline and one for null char
    {
      line[ID_LENGTH] = '\0'; //set 2nd to last character to also be null char
      if(i < NO_DEVICE_TOT) 
      {
        strcat(devices[i], line);   
        i++;
      }
      else break;
    }
    fclose(file);
    free(line);
  }
  else
  {
    perror ( filename );
  }
  
}

void printdevicereading(char devices[NO_DEVICE_TOT][FILE_DIR_LENGTH + 2])
{
  int i;
  
  DIR * dir;
  
  FILE * reading;
  FILE * writing;
  
  char timearray[16];
  char buf[75];
  char tempC[7];
  char filename[64];
  char temptime[11];
  
  time_t t;
  struct tm tm;
  
  for(i = 0; i < NO_DEVICE_TOT; i++)
  {
    dir = opendir(devices[i]);
    if(dir == NULL)
    {
      perror("Couldn't open the w1 device directory\n");
    }
    else
    {
      printf("Device %d exists\n", i);
    }
  }
  for(i = 0; i < NO_DEVICE_TOT; i++)
  {
    strcat(devices[i],"/w1_slave");
  }
  
  while(1)
  {
    t = time(NULL);
    tm = *localtime(&t);
    
    strftime(temptime, sizeof(temptime), "%m-%d-%Y", &tm);
    //printf("before segfault\n");
    strcpy(filename, "/home/pi/CurrDraft/templogs/");
    strcat(filename, temptime);
    strcat(filename, ".txt");
    writing = fopen(filename,"a+");
    strftime(timearray, sizeof(timearray), "Time: %H:%M:%S\n", &tm);
    fputs(timearray,writing);
    for(i = 0; i < NO_DEVICE_TOT; i++)
    {
      reading = fopen(devices[i],"r");
      fread(buf, sizeof(char), 74, reading);
      buf[74] = '\0';
      strncpy(tempC, strstr(buf, "t=") + 2, 5);
      tempC[5] = '\n';
      tempC[6] = '\0';
      fputs(tempC, writing);
      buf[0] = '\0';
      fclose(reading);
    }
    //printf("before segfault\n");
    fclose(writing);
    //printf("before segfault\n");
  }
}

int main(void)
{
  int i;
  char device_ids[NO_DEVICE_TOT][FILE_DIR_LENGTH+2];
  
  for(i = 0; i < NO_DEVICE_TOT; i++)
  {
    strcpy(device_ids[i],"/sys/bus/w1/devices/");
    strcat(device_ids[i],"28-");
  }
  
  initdeviceid(device_ids);
  
  printdevicereading(device_ids);

  return 0;
}
