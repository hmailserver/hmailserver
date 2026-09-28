// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

#include "../hMailServer/resource.h"       // main symbols
#include "../hMailServer/hMailServer.h"

#include "../Common/BO/ARCTrustedSealers.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// InterfaceARCTrustedSealers

class ATL_NO_VTABLE InterfaceARCTrustedSealers :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceARCTrustedSealers, &CLSID_ARCTrustedSealers>,
	public IDispatchImpl<IInterfaceARCTrustedSealers, &IID_IInterfaceARCTrustedSealers, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
	InterfaceARCTrustedSealers()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEARCTRUSTEDSEALERS)


BEGIN_COM_MAP(InterfaceARCTrustedSealers)
	COM_INTERFACE_ENTRY(IInterfaceARCTrustedSealers)
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
   STDMETHOD(Refresh)();

   STDMETHOD(get_Item)(/*[in]*/ long Index, /*[out, retval]*/ IInterfaceARCTrustedSealer **pVal);
   STDMETHOD(get_Count)(/*[out, retval]*/ long *pVal);
   STDMETHOD(get_ItemByDBID)(/*[in]*/ long lDBID, /*[out, retval]*/ IInterfaceARCTrustedSealer** pVal);
   STDMETHOD(get_ItemByName)(/*[in]*/ BSTR sName, /*[out, retval]*/ IInterfaceARCTrustedSealer** pVal);
   STDMETHOD(DeleteByDBID)(/*[in]*/ long DBID);
   STDMETHOD(Add)(/*[out, retval]*/ IInterfaceARCTrustedSealer **pVal);

   void Attach(std::shared_ptr<HM::ARCTrustedSealers> pTrustedSealers);

public:

   std::shared_ptr<HM::ARCTrustedSealers> trusted_sealers_;

};

OBJECT_ENTRY_AUTO(__uuidof(ARCTrustedSealers), InterfaceARCTrustedSealers)
