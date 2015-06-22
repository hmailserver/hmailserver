// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../hMailServer/resource.h"       // main symbols
#include "../hMailServer/hMailServer.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

namespace HM
{
   class IniFileSettings;
}

class ATL_NO_VTABLE InterfaceDirectories :
   public CComObjectRootEx<CComSingleThreadModel>,
   public CComCoClass<InterfaceDirectories, &CLSID_Directories>,
   public IDispatchImpl<IInterfaceDirectories, &IID_IInterfaceDirectories, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
   InterfaceDirectories();
   
   void LoadSettings(HM::IniFileSettings *pIniFileSettings)
   {
      ini_file_settings_ = pIniFileSettings;
   }

   DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEDIRECTORIES)


   BEGIN_COM_MAP(InterfaceDirectories)
      COM_INTERFACE_ENTRY(IInterfaceDirectories)
      COM_INTERFACE_ENTRY(IDispatch)
   END_COM_MAP()



   DECLARE_PROTECT_FINAL_CONSTRUCT()

   HRESULT FinalConstruct()
   {
      return S_OK;
   }

   void FinalRelease()
   {
   }

public:

   

   STDMETHOD(get_ProgramDirectory)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_ProgramDirectory)(/*[in]*/ BSTR newVal);

   STDMETHOD(get_DatabaseDirectory)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_DatabaseDirectory)(/*[in]*/ BSTR newVal);

   STDMETHOD(get_DataDirectory)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_DataDirectory)(/*[in]*/ BSTR newVal);

   STDMETHOD(get_LogDirectory)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_LogDirectory)(/*[in]*/ BSTR newVal);

   STDMETHOD(get_TempDirectory)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_TempDirectory)(/*[in]*/ BSTR newVal);

   STDMETHOD(get_EventDirectory)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_EventDirectory)(/*[in]*/ BSTR newVal);

   STDMETHOD(get_DBScriptDirectory)(/*[out, retval]*/ BSTR *pVal);
private:

   HM::IniFileSettings* ini_file_settings_;
};

OBJECT_ENTRY_AUTO(__uuidof(Directories), InterfaceDirectories)
