/*
* Copyright (c) 2017 Michael Mathers
*/
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <iomanip>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>


namespace
{
    typedef Microsoft::WRL::ComPtr<IDXGIAdapter1>                                   _base_adapter_type;
    typedef Microsoft::WRL::ComPtr<IDXGIOutput>                                     _base_output_type;
    typedef std::tuple<_base_output_type, DXGI_OUTPUT_DESC>                         _output_type;
    typedef std::vector<_output_type>                                               _output_list_type;
    typedef std::tuple<_base_adapter_type, DXGI_ADAPTER_DESC1, _output_list_type>   _adapter_type;
    typedef std::vector<_adapter_type>                                              _adapter_list_type;


    void print_adapter_and_output_information(const _adapter_list_type& adapter_list)
    {
        std::cout << "Printing information for " << adapter_list.size() << " adapters" << '\n';
        for (const auto& adapter : adapter_list)
        {
            std::cout << "Adapter:" << '\n';
            std::cout << std::setw(32) << "LUID: " << std::get<1>(adapter).AdapterLuid.HighPart << std::get<1>(adapter).AdapterLuid.LowPart << '\n';
            std::wcout << std::setw(32) << "Description: " << (std::get<1>(adapter).Description) << '\n';
            std::cout << std::setw(32) << "Revision: " << std::get<1>(adapter).Revision << '\n';
            std::cout << std::setw(32) << "Dedicated System Memory: " << (std::get<1>(adapter).DedicatedSystemMemory / 1024 / 1024) << "MB" << '\n';
            std::cout << std::setw(32) << "Shared System Memory: " << (std::get<1>(adapter).SharedSystemMemory / 1024 / 1024) << "MB" << '\n';
            std::cout << std::setw(32) << "Dedicated Video Memory: " << (std::get<1>(adapter).DedicatedVideoMemory / 1024 / 1024) << "MB" << '\n';
            std::cout << std::setw(32) << "Device ID: " << std::get<1>(adapter).DeviceId << '\n';
            std::cout << std::setw(32) << "Subsystem ID: " << std::get<1>(adapter).SubSysId << '\n';
            std::cout << std::setw(32) << "Vendor ID: " << std::get<1>(adapter).VendorId << '\n';
            std::cout << std::setw(32) << "Flags: " << std::get<1>(adapter).Flags << '\n';

            for (const auto& output : std::get<2>(adapter))
            {
                std::cout << "Output:" << '\n';
                std::wcout << std::setw(32) << "Device name: " << std::get<1>(output).DeviceName << '\n';
                std::cout << std::setw(32) << "Output width: " << (std::get<1>(output).DesktopCoordinates.right - std::get<1>(output).DesktopCoordinates.left) << " pixels" << '\n';
                std::cout << std::setw(32) << "Output height: " << (std::get<1>(output).DesktopCoordinates.bottom - std::get<1>(output).DesktopCoordinates.top) << " pixels" << '\n';
                std::cout << std::setw(32) << "Desktop coordinates (left): " << std::get<1>(output).DesktopCoordinates.left << '\n';
                std::cout << std::setw(32) << "Desktop coordinates (right): " << std::get<1>(output).DesktopCoordinates.right << '\n';
                std::cout << std::setw(32) << "Desktop coordinates (top): " << std::get<1>(output).DesktopCoordinates.top << '\n';
                std::cout << std::setw(32) << "Desktop coordinates (bottom): " << std::get<1>(output).DesktopCoordinates.bottom << '\n';
                std::cout << std::setw(32) << "Attached to desktop: " << std::get<1>(output).AttachedToDesktop << '\n';
            }
        }
    }
}


int main(int argc, char *argv[])
{
    Microsoft::WRL::ComPtr<IDXGIFactory5> dxgi_factory;
    HRESULT result;
    if (FAILED(result = CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgi_factory))))
    {
        std::cerr << "Failed to create DXGI factory!" << '\n';
        return result;
    }

    UINT adapter_count = 0;
    _adapter_list_type adapter_list;
    _base_adapter_type adapter;
    while (dxgi_factory->EnumAdapters1(adapter_count++, adapter.ReleaseAndGetAddressOf()) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC1 adapter_descriptor;
        adapter->GetDesc1(&adapter_descriptor);

        UINT output_count = 0;
        _output_list_type output_list;
        _base_output_type output;
        while (adapter->EnumOutputs(output_count++, output.ReleaseAndGetAddressOf()) != DXGI_ERROR_NOT_FOUND)
        {
            DXGI_OUTPUT_DESC output_descriptor;
            output->GetDesc(&output_descriptor);
            output_list.push_back(std::make_tuple(output, output_descriptor));
        }

        adapter_list.push_back(std::make_tuple(adapter, adapter_descriptor, output_list));
    }

    print_adapter_and_output_information(adapter_list);

    return EXIT_SUCCESS;
}
