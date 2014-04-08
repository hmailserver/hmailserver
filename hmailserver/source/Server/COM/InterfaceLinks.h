// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../hMailServer/resource.h"       // main symbols
#include "../hMailServer/hMailServer.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// InterfaceLinks

class ATL_NO_VTABLE InterfaceLinks :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceLinks, &CLSID_Links>,
	public IDispatchImpl<IInterfaceLinks, &IID_IInterfaceLinks, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
	InterfaceLinks()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACELINKS)


BEGIN_COM_MAP(InterfaceLinks)
	COM_INTERFACE_ENTRY(IInterfaceLinks)
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

   STDMETHOD(get_Domain)(/*[in]*/ long DBID, /*[out, retval]*/ IInterfaceDomain* *pVal);
   STDMETHOD(get_Account)(/*[in]*/ long DBID, /*[out, retval]*/ IInterfaceAccount* *pVal);
   STDMETHOD(get_Alias)(/*[in]*/ long DBID, /*[out, retval]*/ IInterfaceAlias* *pVal);
   STDMETHOD(get_DistributionList)(/*[in]*/ long DBID, /*[out, retval]*/ IInterfaceDistributionList* *pVal);

};

OBJECT_ENTRY_AUTO(__uuidof(Links), InterfaceLinks)
