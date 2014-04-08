// InterfaceIPHome.h : Declaration of the InterfaceIPHome

#pragma once
#include "../hMailServer/resource.h"       // main symbols

#include "../hMailServer/hMailServer.h"

namespace HM
{  
   class IPHome;
   class IPHomes;
}

#include "COMCollection.h"

class ATL_NO_VTABLE InterfaceIPHome : 
   public COMCollectionItem<HM::IPHome, HM::IPHomes>,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceIPHome, &CLSID_IPHome>,
	public IDispatchImpl<IInterfaceIPHome, &IID_IInterfaceIPHome, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
	InterfaceIPHome()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEIPHOME)


BEGIN_COM_MAP(InterfaceIPHome)
	COM_INTERFACE_ENTRY(IInterfaceIPHome)
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
   STDMETHOD(get_IPAddress)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_IPAddress)(/*[in]*/ BSTR newVal);


};

OBJECT_ENTRY_AUTO(__uuidof(IPHome), InterfaceIPHome)
