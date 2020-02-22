// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../hMailServer/resource.h"       // main symbols
#include "../hMailServer/hMailServer.h"

#include "../Common/bo/IncomingRelays.h"

// InterfaceIncomingRelays

class ATL_NO_VTABLE InterfaceIncomingRelays :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceIncomingRelays, &CLSID_IncomingRelays>,
	public IDispatchImpl<IInterfaceIncomingRelays, &IID_IInterfaceIncomingRelays, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
	InterfaceIncomingRelays()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEINCOMINGRELAYS)


BEGIN_COM_MAP(InterfaceIncomingRelays)
	COM_INTERFACE_ENTRY(IInterfaceIncomingRelays)
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

   STDMETHOD(get_Item)(/*[in]*/ long Index, /*[out, retval]*/ IInterfaceIncomingRelay **pVal);
   STDMETHOD(get_ItemByDBID)(/*[in]*/ long DBID, /*[out, retval]*/ IInterfaceIncomingRelay **pVal);
   STDMETHOD(get_ItemByName)(/*[in]*/ BSTR sName, /*[out, retval]*/ IInterfaceIncomingRelay **pVal);
   STDMETHOD(get_Count)(/*[out, retval]*/ long *pVal);
   STDMETHOD(Delete)(/*[in]*/ long Index);
   STDMETHOD(DeleteByDBID)(/*[in]*/ long DBID);
   STDMETHOD(Refresh)();
   STDMETHOD(Add)(/*[out, retval]*/ IInterfaceIncomingRelay** pVal);

   void Attach(std::shared_ptr<HM::IncomingRelays> pRanges);
private:


   std::shared_ptr<HM::IncomingRelays> incoming_relays_;
};

OBJECT_ENTRY_AUTO(__uuidof(IncomingRelays), InterfaceIncomingRelays)
