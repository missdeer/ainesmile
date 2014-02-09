#include "stdafx.h"

#if ((defined(WIN32) || defined(_WIN32) || defined(__WIN32__)) || defined(_WIN32) || defined(__WIN32__))
#include <cstdio>
#include <devguid.h>
#include <regstr.h>
#include "util.hpp"
#include "hardware_win32.hpp"

namespace utility {

#define GETADDRESS_RETRY_TIME 3

    static int __getEnableMacAddressWin32(unsigned char* pMacAddr)
    {
        int nRet = -1;
        int nAdapters = 0;

        ULONG outBufLen = 0;
        DWORD dwRetVal = 0;
        PIP_ADAPTER_ADDRESSES pAddresses = NULL;

        for (int retry = 0; retry < GETADDRESS_RETRY_TIME; retry++)
        {
            for (int i = 0; i < 5; i++) 
            {
                dwRetVal =
                    GetAdaptersAddresses(
                    AF_INET, 
                    0,
                    NULL, 
                    pAddresses, 
                    &outBufLen);

                if (dwRetVal != ERROR_BUFFER_OVERFLOW) 
                {
                    break;
                }

                if (pAddresses != NULL) 
                {
                    HeapFree(GetProcessHeap(), 0, (pAddresses));
                }

                pAddresses = (PIP_ADAPTER_ADDRESSES) 
                    HeapAlloc(GetProcessHeap(), 0, (outBufLen));
                if (pAddresses == NULL) 
                {
                    dwRetVal = GetLastError();
                    break;
                }
            }
            if (dwRetVal == 0)
            {
                break;
            }
        }
        if (dwRetVal != 0)
        {
            return nRet;
        }

        while (pAddresses) {             // Terminate if last adapter
            nAdapters++;
            //printf("Friendly Name: %ws    Index: %d    Type:%d\n", pAddresses->FriendlyName, pAddresses->IfIndex, pAddresses->IfType);
            if (pAddresses->IfType == MIB_IF_TYPE_ETHERNET)
            {
                //confirm that the adapter is persistent connection, not WIFI/3G/USB card (NOT STRICTLY)
                if (0 == (WORD)((pAddresses->IfIndex >> 16) & 0xFFFF))
                {
                    for (int i = 0; i < 6; i++) 
                    {
                        pMacAddr[i] = pAddresses->PhysicalAddress[i];
                    }
                    nRet = 6; 
                    break;
                }
            }
            pAddresses = pAddresses->Next;    // Progress through linked list
        }

        return nRet;
    }

    hardware_win32::hardware_win32(void)
    {
    }

    hardware_win32::~hardware_win32(void)
    {
    }

    int hardware_win32::get_HDD_id( std::string& sHDDID )
    {
        sHDDID = get_disk_information();
        return 0;
    }

    int hardware_win32::get_Mac_address( std::string& sMacAddr )
    {
        int ret = 0;
        unsigned char tmpMacAddressBin[MAC_ADDRESS_LENGTH] = {0};
        ret =  __getEnableMacAddressWin32(tmpMacAddressBin);
        if (ret != -1)
        {
            utility::utilities::bin_to_hex(tmpMacAddressBin, MAC_ADDRESS_LENGTH, sMacAddr);
        }
        return ret;
    }

    int hardware_win32::get_CPU_id( std::string& sCPUID )
    {
        char szTmp[40] = {0};
        char szTmp2[40] = {0};
        unsigned long s1 = 0,s2=0;
#ifdef _MSC_VER
#if defined(_WIN64)
#else
        _asm
        {       
            mov     eax,2       
                CPUID       
        }       
        _asm
        {       
            mov     eax,01h
                xor     edx,edx       
                cpuid       
                mov     s1,edx       
                mov     s2,eax       
        }
#endif
#else
        __asm__
            __volatile__
            (
            "movl $1, %%eax\n"
            "xor %%edx,%%edx\n"
            "cpuid\n"
            "movl %%edx, %0\n"
            "movl %%eax, %1\n":"=m"(s1),"=m"(s2)
            );
#endif
        _snprintf(szTmp, sizeof(szTmp), "%08X%08X", s1, s2);       
        sCPUID = szTmp;   
#ifdef _MSC_VER
#if defined(_WIN64)
#else
        _asm
        {       
            mov     eax,03h      
                xor     ecx,ecx       
                xor     edx,edx       
                cpuid       
                mov     s1,edx       
                mov     s2,ecx       
        }       
#endif
#else
        __asm__
            __volatile__
            (
            "movl $3, %%eax\n"
            "xor %%ecx, %%ecx\n"
            "xor %%edx, %%edx\n"
            "cpuid\n"
            "movl %%edx, %0\n"
            "movl %%ecx, %1\n":"=m"(s1),"=m"(s2)
            );

#endif

        _snprintf(szTmp2, sizeof(szTmp2), "%08X%08X", s1, s2);       
        sCPUID += szTmp2;

        return 0;
    }

