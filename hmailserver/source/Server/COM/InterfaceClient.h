// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../hMailServer/resource.h"       // main symbols

#include "../hMailServer/hMailServer.h"


// InterfaceClient

namespace HM
{
   class ClientInfo;
}

class ATL_NO_VTABLE InterfaceClient : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceClient, &CLSID_Client>,
	public IDispatchImpl<IInterfaceClient, &IID_IInterfaceClient, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	InterfaceClient()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACECLIENT)


BEGIN_COM_MAP(InterfaceClient)
	COM_INTERFACE_ENTRY(IInterfaceClient)
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
   void AttachItem(std::shared_ptr<HM::ClientInfo> pClientInfo);

   STDMETHOD(get_Port)(LONG* pVal);
   STDMETHOD(get_IPAddress)(BSTR* pVal);
   STDMETHOD(get_Username)(BSTR* pVal);
   STDMETHOD(get_HELO)(BSTR* pVal);
   STDMETHOD(get_Authenticated)(VARIANT_BOOL* pVal);
   STDMETHOD(get_EncryptedConnection)(VARIANT_BOOL* pVal);
   STDMETHOD(get_CipherVersion)(BSTR* pVal);
   STDMETHOD(get_CipherName)(BSTR* pVal);
   STDMETHOD(get_CipherBits)(LONG* pVal);
   STDMETHOD(get_SessionID)(LONG* pVal);

private:

   std::shared_ptr<HM::ClientInfo> client_info_;
};

OBJECT_ENTRY_AUTO(__uuidof(Client), InterfaceClient)
