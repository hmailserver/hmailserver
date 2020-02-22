// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../hMailServer/resource.h"       // main symbols
#include "../hMailServer/hMailServer.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// InterfaceMessageIndexing

class ATL_NO_VTABLE InterfaceMessageIndexing :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceMessageIndexing, &CLSID_MessageIndexing>,
	public IDispatchImpl<IInterfaceMessageIndexing, &IID_IInterfaceMessageIndexing, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
   InterfaceMessageIndexing();

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEMESSAGEINDEXING)


BEGIN_COM_MAP(InterfaceMessageIndexing)
	COM_INTERFACE_ENTRY(IInterfaceMessageIndexing)
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

   bool LoadSettings();

   STDMETHOD(get_Enabled)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_Enabled)(/*[in]*/ VARIANT_BOOL newVal);	

   STDMETHOD(get_TotalMessageCount)(/*[out, retval]*/ long *pVal);
   STDMETHOD(get_TotalIndexedCount)(/*[out, retval]*/ long *pVal);

   STDMETHOD(Clear)();
   STDMETHOD(Index)();
private:

   HM::Configuration* config_;
};

OBJECT_ENTRY_AUTO(__uuidof(MessageIndexing), InterfaceMessageIndexing)
