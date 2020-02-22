// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../hMailServer/resource.h"       // main symbols
#include "../hMailServer/hMailServer.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

namespace HM
{
   class MimeHeader;
   class MimeField; 
}

// InterfaceMessageHeader

class ATL_NO_VTABLE InterfaceMessageHeader :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceMessageHeader, &CLSID_MessageHeader>,
	public IDispatchImpl<IInterfaceMessageHeader, &IID_IInterfaceMessageHeader, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
   InterfaceMessageHeader();
	
   void AttachItem (std::shared_ptr<HM::MimeHeader> pHeader, HM::MimeField *pField);

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEMESSAGEHEADER)


BEGIN_COM_MAP(InterfaceMessageHeader)
	COM_INTERFACE_ENTRY(IInterfaceMessageHeader)
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
   STDMETHOD(Delete)();

   STDMETHOD(get_Name)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_Name)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_Value)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_Value)(/*[in]*/ BSTR newVal);

private:

   std::shared_ptr<HM::MimeHeader> header_;
   HM::MimeField *object_;

};

OBJECT_ENTRY_AUTO(__uuidof(MessageHeader), InterfaceMessageHeader)
