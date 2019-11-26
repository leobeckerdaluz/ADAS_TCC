# Declara-se as bibliotecas que serão utilizadas no código
if not __debug__:
    import RPi.GPIO as GPIO
import time
import random
from multiprocessing import Lock
# from multiprocessing import Process, Lock
import threading 
import statistics
import sys
import serial

if not __debug__:
    #Configura o uso do terminal serial e da taxa de transmissão de dados
    ser = serial.Serial("/dev/ttyS0", 9600)

ACTIVE_SENSOR0 = True
ACTIVE_SENSOR1 = True
ACTIVE__CAMERA = False

# Distâncias para alteração de estados
AUTOBRAKE_DIST = 15.0 #cm
COLLISION_DIST = 22.0 #cm

# Constantes de amostra
SENSOR_READ_INTERVAL = 0.00002 #s
LEAVE_SAMPLE_INTERVAL = 0.01 #s
PROCESS_SENSOR_DATA_INTERVAL = 0.1 #s

# ID de cada estado
AUTOBRAKE_STATE_ID = 2
PRECOLLISION_STATE_ID = 1
NORMAL_STATE_ID = 0

# Eventos para considerar de fato a mudança de estado
EVENTS_ARRAY_SIZE = 10                  # Tamanho do vetor que armazena os últimos estados
COUNT_AUTOBRAKE_FALSE_ALARM = 3         # Qtde de falsos alarmes permitidos antes de mudar para Autobrake
COUNT_PRECOLLISION_FALSE_ALARM = 2      # Qtde de falsos alarmes permitidos antes de mudar para Collision
COUNT_NORMAL_FALSE_ALARM = 2            # Qtde de falsos alarmes permitidos antes de mudar de volta para Normal

# SENSORS ENUM
SENSOR0_ID = 0
SENSOR1_ID = 1

# Lista com os últimos estados
last_states = [0] * EVENTS_ARRAY_SIZE

# Estado atual do sistema
current_state = 0
current_speed = 0
less_distance = 0

# GPIO pins
START_LED = 18
GPIO_TRIGGER_0 = 23
GPIO_ECHO_0 = 24
GPIO_TRIGGER_1 = 4
GPIO_ECHO_1 = 17

lock_0 = Lock()
lock_1 = Lock()
lock_camera = Lock()

sensor0_distances = []
sensor1_distances = []
camera_distances = []
    
def get_speed_via_serial_thread(ID):
    global current_speed

    while True:
        # print("\nDEU CERTO!\n")
        line = ser.readline()
        # print("\n")
        
        # print(line)
        # substr = line[2:-1]
        current_speed = float(line)
        # print(current_speed)
        
        # print("\n")

def sensor_thread(ID, GPIO_TRIGGER, GPIO_ECHO):
    print(str(ID) + ": Começou a thread! Pins: ->" + str(GPIO_TRIGGER) + " e " + str(GPIO_ECHO))

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
    else:
        #set GPIO direction
        GPIO.setup(GPIO_TRIGGER, GPIO.OUT)
        GPIO.setup(GPIO_ECHO, GPIO.IN)

        while True:
            # Turn on Trigger
            GPIO.output(GPIO_TRIGGER, True)
        
            # set Trigger after 0.01ms to LOW
            time.sleep(SENSOR_READ_INTERVAL)
            
            # Turn off Trigger
            GPIO.output(GPIO_TRIGGER, False)
        
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

