// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once
#include "../hMailServer/resource.h"       // main symbols

#include "../hMailServer/hMailServer.h"

#include "../common/bo/Messages.h"

// InterfaceMessages

class ATL_NO_VTABLE InterfaceMessages : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceMessages, &CLSID_Messages>,
	public IDispatchImpl<IInterfaceMessages, &IID_IInterfaceMessages, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
	InterfaceMessages()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEMESSAGES)


BEGIN_COM_MAP(InterfaceMessages)
	COM_INTERFACE_ENTRY(IInterfaceMessages)
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
   STDMETHOD(DeleteByDBID)(hyper lDBID);
   STDMETHOD(get_ItemByDBID)(hyper DBID, /*[out, retval]*/ IInterfaceMessage* *pVal);
   STDMETHOD(get_Item)(long Index, /*[out, retval]*/ IInterfaceMessage* *pVal);
   STDMETHOD(get_Count)(/*[out, retval]*/ long *pVal);
   STDMETHOD(Add)(IInterfaceMessage **pVal);

   void Attach(std::shared_ptr<HM::Messages> pMessages);
   STDMETHOD(Clear)();

private:

   std::shared_ptr<HM::Messages> messages_;
};

OBJECT_ENTRY_AUTO(__uuidof(Messages), InterfaceMessages)