    int hardware_win32::get_root_path_id( std::string& sRootID )
    {
        char volumeName[MAX_PATH + 1] = { 0 };
        char fileSystemName[MAX_PATH + 1] = { 0 };
        DWORD serialNumber = 0;
        DWORD maxComponentLen = 0;
        DWORD fileSystemFlags = 0;
        std::string sPath = ":\\\\";
        std::string sDrive;
        if (!get_system_disk_number_info(sDrive, NULL))
        {
            return -1;
        }
        sPath = sDrive + sPath;
        if (!GetVolumeInformationA(
            sPath.c_str(),
            volumeName,
            ARRAYSIZE(volumeName),
            &serialNumber,
            &maxComponentLen,
            &fileSystemFlags,
            fileSystemName,
            ARRAYSIZE(fileSystemName)))
        {
            return -1;
        }
        std::stringstream ss;
        ss << serialNumber;
        ss >> sRootID;

        return 0;
    }

    int hardware_win32::get_CPU_core_count()
    {
        SYSTEM_INFO sysinfo;
        GetSystemInfo(&sysinfo);
        return sysinfo.dwNumberOfProcessors;
    }

    std::string hardware_win32::get_disk_information()
    {
        std::string szDiskInformation="";
        std::string mDiskID="";
        int len=0;
        BYTE sDisk[4096];
        UINT uDiskLen = 0;


        // BYTE sCpu[4096];
        UINT uCpuLen = 0;

        OSVERSIONINFO ovi = { 0 };
        ovi.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
        GetVersionEx( &ovi );

        // Only Windows 2000, Windows XP, Windows Server 2003...
        if ( ovi.dwPlatformId != VER_PLATFORM_WIN32_NT )
            return "";

        if (winnt_HD_serial_number_as_physical_read( sDisk, &uDiskLen, 1024 ) )
        {
            sDisk[uDiskLen]=0;
        }
        else if (winnt_HD_serial_number_as_scsi_read( sDisk, &uDiskLen, 1024 ))
        {
            sDisk[uDiskLen]=0;
        }
        //hd serial no.
        char mcode[1024];
        memcpy(mcode ,sDisk,uDiskLen);
        //customized serial no.
        len=uDiskLen+uCpuLen;
        mcode[len]=0;
        mDiskID=mcode;

        //trim left and right spaces
        std::string::iterator pl = find_if (mDiskID.begin(),mDiskID.end(),std::not1(std::ptr_fun(::isspace)));   
        mDiskID.erase(mDiskID.begin(), pl);   
        std::string::reverse_iterator pr = find_if (mDiskID.rbegin(),mDiskID.rend(),std::not1(std::ptr_fun(::isspace)));   
        mDiskID.erase(pr.base(), mDiskID.end());

        int strLen=0;
        strLen=mDiskID.length();
        mDiskID =mDiskID.substr(0,strLen);
        szDiskInformation = mDiskID;
        return szDiskInformation;
    }

    BOOL hardware_win32::get_system_disk_number_info( std::string& sDrive, STORAGE_DEVICE_NUMBER* pDeviceNum )
    {
        HANDLE hPhysicalDriveIOCTL = 0;
        char szName[MAX_PATH];
        if (!GetWindowsDirectoryA(szName, MAX_PATH))
        {
            return false;
        }
        std::string sWin = szName, sWinDrive;
        std::string::size_type colon_pos;
        if ((colon_pos = sWin.find(':')) != std::string::npos)
        {
            sWinDrive = sWin.substr(0, colon_pos);
        }
        else
        {
            return false;
        }
        sDrive = sWinDrive;
        char szDriveName[256] = {0}; 
        if (pDeviceNum == NULL)
        {
            return true;
        }

        _snprintf(szDriveName, sizeof(szDriveName), "\\\\.\\\\%s:", sWinDrive.c_str());
        hPhysicalDriveIOCTL = CreateFile((LPCTSTR) szDriveName,
            GENERIC_READ | GENERIC_WRITE, 
            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
            OPEN_EXISTING, 0, NULL);
        if (hPhysicalDriveIOCTL == INVALID_HANDLE_VALUE )
        {
            return false;
        }

        DWORD cbBytesReturned = 0;
        BOOL bSucceed = true;
        if (!DeviceIoControl( hPhysicalDriveIOCTL, IOCTL_STORAGE_GET_DEVICE_NUMBER,
            NULL, 
            0,
            pDeviceNum,
            sizeof(STORAGE_DEVICE_NUMBER),
            &cbBytesReturned, NULL))
        {
            bSucceed = false;
        }

        CloseHandle( hPhysicalDriveIOCTL );
        return bSucceed;
    }

