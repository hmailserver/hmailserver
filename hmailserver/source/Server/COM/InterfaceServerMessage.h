// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../hMailServer/resource.h"       // main symbols
#include "../hMailServer/hMailServer.h"

#include "COMCollection.h"

#include "..\Common\BO\ServerMessage.h"
#include "..\Common\BO\ServerMessages.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// InterfaceServerMessage

class ATL_NO_VTABLE InterfaceServerMessage :
   public COMCollectionItem<HM::ServerMessage, HM::ServerMessages>,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceServerMessage, &CLSID_ServerMessage>,
	public IDispatchImpl<IInterfaceServerMessage, &IID_IInterfaceServerMessage, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
	InterfaceServerMessage()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACESERVERMESSAGE)


BEGIN_COM_MAP(InterfaceServerMessage)
	COM_INTERFACE_ENTRY(IInterfaceServerMessage)
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

   STDMETHOD(Save)();
   STDMETHOD(get_ID)(/*[out, retval]*/ long *pVal);
   STDMETHOD(get_Name)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_Name)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_Text)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_Text)(/*[in]*/ BSTR newVal);

};

OBJECT_ENTRY_AUTO(__uuidof(ServerMessage), InterfaceServerMessage)
