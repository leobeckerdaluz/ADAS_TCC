#Libraries
import RPi.GPIO as GPIO
import time
 
#GPIO Mode (BOARD / BCM)
GPIO.setmode(GPIO.BCM)

#defines
VERIFY_INTERVAL = 10 #s
GPIO_LEFT_ENCODER = 4

COUNT_LIMIT = 20

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

    if(global_count >= COUNT_LIMIT):
        # Stop time
        StopTime = time.time()
        # Zera contador
        global_count = 0
        # time difference between start and arrival
        TimeElapsed = StopTime - StartTime
        print ("TIME: " + str(TimeElapsed))
        # Calcula o RPM
        RPM = 1/TimeElapsed*60
        print ("RPM: " + str(RPM))
        # Inicia a contagem novamente
        StartTime = time.time()

# when a falling edge is detected on port 17, regardless of whatever   
# else is happening in the program, the function my_callback will be run  
GPIO.add_event_detect(GPIO_LEFT_ENCODER, GPIO.FALLING, callback=my_callback, bouncetime=1) 

if __name__ == '__main__':
    try:
        

        while True:
            print ("ON!")
            time.sleep(VERIFY_INTERVAL)
 
        # Reset by pressing CTRL + C
    except KeyboardInterrupt:
        print("Measurement stopped by User")
        GPIO.cleanup()