    BOOL hardware_win32::winnt_HD_serial_number_as_physical_read( BYTE* dwSerial, UINT* puSerialLen, UINT uMaxSerialLen )
    {
#define DFP_GET_VERSION    0x00074080
        BOOL bInfoLoaded = FALSE;

        HANDLE hPhysicalDriveIOCTL = 0;
        STORAGE_DEVICE_NUMBER DeviceNum;
        std::string sTemp;
        if (!get_system_disk_number_info(sTemp, &DeviceNum))
        {
            return false;
        }
        UINT uDrive = DeviceNum.DeviceNumber;
        // Windows NT, Windows 2000, must have admin rights
        char szDriveName[128] = {0};
        _snprintf(szDriveName, sizeof(szDriveName), "\\\\.\\PhysicalDrive%d", uDrive);
        hPhysicalDriveIOCTL = CreateFileA(szDriveName,
            GENERIC_READ | GENERIC_WRITE, 
            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
            OPEN_EXISTING, 0, NULL);
        if ( hPhysicalDriveIOCTL != INVALID_HANDLE_VALUE )
        {
            GETVERSIONOUTPARAMS VersionParams = { 0 };
            DWORD               cbBytesReturned = 0;

            //STORAGE_DEVICE_NUMBER pNum;
            // Get the version, etc of PhysicalDrive IOCTL

            if ( DeviceIoControl( hPhysicalDriveIOCTL, DFP_GET_VERSION,
                NULL, 
                0,
                &VersionParams,
                sizeof( GETVERSIONOUTPARAMS ),
                &cbBytesReturned, NULL))
            {
                // If there is a IDE device at number "i" issue commands
                // to the device
                if ( VersionParams.bIDEDeviceMap != 0 )
                {
                    BYTE             bIDCmd = 0;   // IDE or ATAPI IDENTIFY cmd
                    SENDCMDINPARAMS scip = { 0 };

                    // Now, get the ID sector for all IDE devices in the system.
                    // If the device is ATAPI use the IDE_ATAPI_IDENTIFY command,
                    // otherwise use the IDE_ATA_IDENTIFY command
                    bIDCmd = ( VersionParams.bIDEDeviceMap >> uDrive & 0x10 ) ? IDE_ATAPI_IDENTIFY : IDE_ATA_IDENTIFY;
                    BYTE IdOutCmd[sizeof( SENDCMDOUTPARAMS ) + IDENTIFY_BUFFER_SIZE - 1] = { 0 };

                    if ( do_identify( hPhysicalDriveIOCTL, 
                        &scip, 
                        ( PSENDCMDOUTPARAMS )&IdOutCmd, 
                        ( BYTE )bIDCmd,
                        ( BYTE )uDrive,
                        &cbBytesReturned ) )
                    {
                        if ( * puSerialLen + 20U <= uMaxSerialLen )
                        {
                            CopyMemory( dwSerial + * puSerialLen, ( ( USHORT* )( ( ( PSENDCMDOUTPARAMS )IdOutCmd )->bBuffer ) ) + 10, 20 ); //serial no.

                            // Cut off the trailing blanks
                            UINT i = 0;
                            for (i = 20; i != 0U && ' '== dwSerial[*puSerialLen + i - 1]; -- i)
                            {}
                            *puSerialLen += i;

                            CopyMemory( dwSerial + * puSerialLen, ( ( USHORT* )( ( ( PSENDCMDOUTPARAMS )IdOutCmd )->bBuffer ) ) + 27, 40 ); // type

                            // Cut off the trailing blanks
                            for (i = 40; i != 0U && ' ' == dwSerial[*puSerialLen + i - 1]; -- i )
                            {}
                            *puSerialLen += i;


                            bInfoLoaded = TRUE;
                        }
                        else
                        {
                            ::CloseHandle( hPhysicalDriveIOCTL );
                            return bInfoLoaded;
                        }
                    }
                }
            }
            CloseHandle( hPhysicalDriveIOCTL );
        }

        return bInfoLoaded;
    }

