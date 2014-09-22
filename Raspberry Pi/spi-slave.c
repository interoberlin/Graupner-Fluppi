/*
 * Rasperry Pi SPI slave
 *
 * SS: Slave select; low, when this slave is talked to
 * SCLK: SPI data pulse clock; high, when a bit can be read on MOSI pin
 * MOSI: Master out slave in
 * MISO: Master in slave out
 *
 * Published under the terms and conditions of the GNU GPL v3.
 */

#include <stdio.h>
#include <stdbool.h>
#include <wiringPi.h>

#define SS   23
#define SCLK 22
#define MOSI 17 // our input 
#define MISO 27  // our output

bool selected = false;

unsigned char buffer[64];
int length = 0;

void initGPIO()
{
    wiringPiSetupGpio();

    pinMode(SS, INPUT);
    pinMode(SCLK, INPUT);
    pinMode(MOSI, INPUT);
    pinMode(MISO, OUTPUT);

    digitalWrite(MISO, LOW);
}

void slaveSelect()
{
    selected = true;
}

bool disableSlaveUnselectISR = false;

void clockTick()
{
    if (selected)
    {
        disableSlaveUnselectISR = true;
        buffer[length] = digitalRead(MOSI);
        length = (length % 64)+1;
        disableSlaveUnselectISR = false;
    }
}

void slaveUnselect()
{
    selected = false;

    if (!disableSlaveUnselectISR)
    {
        disableSlaveUnselectISR = true;
        if (!selected)
        {
            digitalWrite(MISO, LOW);
        } else {
            digitalWrite(MISO, HIGH);
        }
        disableSlaveUnselectISR = false;
    }
}

void attachInterruptHandlers()
{
    wiringPiISR(SS, INT_EDGE_FALLING, slaveSelect);
    wiringPiISR(SS, INT_EDGE_RISING, slaveUnselect);
    wiringPiISR(SCLK, INT_EDGE_RISING, clockTick);
}
 
int main()
{
    initGPIO();
    attachInterruptHandlers();
    while (1)
    {
        delay(3000);
    }
}
