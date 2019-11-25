import threading 
import time

def sensor_processing(string): 
  for num in range(10): 
    time.sleep(2) 
    print("%s: Iteration: %d" %(string, num))

def cam_processing(string): 
  for num in range(20): 
    time.sleep(1) 
    print("%s: Iteration: %d" %(string, num))

thread_1 = threading.Thread(target=sensor_processing, args=("Sensor 1",))
thread_1.start()

thread_2 = threading.Thread(target=cam_processing, args=("frame",))
thread_2.start()

print("entrou aqui")