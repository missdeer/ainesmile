#include "stdafx.h"

#if defined(__APPLE__)
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/network/IOEthernetInterface.h>
#include <IOKit/network/IONetworkInterface.h>
#include <IOKit/network/IOEthernetController.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <mach-o/dyld.h>
#include "util.hpp"
#include "hardware_osx.hpp"

namespace utility {
    static kern_return_t FindEthernetInterfaces(io_iterator_t *matchingServices)
    {
        kern_return_t           kernResult; 
        CFMutableDictionaryRef  matchingDict;
        CFMutableDictionaryRef  propertyMatchDict;

        // Ethernet interfaces are instances of class kIOEthernetInterfaceClass. 
        // IOServiceMatching is a convenience function to create a dictionary with the key kIOProviderClassKey and 
        // the specified value.

        matchingDict = IOServiceMatching(kIOEthernetInterfaceClass);

        // Note that another option here would be:
        // matchingDict = IOBSDMatching("en0");
        // but en0: isn't necessarily the primary interface, especially on systems with multiple Ethernet ports.

        if (NULL == matchingDict)
        {
            //LOG(ERROR) << "IOServiceMatching returned a NULL dictionary.";
        }
        else
        {
            // Each IONetworkInterface object has a Boolean property with the key kIOPrimaryInterface. Only the
            // primary (built-in) interface has this property set to TRUE.

            // IOServiceGetMatchingServices uses the default matching criteria defined by IOService. This considers
            // only the following properties plus any family-specific matching in this order of precedence 
            // (see IOService::passiveMatch):
            //
            // kIOProviderClassKey (IOServiceMatching)
            // kIONameMatchKey (IOServiceNameMatching)
            // kIOPropertyMatchKey
            // kIOPathMatchKey
            // kIOMatchedServiceCountKey
            // family-specific matching
            // kIOBSDNameKey (IOBSDNameMatching)
            // kIOLocationMatchKey

            // The IONetworkingFamily does not define any family-specific matching. This means that in            
            // add that property to a separate dictionary and then add that to our matching dictionary
            // specifying kIOPropertyMatchKey.

            propertyMatchDict = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
                &kCFTypeDictionaryKeyCallBacks,
                &kCFTypeDictionaryValueCallBacks);

            if (NULL == propertyMatchDict)
            {
                //LOG(ERROR) << "CFDictionaryCreateMutable returned a NULL dictionary.";
            }
            else
            {
                // Set the value in the dictionary of the property with the given key, or add the key 
                // to the dictionary if it doesn't exist. This call retains the value object passed in.
                CFDictionarySetValue(propertyMatchDict, CFSTR(kIOPrimaryInterface), kCFBooleanTrue); 

                // Now add the dictionary containing the matching value for kIOPrimaryInterface to our main
                // matching dictionary. This call will retain propertyMatchDict, so we can release our reference 
                // on propertyMatchDict after adding it to matchingDict.
                CFDictionarySetValue(matchingDict, CFSTR(kIOPropertyMatchKey), propertyMatchDict);
                CFRelease(propertyMatchDict);
            }
        }

        // IOServiceGetMatchingServices retains the returned iterator, so release the iterator when we're done with it.
        // IOServiceGetMatchingServices also consumes a reference on the matching dictionary so we don't need to release
        // the dictionary explicitly.
        kernResult = IOServiceGetMatchingServices(kIOMasterPortDefault, matchingDict, matchingServices);    
        if (KERN_SUCCESS != kernResult)
        {
            //LOG(ERROR) << "IOServiceGetMatchingServices returned " << kernResult;
        }

