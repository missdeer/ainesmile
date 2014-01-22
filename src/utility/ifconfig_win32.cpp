#include "stdafx.h"

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
#include <winsock2.h> 
#include <Iphlpapi.h> 
#include <stdio.h> 
#pragma comment(lib,"Iphlpapi.lib") 
#include "ifconfig_win32.hpp"

namespace utility {
    ifconfig_win32::ifconfig_win32(void)
    {
    }

    ifconfig_win32::~ifconfig_win32(void)
    {
    }

    void ifconfig_win32::get_ifconfig( std::vector<if_config>& ifs )
    {
        PIP_ADAPTER_INFO pAdapterInfo; 
        PIP_ADAPTER_INFO pAdapter = NULL; 
        ULONG ulOutBufLen; 
        pAdapterInfo=(PIP_ADAPTER_INFO)malloc(sizeof(IP_ADAPTER_INFO)); 
        ulOutBufLen = sizeof(IP_ADAPTER_INFO); 

        if (GetAdaptersInfo( pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) 
        { 
            free(pAdapterInfo); 
            pAdapterInfo = (IP_ADAPTER_INFO *) malloc (ulOutBufLen); 
        } 

        if (GetAdaptersInfo( pAdapterInfo, &ulOutBufLen) == NO_ERROR) 
        { 
            pAdapter = pAdapterInfo; 
            while (pAdapter) 
            { 
                if_config if_cfg;
                if_cfg.dhcp_enabled_ = !!(pAdapter->DhcpEnabled);
                if_cfg.name_ = pAdapter->AdapterName;
                if_cfg.description_ = pAdapter->Description;
                char mac_address[20] = {0};
                _snprintf(mac_address, sizeof(mac_address), "%02x-%02x-%02x-%02x-%02x-%02x", 
                    pAdapter->Address[0], 
                    pAdapter->Address[1], 
                    pAdapter->Address[2], 
                    pAdapter->Address[3], 
                    pAdapter->Address[4], 
                    pAdapter->Address[5]); 
                if_cfg.mac_address_ = mac_address;
                if_cfg.ip_ = pAdapter->IpAddressList.IpAddress.String;
                if_cfg.submask_ = pAdapter->IpAddressList.IpMask.String;
                if_cfg.gateway_ = pAdapter->GatewayList.IpAddress.String;
                if_cfg.primary_dns_ = pAdapter->PrimaryWinsServer.IpAddress.String;
                if_cfg.second_dns_ = pAdapter->SecondaryWinsServer.IpAddress.String;

                IP_PER_ADAPTER_INFO* pPerAdapt  = NULL;  
                ULONG ulLen = 0;  
                int err = GetPerAdapterInfo( pAdapter->Index, pPerAdapt, &ulLen);  
                if ( err == ERROR_BUFFER_OVERFLOW ) 
                {  
                    pPerAdapt = ( IP_PER_ADAPTER_INFO* ) HeapAlloc(GetProcessHeap(),   
                        HEAP_ZERO_MEMORY, ulLen);  
                    err = GetPerAdapterInfo( pAdapter->Index, pPerAdapt, &ulLen );  
                    if ( err == ERROR_SUCCESS )
                    {  
                        IP_ADDR_STRING* pNext = &( pPerAdapt->DnsServerList );  
                        if (pNext && strcmp(pNext->IpAddress.String, "") != 0)  
                        {  
                            if_cfg.auto_dns_ = false;
                            if_cfg.primary_dns_ = pNext->IpAddress.String;
                            if (pNext = pNext->Next)  
                                if_cfg.second_dns_ =  pNext->IpAddress.String;  
                        }   
                        else
                        {
                            if_cfg.auto_dns_ = true;
                        }
                    }  
                }

                ifs.push_back(if_cfg);
                pAdapter = pAdapter->Next; 
            } 
        } 
        free(pAdapterInfo); 
    }

    bool ifconfig_win32::set_ifconfig( const if_config& ifs )
    {
        return true;
    }

} // namespace utility

#endif
