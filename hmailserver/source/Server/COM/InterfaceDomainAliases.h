// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once
#include "../hMailServer/resource.h"       // main symbols

#include "../hMailServer/hMailServer.h"

namespace HM
{
   class DomainAliases;
}



class ATL_NO_VTABLE InterfaceDomainAliases : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceDomainAliases, &CLSID_DomainAliases>,
	public IDispatchImpl<IInterfaceDomainAliases, &IID_IInterfaceDomainAliases, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator

{
public:
	InterfaceDomainAliases()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEDOMAINALIASES)


BEGIN_COM_MAP(InterfaceDomainAliases)
	COM_INTERFACE_ENTRY(IInterfaceDomainAliases)
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

   void Attach(std::shared_ptr<HM::DomainAliases> pDomainAliases);

public:

   STDMETHOD(get_Count)(LONG* pVal);

   STDMETHOD(get_ItemByDBID)(/*[in]*/ long DBID, /*[out, retval]*/ IInterfaceDomainAlias** pVal);
   STDMETHOD(get_Item)(/*[in]*/ long Index, /*[out, retval]*/ IInterfaceDomainAlias **pVal);
   STDMETHOD(Refresh)();
   STDMETHOD(Delete)(/*[in]*/ long Index);
   STDMETHOD(DeleteByDBID)(/*[in]*/ long Index);

   STDMETHOD(Add)(/*[out, retval]*/ IInterfaceDomainAlias **pVal);

private:

   std::shared_ptr<HM::DomainAliases> domain_aliases_;

};

OBJECT_ENTRY_AUTO(__uuidof(DomainAliases), InterfaceDomainAliases)
