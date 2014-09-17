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
   class SSLCertificates; 
}


class ATL_NO_VTABLE InterfaceSSLCertificates :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceSSLCertificates, &CLSID_SSLCertificates>,
	public IDispatchImpl<IInterfaceSSLCertificates, &IID_IInterfaceSSLCertificates, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator,
   public ISupportErrorInfo
{
public:
	InterfaceSSLCertificates()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACESSLCERTIFICATES)


BEGIN_COM_MAP(InterfaceSSLCertificates)
	COM_INTERFACE_ENTRY(IInterfaceSSLCertificates)
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

   STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

public:
   STDMETHOD(Refresh)();
   STDMETHOD(Clear)();

   STDMETHOD(get_Item)(/*[in]*/ long Index, /*[out, retval]*/ IInterfaceSSLCertificate **pVal);
   STDMETHOD(get_Count)(/*[out, retval]*/ long *pVal);
   STDMETHOD(get_ItemByDBID)(/*[in]*/ long lDBID, /*[out, retval]*/ IInterfaceSSLCertificate** pVal);
   STDMETHOD(DeleteByDBID)(/*[in]*/ long DBID);
   STDMETHOD(Add)(/*[out, retval]*/ IInterfaceSSLCertificate **pVal);

   void Attach(std::shared_ptr<HM::SSLCertificates> pSSLCertificates);

public:

   std::shared_ptr<HM::SSLCertificates> ssl_certificates_;


};

OBJECT_ENTRY_AUTO(__uuidof(SSLCertificates), InterfaceSSLCertificates)
