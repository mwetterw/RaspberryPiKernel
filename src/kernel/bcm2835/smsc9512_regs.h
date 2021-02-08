#ifndef _H_BCM2835_SMSC9512_REGS
#define _H_BCM2835_SMSC9512_REGS

enum smsc9512_regs
{
    // System Control and Status Registers (0x0000 -> 0x00FF)
    ID_REV          = 0x0000, // Device ID and Revision Register

    INT_STS         = 0x0008, // Interrupt Status Register
    RX_CFG          = 0x000C, // Receive Configuration Register
    TX_CFG          = 0x0010, // Transmit Configuration Register
    HW_CFG          = 0x0014, // Hardware Configuration Register
    RX_FIFO_INF     = 0x0018, // Receive FIFO Information Register
    TX_FIFO_INF     = 0x001C, // Transmit FIFO Information Register
    PMT_CTL         = 0x0020, // Power Management Control Register
    LED_GPIO_CFG    = 0x0024, // LED GPIO Configuration Register
    GPIO_CFG        = 0x0028, // GPIO Configuration Register
    AFC_CFG         = 0x002C, // Auto Flow Control Configuration Register
    E2P_CMD         = 0x0030, // EEPROM Command Register
    E2P_DATA        = 0x0034, // EEPROM Data Register
    BURST_CAP       = 0x0038, // Burst Cap Register (USB TX)

    DP_SEL          = 0x0040, // Data Port Select Register
    DP_CMD          = 0x0044, // Data Port Command Register
    DP_ADDR         = 0x0048, // Data Port Address Register
    DP_DATA0        = 0x004C, // Data Port Data 0 Register
    DP_DATA1        = 0x0050, // Data Port Data 1 Register

    GPIO_WAKE       = 0x0064, // GPIO Wake Enable and Polarity Register
    INT_EP_CTL      = 0x0068, // Interrupt Endpoint Control Register
    BULK_IN_DLY     = 0x006C, // Bulk IN Delay Register


    // MAC Control and Status Registers (0x0100 -> 0x01FC)
    MAC_CR          = 0x0100, // MAC Control Register
    ADDRH           = 0x0104, // MAC Address High Register
    ADDRL           = 0x0108, // MAC Address Low Register
    HASHH           = 0x010C, // Multicast Hash Table High Register
    HASHL           = 0x0110, // Multicast Hash Table Low Register
    MII_ACCESS      = 0x0114, // MII Access Register
    MII_DATA        = 0x0118, // MII Data Register
    FLOW            = 0x011C, // Flow Control Register
    VLAN1           = 0x0120, // VLAN1 Tag Register
    VLAN2           = 0x0124, // VLAN2 Tag Register
    WUFF            = 0x0128, // Wake Up Frame Filter Register
    WUCSR           = 0x012C, // Wake Up Control and Status Register
    COE_CR          = 0x0130, // Checksum Offload Engine Control Register


    // Hub Configuration Registers (0x3000 -> 0x30FF)
};

#endif
