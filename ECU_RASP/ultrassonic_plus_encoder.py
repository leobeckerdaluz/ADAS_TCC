#Libraries
import RPi.GPIO as GPIO
import time
 
#GPIO Mode (BOARD / BCM)
GPIO.setmode(GPIO.BCM)

# Constants
AUTOBRAKE_DIST = 7.0 #cm
COLLISION_DIST = 15.0 #cm
VERIFY_INTERVAL = 0.1 #s

#defines
GPIO_TRIGGER = 25
GPIO_ECHO = 16

GPIO_COLLISION = 23
GPIO_AUTOBRAKE = 24

#set GPIO direction (IN / OUT)
GPIO.setup(GPIO_TRIGGER, GPIO.OUT)
GPIO.setup(GPIO_ECHO, GPIO.IN)
GPIO.setup(GPIO_COLLISION, GPIO.OUT)
GPIO.setup(GPIO_AUTOBRAKE, GPIO.OUT)

GPIO_LEFT_ENCODER = 14
COUNT_LIMIT = 20

COUNT_MIN_LIMIT = 4
DIST_4_LIMIT = 8.16814089933

#set GPIO direction (IN / OUT)
GPIO.setup(GPIO_LEFT_ENCODER, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)

setado = False
global_count = 0
StartTime = time.time()
StopTime = time.time()

def my_callback(channel):
    global global_count
    global StartTime
    global StopTime

    global_count += 1

    print (".")

    if(global_count >= COUNT_MIN_LIMIT):
        # Stop time
        StopTime = time.time()
        # Zera contador
        global_count = 0
        # time difference between start and arrival
        TimeElapsed = StopTime - StartTime
        print ("TIME: " + str(TimeElapsed))
        # Calcula o RPM
        RPM = 1/TimeElapsed*60/5
        print ("RPM: " + str(RPM))
        # Calcula velocidade
        speed = DIST_4_LIMIT/TimeElapsed
        print ("Speed: " + str(speed) + " cm/s")
        # Inicia a contagem novamente
        StartTime = time.time()


def distance():
    # set Trigger to HIGH
    GPIO.output(GPIO_TRIGGER, True)
 
    # set Trigger after 0.01ms to LOW
    time.sleep(0.00001)
    GPIO.output(GPIO_TRIGGER, False)
 
    StartTimeInside = time.time()
    StopTimeInside = time.time()
 
    # save StartTime
    while GPIO.input(GPIO_ECHO) == 0:
        StartTimeInside = time.time()
 
    # save time of arrival
    while GPIO.input(GPIO_ECHO) == 1:
        StopTimeInside = time.time()
 
    # time difference between start and arrival
    TimeElapsed = StopTimeInside - StartTimeInside
    # multiply with the sonic speed (34300 cm/s)
    # and divide by 2, because there and back
    distance = (TimeElapsed * 34300) / 2
 
    return distance
 

# when a falling edge is detected on port 17, regardless of whatever   
# else is happening in the program, the function my_callback will be run  
GPIO.add_event_detect(GPIO_LEFT_ENCODER, GPIO.FALLING, callback=my_callback, bouncetime=1) 


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