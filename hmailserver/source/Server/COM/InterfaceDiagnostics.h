// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../hMailServer/resource.h"       // main symbols
#include "../hMailServer/hMailServer.h"

#include "../Common/Diagnostics/Diagnostic.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// InterfaceDiagnostics

class ATL_NO_VTABLE InterfaceDiagnostics :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceDiagnostics, &CLSID_Diagnostics>,
	public IDispatchImpl<IInterfaceDiagnostics, &IID_IInterfaceDiagnostics, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
	InterfaceDiagnostics()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEDIAGNOSTICS)


BEGIN_COM_MAP(InterfaceDiagnostics)
	COM_INTERFACE_ENTRY(IInterfaceDiagnostics)
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

   STDMETHOD(PerformTests)(IInterfaceDiagnosticResults ** pTextResult);

   STDMETHOD(get_LocalDomainName)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_LocalDomainName)(/*[in]*/ BSTR newVal);

   STDMETHOD(get_TestDomainName)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_TestDomainName)(/*[in]*/ BSTR newVal);

   HM::Diagnostic diagnostics_;
};

OBJECT_ENTRY_AUTO(__uuidof(Diagnostics), InterfaceDiagnostics)
