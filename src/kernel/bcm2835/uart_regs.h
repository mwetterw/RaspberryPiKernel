enum uart_regs
{
    DR      = 0x00, // Data Register
    RSRECR  = 0x04, // RX Status Register / Error Clear Register
    FR      = 0x18, // Flag Register
    IBRD    = 0x24, // Integer Baud Rate Divisor
    FBRD    = 0x28, // Fractional Baud Rate Divisor
    LCRH    = 0x2c, // Line Control Register
    CR      = 0x30, // Control Register
    IFLS    = 0x34, // Interrupt FIFO Level Select Register
    IMSC    = 0x38, // Interrupt Mask Set Clear Register
    RIS     = 0x3c, // Raw Interrupt Status Register
    MIS     = 0x40, // Masked Interrupt Status Register
    ICR     = 0x44, // Interrupt Clear Register

    ITCR    = 0x80, // Test Control Register
    ITIP    = 0x84, // Integration Test Input Register
    ITOP    = 0x88, // Integration Test Output Register
    TDR     = 0x8c, // Test Data Register
};

// Data Register
enum DR
{
    DR_OE   = ( 1 << 11 ),  // Overrun Error
    DR_BE   = ( 1 << 10 ),  // Break Error
    DR_PE   = ( 1 << 9 ),   // Parity Error
    DR_FE   = ( 1 << 8 ),   // Framing Error (no valid stop bit)
    DR_DATA = 0xff,         // RX/TX Data Register
};

// RX Status Register / Error Clear Register
enum RSRECR
{
    RSRECR_OE   = ( 1 << 3 ),   // Overrun Error
    RSRECR_BE   = ( 1 << 2 ),   // Break Error
    RSRECR_PE   = ( 1 << 1 ),   // Parity Error
    RSRECR_FE   = ( 1 << 0 ),   // Framing Error
};

// Flag Register
enum FR
{
    FR_TXFE     = ( 1 << 7 ), // TX FIFO Empty
    FR_RXFF     = ( 1 << 6 ), // RX FIFO Full
    FR_TXFF     = ( 1 << 5 ), // TX FIFO Full
    FR_RXFE     = ( 1 << 4 ), // RX FIFO Empty
    FR_BUSY     = ( 1 << 3 ), // Is UART busy transmitting data?
    FR_CTS      = ( 1 << 0 ), // Clear To Send (complement of UART CTS)
};

// Integer Baud Rate Divisor
// BAUDIV = (FUARTCLK/(16*BAUD RATE))
enum IBRD
{
    IBRD_MASK   = 0xf, // Lower 16 bits only
};

// Fractional Baud Rate Divisor
enum FBRD
{
    FBRD_MASK   = 0x3f, // Lower 6 bits only
};

// Line Control Register
enum LCRH
{
    LCRH_SPS        = ( 1 << 7 ),   // Stick Parity Select
    // Word Length (data bits in a frame)
    LCRH_WLEN_SHIFT = 5,
        LCRH_WLEN_5BITS = ( 0 << LCRH_WLEN_SHIFT ),
        LCRH_WLEN_6BITS = ( 1 << LCRH_WLEN_SHIFT ),
        LCRH_WLEN_7BITS = ( 2 << LCRH_WLEN_SHIFT ),
        LCRH_WLEN_8BITS = ( 3 << LCRH_WLEN_SHIFT ),
    LCRH_FEN        = ( 1 << 4 ),   // FIFO Enable
    LCRH_STP2       = ( 1 << 3 ),   // TX 2 stop bits
    LCRH_EPS        = ( 1 << 2 ),   // Even Parity Select
    LCRH_PEN        = ( 1 << 1 ),   // Parity Enable
    LCRH_BRK        = ( 1 << 0 ),   // Send Break
};

// Control Register
enum CR
{
    CR_CTSEN        = ( 1 << 15 ),  // CTS hardware flow control Enable
    CR_RTSEN        = ( 1 << 14 ),  // RTS hardware flow control Enable
    CR_RTS          = ( 1 << 11 ),  // Request To Send (complement of RTS)
    CR_RXE          = ( 1 << 9 ),   // RX Enable
    CR_TXE          = ( 1 << 8 ),   // TX Enable
    CR_LBE          = ( 1 << 7 ),   // Loopback Enable
    CR_UARTEN       = ( 1 << 0 ),   // UART Enable
};

// Interrupt FIFO Level Select
enum IFLS
{
    IFLS_RXIFLSEL_SHIFT = 3,
        IFLS_RXIFLSEL_1_8 = ( 0 << IFLS_RXIFLSEL_SHIFT ), // RX FIFO becomes 1/8 full
        IFLS_RXIFLSEL_1_4 = ( 1 << IFLS_RXIFLSEL_SHIFT ), // RX FIFO becomes 1/4 full
        IFLS_RXIFLSEL_1_2 = ( 2 << IFLS_RXIFLSEL_SHIFT ), // RX FIFO becomes 1/2 full
        IFLS_RXIFLSEL_3_4 = ( 3 << IFLS_RXIFLSEL_SHIFT ), // RX FIFO becomes 3/4 full
        IFLS_RXIFLSEL_7_8 = ( 4 << IFLS_RXIFLSEL_SHIFT ), // RX FIFO becomes 7/8 full

    IFLS_TXIFLSEL_SHIFT = 0,
        IFLS_TXIFLSEL_1_8 = ( 0 << IFLS_TXIFLSEL_SHIFT ), // TX FIFO becomes 1/8 full
        IFLS_TXIFLSEL_1_4 = ( 1 << IFLS_TXIFLSEL_SHIFT ), // TX FIFO becomes 1/4 full
        IFLS_TXIFLSEL_1_2 = ( 2 << IFLS_TXIFLSEL_SHIFT ), // TX FIFO becomes 1/2 full
        IFLS_TXIFLSEL_3_4 = ( 3 << IFLS_TXIFLSEL_SHIFT ), // TX FIFO becomes 3/4 full
        IFLS_TXIFLSEL_7_8 = ( 4 << IFLS_TXIFLSEL_SHIFT ), // TX FIFO becomes 7/8 full
};

// Interrupts (valid for IMSC, RIS, MIS and ICR)
enum INT
{
    INT_OEI     = ( 1 << 10 ),  // Overrun Error Interrupt
    INT_BEI     = ( 1 << 9 ),   // Break Error Interrupt
    INT_PEI     = ( 1 << 8 ),   // Parity Error Interrupt
    INT_FEI     = ( 1 << 7 ),   // Framing Error Interrupt
    INT_RTI     = ( 1 << 6 ),   // RX Timeout Interrupt
    INT_TXI     = ( 1 << 5 ),   // TX Interrupt
    INT_RXI     = ( 1 << 4 ),   // RX Interrupt
    INT_CTSMI   = ( 1 << 1 ),   // CTS Modem Interrupt

    INT_ALL     = 0x7f2,
};
