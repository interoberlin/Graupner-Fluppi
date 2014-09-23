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

volatile bool selected = false;

volatile unsigned char buffer[64];
volatile int length = 0;

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
    digitalWrite(MISO, HIGH);
}

void slaveUnselect()
{
    selected = false;
    digitalWrite(MISO, LOW);
}

void slaveSelectEvent()
{
    if (digitalRead(SS) == HIGH)
        slaveSelect();
    else
        slaveUnselect();
}

void clockTick()
{
    if (selected)
    {
        bool value = digitalRead(MOSI); 
        buffer[length] = value;
        length = (length % 64)+1;
//        digitalWrite(MISO, value);
    }
}

void attachInterruptHandlers()
{
    wiringPiISR(SS, INT_EDGE_BOTH, slaveSelectEvent);
//    wiringPiISR(SCLK, INT_EDGE_RISING, clockTick);
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
