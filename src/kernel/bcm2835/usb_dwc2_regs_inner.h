#ifndef _H_BCM2835_DWC2_REGS_INNER
#define _H_BCM2835_DWC2_REGS_INNER

// 0x00008 AHB Configuration Register
union gahbcfg
{
    uint32_t raw;
    struct
    {
        // Note: Broadcom has changed the meaning of hbstlen
        uint32_t glblintrmsk    : 1; // Global Interrupt Mask
        uint32_t hbstlen        : 4; // Burst Length / Type
        uint32_t dmaen          : 1; // DMA Enable
        uint32_t reserved1      : 1;
        uint32_t nptxfemplvl    : 1; // Non-Periodic TX FIFO Empty Level
        uint32_t ptxfemplvl     : 1; // Periodic TX FIFO Empty Level
        uint32_t reserved2      : 12;
        uint32_t remmemsupp     : 1; // Remote Memory Support
        uint32_t notialldmawrit : 1; // Notify all DMA Write Transactions
        uint32_t ahbsingle      : 1; // AHB Single Support
        uint32_t endian         : 1; // Inverse Descriptor Endianness
        uint32_t reserved3      : 7;
    };
};
#define BCM2708_AXI_WAIT ( 1 << 3 )
enum bcm2708_gahbcfg_hbstlen
{
    BCM2708_AXI_BURST_4,
    BCM2708_AXI_BURST_3,
    BCM2708_AXI_BURST_2,
    BCM2708_AXI_BURST_1,
};

// 0x00010 Reset Control Register
union grstctl
{
    uint32_t raw;
    struct
    {
        uint32_t csftrst    : 1; // Core Soft Reset
        uint32_t hsftrst    : 1; // Hclk Soft Reset
        uint32_t frmcntrrst : 1; // Host Frame Counter Reset
        uint32_t intknqflsh : 1; // IN Token Sequence Learning Queue Flush
        uint32_t rxfflsh    : 1; // RX FIFO Flush
        uint32_t txfflsh    : 1; // TX FIFO Flush
        uint32_t txfnum     : 5; // TX FIFO Number
        uint32_t reserved   : 19;
        uint32_t dmareq     : 1; // DMA Request Signal
        uint32_t ahbidle    : 1; // Internal Bus Master Idle
    };
};

// 0x00014 and 18 Interrupt Status & Mask Registers
union gint
{
    uint32_t raw;

    // H = Host only    D = Device only     None = Host & Device
    struct
    {
        uint32_t curmod         : 1; //   Current Mode of Operation (RO)
        uint32_t modemis        : 1; //   Mode Mismatch Interrupt
        uint32_t otgint         : 1; //   OTG Interrupt (RO)
        uint32_t sof            : 1; //   Start of Frame
        uint32_t rxflvl         : 1; //   RX FIFO Non-Empty (RO)
        uint32_t nptxfemp       : 1; //   Non-periodic TX FIFO Empty (RO)
        uint32_t ginnakeff      : 1; // D Global IN Non-periodic NAK Effective (RO)
        uint32_t goutnakeff     : 1; // D Global OUT Non-periodic NAK Effective (RO)
        uint32_t ulpickint      : 1; //   ULPI Carkit Interrupt
        uint32_t i2cint         : 1; //   I²C-compatible Serial Bus Interrupt
        uint32_t earlysusp      : 1; // D Early Suspend
        uint32_t usbsusp        : 1; // D USB Suspend
        uint32_t usbrst         : 1; // D USB Reset
        uint32_t enumdone       : 1; // D Enumeration Done
        uint32_t isooutdrop     : 1; // D Isochronous OUT Packet Dropped Interrupt
        uint32_t eopf           : 1; // D End of Periodic Frame Interrupt
        uint32_t reserved1      : 1;
        uint32_t epmis          : 1; // D Endpoint Mismatch Interrupt
        uint32_t iepint         : 1; // D IN Endpoints Interrupt (RO)
        uint32_t oepint         : 1; // D OUT Endpoints Interrupt (RO)
        uint32_t incompisoin    : 1; // D Incomplete Isochronous IN Transfer
        uint32_t incomplp       : 1; // H Incomplete Periodic Transfer
        uint32_t fetsusp        : 1; // D Data Fetch Suspended
        uint32_t resetdet       : 1; // D Reset Detected
        uint32_t prtint         : 1; // H Host Port Interrupt (RO)
        uint32_t hchint         : 1; // H Host Channels Interrupt (RO)
        uint32_t ptxfemp        : 1; // H Periodic TX FIFO Empty (RO)
        uint32_t reserved2      : 1;
        uint32_t conidstschng   : 1; //   Connector ID Status Change
        uint32_t disconnint     : 1; //   Disconnect Detected Interrupt
        uint32_t sessreqint     : 1; //   Session Request / New Session Detected
        uint32_t wkupint        : 1; //   Resume / Remote Wakeup Detected
    };
};
enum mode { DEVICE_MODE, HOST_MODE }; // For gint.curmod

