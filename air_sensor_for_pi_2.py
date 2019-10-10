# libraries to be installed: Adafruit_Python_DHT, Adafruit_Python_MCP3008
# pins: dht22 -> GPIO21 (#40), mq7 -> channel 0, mq2 -> channel 1 
# to get MCP3008 library:
# [sudo apt-get install git build-essential python-dev
# cd ~
# git clone https://github.com/adafruit/Adafruit_Python_MCP3008.git
# cd Adafruit_Python_MCP3008
# sudo python setup.py install]
#
#
#
#

import time
import Adafruit_GPIO.SPI as SPI
import Adafruit_Python_DHT #TODO
import Adafruit_Python_MCP3008 #TODO

# functions needed
def calculateHeatIndex(humid, temp):
    heatindex = -42.379 + 2.04901523 * temp + 10.14333127 * humid + -0.22475541 * temp*humid + -0.00683783 * pow(temp, 2) + -0.05481717 * pow(humid, 2) + 0.00122874 * pow(temp, 2) * humid + 0.00085282 * temp*pow(humid, 2) + -0.00000199 * pow(temp, 2) * pow(humid, 2)
    
    return heatindex

def calculateMQ7(reading):
    volt = reading / 1024 * 5.0
    gas = (5.0-volt)/volt
    ratio = gas/0.18
    hold = 24.671324557143727/(ratio-0.021209244624595787)
    final = pow(hold, (1/0.7))

    return final

def calculateMQ2(reading):
    volt = reading / 1024 * 5.0
    gas = (5.0-volt)/volt
    ratio = gas/0.40
    hold = 16.177347074757936/(ratio+1.1016286306095602)
    final = pow(hold, (1/0.245))

    return final

# setup ports for use in sensors
SPI_PORT = 0
SPI_DEVICE = 0
mcp = Adafruit_Python_MCP3008.MCP3008(spi=SPI.SpiDev(SPI_PORT, SPI_DEVICE))

dht22 = Adafruit_Python_DHT.DHT22

dhtpin = 21

# read humidity and temperature
humidity, temperature = Adafruit_Python_DHT.read_retry(dht22, dhtpin)

# calculate heat index
if humidity is not None and temperature is not None:
    heatindex = calculateHeatIndex(humidity, temperature)
else:
    print('Failed to read from DHT sensor')

# read mq7 value
mq7value = mcp.read_adc(0)
mq7reading = calculateMQ7(mq7value)

# read mq2 value
mq2value = mcp.read_adc(1)
mq2reading = calculateMQ2(mq2value)

# print values collected to command line
print("Temperature: " + temperature + "C" + "\n")
print("Humidity: " + humidity + "%" + "\n")
print("Heat Index: " + heatindex + "C" + "\n")

print("CO: " + mq7 + "ppm" + "\n")
print("VOCs: " + mq2 + "ppm" + "\n")