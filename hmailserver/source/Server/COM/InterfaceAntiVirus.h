// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once
#include "../hMailServer/resource.h"       // main symbols

#include "../hMailServer/hMailServer.h"

class ATL_NO_VTABLE InterfaceAntiVirus : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceAntiVirus, &CLSID_AntiVirus>,
	public IDispatchImpl<IInterfaceAntiVirus, &IID_IInterfaceAntiVirus, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
	InterfaceAntiVirus();

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEANTIVIRUS)


BEGIN_COM_MAP(InterfaceAntiVirus)
	COM_INTERFACE_ENTRY(IInterfaceAntiVirus)
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
   STDMETHOD(get_ClamWinEnabled)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_ClamWinEnabled)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_ClamWinExecutable)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_ClamWinExecutable)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_ClamWinDBFolder)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_ClamWinDBFolder)(/*[in]*/ BSTR newVal);


   STDMETHOD(get_CustomScannerEnabled)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_CustomScannerEnabled)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_CustomScannerExecutable)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_CustomScannerExecutable)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_CustomScannerReturnValue)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_CustomScannerReturnValue)(/*[in]*/ long newVal);

   STDMETHOD(get_Action)(/*[out, retval]*/ eAntivirusAction *pVal);
   STDMETHOD(put_Action)(/*[in]*/ eAntivirusAction newVal);

   STDMETHOD(get_NotifySender)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_NotifySender)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_NotifyReceiver)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_NotifyReceiver)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_MaximumMessageSize)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_MaximumMessageSize)(/*[in]*/ long newVal);


   STDMETHOD(get_BlockedAttachments)(/*[out, retval]*/ IInterfaceBlockedAttachments** pVal);

   STDMETHOD(get_EnableAttachmentBlocking)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_EnableAttachmentBlocking)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_ClamAVEnabled)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_ClamAVEnabled)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_ClamAVHost)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_ClamAVHost)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_ClamAVPort)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_ClamAVPort)(/*[in]*/ long newVal);

   STDMETHOD(TestCustomerScanner)(BSTR customExecutable, long virusReturnCode, BSTR *messageText, VARIANT_BOOL *pResult);
   STDMETHOD(TestClamWinScanner)(BSTR clamWinExecutable, BSTR clamWinDatabase, BSTR *messageText, VARIANT_BOOL *pResult);
   STDMETHOD(TestClamAVScanner)(BSTR hostname, long port, BSTR *messageText, VARIANT_BOOL *pResult);


private:

   HM::AntiVirusConfiguration &antiVirusConfiguration_;
};

OBJECT_ENTRY_AUTO(__uuidof(AntiVirus), InterfaceAntiVirus)
