# Air Sensor for RPi v1.00
# Pins list:
# DHT22 ->  GPIO_GEN0 -> GPIO17     #11
# MQ7 ->    GPIO_GEN1 -> GPIO18     #12
# MQ2 ->    GPIO_GEN2 -> GPIO27     #13
# Dust ->  GPIO_GEN3 -> GPIO22     #15

import RPi.GPIO as GPIO
import time as time

# Set up pins on RPi to BCM mode and input
GPIO.setmode(GPIO.BCM)
GPIO.setup(0, GPIO.IN)
GPIO.setup(1, GPIO.IN)
GPIO.setup(2, GPIO.IN)
GPIO.setup(3, GPIO.IN)

const int dht = 0
const int mq7pin = 1
const int mq2pin = 2
const int dustpin = 3 

def readValue(int pin) {
    input = GPIO.input(pin)
    return input
}

def calculateHI(float temp, float humid) {
    # Based on formula for calculating heat index
    hic = -42.379 +
        2.04901523 * temp +
        10.14333127 * humid +
        -0.22475541 * temp*humid +
        -0.00683783 * pow(temp, 2) +
        -0.05481717 * pow(humid, 2) +
        0.00122874 * pow(temp, 2) * humid +
        0.00085282 * temp*pow(humid, 2) +
        -0.00000199 * pow(temp, 2) * pow(humid, 2)

    return hic
}

def calculateMQ7(float reading) {
    volt = reading / 1024 * 5.0
    gas = (5.0-volt)/volt
    ratio = gas/0.18
    hold = 24.671324557143727/(ratio-0.021209244624595787)
    final = pow(hold, (1/0.7))

    return final
}

def calculateMQ2(float reading) {
    volt = reading / 1024 * 5.0
    gas = (5.0-volt)/volt
    ratio = gas/0.40
    hold = 16.177347074757936/(ratio+1.1016286306095602)
    final = pow(hold, (1/0.245))

    return final
}

def readDustValue(int pin) {
    long duration, starttime
    long sampletime_ms = 30000
    long lowpulseoccupancy = 0
    float ratio
    float concentration

    starttime = time.time()
    duration = 
    
}

# Preheat the sensors
GPIO.output(dustpin, GPIO.HIGH)
GPIO.output(mq7pin, GPIO.HIGH)
GPIO.output(mq2pin, GPIO.HIGH)

while (True) {
    
    # Delay 5 minutes
    time.sleep(5*60)
    GPIO.output(dustpin, GPIO.LOW)
    GPIO.output(mq7pin, GPIO.LOW)
    GPIO.output(mq2pin, GPIO.LOW)

    # Take readings of temperature and humidity from DHT22
    humidity, temperature = readValue(dht)

    # Calculate the heat index
    heatindex = calculateHI(temperature, humidity)

    # Take readings from the MQ7 and turn into ppm
    mq7reading = readValue(mq7pin)
    mq7final = calculateMQ7(mq7reading)

    # Take readings from the MQ2 and turn into ppm
    mq2reading = readValue(mq2pin)
    mq2final = calculateMQ2(mq2reading)
}