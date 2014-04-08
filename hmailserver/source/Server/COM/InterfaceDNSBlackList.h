// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once
#include "../hMailServer/resource.h"       // main symbols

#include "../hMailServer/hMailServer.h"

#include "COMCollection.h"

#include "..\Common\BO\DNSBlackList.h"

namespace HM 
{ 
   class DNSBlackLists; 
   class DNSBlackList;
}

class ATL_NO_VTABLE InterfaceDNSBlackList : 
   public COMCollectionItem<HM::DNSBlackList, HM::DNSBlackLists>,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceDNSBlackList, &CLSID_DNSBlackList>,
	public IDispatchImpl<IInterfaceDNSBlackList, &IID_IInterfaceDNSBlackList, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
	InterfaceDNSBlackList()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEDNSBLACKLIST)


BEGIN_COM_MAP(InterfaceDNSBlackList)
	COM_INTERFACE_ENTRY(IInterfaceDNSBlackList)
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


   STDMETHOD(Save)();
   STDMETHOD(Delete)();

   STDMETHOD(get_Active)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_Active)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_ID)(/*[out, retval]*/ long *pVal);

   STDMETHOD(get_DNSHost)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_DNSHost)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_RejectMessage)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_RejectMessage)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_ExpectedResult)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_ExpectedResult)(/*[in]*/ BSTR newVal);

   STDMETHOD(get_Score)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_Score)(/*[in]*/ long newVal);

public:

};

OBJECT_ENTRY_AUTO(__uuidof(DNSBlackList), InterfaceDNSBlackList)