// 0x00024 Receive FIFO Size Register
union grxf
{
    uint32_t raw;
    struct
    {
        uint16_t siz;
        uint16_t reserved;
    };
};

// 0x00028 & 100 : Non-Periodic / Host Periodic Transmit FIFO Size Registers
union txf
{
    uint32_t raw;
    struct
    {
        uint16_t addr;
        uint16_t siz;
    };
};

// 0x00040 Synopsys ID Register
union gsnpsid
{
    uint32_t raw;
    struct
    {
        uint16_t version;
        uint16_t product;
    };
};

// 0x00048 User Hardware Config2 Register
union ghwcfg2
{
    uint32_t raw;
    struct
    {
        uint32_t otgmode        : 3; // Mode of Operation
        uint32_t otgarch        : 2; // Architecture
        uint32_t singpnt        : 1; // Point to Point
        uint32_t hsphytype      : 2; // High-Speed PHY Interface Type
        uint32_t fsphytype      : 2; // Full-Spped PHY Interface Type
        uint32_t numdeveps      : 4; // Number of Device Endpoints
        uint32_t numhstchnl     : 4; // Number of Host Channels
        uint32_t periosupport   : 1; // Periodic OUT Channels Supported in Host Mode
        uint32_t dynfifosizing  : 1; // Dynamic FIFO Sizing Enabled
        uint32_t reserved1      : 2;
        uint32_t nptxqdepth     : 2; // Non-periodic Request Queue Depth
        uint32_t ptxqdepth      : 2; // Host Mode Periodic Queue Depth
        uint32_t tknqdepth      : 4; // Device Mode IN Token Sequence Learning Queue Depth
        uint32_t reserved2      : 2;
    };
};

enum ghwcfg2_otgmode
{
    // Dual-Role Device (Host & Device)
    OTGMODE_DRD_HNP_SRP,
    OTGMODE_DRD_SRP_ONLY,
    OTGMODE_DRD_NO_HNP_NO_SRP,

    // Device Only
    OTGMODE_DEV_SRP,
    OTGMODE_DEV_NO_SRP,

    // Host Only
    OTGMODE_HOST_SRP,
    OTGMODE_HOST_NO_SRP,

    OTGMODE_RESERVED,
};

// 0x0004C User Hardware Config3 Register
union ghwcfg3
{
    uint32_t raw;
    struct
    {
        uint32_t xfersizewidth  : 4;  // Width of Transfer Size Counters
        uint32_t pktsizewidth   : 3;  // Width of Packet Size Counters
        uint32_t otgen          : 1;  // OTG Function Enabled
        uint32_t i2cintsel      : 1;  // I²C Interface Selection
        uint32_t vndctlsupt     : 1;  // Vendor Control Interface Supported
        uint32_t optfeature     : 1;  // Optionnal Features Supported
        uint32_t rsttype        : 1;  // Reset Type (synchronous)
        uint32_t adpsupport     : 1;  // ADP Logic Support
        uint32_t hsicmode       : 1;  // HSIC Mode Support
        uint32_t bcsupport      : 1;  // Battery Charger Support
        uint32_t lpmmode        : 1;  // LPM Mode Enable/Disabled
        uint32_t dfifodepth     : 16; // Data FIFO Depth (35-bits words)
    };
};

// 0x00400 Host Configuration Register (hcfg)
union hcfg
{
    uint32_t raw;
    struct
    {
        uint32_t fslspclksel    : 2; // FS/LS PHY Clock Select
        uint32_t fslssupp       : 1; // FS- and LS-only Support
        uint32_t reserved1      : 4;
        uint32_t ena32khzs      : 1; // Enable 32 Khz Suspend Mode
        uint32_t resvalid       : 8; // Resume Validation Period
        uint32_t reserved2      : 7;
        uint32_t descdma        : 1; // Enable Scatter/Gather DMA
        uint32_t frlisten       : 2; // Frame List Entries
        uint32_t perschedena    : 1; // Periodic Scheduling Enable
        uint32_t reserved3      : 4;
        uint32_t modechtimen    : 1; // Mode Change Ready Timer Enable
    };
};

// 0x00440 Host Port Control and Status Register
/* XXX Writing this register back to itself will disable the root hub port...
 * This is due to the bad design of this register (mixed bit types).
 * Care must be taken when writing to this reg for all WC bits.
 * WC = Write 1 to Clear */
