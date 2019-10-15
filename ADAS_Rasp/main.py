#Libraries
if not __debug__:
    import RPi.GPIO as GPIO
import time
import random
from multiprocessing import Lock
# from multiprocessing import Process, Lock
import threading 
# import statistics
import numpy as np

# Constants
AUTOBRAKE_DIST = 7.0 #cm
COLLISION_DIST = 15.0 #cm
VERIFY_INTERVAL = 0.1 #s

#set GPIO Pins
GPIO_TRIGGER_0 = 25
GPIO_ECHO_0 = 16
GPIO_TRIGGER_1 = 26
GPIO_ECHO_1 = 17
GPIO_TRIGGER_2 = 27
GPIO_ECHO_2 = 18
GPIO_COLLISION = 23
GPIO_AUTOBRAKE = 24

mutex_GPIO_WRITE = Lock()
lock_0 = Lock()
lock_1 = Lock()
lock_2 = Lock()
lock_camera = Lock()

sensor0_distances = []
sensor1_distances = []
sensor2_distances = []
camera_distances = []

def sensor_thread(ID, GPIO_TRIGGER, GPIO_ECHO):
    print(str(ID) + ": Chegou " + str(GPIO_TRIGGER) + " e " + str(GPIO_ECHO))

    if __debug__:
        while True:
            if (ID == 0):
                dist = random.uniform(5.0, 10.0)
                # print(str(ID) + " dist: " + str(dist))
                lock_0.acquire()
                sensor0_distances.append(dist)
                lock_0.release()
                # print("adicionou 0")
                time.sleep(0.00001)
            elif (ID == 1):
                dist = random.uniform(10.0, 15.0)
                # print(str(ID) + " dist: " + str(dist))
                lock_1.acquire()
                sensor1_distances.append(dist)
                lock_1.release()
                # print("adicionou 1")
                time.sleep(0.00001)
            elif (ID == 2):
                dist = random.uniform(15.0, 20.0)
                # print(str(ID) + " dist: " + str(dist))
                lock_2.acquire()
                sensor2_distances.append(dist)
                lock_2.release()
                time.sleep(0.00001)
    else:
        with mutex_GPIO_WRITE:
            #set GPIO direction
            GPIO.setup(GPIO_TRIGGER, GPIO.OUT)
            GPIO.setup(GPIO_ECHO, GPIO.IN)

        while True:
            with mutex_GPIO_WRITE:
                # set Trigger to HIGH
                GPIO.output(GPIO_TRIGGER, True)
        
            # set Trigger after 0.01ms to LOW
            time.sleep(0.00001)
            
            with mutex_GPIO_WRITE:
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

            if (ID == 0):
                # print(str(ID) + " dist: " + str(dist))
                lock_0.acquire()
                sensor0_distances.append(distance)
                lock_0.release()
            elif (ID == 1):
                # print(str(ID) + " dist: " + str(dist))
                lock_1.acquire()
                sensor1_distances.append(distance)
                lock_1.release()
            elif (ID == 2):
                # print(str(ID) + " dist: " + str(dist))
                lock_2.acquire()
                sensor2_distances.append(distance)
                lock_2.release()

def image_processing_thread(ID): 
    print(str(ID) + ": Image Processing")
    
    if __debug__:
        while True:
            dist = random.uniform(20.0, 25.0)
            lock_camera.acquire()
            camera_distances.append(dist)
            lock_camera.release()
            # print("dist camera: " + str(camera_dist))
            time.sleep(0.1)
    else:    
        try:
            blablabla_frame = 20
        # Reset by pressing CTRL + C
        except KeyboardInterrupt:
            print("Measurement stopped by User")
            if not __debug__:
                GPIO.cleanup()