def get_median_distances_thread(ID, ACTIVE_SENSOR0, ACTIVE_SENSOR1, ACTIVE__CAMERA): 
    global current_state
    global current_speed
    global less_distance
    print("{}: Processa as distâncias de cada sensor".format(ID))

    i = 0
    while True:
        # -----------------------------
        # Get distances

        if ACTIVE__CAMERA:
            lock_camera.acquire()
            camera_distances_array_len = len(camera_distances)
            if (camera_distances_array_len>0):
                camera_median = statistics.median(camera_distances)
                camera_distances.clear()
                # print("cont camera: {}".format(camera_distances_array_len))
                print("camera: {}".format(camera_median))
            else:
                print("PDI COM PROBLEMAS! cont: {}".format(camera_distances_array_len))
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
    
        # -----------------------------
        # Get the minimum distance
        less_distance = min(sensor0_median, sensor1_median)
    
        # -----------------------------
        # Check Collision
        
        if (less_distance < AUTOBRAKE_DIST):
            # A distância é muito pequena! Logo, verifica se o número de ocorrências 
            # ... foi maior que o limite de alarmes falsos permitidos
            if (last_states.count(PRECOLLISION_STATE_ID)+last_states.count(AUTOBRAKE_STATE_ID) > COUNT_AUTOBRAKE_FALSE_ALARM):
                # Altera o estado para autobrake
                print("AUTOBRAKE!")
                current_state = AUTOBRAKE_STATE_ID
                last_states[i] = AUTOBRAKE_STATE_ID
            else:
                print("++ QUASE AUTOBRAKE")
                # Senão salva esse alarme somente como uma possível colisão
                last_states[i] = PRECOLLISION_STATE_ID

        elif (less_distance < COLLISION_DIST):
            
            # A distância ainda é segura, mas ficar de olho! Logo, verifica se o número de 
            # ... ocorrências foi maior que o limite de alarmes falsos permitidos
            if (last_states.count(PRECOLLISION_STATE_ID) > COUNT_PRECOLLISION_FALSE_ALARM):
                # Altera o estado para autobrake
                print("COLLISION!")
                current_state = PRECOLLISION_STATE_ID
            else:
                print("++ QUASE POSSÍVEL COLISÃO")

            # Se realmente é uma possível colisão ou não, salva esse alarme
            last_states[i] = PRECOLLISION_STATE_ID
                
        else:
            # A distância é segura!! Logo, verifica se o número de ocorrências 
            # ... foi maior que o limite de alarmes falsos permitidos
            if (last_states.count(NORMAL_STATE_ID) > COUNT_NORMAL_FALSE_ALARM):
                # Altera o estado para normal
                print("NORMAL!")
                current_state = NORMAL_STATE_ID
            else:
                print("++ QUASE TUDO NORMAL")
            
            # Se realmente o estado virou normal de fato ou ainda é probabilidade, salva esse alarme
            last_states[i] = NORMAL_STATE_ID


        # Aguarda por um tempo para realizar os cálculos novamente
        time.sleep(PROCESS_SENSOR_DATA_INTERVAL)
        
        # Envia por serial o estado atual
        print("--------------------")
        print("--------------------")
        print("state: " + str(current_state))
        print("speed: " + str(current_speed))
        print("distance: " + str(less_distance))
        print("--------------------")
        print("--------------------")
        if not __debug__:
            ser.write(str.encode(str(current_state) + '_'+str(i)))

        # Inrementa ou zera o iterador do array dos últimos estados
        i = 0 if i>=(EVENTS_ARRAY_SIZE-1) else i+1

if __name__ == '__main__':
    if not __debug__:
        GPIO.cleanup()
        #Configuração inicial dos terminais GPIO
        GPIO.setwarnings(False)
        #GPIO Mode (BOARD / BCM)
        GPIO.setmode(GPIO.BCM)
        GPIO.setup(START_LED, GPIO.OUT)

    # Turn on Start Program Indicator LED
    GPIO.output(START_LED, True)

    if ACTIVE_SENSOR0:
        sensor0_thread = threading.Thread(target=sensor_thread, args=(0, GPIO_TRIGGER_0, GPIO_ECHO_0,))
        sensor0_thread.start()
    
    if ACTIVE_SENSOR1:
        sensor1_thread = threading.Thread(target=sensor_thread, args=(1, GPIO_TRIGGER_1, GPIO_ECHO_1,))
        sensor1_thread.start()
    
    if ACTIVE__CAMERA:
        image_processing_thread = threading.Thread(target=image_processing_thread, args=(3,))
        image_processing_thread.start()

    get_speed = threading.Thread(target=get_speed_via_serial_thread, args=(4,))
    get_speed.start()
    
    print ("----------------------------------------------")
    time.sleep(1)
    
    get_median_distances_thread = threading.Thread(target=get_median_distances_thread, args=(5, ACTIVE_SENSOR0, ACTIVE_SENSOR1, ACTIVE__CAMERA))
    get_median_distances_thread.start()
