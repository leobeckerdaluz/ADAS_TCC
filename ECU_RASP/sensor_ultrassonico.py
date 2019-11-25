#Libraries
import RPi.GPIO as GPIO
import time
 
#GPIO Mode (BOARD / BCM)
GPIO.setmode(GPIO.BCM)

# Constants
AUTOBRAKE_DIST = 7.0 #cm
COLLISION_DIST = 15.0 #cm
VERIFY_INTERVAL = 0.1 #s

#set GPIO Pins
GPIO_TRIGGER = 25
GPIO_ECHO = 16

GPIO_COLLISION = 23
GPIO_AUTOBRAKE = 24

#set GPIO direction (IN / OUT)
GPIO.setup(GPIO_TRIGGER, GPIO.OUT)
GPIO.setup(GPIO_ECHO, GPIO.IN)
GPIO.setup(GPIO_COLLISION, GPIO.OUT)
GPIO.setup(GPIO_AUTOBRAKE, GPIO.OUT)

def distance():
    # set Trigger to HIGH
    GPIO.output(GPIO_TRIGGER, True)
 
    # set Trigger after 0.01ms to LOW
    time.sleep(0.00001)
    GPIO.output(GPIO_TRIGGER, False)
 
    StartTime = time.time()
    StopTime = time.time()
 
    # save StartTime
    while GPIO.input(GPIO_ECHO) == 0:
        StartTime = time.time()
 
    # save time of arrival
    while GPIO.input(GPIO_ECHO) == 1:
        StopTime = time.time()
 
    # time difference between start and arrival
    TimeElapsed = StopTime - StartTime
    # multiply with the sonic speed (34300 cm/s)
    # and divide by 2, because there and back
    distance = (TimeElapsed * 34300) / 2
 
    return distance
 
if __name__ == '__main__':
    try:
        while True:
            dist = distance()

            if(dist < AUTOBRAKE_DIST):
                print("AUTOBRAKE!")
                GPIO.output(GPIO_AUTOBRAKE, True)
                GPIO.output(GPIO_COLLISION, True)
            elif (dist < COLLISION_DIST):
                print("COLLISION!")
                GPIO.output(GPIO_AUTOBRAKE, False)
                GPIO.output(GPIO_COLLISION, True)
            else:
                print("NORMAL!")
                GPIO.output(GPIO_AUTOBRAKE, False)
                GPIO.output(GPIO_COLLISION, False)

            print ("Measured Distance = %.1f cm" % dist)
            time.sleep(VERIFY_INTERVAL)
 
        # Reset by pressing CTRL + C
    except KeyboardInterrupt:
        print("Measurement stopped by User")
        GPIO.cleanup()