// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once
#include "../hMailServer/resource.h"       // main symbols

#include "../hMailServer/hMailServer.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

#include "../Common/Diagnostics/DiagnosticResult.h"

class ATL_NO_VTABLE InterfaceDiagnosticResult :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceDiagnosticResult, &CLSID_DiagnosticResult>,
	public IDispatchImpl<IInterfaceDiagnosticResult, &IID_IInterfaceDiagnosticResult, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
	InterfaceDiagnosticResult()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEDIAGNOSTICRESULT)


BEGIN_COM_MAP(InterfaceDiagnosticResult)
	COM_INTERFACE_ENTRY(IInterfaceDiagnosticResult)
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

   STDMETHOD(get_Name)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(get_Description)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(get_ExecutionDetails)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(get_Result)(/*[out, retval]*/ VARIANT_BOOL *pVal);

   void AttachResult(HM::DiagnosticResult result) {result_ = result; }

   HM::DiagnosticResult result_;
};

OBJECT_ENTRY_AUTO(__uuidof(DiagnosticResult), InterfaceDiagnosticResult)
