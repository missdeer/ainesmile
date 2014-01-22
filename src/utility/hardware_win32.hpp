#ifndef _UTILITY_HARDWARE_WIN32_HPP_
#define _UTILITY_HARDWARE_WIN32_HPP_

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
    //Get HardDisk ID
#define FILE_DEVICE_SCSI              0x0000001b
#define IOCTL_SCSI_MINIPORT_IDENTIFY ( ( FILE_DEVICE_SCSI << 16 ) + 0x0501 )
#define IOCTL_SCSI_MINIPORT 0x0004D008 // see NTDDSCSI.H for definition
#define IDENTIFY_BUFFER_SIZE 512
#define SENDIDLENGTH ( sizeof( SENDCMDOUTPARAMS ) + IDENTIFY_BUFFER_SIZE )
#define IDE_ATAPI_IDENTIFY 0xA1 // Returns ID sector for ATAPI.
#define IDE_ATA_IDENTIFY    0xEC // Returns ID sector for ATA.
#define DFP_RECEIVE_DRIVE_DATA   0x0007c088

#define PCI_CARD_IDENTITY   "\\\\?\\PCI"
#define MAC_ADDRESS_LENGTH  6

namespace utility {
    class hardware_win32
    {
    public:
        typedef struct _DRIVERSTATUS
        {
            BYTE bDriverError; // Error code from driver, or 0 if no error.
            BYTE bIDEStatus;    // Contents of IDE Error register.
            // Only valid when bDriverError is SMART_IDE_ERROR.
            BYTE bReserved[2]; // Reserved for future expansion.
            DWORD dwReserved[2]; // Reserved for future expansion.
        } DRIVERSTATUS, *PDRIVERSTATUS, *LPDRIVERSTATUS;

        typedef struct _SENDCMDOUTPARAMS
        {
            DWORD         cBufferSize;   // Size of bBuffer in bytes
            DRIVERSTATUS DriverStatus; // Driver status structure.
            BYTE          bBuffer[1];    // Buffer of arbitrary length in which to store the data read from the                                                       // drive.
        } SENDCMDOUTPARAMS, *PSENDCMDOUTPARAMS, *LPSENDCMDOUTPARAMS;

        typedef struct _IDEREGS
        {
            BYTE bFeaturesReg;       // Used for specifying SMART "commands".
            BYTE bSectorCountReg;    // IDE sector count register
            BYTE bSectorNumberReg;   // IDE sector number register
            BYTE bCylLowReg;         // IDE low order cylinder value
            BYTE bCylHighReg;        // IDE high order cylinder value
            BYTE bDriveHeadReg;      // IDE drive/head register
            BYTE bCommandReg;        // Actual IDE command.
            BYTE bReserved;          // reserved for future use. Must be zero.
        } IDEREGS, *PIDEREGS, *LPIDEREGS;

        typedef struct _SENDCMDINPARAMS
        {
            DWORD     cBufferSize;   // Buffer size in bytes
            IDEREGS   irDriveRegs;   // Structure with drive register values.
            BYTE bDriveNumber;       // Physical drive number to send 
            // command to (0,1,2,3).
            BYTE bReserved[3];       // Reserved for future expansion.
            DWORD     dwReserved[4]; // For future use.
            BYTE      bBuffer[1];    // Input buffer.
        } SENDCMDINPARAMS, *PSENDCMDINPARAMS, *LPSENDCMDINPARAMS;


        typedef struct _GETVERSIONOUTPARAMS
        {
            BYTE bVersion;      // Binary driver version.
            BYTE bRevision;     // Binary driver revision.
            BYTE bReserved;     // Not used.
            BYTE bIDEDeviceMap; // Bit map of IDE devices.
            DWORD fCapabilities; // Bit mask of driver capabilities.
            DWORD dwReserved[4]; // For future use.
        } GETVERSIONOUTPARAMS, *PGETVERSIONOUTPARAMS, *LPGETVERSIONOUTPARAMS;

        typedef struct _SRB_IO_CONTROL
        {
            ULONG HeaderLength;
            UCHAR Signature[8];
            ULONG Timeout;
            ULONG ControlCode;
            ULONG ReturnCode;
            ULONG Length;
        } SRB_IO_CONTROL, *PSRB_IO_CONTROL;

        typedef struct _IDSECTOR
        {
            USHORT wGenConfig;
            USHORT wNumCyls;
            USHORT wReserved;
            USHORT wNumHeads;
            USHORT wBytesPerTrack;
            USHORT wBytesPerSector;
            USHORT wSectorsPerTrack;
            USHORT wVendorUnique[3];
            CHAR    sSerialNumber[20];
            USHORT wBufferType;
            USHORT wBufferSize;
            USHORT wECCSize;
            CHAR    sFirmwareRev[8];
            CHAR    sModelNumber[40];
            USHORT wMoreVendorUnique;
            USHORT wDoubleWordIO;
            USHORT wCapabilities;
            USHORT wReserved1;
            USHORT wPIOTiming;
            USHORT wDMATiming;
            USHORT wBS;
            USHORT wNumCurrentCyls;
            USHORT wNumCurrentHeads;
            USHORT wNumCurrentSectorsPerTrack;
            ULONG   ulCurrentSectorCapacity;
            USHORT wMultSectorStuff;
            ULONG   ulTotalAddressableSectors;
            USHORT wSingleWordDMA;
            USHORT wMultiWordDMA;
            BYTE    bReserved[128];
        } IDSECTOR, *PIDSECTOR;

        hardware_win32(void);
        ~hardware_win32(void);

        static int get_HDD_id(std::string& sHDDID);
        static int get_Mac_address(std::string& sMacAddr);
        static int get_CPU_id(std::string& sCPUID);    
        static int get_root_path_id(std::string& sRootID);
        static int get_CPU_core_count();

    protected:
        static std::string get_disk_information();
        static BOOL get_system_disk_number_info(std::string& sDrive, STORAGE_DEVICE_NUMBER* pDeviceNum);
        static BOOL winnt_HD_serial_number_as_physical_read( BYTE* dwSerial, UINT* puSerialLen, UINT uMaxSerialLen );
        static BOOL do_identify( HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP, PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum, PDWORD lpcbBytesReturned );
        static BOOL winnt_HD_serial_number_as_scsi_read( BYTE* dwSerial, UINT* puSerialLen, UINT uMaxSerialLen );
    };
} // namespace utility
#endif

#endif
