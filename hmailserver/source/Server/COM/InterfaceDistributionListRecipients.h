// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once
#include "../hMailServer/resource.h"       // main symbols

#include "../hMailServer/hMailServer.h"

namespace HM
{
   class DistributionListRecipients;
}

class ATL_NO_VTABLE InterfaceDistributionListRecipients : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceDistributionListRecipients, &CLSID_DistributionListRecipients>,
	public IDispatchImpl<IInterfaceDistributionListRecipients, &IID_IInterfaceDistributionListRecipients, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
	InterfaceDistributionListRecipients()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEDISTRIBUTIONLISTRECIPIENTS)


BEGIN_COM_MAP(InterfaceDistributionListRecipients)
	COM_INTERFACE_ENTRY(IInterfaceDistributionListRecipients)
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
   STDMETHOD(get_Count)(/*[out, retval]*/ long *pVal);
   STDMETHOD(get_Item)(/*[in]*/ long Index, /*[out, retval]*/ IInterfaceDistributionListRecipient **pVal);
   STDMETHOD(get_ItemByDBID)(/*[in]*/ long DBID, /*[out, retval]*/ IInterfaceDistributionListRecipient **pVal);
   STDMETHOD(Delete)(/*[in]*/ long Index);
   STDMETHOD(Add)(/*[out, retval]*/ IInterfaceDistributionListRecipient** pVal);
   STDMETHOD(DeleteByDBID)(/*[in]*/ long DBID);
   STDMETHOD(Refresh)();

   void Attach(std::shared_ptr<HM::DistributionListRecipients> pRecipients);
   void SetListID(__int64 lListID) {list_id_ = lListID; }

private:


   std::shared_ptr<HM::DistributionListRecipients> recipients_;
   __int64 list_id_;

};

OBJECT_ENTRY_AUTO(__uuidof(DistributionListRecipients), InterfaceDistributionListRecipients)
