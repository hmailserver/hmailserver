// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once
#include "../hMailServer/resource.h"       // main symbols

#include "../hMailServer/hMailServer.h"

#include "../common/bo/DNSBlackLists.h"


class ATL_NO_VTABLE InterfaceDNSBlackLists : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceDNSBlackLists, &CLSID_DNSBlackLists>,
	public IDispatchImpl<IInterfaceDNSBlackLists, &IID_IInterfaceDNSBlackLists, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
	InterfaceDNSBlackLists()
	{
	}

   bool LoadSettings();

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEDNSBLACKLISTS)


BEGIN_COM_MAP(InterfaceDNSBlackLists)
	COM_INTERFACE_ENTRY(IInterfaceDNSBlackLists)
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

   STDMETHOD(Refresh)();

   STDMETHOD(get_Item)(/*[in]*/ long Index, /*[out, retval]*/ IInterfaceDNSBlackList **pVal);
   STDMETHOD(get_Count)(/*[out, retval]*/ long *pVal);
   STDMETHOD(get_ItemByDBID)(/*[in]*/ long lDBID, /*[out, retval]*/ IInterfaceDNSBlackList** pVal);
   STDMETHOD(DeleteByDBID)(/*[in]*/ long DBID);
   STDMETHOD(Add)(/*[out, retval]*/ IInterfaceDNSBlackList **pVal);
   STDMETHOD(get_ItemByDNSHost)(BSTR ItemName, IInterfaceDNSBlackList **pVal);

   void Attach(std::shared_ptr<HM::DNSBlackLists> pBlackLists);

public:

   std::shared_ptr<HM::DNSBlackLists> black_lists_;

};

OBJECT_ENTRY_AUTO(__uuidof(DNSBlackLists), InterfaceDNSBlackLists)
