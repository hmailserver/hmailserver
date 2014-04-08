// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../hMailServer/resource.h"       // main symbols
#include "../hMailServer/hMailServer.h"
#include "COMCollection.h"

namespace HM
{  
   class IncomingRelay;
   class IncomingRelays;
}

class ATL_NO_VTABLE InterfaceIncomingRelay :
   public COMCollectionItem<HM::IncomingRelay, HM::IncomingRelays>,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceIncomingRelay, &CLSID_IncomingRelay>,
	public IDispatchImpl<IInterfaceIncomingRelay, &IID_IInterfaceIncomingRelay, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
	InterfaceIncomingRelay()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEINCOMINGRELAY)


BEGIN_COM_MAP(InterfaceIncomingRelay)
	COM_INTERFACE_ENTRY(IInterfaceIncomingRelay)
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

   STDMETHOD(Save)();

   STDMETHOD(get_ID)(/*[out, retval]*/ long *pVal);

   STDMETHOD(get_Name)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_Name)(/*[in]*/ BSTR newVal);

   STDMETHOD(get_LowerIP)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_LowerIP)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_UpperIP)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_UpperIP)(/*[in]*/ BSTR newVal);

   STDMETHOD(Delete)();

};

OBJECT_ENTRY_AUTO(__uuidof(IncomingRelay), InterfaceIncomingRelay)
