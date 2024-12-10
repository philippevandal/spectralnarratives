#to enable SPI 
#sudo raspi-config nonint do_spi 0

from gpiozero import MCP3008
import RPi.GPIO as GPIO
GPIO.setmode(GPIO.BCM)

vaporiser = MCP3008(0) #channel 1 for ADC

def iterationMS():
    return

while True:
    if vaporiser > 40:
        iterationMS()