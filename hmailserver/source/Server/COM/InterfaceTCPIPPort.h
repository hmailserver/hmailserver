// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../hMailServer/resource.h"       // main symbols
#include "../hMailServer/hMailServer.h"

#include "COMCollection.h"

#include "..\Common\BO\TCPIPPort.h"
#include "..\Common\BO\TCPIPPorts.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// InterfaceTCPIPPort

class ATL_NO_VTABLE InterfaceTCPIPPort :  
   public COMCollectionItem<HM::TCPIPPort, HM::TCPIPPorts>,
   public ISupportErrorInfo,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceTCPIPPort, &CLSID_TCPIPPort>,
	public IDispatchImpl<IInterfaceTCPIPPort, &IID_IInterfaceTCPIPPort, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
	InterfaceTCPIPPort()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACETCPIPPORT)


BEGIN_COM_MAP(InterfaceTCPIPPort)
	COM_INTERFACE_ENTRY(IInterfaceTCPIPPort)
	COM_INTERFACE_ENTRY(IDispatch)
   COM_INTERFACE_ENTRY(ISupportErrorInfo)
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

   STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

   STDMETHOD(Delete)();
   STDMETHOD(Save)();
   STDMETHOD(get_ID)(/*[out, retval]*/ long *pVal);

   STDMETHOD(get_Protocol)(/*[out, retval]*/ eSessionType *pVal);
   STDMETHOD(put_Protocol)(/*[in]*/ eSessionType newVal);

   STDMETHOD(get_PortNumber)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_PortNumber)(/*[in]*/ long newVal);

   STDMETHOD(get_UseSSL)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_UseSSL)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_Address)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_Address)(/*[in]*/ BSTR newVal);

   STDMETHOD(get_SSLCertificateID)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_SSLCertificateID)(/*[in]*/ long newVal);

   STDMETHOD(get_ConnectionSecurity)(/*[out, retval]*/ eConnectionSecurity *pVal);
   STDMETHOD(put_ConnectionSecurity)(/*[in]*/ eConnectionSecurity newVal);

};

OBJECT_ENTRY_AUTO(__uuidof(TCPIPPort), InterfaceTCPIPPort)
