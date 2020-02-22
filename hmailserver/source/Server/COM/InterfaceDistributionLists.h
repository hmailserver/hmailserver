// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once
#include "../hMailServer/resource.h"       // main symbols

#include "../hMailServer/hMailServer.h"

namespace HM
{
   class DistributionLists;
   class DistributionList;
}

// InterfaceDistributionLists

class ATL_NO_VTABLE InterfaceDistributionLists : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceDistributionLists, &CLSID_DistributionLists>,
	public IDispatchImpl<IInterfaceDistributionLists, &IID_IInterfaceDistributionLists, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
	InterfaceDistributionLists()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEDISTRIBUTIONLISTS)


BEGIN_COM_MAP(InterfaceDistributionLists)
	COM_INTERFACE_ENTRY(IInterfaceDistributionLists)
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
   STDMETHOD(DeleteByDBID)(/*[in]*/ long DBID);
   STDMETHOD(get_Count)(/*[out, retval]*/ long *pVal);   
   STDMETHOD(get_ItemByDBID)(/*[in]*/ long DBID, /*[out, retval]*/ IInterfaceDistributionList **pVal);
   STDMETHOD(Refresh)();
   STDMETHOD(get_Item)(/*[in]*/ long Index, /*[out, retval]*/ IInterfaceDistributionList **pVal);
   STDMETHOD(Delete)(/*[in]*/ long Index);
   STDMETHOD(Add)(/*[out, retval]*/ IInterfaceDistributionList** pVal);
   STDMETHOD(get_ItemByAddress)(/*[in]*/ BSTR sAddress, /*[out, retval]*/ IInterfaceDistributionList **pVal);

   void Attach(std::shared_ptr<HM::DistributionLists> pDistributionLists);
   void SetDomain(__int64 Domain) { domain_id_ = Domain; }

private:
   void InternalAdd(std::shared_ptr<HM::DistributionList> pObject);

   std::shared_ptr<HM::DistributionLists> distribution_lists_;
   __int64 domain_id_;

};

OBJECT_ENTRY_AUTO(__uuidof(DistributionLists), InterfaceDistributionLists)