        return kernResult;
    }

    // Given an iterator across a set of Ethernet interfaces, return the MAC address of the last one.
    // If no interfaces are found the MAC address is set to an empty string.
    // In this sample the iterator should contain just the primary interface.
    static kern_return_t GetMACAddress(io_iterator_t intfIterator, UInt8 *MACAddress, UInt8 bufferSize)
    {
        io_object_t     intfService;
        io_object_t     controllerService;
        kern_return_t   kernResult = KERN_FAILURE;

        // Make sure the caller provided enough buffer space. Protect against buffer overflow problems.
        if (bufferSize < kIOEthernetAddressSize)
        {
            return kernResult;
        }

        // Initialize the returned address
        bzero(MACAddress, bufferSize);

        // IOIteratorNext retains the returned object, so release it when we're done with it.
        while ((intfService = IOIteratorNext(intfIterator)))
        {
            CFTypeRef   MACAddressAsCFData;        

            // IONetworkControllers can't be found directly by the IOServiceGetMatchingServices call, 
            // since they are hardware nubs and do not participate in driver matching. In other words,
            // registerService() is never called on them. So we've found the IONetworkInterface and will 
            // get its parent controller by asking for it specifically.
            // IORegistryEntryGetParentEntry retains the returned object, so release it when we're done with it.
            kernResult = IORegistryEntryGetParentEntry(intfService,
                kIOServicePlane,
                &controllerService);

            if (KERN_SUCCESS != kernResult)
            {
                //LOG(ERROR) << "IORegistryEntryGetParentEntry returned " << kernResult;
            }
            else
            {
                // Retrieve the MAC address property from the I/O Registry in the form of a CFData
                MACAddressAsCFData = IORegistryEntryCreateCFProperty(controllerService,
                    CFSTR(kIOMACAddress),
                    kCFAllocatorDefault,
                    0);
                if (MACAddressAsCFData)
                {
                    //CFShow(MACAddressAsCFData); // for display purposes only; output goes to stderr

                    // Get the raw bytes of the MAC address from the CFData
                    CFDataGetBytes((CFDataRef)MACAddressAsCFData, CFRangeMake(0, kIOEthernetAddressSize), MACAddress);
                    CFRelease(MACAddressAsCFData);
                }

                // Done with the parent Ethernet controller object so we release it.
                (void) IOObjectRelease(controllerService);
            }

            // Done with the Ethernet interface object so we release it.
            (void) IOObjectRelease(intfService);
        }

        return kernResult;
    }

    int __getMacAddressOSX(unsigned char pAddr[6])
    {
        kern_return_t   kernResult = KERN_SUCCESS;
        io_iterator_t   intfIterator;
        UInt8           MACAddress[kIOEthernetAddressSize];
        kernResult = FindEthernetInterfaces(&intfIterator);

        if (KERN_SUCCESS != kernResult) 
        {
            //LOG(ERROR) << "FindEthernetInterfaces returned" << kernResult;
        }
        else 
        {
            kernResult = GetMACAddress(intfIterator, MACAddress, sizeof(MACAddress));

            if (KERN_SUCCESS != kernResult) 
            {
                //LOG(ERROR) << "GetMACAddress returned " << kernResult;
            }
            else 
            {
                pAddr[0] = MACAddress[0];
                pAddr[1] = MACAddress[1];
                pAddr[2] = MACAddress[2];
                pAddr[3] = MACAddress[3];
                pAddr[4] = MACAddress[4];
                pAddr[5] = MACAddress[5];

            }
        }   

        (void) IOObjectRelease(intfIterator);   // Release the iterator.
        return 0;
    }

    hardware_osx::hardware_osx(void)
    {
    }

    hardware_osx::~hardware_osx(void)
    {
    }

    int hardware_osx::get_HDD_id( std::string& sHDDID )
    {

    }

    int hardware_osx::get_Mac_address( std::string& sMacAddr )
    {
#define MAC_ADDRESS_LENGTH 6
        int ret = 0;
        unsigned char tmpMacAddressBin[MAC_ADDRESS_LENGTH] = {0};
        ret = __getMacAddressOSX(tmpMacAddressBin);
        if (ret != -1)
        {
            utility::utilities::bin_to_hex(tmpMacAddressBin, MAC_ADDRESS_LENGTH, sMacAddr);
        }
        return ret;

    }

    int hardware_osx::get_CPU_id( std::string& sCPUID )
    {
        char szTmp[40] = {0};
        char szTmp2[40] = {0};
        unsigned long s1 = 0,s2=0;

        __asm__
            __volatile__
            (
            "movl $1, %%eax\n"
            "xor %%edx,%%edx\n"
            "cpuid\n"
            "movl %%edx, %0\n"
            "movl %%eax, %1\n":"=m"(s1),"=m"(s2)
            );
        snprintf(szTmp, sizeof(szTmp), "%08X%08X", s1, s2);       
        sCPUID = szTmp;   
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

        snprintf(szTmp2, sizeof(szTmp2), "%08X%08X", s1, s2);       
        sCPUID += szTmp2;

        return 0;
    }

    int hardware_osx::get_root_path_id( std::string& sRootID )
    {
#define MTAB_MAX_SIZE 2048
        /*char result[MTAB_MAX_SIZE] = {0};
        char devname[NAME_MAX] = {0};
        FILE* fp = fopen("/etc/mtab", "r");
        if (!fp)
        {
        return -1;
        }
        int ret = fread(result ,1, MTAB_MAX_SIZE, fp);
        fclose(fp);
        if (ret = 0)
        {
        return -1;
        }
        sscanf(result, "%s / ", devname);
        if (!strlen(devname))
        {
        return -1;
        }
        //char cmd[NAME_MAX] = "/lib/udev/vol_id -u ";
        char cmd[NAME_MAX] = "/sbin/blkid -s UUID -o value ";
        strcat(cmd, devname);
        char serialNumber[NAME_MAX] = {0};
        FILE* stream = NULL;
        stream = popen(cmd, "r");
        if (!stream)
        {
        return -1;
        }
        fread(serialNumber, 1,NAME_MAX, stream);
        pclose(stream);*/

        char serialNumber[NAME_MAX] = "f98c4a27-215e-44bd-ba86-4a8df2869714";
        std::stringstream ss;
        ss << serialNumber;
        ss >> sRootID;

        return 0;
    }

    int hardware_osx::get_CPU_core_count()
    {
        int nm[2];
        size_t len = 4;
        uint32_t count;

        nm[0] = CTL_HW; nm[1] = HW_AVAILCPU;
        sysctl(nm, 2, &count, &len, NULL, 0);

        if (count < 1) {
            nm[1] = HW_NCPU;
            sysctl(nm, 2, &count, &len, NULL, 0);
            if (count < 1) { count = 1; }
        }
        return count;
    }
} // namespace utility
#endif
