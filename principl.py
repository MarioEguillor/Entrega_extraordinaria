#Ejemplo para recibir datos por el puerto serie
from datetime import datetime
import time
import serial
import time
import csv
import paho.mqtt.publish as publish
import json

uart_baudrate = 115200 #debe ser = que el configurado en el tx, este caso la EK
valor_pot= 0
# WINDOWS
# uscar en device manager el numero del puerto COM 
uart_serial = serial.Serial('/dev/ttyUSB0',115200, timeout=10,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS)
def write_to_csv(data):
    with open('datosrecibidos.txt', 'a', newline='') as file:
        writer = csv.writer(file)
        writer.writerow([data, get_current_timestamp()])
def get_current_timestamp():
    return datetime.now().strftime("%Y-%m-%d %H:%M:%S")
if uart_serial.is_open:
 while True:
    size = uart_serial.inWaiting()
    if size:
        data = uart_serial.read(size)
 # El dato recibido está en formato de bytes
        received_data_string = data.decode('utf-8')
        print (received_data_string)
        write_to_csv(received_data_string)
        valor_pot = received_data_string
        topic = "pot"
# 2. definir el hostname

        mensaje= {
            "timestamp": get_current_timestamp(),
            "value": valor_pot,
            }
        mensaje_json= json.dumps(mensaje)
        hostname = "grupo8" #hostname de vuestra RPi
        publish.single(topic=topic, payload=mensaje_json, qos=1, hostname=hostname,keepalive=60)
        print("Pub done")
    else:
     print ('no data')
     time.sleep(1)
else:
 print ('Puerto serie no abierto')
# uart_serial.close() # cerrar
 #1. definir el topic 
