#Libraries
if not __debug__:
    import RPi.GPIO as GPIO
import time
import random
from multiprocessing import Lock
# from multiprocessing import Process, Lock
import threading 
import statistics
import sys

# Constants
AUTOBRAKE_DIST = 12.0 #cm
COLLISION_DIST = 18.0 #cm
SENSOR_READ_INTERVAL = 0.00001 #s
GET_MEDIAN_INTERVAL = 0.05 #s
LEAVE_SAMPLE_INTERVAL = 0.005 #s

# IDs
SENSOR0_ID = 0
SENSOR1_ID = 1
SENSOR2_ID = 2

#set SENSORS Pins
GPIO_TRIGGER_0 = 23
GPIO_ECHO_0 = 24
GPIO_TRIGGER_1 = 4
GPIO_ECHO_1 = 17
GPIO_TRIGGER_2 = 27
GPIO_ECHO_2 = 22
#set OUTPUTS Pins
GPIO_COLLISION = 20
GPIO_AUTOBRAKE = 21

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
            if (ID == SENSOR0_ID):
                dist = random.uniform(5.0, 10.0)
                # print(str(ID) + " dist: " + str(dist))
                lock_0.acquire()
                sensor0_distances.append(dist)
                lock_0.release()
                # print("adicionou 0")
                time.sleep(0.00001)
            elif (ID == SENSOR1_ID):
                dist = random.uniform(10.0, 15.0)
                # print(str(ID) + " dist: " + str(dist))
                lock_1.acquire()
                sensor1_distances.append(dist)
                lock_1.release()
                # print("adicionou 1")
                time.sleep(0.00001)
            elif (ID == SENSOR2_ID):
                dist = random.uniform(15.0, 20.0)
                # print(str(ID) + " dist: " + str(dist))
                lock_2.acquire()
                sensor2_distances.append(dist)
                lock_2.release()
                time.sleep(0.00001)
    else:
        mutex_GPIO_WRITE.acquire()
        #set GPIO direction
        GPIO.setup(GPIO_TRIGGER, GPIO.OUT)
        GPIO.setup(GPIO_ECHO, GPIO.IN)
        mutex_GPIO_WRITE.release()

        while True:
            
            # Turn on Trigger
            mutex_GPIO_WRITE.acquire()
            GPIO.output(GPIO_TRIGGER, True)
            mutex_GPIO_WRITE.release()
        
            # set Trigger after 0.01ms to LOW
            time.sleep(SENSOR_READ_INTERVAL)
            
            # Turn off Trigger
            mutex_GPIO_WRITE.acquire()
            GPIO.output(GPIO_TRIGGER, False)
            mutex_GPIO_WRITE.release()
        
            SampleStartTime = time.time()
            
            # save StartTime
            StartTime = time.time()
            while ((GPIO.input(GPIO_ECHO) == 0) and (time.time() - SampleStartTime < LEAVE_SAMPLE_INTERVAL)):
                StartTime = time.time()
        
            # Desiste da amostra e parte pra outra
            if (time.time() - SampleStartTime > LEAVE_SAMPLE_INTERVAL):
                # print("Sample time maior. DESISTINDO 1!")
                continue

            # save time of arrival
            StopTime = time.time()
            while ((GPIO.input(GPIO_ECHO) == 1) and (time.time() - SampleStartTime < LEAVE_SAMPLE_INTERVAL)):
                StopTime = time.time()

            # Desiste da amostra e parte pra outra
            if (time.time() - SampleStartTime > LEAVE_SAMPLE_INTERVAL):
                # print("Sample time maior. DESISTINDO 2!")
                continue
        
            # SampleStopTime = time.time()
            # SampleTimeElapsed = SampleStopTime - SampleStartTime
            # print("sample time: {}".format(SampleTimeElapsed*1000))

            # time difference between start and arrival
            TimeElapsed = StopTime - StartTime
            # multiply with the sonic speed (34300 cm/s)
            # and divide by 2, because there and back
            distance = (TimeElapsed * 34300) / 2

            if (ID == SENSOR0_ID):
                # print(str(ID) + " dist: " + str(dist))
                lock_0.acquire()
                sensor0_distances.append(distance)
                lock_0.release()
            elif (ID == SENSOR1_ID):
                # print(str(ID) + " dist: " + str(dist))
                lock_1.acquire()
                sensor1_distances.append(distance)
                lock_1.release()
            elif (ID == SENSOR2_ID):
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
        dist = random.uniform(20.0, 25.0)
        lock_camera.acquire()
        camera_distances.append(dist)
        lock_camera.release()
        time.sleep(0.1)

