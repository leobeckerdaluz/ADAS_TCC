#Libraries
import RPi.GPIO as GPIO
import time
 
#GPIO Mode (BOARD / BCM)
GPIO.setmode(GPIO.BCM)

#defines
VERIFY_INTERVAL = 1 #s
GPIO_LEFT_ENCODER = 4

#set GPIO direction (IN / OUT)
GPIO.setup(GPIO_LEFT_ENCODER, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)

setado = False
global_count = 0

def my_callback(channel):
    global global_count
    global_count += 1

    print (".")

# when a falling edge is detected on port 17, regardless of whatever   
# else is happening in the program, the function my_callback will be run  
GPIO.add_event_detect(GPIO_LEFT_ENCODER, GPIO.FALLING, callback=my_callback, bouncetime=1) 

if __name__ == '__main__':
    try:
        while True:
            print ("Verificando encoder!")
            time.sleep(VERIFY_INTERVAL)
 
        # Reset by pressing CTRL + C
    except KeyboardInterrupt:
        print("Measurement stopped by User")
        GPIO.cleanup()