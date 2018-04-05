import RPi.GPIO as GPIO
import time, sys
import datetime
import logging
import os
#from subprocess import call

GPIO.setmode(GPIO.BCM)
GPIO.setup(25, GPIO.IN, pull_up_down = GPIO.PUD_UP)

global count
count = 0
event = 0

base_dir = '/sys/bus/w1/devices/'
device_files = []
#tempids = []
#lines = []
#temp_list = []

f = open("/home/pi/CurrDraft/w1-ids.txt",'r')
tempids = f.readlines()
f.close()

i = 0
for i in range(0,2):
    device_files.append(base_dir + "28-" + str(tempids[i][:12]) + "/w1_slave")
    i = i + 1
#print device_files

def read_devices_in(temp_lists):
    lines = []
    #temp_list = []
    for device in device_files:
        f = open(device, 'r')
        lines.append(f.readlines())
        f.close()
    for line in lines:
        if line[0].strip()[-3:] != 'YES':
            continue
        equal_pos = line[1].find('t=')
        if equal_pos != -1:
            temp_lists.append(line[1][equal_pos+2:equal_pos+7])
            #print temp_list
    #print temp_lists


def countPulse():
   #temp_list = []
   #read_devices_in(temp_list)
   #GPIO.remove_event_detect(25)
   ts = time.time()
   std = datetime.datetime.fromtimestamp(ts).strftime('%m-%d-%Y')
   std = "/home/pi/CurrDraft/waterflowlogs/" + std
   stt = datetime.datetime.fromtimestamp(ts).strftime('%H:%M:%S.%f')
   global count
   count = count+1

   logging.basicConfig(format ='%(message)s',filename=std, level=logging.DEBUG)
   logging.info(stt)
   
   for device in device_files:
       #call(["tail",device,"|","grep","-o","\'.....$\'",">>",std])
       os.system("tail -n 1 "+device+" | grep -o '.....$' >> "+std+" &")
   
   print("Time it took: %s" % (time.time() - start_time))
   #print temp_list
   #for temper in temp_list:
   #    logging.info(temper)

   #print st
   #print count
   
##GPIO.add_event_detect(17, GPIO.RISING, callback=gate_moving_callback, bouncetime=500)

#GPIO.add_event_detect(25, GPIO.FALLING, callback=countPulse, bouncetime=500)

#logging.basicConfig(format='%(message)s',filename='waterflow.log',level=logging.DEBUG)
#logging.basicConfig(format='%(asctime)s %(message)s',datefmt='%m/%d/%Y %I:%M:%S %p')
while True:
    try:
        #if(event == 0):
        #    event = 1
        #    GPIO.add_event_detect(25, GPIO.FALLING, callback=countPulse, bouncetime=500)
        GPIO.wait_for_edge(25, GPIO.FALLING)
        time.sleep(0.5)
        start_time = time.time()
        if(GPIO.input(25) == 0):
            #start_time = time.time()
            countPulse()
        #time.sleep(0.5)
        count = 0;
    except KeyboardInterrupt:
    #    print '\ncaught keyboard interrupt!, bye'
        GPIO.cleanup()
        sys.exit()
