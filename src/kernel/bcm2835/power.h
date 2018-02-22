#ifndef _H_BCM2835_POWER
#define _H_BCM2835_POWER

enum pm_device
{
    POWER_SDCARD,
    POWER_UART0,
    POWER_UART1,
    POWER_USB_HCD,
    POWER_I2C0,
    POWER_I2C1,
    POWER_I2C2,
    POWER_SPI,
    POWER_CCP2TX,
};

enum pm_state
{
    POWER_OFF,
    POWER_ON,
};

void power_init ( );
int power_device ( enum pm_device, enum pm_state );

#endif
