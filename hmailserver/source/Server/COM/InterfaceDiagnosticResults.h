// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once
#include "../hMailServer/resource.h"       // main symbols

#include "../hMailServer/hMailServer.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

#include "../Common/Diagnostics/DiagnosticResult.h"

// InterfaceDiagnosticResults

class ATL_NO_VTABLE InterfaceDiagnosticResults :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceDiagnosticResults, &CLSID_DiagnosticResults>,
	public IDispatchImpl<IInterfaceDiagnosticResults, &IID_IInterfaceDiagnosticResults, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
	InterfaceDiagnosticResults()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEDIAGNOSTICRESULTS)


BEGIN_COM_MAP(InterfaceDiagnosticResults)
	COM_INTERFACE_ENTRY(IInterfaceDiagnosticResults)
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
   STDMETHOD(get_Item)(/*[in]*/ long Index, /*[out, retval]*/ IInterfaceDiagnosticResult* *pVal);
   STDMETHOD(get_Count)(/*[out, retval]*/ long* pVal);

   void AttachResults(std::vector<HM::DiagnosticResult> results) {results_ = results; }

   std::vector<HM::DiagnosticResult> results_;
};

OBJECT_ENTRY_AUTO(__uuidof(DiagnosticResults), InterfaceDiagnosticResults)
