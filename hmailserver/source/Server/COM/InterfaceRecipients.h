// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once
#include "../hMailServer/resource.h"       // main symbols

#include "../hMailServer/hMailServer.h"

namespace HM {class Message; }

class ATL_NO_VTABLE InterfaceRecipients : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceRecipients, &CLSID_Recipients>,
	public IDispatchImpl<IInterfaceRecipients, &IID_IInterfaceRecipients, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
	InterfaceRecipients()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACERECIPIENTS)


BEGIN_COM_MAP(InterfaceRecipients)
	COM_INTERFACE_ENTRY(IInterfaceRecipients)
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
   
   void Attach(std::shared_ptr<HM::Message> pMessage);

   STDMETHOD(get_Item)(/*[in]*/ long Index, /*[out, retval]*/ IInterfaceRecipient **pVal);
   STDMETHOD(get_Count)(/*[out, retval]*/ long *pVal);


private:

   std::shared_ptr<HM::Message> message_;
};

OBJECT_ENTRY_AUTO(__uuidof(Recipients), InterfaceRecipients)
