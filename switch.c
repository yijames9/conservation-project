#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void sigfun(int sig) //If program is forcebly cancelled, it will close the text file to save changes.
{
  printf("Files terminated.\n");
  system("sudo pkill datalogging");
  system("sudo pkill control1");
  //system("sudo pkill control2");
  //system("sudo pkill flowmeterread");
  system("sudo pkill python");
  exit(1);
}

int main (void)
{
  int fileopened = 1;
  wiringPiSetup ();
  //pinMode (0, OUTPUT);
  //pinMode (1, INPUT);
  //digitalWrite (0, LOW);

  //int prevButton = HIGH, LED = 0;
  (void) signal(SIGINT, sigfun);
  (void) signal(SIGTERM, sigfun);
  printf("Files begin.\n");
  system("/home/pi/CurrDraft/datalogging &");// >> /home/pi/CurrDraft/logs/datalogging.log &");
  system("/home/pi/CurrDraft/control1 &");// >> /home/pi/CurrDraft/logs/control1.log &");
  //system("sudo /home/pi/CurrDraft/control2&");// >> /home/pi/CurrDraft/logs/control2.log &");
  //system("sudo /home/pi/CurrDraft/flowmeterread >> /home/pi/CurrDraft/logs/flowmeterread.log &");
  system("python /home/pi/CurrDraft/flowmeterread.py &");// >> /home/pi/CurrDraft/logs/flowmeterread.log &");
  for(;;)
  {
    if(fileopened == 0)// && digitalRead(1) == HIGH)
    {
      printf("Files resume.\n");
      //system("sudo /home/pi/CurrDraft/datalogging >> /home/pi/CurrDraft/logs/datalogging.log &");
      system("/home/pi/CurrDraft/control1 &"); //>> /home/pi/CurrDraft/logs/control1.log &");
      //system("sudo /home/pi/CurrDraft/control2 &"); //>> /home/pi/CurrDraft/logs/control2.log &");
      //system("sudo /home/pi/CurrDraft/flowmeterread >> /home/pi/CurrDraft/logs/flowmeterread.log &");
      //system("sudo python /home/pi/CurrDraft/flowmeterread")
      system("python /home/pi/CurrDraft/flowmeterread.py &"); //>> /home/pi/CurrDraft/logs/flowmeterread.log &");
      fileopened = 1;
    }
    /*else if(fileopened == 1)// && digitalRead(1) == LOW)
    {
      printf("Files paused.\n");
      //system("sudo pkill datalogging");
      system("sudo pkill control1");
      system("sudo pkill control2");
      //system("sudo pkill flowmeterread");
      system("sudo pkill python");
      fileopened = 0;
    }*/
  }
  return 0;
}