def get_median_distances_thread(ID, ACTIVE_SENSOR0, ACTIVE_SENSOR1, ACTIVE_SENSOR2, ACTIVE__CAMERA): 
    print("{}: Real Distances".format(ID))

    while True:
        if ACTIVE__CAMERA:
            lock_camera.acquire()
            camera_len = len(camera_distances)
            if (camera_len>0):
                camera_median = statistics.median(camera_distances)
                camera_distances.clear()
                # print("cont camera: {}".format(camera_len))
                print("camera: {}".format(camera_median))
            else:
                print("PDI COM PROBLEMAS! cont: {}".format(camera_len))
            lock_camera.release()

        if ACTIVE_SENSOR0:
            lock_0.acquire()
            sensor0_len = len(sensor0_distances)
            if (sensor0_len>0):
                sensor0_median = statistics.median(sensor0_distances)
                sensor0_distances.clear()
                # print("cont sensor0: {}".format(sensor0_len))
                print("sensor0: {}".format(sensor0_median))
            else:
                sensor0_median = 123.44
                print("sensor0 COM PROBLEMAS! cont: {}".format(sensor0_len))
            lock_0.release()
        
        if ACTIVE_SENSOR1:
            lock_1.acquire()
            sensor1_len = len(sensor1_distances)
            if (sensor1_len>0):
                sensor1_median = statistics.median(sensor1_distances)
                sensor1_distances.clear()
                # print("cont sensor1: {}".format(sensor1_len))
                print("sensor1: {}".format(sensor1_median))
            else:
                sensor1_median = 123.44
                print("sensor1 COM PROBLEMAS! cont: {}".format(sensor1_len))
            lock_1.release()
        
        if ACTIVE_SENSOR2:
            lock_2.acquire()
            sensor2_len = len(sensor2_distances)
            if (sensor2_len>0):
                sensor2_median = statistics.median(sensor2_distances)
                sensor2_distances.clear()
                # print("cont sensor2: {}".format(sensor2_len))
                print("sensor2: {}".format(sensor2_median))
            else:
                sensor2_median = 123.44
                print("sensor2 COM PROBLEMAS! cont: {}".format(sensor2_len))
            lock_2.release()
        
        # ---------------
        # Check Collision
        if ((ACTIVE_SENSOR0 and (sensor0_median < AUTOBRAKE_DIST)) or 
            (ACTIVE_SENSOR1 and (sensor1_median < AUTOBRAKE_DIST)) or 
            (ACTIVE_SENSOR2 and (sensor2_median < AUTOBRAKE_DIST)) or
            (ACTIVE__CAMERA and (camera_median < AUTOBRAKE_DIST))):
            
            print("AUTOBRAKE")
            if not __debug__:
                mutex_GPIO_WRITE.acquire()
                GPIO.output(GPIO_AUTOBRAKE, True)
                GPIO.output(GPIO_COLLISION, True)
                mutex_GPIO_WRITE.release()

        elif ( (ACTIVE_SENSOR0 and (sensor0_median < COLLISION_DIST)) or 
            (ACTIVE_SENSOR1 and (sensor1_median < COLLISION_DIST)) or 
            (ACTIVE_SENSOR2 and (sensor2_median < COLLISION_DIST)) or
            (ACTIVE__CAMERA and (camera_median < COLLISION_DIST))):
            
            print("COLLISION")
            if not __debug__:
                mutex_GPIO_WRITE.acquire()
                GPIO.output(GPIO_AUTOBRAKE, False)
                GPIO.output(GPIO_COLLISION, True)
                mutex_GPIO_WRITE.release()
        else:
            print("NORMAL!")
            if not __debug__:
                mutex_GPIO_WRITE.acquire()
                GPIO.output(GPIO_AUTOBRAKE, False)
                GPIO.output(GPIO_COLLISION, False)
                mutex_GPIO_WRITE.release()
        
        time.sleep(GET_MEDIAN_INTERVAL)
        
        # print("-----------------------")