def get_median_distances_thread(ID): 
    print("{}: Real Distances".format(ID))

    if __debug__:
        while True:
            lock_camera.acquire()
            camera_len = len(camera_distances)
            # camera_median = statistics.median(camera_distances)
            camera_median = np.median(camera_distances)
            # op = np.median(dataset)
            camera_distances.clear()
            lock_camera.release()
            
            lock_0.acquire()
            sensor0_len = len(sensor0_distances)
            # sensor0_median = statistics.median(sensor0_distances)
            sensor0_median = np.median(sensor0_distances)
            sensor0_distances.clear()
            lock_0.release()
            
            lock_1.acquire()
            sensor1_len = len(sensor1_distances)
            # sensor1_median = statistics.median(sensor1_distances)
            sensor1_median = np.median(sensor1_distances)
            sensor1_distances.clear()
            lock_1.release()
            
            lock_2.acquire()
            sensor2_len = len(sensor2_distances)
            # sensor2_median = statistics.median(sensor2_distances)
            sensor2_median = np.median(sensor2_distances)
            sensor2_distances.clear()
            lock_2.release()
            print("-----------------------")

            if ((sensor0_median < AUTOBRAKE_DIST) or 
                (sensor1_median < AUTOBRAKE_DIST) or 
                (sensor2_median < AUTOBRAKE_DIST) or
                (camera_median < AUTOBRAKE_DIST)):
                
                print("AUTOBRAKE!")
                if not __debug__:
                    with mutex_GPIO_WRITE:
                        GPIO.output(GPIO_AUTOBRAKE, True)
                        GPIO.output(GPIO_COLLISION, True)

            elif ((sensor0_median < COLLISION_DIST) or 
                (sensor1_median < COLLISION_DIST) or 
                (sensor2_median < COLLISION_DIST) or
                (camera_median < COLLISION_DIST)):
                
                print("COLLISION!")
                if not __debug__:
                    with mutex_GPIO_WRITE:
                        GPIO.output(GPIO_AUTOBRAKE, False)
                        GPIO.output(GPIO_COLLISION, True)
            else:
                print("NORMAL!")
                if not __debug__:
                    with mutex_GPIO_WRITE:
                        GPIO.output(GPIO_AUTOBRAKE, False)
                        GPIO.output(GPIO_COLLISION, False)
            
            print("cont camera: {}".format(camera_len))
            print("dist camera: {}".format(camera_median))
            print("cont sensor0: {}".format(sensor0_len))
            print("dist sensor0: {}".format(sensor0_median))
            print("cont sensor1: {}".format(sensor1_len))
            print("dist sensor1: {}".format(sensor1_median))
            print("cont sensor2: {}".format(sensor2_len))
            print("dist sensor2: {}".format(sensor2_median))
            time.sleep(0.5)
    else:    
        blablabla_frame = 20
        if not __debug__:
            GPIO.cleanup()
            # As far as I am aware only the RPi.GPIO and RPIO.GPIO Python modules have a cleanup method. The cleanup method sets all the gpios you have used to be inputs and disables the internal pull-ups/downs for those gpios.

if __name__ == '__main__':
    if not __debug__:
        #GPIO Mode (BOARD / BCM)
        GPIO.setmode(GPIO.BCM)
        # Set GPIO direction    
        GPIO.setup(GPIO_COLLISION, GPIO.OUT)
        GPIO.setup(GPIO_AUTOBRAKE, GPIO.OUT)

    sensor0_thread = threading.Thread(target=sensor_thread, args=(0, GPIO_TRIGGER_0, GPIO_ECHO_0,))
    # sensor0_thread = Process(target=sensor_thread, args=(0, GPIO_TRIGGER_0, GPIO_ECHO_0,))
    sensor0_thread.start()
    
    sensor1_thread = threading.Thread(target=sensor_thread, args=(1, GPIO_TRIGGER_1, GPIO_ECHO_1,))
    # sensor1_thread = Process(target=sensor_thread, args=(1, GPIO_TRIGGER_1, GPIO_ECHO_1,))
    sensor1_thread.start()
    
    sensor2_thread = threading.Thread(target=sensor_thread, args=(2, GPIO_TRIGGER_2, GPIO_ECHO_2,))
    # sensor2_thread = Process(target=sensor_thread, args=(2, GPIO_TRIGGER_2, GPIO_ECHO_2,))
    sensor2_thread.start()
    
    image_processing_thread = threading.Thread(target=image_processing_thread, args=(3,))
    # image_processing_thread = Process(target=image_processing_thread, args=(3,))
    image_processing_thread.start()
    
    get_median_distances_thread = threading.Thread(target=get_median_distances_thread, args=(4,))
    # get_mean_distances_thread = Process(target=get_mean_distances_thread, args=(4,))
    get_median_distances_thread.start()