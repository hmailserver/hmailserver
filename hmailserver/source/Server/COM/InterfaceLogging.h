// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once
#include "../hMailServer/resource.h"       // main symbols

#include "../hMailServer/hMailServer.h"


// InterfaceLogging

class ATL_NO_VTABLE InterfaceLogging : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceLogging, &CLSID_Logging>,
	public IDispatchImpl<IInterfaceLogging, &IID_IInterfaceLogging, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
   InterfaceLogging();

   bool LoadSettings();

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACELOGGING)


BEGIN_COM_MAP(InterfaceLogging)
	COM_INTERFACE_ENTRY(IInterfaceLogging)
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
   STDMETHOD(get_LogDebug)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_LogDebug)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_Enabled)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_Enabled)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_LogSMTP)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_LogSMTP)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_LogPOP3)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_LogPOP3)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_LogIMAP)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_LogIMAP)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_AWStatsEnabled)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_AWStatsEnabled)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_LogTCPIP)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_LogTCPIP)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_LogApplication)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_LogApplication)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(EnableLiveLogging)(VARIANT_BOOL newVal);

   STDMETHOD(get_MaskPasswordsInLog)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_MaskPasswordsInLog)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_KeepFilesOpen)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_KeepFilesOpen)(/*[in]*/ VARIANT_BOOL newVal);


   STDMETHOD(get_Device)(/*[out, retval]*/ eLogDevice *pVal);
   STDMETHOD(put_Device)(/*[in]*/ eLogDevice newVal);

   STDMETHOD(get_LogFormat)(/*[out, retval]*/ eLogOutputFormat *pVal);
   STDMETHOD(put_LogFormat)(/*[in]*/ eLogOutputFormat newVal);


   int COMLogDevice2INTLogDevice_(eLogDevice newVal);
   eLogDevice INTLogDevice2COMLogDevice_(int RelayMode);

   int COMLogFormat2IntLogFormat_(eLogOutputFormat newVal);
   eLogOutputFormat IntLogFormat2ComLogFormat_(int RelayMode);   

   STDMETHOD(get_Directory)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(get_LiveLog)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(get_LiveLoggingEnabled)(/*[out, retval]*/ VARIANT_BOOL *pVal);

   STDMETHOD(get_CurrentEventLog)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(get_CurrentErrorLog)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(get_CurrentAwstatsLog)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(get_CurrentDefaultLog)(/*[out, retval]*/ BSTR *pVal);
private:

   HM::Configuration *config_;
   HM::IniFileSettings *ini_file_settings_;
};

OBJECT_ENTRY_AUTO(__uuidof(Logging), InterfaceLogging)