if __name__ == '__main__':
    GPIO.cleanup()

    ACTIVE_SENSOR0 = False
    if (sys.argv[1] == str(1)):
        ACTIVE_SENSOR0 = True
    print ("Status SENSOR0: %s" % (sys.argv[1]))
    print ("Status SENSOR0 ACTIVE: %s" % ACTIVE_SENSOR0)

    ACTIVE_SENSOR1 = False
    if (sys.argv[2] == str(1)):
        ACTIVE_SENSOR1 = True
    print ("Status SENSOR1: %s" % (sys.argv[2]))
    print ("Status SENSOR1 ACTIVE: %s" % ACTIVE_SENSOR1)

    ACTIVE_SENSOR2 = False
    if (sys.argv[3] == str(1)):
        ACTIVE_SENSOR2 = True
    print ("Status SENSOR2: %s" % (sys.argv[3]))
    print ("Status SENSOR2 ACTIVE: %s" % ACTIVE_SENSOR2)

    ACTIVE__CAMERA = False
    if (sys.argv[4] == str(1)):
        ACTIVE__CAMERA = True
    print ("Status CAMERA: %s" % (sys.argv[4]))
    print ("Status CAMERA ACTIVE: %s" % ACTIVE__CAMERA)

    if not __debug__:
        #GPIO Mode (BOARD / BCM)
        GPIO.setmode(GPIO.BCM)
        # Set GPIO direction    
        GPIO.setup(GPIO_COLLISION, GPIO.OUT)
        GPIO.setup(GPIO_AUTOBRAKE, GPIO.OUT)

    if ACTIVE_SENSOR0:
        sensor0_thread = threading.Thread(target=sensor_thread, args=(0, GPIO_TRIGGER_0, GPIO_ECHO_0,))
        # sensor0_thread = Process(target=sensor_thread, args=(0, GPIO_TRIGGER_0, GPIO_ECHO_0,))
        sensor0_thread.start()
        # sensor0_thread.join()
    
    if ACTIVE_SENSOR1:
        sensor1_thread = threading.Thread(target=sensor_thread, args=(1, GPIO_TRIGGER_1, GPIO_ECHO_1,))
        # sensor1_thread = Process(target=sensor_thread, args=(1, GPIO_TRIGGER_1, GPIO_ECHO_1,))
        sensor1_thread.start()
        # sensor1_thread.join()
    
    if ACTIVE_SENSOR2:
        sensor2_thread = threading.Thread(target=sensor_thread, args=(2, GPIO_TRIGGER_2, GPIO_ECHO_2,))
        # sensor2_thread = Process(target=sensor_thread, args=(2, GPIO_TRIGGER_2, GPIO_ECHO_2,))
        sensor2_thread.start()
        # sensor2_thread.join()
    
    if ACTIVE__CAMERA:
        image_processing_thread = threading.Thread(target=image_processing_thread, args=(3,))
        # image_processing_thread = Process(target=image_processing_thread, args=(3,))
        image_processing_thread.start()
        # image_processing_thread.join()

    print ("----------------------------------------------")
    time.sleep(1)
    
    get_median_distances_thread = threading.Thread(target=get_median_distances_thread, args=(4, ACTIVE_SENSOR0, ACTIVE_SENSOR1, ACTIVE_SENSOR2, ACTIVE__CAMERA))
    # get_mean_distances_thread = Process(target=get_mean_distances_thread, args=(4,))
    get_median_distances_thread.start()
    # get_median_distances_thread.join()