union hprt
{
    uint32_t raw;
    struct
    {
        uint32_t prtconnsts     : 1; // Port Connect Status (RO)
        uint32_t prtconndet     : 1; // Port Connect Detected (WC)

        /* Is port enabled by the core?
         * This is set only by the core itself after the port reset sequence.
         * The core clears it when an overcurrent or disconnect condition occurs.
         * The driver can also clear it by writing a 1 (XXX this disables port).
         * The driver cannot enable the port using this bit. */
        uint32_t prtena         : 1; // Port is Enabled by the Core (WC)
        uint32_t prtenchng      : 1; // Port En/Dis Change (WC)

        uint32_t prtovrcuract   : 1; // Port Overcurrent Active (RO)
        uint32_t prtovrcurchng  : 1; // Port Overcurrent Change (WC)

        uint32_t prtres         : 1; // Port Resume
        uint32_t prtsusp        : 1; // Port Suspend

        uint32_t prtrst         : 1; // Port Reset
        uint32_t reserved1      : 1;
        uint32_t prtlnsts       : 2; // Port Line Status, D+/D- status (RO)
        uint32_t prtpwr         : 1; // Port Power (to enable/disable the port)
        uint32_t prttstctl      : 4; // Port Test Control
        uint32_t prtspd         : 2; // Port Speed {HS, FS, LS, Reserved} (RO)
        uint32_t reserved2      : 13;
    };
};
enum hprt_prtspd
{
    HPRT_PRTSPD_HS,
    HPRT_PRTSPD_FS,
    HPRT_PRTSPD_LS,
    HPRT_PRTSPD_RESERVED,
};
#define DWC2_HPRT_WC_MASK (0b101110)

// 0x00500 Host Channel Characteristics Register
union hcchar
{
    uint32_t raw;
    struct
    {
        uint32_t mps            : 11; // Maximum Packet Size
        uint32_t epnum          :  4; // Endpoint Number
        uint32_t epdir          :  1; // Endpoint Direction (IN/OUT)
        uint32_t reserved       :  1;
        uint32_t lspddev        :  1; // Low-Speed Device
        uint32_t eptype         :  2; // Endpoint Type
        uint32_t mcec           :  2; // Multi Count / Error Count
        uint32_t devaddr        :  7; // Device Address
        uint32_t oddfrm         :  1; // Odd Frame
        uint32_t chdis          :  1; // Channel Disable (WS SC SS)
        uint32_t chena          :  1; // Channel Enable (WS SC)
    };
};
enum hcchar_epdir
{
    HCCHAR_EPDIR_OUT,
    HCCHAR_EPDIR_IN,
};
enum hcchar_eptype
{
    HCCHAR_EPTYPE_CTRL,
    HCCHAR_EPTYPE_ISOC,
    HCCHAR_EPTYPE_BLK,
    HCCHAR_EPTYPE_IRQ,
};

// 0x00508 Host Channel Interrupt and 0x0050C Interrupt Mask Register
union hcint
{
    uint32_t raw;
    struct
    {
        uint32_t xfercompl      :  1; // Transfer Completed
        uint32_t chhltd         :  1; // Channel Halted
        uint32_t ahberr         :  1; // AHB Error
        uint32_t stall          :  1; // STALL Response Received
        uint32_t nak            :  1; // NAK Response Received
        uint32_t ack            :  1; // ACK Response Received
        uint32_t nyet           :  1; // NYET Reponse Received
        uint32_t xacterr        :  1; // Transaction Error
        uint32_t bblerr         :  1; // Babble Error
        uint32_t frmovrun       :  1; // Frame Overrun
        uint32_t datatglerr     :  1; // Data Toggle Error
        uint32_t bna            :  1; // Buffer Not Available
        uint32_t xcsxacterr     :  1; // Excessive Transaction Error
        uint32_t desclstroll    :  1; // Descriptor Rollover
        uint32_t reserved       : 18;
    };
};

// 0x00510 Host Channel Transfer Size Register
union hctsiz
{
    uint32_t raw;
    struct
    {
        uint32_t xfersize       : 19; // Transfer Size
        uint32_t pktcnt         : 10; // Packet Count
        uint32_t pid            :  2; // PID (Packet ID)
        uint32_t dopng          :  1; // Do Ping
    };
};
enum hctsiz_pid
{
    HCTSIZ_PID_DATA0,
    HCTSIZ_PID_DATA2,
    HCTSIZ_PID_DATA1,
    HCTSIZ_PID_MDATA_SETUP,
};
#endif