    BOOL hardware_win32::do_identify( HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP, PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum, PDWORD lpcbBytesReturned )
    {
        // Set up data structures for IDENTIFY command.
        pSCIP->cBufferSize                  = IDENTIFY_BUFFER_SIZE;
        pSCIP->irDriveRegs.bFeaturesReg     = 0;
        pSCIP->irDriveRegs.bSectorCountReg = 1;
        pSCIP->irDriveRegs.bSectorNumberReg = 1;
        pSCIP->irDriveRegs.bCylLowReg       = 0;
        pSCIP->irDriveRegs.bCylHighReg      = 0;

        // calc the drive number.
        pSCIP->irDriveRegs.bDriveHeadReg = 0xA0 | ( ( bDriveNum & 1 ) << 4 );

        // The command can either be IDE identify or ATAPI identify.
        pSCIP->irDriveRegs.bCommandReg = bIDCmd;
        pSCIP->bDriveNumber = bDriveNum;
        pSCIP->cBufferSize = IDENTIFY_BUFFER_SIZE;

        return DeviceIoControl( hPhysicalDriveIOCTL, DFP_RECEIVE_DRIVE_DATA,
            ( LPVOID ) pSCIP,
            sizeof( SENDCMDINPARAMS ) - 1,
            ( LPVOID ) pSCOP,
            sizeof( SENDCMDOUTPARAMS ) + IDENTIFY_BUFFER_SIZE - 1,
            lpcbBytesReturned, NULL );
    }

    BOOL hardware_win32::winnt_HD_serial_number_as_scsi_read( BYTE* dwSerial, UINT* puSerialLen, UINT uMaxSerialLen )
    {

        BOOL bInfoLoaded = FALSE;
        HANDLE hScsiDriveIOCTL = 0;
        char   szDriveName[256] = {0};
        std::string sTemp;
        STORAGE_DEVICE_NUMBER DeviceNum;
        if (!get_system_disk_number_info(sTemp, &DeviceNum))
        {
            return false;
        }
        // Try to get a handle to PhysicalDrive IOCTL, report failure
        // and exit if can't.
        UINT uDeviceNumber = DeviceNum.DeviceNumber;
        _snprintf( szDriveName, sizeof(szDriveName), "\\\\.\\Scsi%d:", uDeviceNumber);

        // Windows NT, Windows 2000, any rights should do
        hScsiDriveIOCTL = CreateFile((LPCTSTR)szDriveName,
            GENERIC_READ | GENERIC_WRITE, 
            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
            OPEN_EXISTING, 0, NULL);

        if ( hScsiDriveIOCTL != INVALID_HANDLE_VALUE )
        {
            int iDrive = 0;
            for ( iDrive = 0; iDrive < 2; ++ iDrive )
            {
                char szBuffer[sizeof( SRB_IO_CONTROL ) + SENDIDLENGTH] = { 0 };

                SRB_IO_CONTROL* p = ( SRB_IO_CONTROL* )szBuffer;
                SENDCMDINPARAMS* pin = ( SENDCMDINPARAMS* )( szBuffer + sizeof( SRB_IO_CONTROL ) );
                DWORD dwResult;

                p->HeaderLength = sizeof( SRB_IO_CONTROL );
                p->Timeout = 10000;
                p->Length = SENDIDLENGTH;
                p->ControlCode = IOCTL_SCSI_MINIPORT_IDENTIFY;
                strncpy( ( char* )p->Signature, "SCSIDISK", 8 );

                pin->irDriveRegs.bCommandReg = IDE_ATA_IDENTIFY;
                pin->bDriveNumber = iDrive;

                if ( DeviceIoControl( hScsiDriveIOCTL, IOCTL_SCSI_MINIPORT, 
                    szBuffer,
                    sizeof( SRB_IO_CONTROL ) + sizeof( SENDCMDINPARAMS ) - 1,
                    szBuffer,
                    sizeof( SRB_IO_CONTROL ) + SENDIDLENGTH,
                    &dwResult, NULL ) )
                {
                    SENDCMDOUTPARAMS* pOut = ( SENDCMDOUTPARAMS* )( szBuffer + sizeof( SRB_IO_CONTROL ) );
                    IDSECTOR* pId = ( IDSECTOR* )( pOut->bBuffer );
                    if ( pId->sModelNumber[0] )
                    {
                        if ( * puSerialLen + 20U <= uMaxSerialLen )
                        {
                            // serial no.
                            CopyMemory( dwSerial + * puSerialLen, ( ( USHORT* )pId ) + 10, 20 );

                            // Cut off the trailing blanks
                            UINT i = 0;
                            for (i = 20; i != 0U && ' ' == dwSerial[* puSerialLen + i - 1]; -- i )
                            {}
                            * puSerialLen += i;

                            // type.
                            CopyMemory( dwSerial + * puSerialLen, ( ( USHORT* )pId ) + 27, 40 );
                            // Cut off the trailing blanks
                            for (i = 40; i != 0U && ' ' == dwSerial[* puSerialLen + i - 1]; -- i )
                            {}
                            * puSerialLen += i;

                            bInfoLoaded = TRUE;
                        }
                        else
                        {
                            ::CloseHandle( hScsiDriveIOCTL );
                            return bInfoLoaded;
                        }
                    }
                }
            }
            ::CloseHandle( hScsiDriveIOCTL );
        }

        return bInfoLoaded;
    }

} // namespace utility

#endif

