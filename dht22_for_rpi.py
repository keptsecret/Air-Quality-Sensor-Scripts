import RPi.GPIO as GPIO 

GPIO.setmode(GPIO.BCM)
GPIO.setup(0, GPIO.IN)

humid, temp = GPIO.input(0)

heatindex = -42.379 +
        2.04901523 * temp +
        10.14333127 * humid +
        -0.22475541 * temp*humid +
        -0.00683783 * pow(temp, 2) +
        -0.05481717 * pow(humid, 2) +
        0.00122874 * pow(temp, 2) * humid +
        0.00085282 * temp*pow(humid, 2) +
        -0.00000199 * pow(temp, 2) * pow(humid, 2)

print(humid + '\n')
print(temp + '\n')
print(heatindex)