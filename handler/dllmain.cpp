#include <windows.h>
#include <Guiddef.h>
#include "ClassFactory.h"           // For the class factory
#include "Reg.h"

// {BB33EADB-A152-4BC7-BADF-ED2EA769ED57}
const CLSID CLSID_FileContextMenuExt =
{ 0xbb33eadb, 0xa152, 0x4bc7,{ 0xba, 0xdf, 0xed, 0x2e, 0xa7, 0x69, 0xed, 0x57 } };


HINSTANCE   g_hInst     = NULL;
long        g_cDllRef   = 0;


BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        // Hold the instance of this DLL module, we will use it to get the
        // path of the DLL to register the component.
        g_hInst = hModule;
        DisableThreadLibraryCalls(hModule);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


//
//   FUNCTION: DllGetClassObject
//
//   PURPOSE: Create the class factory and query to the specific interface.
//
//   PARAMETERS:
//   * rclsid - The CLSID that will associate the correct data and code.
//   * riid - A reference to the identifier of the interface that the caller
//     is to use to communicate with the class object.
//   * ppv - The address of a pointer variable that receives the interface
//     pointer requested in riid. Upon successful return, *ppv contains the
//     requested interface pointer. If an error occurs, the interface pointer
//     is NULL.
//
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void **ppv)
{
    HRESULT hr = CLASS_E_CLASSNOTAVAILABLE;

    if (IsEqualCLSID(CLSID_FileContextMenuExt, rclsid))
    {
        hr = E_OUTOFMEMORY;

        ClassFactory *pClassFactory = new ClassFactory();
        if (pClassFactory)
        {
            hr = pClassFactory->QueryInterface(riid, ppv);
            pClassFactory->Release();
        }
    }

    return hr;
}


//
//   FUNCTION: DllCanUnloadNow
//
//   PURPOSE: Check if we can unload the component from the memory.
//
//   NOTE: The component can be unloaded from the memory when its reference
//   count is zero (i.e. nobody is still using the component).
//
STDAPI DllCanUnloadNow(void)
{
    return g_cDllRef > 0 ? S_FALSE : S_OK;
}


//
//   FUNCTION: DllRegisterServer
//
//   PURPOSE: Register the COM server and the context menu handler.
//
STDAPI DllRegisterServer(void)
{
    HRESULT hr;

    wchar_t szModule[MAX_PATH];
    if (GetModuleFileName(g_hInst, szModule, ARRAYSIZE(szModule)) == 0)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        return hr;
    }

    // Register the component.
    hr = RegisterInprocServer(szModule, CLSID_FileContextMenuExt,
        L"SplitterShellExtContextMenuHandler.FileContextMenuExt Class",
        L"Apartment");

    if (SUCCEEDED(hr))
    {
        hr = RegisterHandlerClass(CLSID_FileContextMenuExt, L"SplitterShellExtContextMenuHandler.FileContextMenuExt", L"SplitterShellExtContextMenuHandler.FileContextMenuExt Class");
    }
    // Register the context menu handlers.
    if (SUCCEEDED(hr))
    {
        hr = RegisterShellExtContextMenuHandler(L".bmp",
            CLSID_FileContextMenuExt,
            L"SplitterShellExtContextMenuHandler.FileContextMenuExt");
    }
    if (SUCCEEDED(hr))
    {
        hr = RegisterShellExtContextMenuHandler(L".gif",
            CLSID_FileContextMenuExt,
            L"SplitterShellExtContextMenuHandler.FileContextMenuExt");
    }
    if (SUCCEEDED(hr))
    {
        hr = RegisterShellExtContextMenuHandler(L".ico",
            CLSID_FileContextMenuExt,
            L"SplitterShellExtContextMenuHandler.FileContextMenuExt");
    }
    if (SUCCEEDED(hr))
    {
        hr = RegisterShellExtContextMenuHandler(L".jpe",
            CLSID_FileContextMenuExt,
            L"SplitterShellExtContextMenuHandler.FileContextMenuExt");
    }
    if (SUCCEEDED(hr))
    {
        hr = RegisterShellExtContextMenuHandler(L".jpeg",
            CLSID_FileContextMenuExt,
            L"SplitterShellExtContextMenuHandler.FileContextMenuExt");
    }
    if (SUCCEEDED(hr))
    {
        hr = RegisterShellExtContextMenuHandler(L".jpg",
            CLSID_FileContextMenuExt,
            L"SplitterShellExtContextMenuHandler.FileContextMenuExt");
    }
    if (SUCCEEDED(hr))
    {
        hr = RegisterShellExtContextMenuHandler(L".png",
            CLSID_FileContextMenuExt,
            L"SplitterShellExtContextMenuHandler.FileContextMenuExt");
    }
    if (SUCCEEDED(hr))
    {
        hr = RegisterShellExtContextMenuHandler(L".tga",
            CLSID_FileContextMenuExt,
            L"SplitterShellExtContextMenuHandler.FileContextMenuExt");
    }
    if (SUCCEEDED(hr))
    {
        hr = RegisterShellExtContextMenuHandler(L".tif",
            CLSID_FileContextMenuExt,
            L"SplitterShellExtContextMenuHandler.FileContextMenuExt");
    }
    if (SUCCEEDED(hr))
    {
        hr = RegisterShellExtContextMenuHandler(L".tiff",
            CLSID_FileContextMenuExt,
            L"SplitterShellExtContextMenuHandler.FileContextMenuExt");
    }

    return hr;
}


//
//   FUNCTION: DllUnregisterServer
//
//   PURPOSE: Unregister the COM server and the context menu handler.
//
STDAPI DllUnregisterServer(void)
{
    HRESULT hr = S_OK;

    wchar_t szModule[MAX_PATH];
    if (GetModuleFileName(g_hInst, szModule, ARRAYSIZE(szModule)) == 0)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        return hr;
    }

    // Unregister the component.
    hr = UnregisterInprocServer(CLSID_FileContextMenuExt);
    if (SUCCEEDED(hr))
    {
        hr = UnregisterHandlerClass(L"SplitterShellExtContextMenuHandler.FileContextMenuExt");
    }

    // Unregister the context menu handlers.
    if (SUCCEEDED(hr))
    {
        // Ignore result so same ProgID can handle multiple extensions
        (void)UnregisterShellExtContextMenuHandler(L".bmp", CLSID_FileContextMenuExt);
        (void)UnregisterShellExtContextMenuHandler(L".gif", CLSID_FileContextMenuExt);
        (void)UnregisterShellExtContextMenuHandler(L".ico", CLSID_FileContextMenuExt);
        (void)UnregisterShellExtContextMenuHandler(L".jpe", CLSID_FileContextMenuExt);
        (void)UnregisterShellExtContextMenuHandler(L".jpeg", CLSID_FileContextMenuExt);
        (void)UnregisterShellExtContextMenuHandler(L".jpg", CLSID_FileContextMenuExt);
        (void)UnregisterShellExtContextMenuHandler(L".png", CLSID_FileContextMenuExt);
        (void)UnregisterShellExtContextMenuHandler(L".tga", CLSID_FileContextMenuExt);
        (void)UnregisterShellExtContextMenuHandler(L".tif", CLSID_FileContextMenuExt);
        (void)UnregisterShellExtContextMenuHandler(L".tiff", CLSID_FileContextMenuExt);
    }

    return hr;
}