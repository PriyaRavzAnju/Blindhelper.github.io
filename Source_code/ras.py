import serial
import string
import logging
import atexit
import urllib2
import sys
from pprint import pprint
import traceback
import os

def closeAll(serialConnection,logger,hdlr):
    """ to safely close serial ports on exit """
    serialConnection.close()
    logger.info('Serial port closed')
    logger.removeHandler(hdlr)

def find_between( strr, first, last ):
    """ to find the substring between two string s"""
    try:
        start = strr.index( first ) + len( first )
        end = strr.index( last, start )
        return strr[start:end]
    except ValueError:
        return ""   

serialConnection=serial.Serial("/dev/ttyAMA0",9600) #specify the port and the baud rate for the serial connection
#serialConnection.open() #open the serial connection
# print "Opened Serial Connection to serialConnection"
logger = logging.getLogger('RPi2serialConnection') # for logging purposes 
hdlr = logging.FileHandler('/home/pi/Desktop/RPi2serialConnection.log')
formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
hdlr.setFormatter(formatter)
logger.addHandler(hdlr) 
logger.setLevel(logging.INFO)

atexit.register(closeAll, serialConnection, logger, hdlr)
   
while True:
    try:
        print "reading serial"
        reading = serialConnection.readline(); 
        # readings from controller *L8.529595Y76.937810# where L is Latitude, Y is Longitude
        # readings from controller *SLeftRightStraight# where S is String
        print reading
        if reading.index("L") == 1:
        lat = find_between(reading,'L','Y')
        lon = find_between(reading,'Y','#')
        get_url = 'http://smartnodes.in/bh/update_location.php?lat='+lat+'&lng='+lon
        print get_url
        response = urllib2.urlopen(get_url)
        print 'save response : '+ response.read()       
       else:
        direction = find_between(reading,'S','#')
        os.system('echo '+direction+' | festival --tts')
          


    except Exception,e:
        traceback.print_exc()
        pass
serialConnection.close()
