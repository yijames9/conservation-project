#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <wiringPi.h>

#define FILE_DIR_LENGTH 64
#define ID_LENGTH 12 /*Length of temperature sensor id is 12.*/
//#define NO_DEVICES_A 1
//#define NO_DEVICES_B 1
#define NO_DEVICE_TOT 5 /*Total number of temperature sensors connected.*/
//#define NO_DEVICE_TYPES 2

void sigfun(int sig) /*If program is forcebly cancelled, program will turn relay off.*/
{
  //printf("Writing High\n");
  digitalWrite(4, HIGH);
  exit(1);
}

void initdeviceid(char devices[NO_DEVICE_TOT][FILE_DIR_LENGTH + 2]) /*Reads device ids from a list.*/
//, int count[2]) //POSSIBLY ADD COUNT TO MAKE IT SO THIS FILE GETS EDITTED LESS
{
  int i = 0;
  static const char filename[] = "/home/pi/CurrDraft/w1-ids.txt";
  FILE *file = fopen ( filename, "r" );
  if ( file != NULL )
  {
    char * line = (char*) malloc((ID_LENGTH+2) * sizeof(char)); //Add 2 to size of ID_LENGTH. One for newline and one for null char.
    //Dynamic allocation is used so memory can be freed, since line is only used temporarily.
    while ( fgets ( line , ID_LENGTH+2, file ) != NULL )
    {
      line[ID_LENGTH] = '\0'; //Make sure 2nd to last character is also null char, to get rid of potential junk.
      if(i < NO_DEVICE_TOT) //Check if the number of devices read are in the range of the total number of devices.
      {
        strcat(devices[i], line); //Append read unique device id to the device directory.
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

void relaycontrol(char devices[NO_DEVICE_TOT][FILE_DIR_LENGTH + 2])
//Actually controls the relay.
{
  int i;
  int check;
  int temp;
  
  DIR * dir;
  
  FILE * reading;
  
  //char timearray[16];
  char buf[75];
  char tempC[NO_DEVICE_TOT][6];
  
  //time_t t;
  //struct tm tm;
  
  for(i = 0; i < NO_DEVICE_TOT; i++)// Check if all directories exists.
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
  for(i = 0; i < NO_DEVICE_TOT; i++)// Append device list of directories with w1_slave for future reading of device files.
  {
    strcat(devices[i],"/w1_slave");
  }
  
  while(1)// Relay control loop.
  {
    check = 0; // For checking temperature.
    //t = time(NULL);
    //tm = *localtime(&t);
    //strftime(filename, sizeof(filename), "%m-%d-%Y", &tm);
    //strftime(timearray, sizeof(timearray), "Time: %H:%M:%S\n", &tm);
    for(i = 0; i < NO_DEVICE_TOT; i++) //Store read temperatures in a list.
    {
      reading = fopen(devices[i],"r");
      fread(buf, sizeof(char), 74, reading);
      buf[74] = '\0';
      strncpy(tempC[i], strstr(buf, "t=") + 2, 5);
      tempC[i][5] = '\0';
      //tempC[i][6] = '\0';
      //fputs(tempC, writing);
      buf[0] = '\0';
      fclose(reading);
    }
    for(i = 0; i < NO_DEVICE_TOT; i++)//Takes average of all temperatures.
    {
      temp = atoi(tempC[i])/NO_DEVICE_TOT;
      //printf("Half of Device %d: %d\n",i, temp);
      check += temp;
    }
    //printf("Average: %d\n", check);
    //If temperatures are greater than 10.8 degrees Celcius, then turn the relay on. Otherwise turn the relay off.
    if(check > 10800)
    {
      //printf("Writing Low\n");
      digitalWrite(4, LOW);
    }
    else
    {
      //printf("Writing High\n");
      digitalWrite(4, HIGH);
    }
  }
}

int main(void)
{
  int i;
  char device_ids[NO_DEVICE_TOT][FILE_DIR_LENGTH+2];
  
  wiringPiSetup(); //Sets up wiring pi.
  pinMode(4, OUTPUT); //BCM 24
  (void) signal(SIGINT, sigfun);//Setup signals to catch. Interrupt (like ctrl-c) and terminate (like the kill command).
  (void) signal(SIGTERM, sigfun);
  
  for(i = 0; i < NO_DEVICE_TOT; i++)//Setup device_ids list with base directory for all temperature sensors.
  {
    strcpy(device_ids[i],"/sys/bus/w1/devices/28-");
  }
  
  initdeviceid(device_ids);
  
  relaycontrol(device_ids);

  return 0;
}
