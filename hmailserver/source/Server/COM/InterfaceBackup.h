// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "..\hMailServer\resource.h"       // main symbols

#include "..\hMailServer\hMailServer.h"


// InterfaceBackup

class ATL_NO_VTABLE InterfaceBackup : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceBackup, &CLSID_Backup>,
	public IDispatchImpl<IInterfaceBackup, &IID_IInterfaceBackup, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
	InterfaceBackup()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEBACKUP)


BEGIN_COM_MAP(InterfaceBackup)
	COM_INTERFACE_ENTRY(IInterfaceBackup)
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

   void Attach(std::shared_ptr<HM::Backup> pBackup);
   
   STDMETHOD(StartRestore)(void);

   STDMETHOD(get_ContainsSettings)(VARIANT_BOOL* pVal);
   STDMETHOD(get_ContainsDomains)(VARIANT_BOOL* pVal);
   STDMETHOD(get_ContainsMessages)(VARIANT_BOOL* pVal);


   STDMETHOD(get_RestoreSettings)(VARIANT_BOOL* pVal);
   STDMETHOD(put_RestoreSettings)(VARIANT_BOOL newVal);
   STDMETHOD(get_RestoreDomains)(VARIANT_BOOL* pVal);
   STDMETHOD(put_RestoreDomains)(VARIANT_BOOL newVal);
   STDMETHOD(get_RestoreMessages)(VARIANT_BOOL* pVal);
   STDMETHOD(put_RestoreMessages)(VARIANT_BOOL newVal);

private:

   std::shared_ptr<HM::Backup> backup_;

};

OBJECT_ENTRY_AUTO(__uuidof(Backup), InterfaceBackup)
