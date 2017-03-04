enum gpio_regs
{
    GPFSEL0     = 0x00, // GPIO Function Select 0
    GPFSEL1     = 0x04, // GPIO Function Select 1
    GPFSEL2     = 0x08, // GPIO Function Select 2
    GPFSEL3     = 0x0c, // GPIO Function Select 3
    GPFSEL4     = 0x10, // GPIO Function Select 4
    GPFSEL5     = 0x14, // GPIO Function Select 5

    GPSET0      = 0x1c, // GPIO Pin Output Set 0
    GPSET1      = 0x20, // GPIO Pin Output Set 1

    GPCLR0      = 0x28, // GPIO Pin Output Clear 0
    GPCLR1      = 0x2c, // GPIO Pin Output Clear 1

    GPLEV0      = 0x34, // GPIO Pin Level 0
    GPLEV1      = 0x38, // GPIO Pin Level 1

    GPEDS0      = 0x40, // GPIO Pin Event Detect Status 0
    GPEDS1      = 0x44, // GPIO Pin Event Detect Status 1

    GPREN0      = 0x4c, // GPIO Pin Rising Edge Detect Enable 0
    GPREN1      = 0x50, // GPIO Pin Rising Edge Detect Enable 1

    GPFEN0      = 0x58, // GPIO Pin Falling Edge Detect Enable 0
    GPFEN1      = 0x5c, // GPIO Pin Falling Edge Detect Enable 1

    GPHEN0      = 0x64, // GPIO Pin High Detect Enable 0
    GPHEN1      = 0x68, // GPIO Pin High Detect Enable 1

    GPLEN0      = 0x70, // GPIO Pin Low Detect Enable 0
    GPLEN1      = 0x74, // GPIO Pin Low Detect Enable 1

    GPAREN0     = 0x7c, // GPIO Pin Async Rising Edge Detect Enable 0
    GPAREN1     = 0x80, // GPIO Pin Async Rising Edge Detect Enable 1

    GPAFEN0     = 0x88, // GPIO Pin Async Falling Edge Detect Enable 0
    GPAFEN1     = 0x8c, // GPIO Pin Async Falling Edge Detect Enable 1

    GPPUD       = 0x94, // GPIO Pin Pull-Up/Down Enable
    GPPUDCLK0   = 0x98, // GPIO Pin Pull-Up/Down Enable Clock 0
    GPPUDCLK1   = 0x9c, // GPIO Pin Pull-Up/Down Enable Clock 1
};
