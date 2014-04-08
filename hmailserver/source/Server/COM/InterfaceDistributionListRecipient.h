// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once
#include "../hMailServer/resource.h"       // main symbols

#include "../hMailServer/hMailServer.h"

#include "COMCollection.h"

namespace HM
{
   class DistributionListRecipient;
   class DistributionListRecipients;
}

class ATL_NO_VTABLE InterfaceDistributionListRecipient : 
   public COMCollectionItem<HM::DistributionListRecipient, HM::DistributionListRecipients>,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceDistributionListRecipient, &CLSID_DistributionListRecipient>,
	public IDispatchImpl<IInterfaceDistributionListRecipient, &IID_IInterfaceDistributionListRecipient, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator,
   public ISupportErrorInfo
{
public:
	InterfaceDistributionListRecipient()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEDISTRIBUTIONLISTRECIPIENT)


BEGIN_COM_MAP(InterfaceDistributionListRecipient)
	COM_INTERFACE_ENTRY(IInterfaceDistributionListRecipient)
	COM_INTERFACE_ENTRY(IDispatch)
   COM_INTERFACE_ENTRY(ISupportErrorInfo)
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

   STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

   STDMETHOD(get_ID)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_ID)(/*[in]*/ long newVal);
   STDMETHOD(get_RecipientAddress)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_RecipientAddress)(/*[in]*/ BSTR newVal);
   STDMETHOD(Delete)();
   STDMETHOD(Save)();

private:

};

OBJECT_ENTRY_AUTO(__uuidof(DistributionListRecipient), InterfaceDistributionListRecipient)
