import machine, network, time, urequests
from machine import Pin, I2C
from bme280 import *
import ssd1306

ssid = 'INFINITUM35E5_2.4' 
password = 'Sq3rpq3iNT'
url = "https://api.thingspeak.com/update?api_key=LARGDQ4A7ZX0256G"

red = network.WLAN(network.STA_IF)

red.active(True)
print("Activando red...")
red.connect(ssid, password)
print("Conectando a red...")

while not red.isconnected():
    pass

print('Conexión correcta')
print(red.ifconfig())

ultima_peticion = time.time()
intervalo_peticiones = 15

i2c = I2C(1, sda=Pin(2), scl=Pin(3))
bme = BME280(i2c=i2c)
oled = ssd1306.SSD1306_I2C(128, 64, i2c)  # Inicializar SSD1306 (128x64)

def reconectar():
    print('Fallo de conexión. Reconectando...')
    time.sleep(10)
    machine.reset()

while True:
    try:
        if (time.time() - ultima_peticion) > intervalo_peticiones:
            print("Leyendo datos del sensor BME280...")
            sensor_data = bme.read_compensated_data()
            temp = sensor_data[0] / 100  # Temperatura en °C
            press = sensor_data[1] / 25600  # Presión en hPa
            hum = sensor_data[2] / 1024  # Humedad en %
            print(f"Temperatura: {temp:.2f}°C, Presión: {press:.2f} hPa, Humedad: {hum:.2f}%")

            # Actualizar datos en ThingSpeak
            print("Enviando datos a ThingSpeak...")
            respuesta = urequests.get(url + "&field1=" + str(temp) + "&field2=" + str(press) + "&field3=" + str(hum))
            print("Respuesta de ThingSpeak: " + str(respuesta.status_code))
            respuesta.close()
            
            # Mostrar datos en OLED
            print("Actualizando pantalla OLED...")
            oled.fill(0)
            oled.text("Temp: {:.2f} C".format(temp), 0, 0)
            oled.text("Press: {:.2f} hPa".format(press), 0, 10)
            oled.text("Hum: {:.2f} %".format(hum), 0, 20)
            oled.show()
            
            ultima_peticion = time.time()
    except OSError as e:
        print(f"Error OSError: {e}")
        reconectar()
