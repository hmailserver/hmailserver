// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once
#include "../hMailServer/resource.h"       // main symbols

#include "../hMailServer/hMailServer.h"

#include "../common/bo/Routes.h"

namespace HM
{
   class Route;
}

class ATL_NO_VTABLE InterfaceRoutes : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceRoutes, &CLSID_Routes>,
	public IDispatchImpl<IInterfaceRoutes, &IID_IInterfaceRoutes, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
	InterfaceRoutes()
	{
	}

   bool LoadSettings();

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEROUTES)


BEGIN_COM_MAP(InterfaceRoutes)
	COM_INTERFACE_ENTRY(IInterfaceRoutes)
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

   STDMETHOD(get_Item)(/*[in]*/ long Index, /*[out, retval]*/ IInterfaceRoute **pVal);
   STDMETHOD(get_Count)(/*[out, retval]*/ long *pVal);

   STDMETHOD(get_ItemByName)(/*[in]*/ BSTR ItemName, /*[out, retval]*/ IInterfaceRoute** pVal);
   STDMETHOD(get_ItemByDBID)(/*[in]*/ long lDBID, /*[out, retval]*/ IInterfaceRoute** pVal);
   STDMETHOD(DeleteByDBID)(/*[in]*/ long DBID);

   STDMETHOD(Add)(/*[out, retval]*/ IInterfaceRoute **pVal);

   STDMETHOD(Refresh)();

   void Attach(std::shared_ptr<HM::Routes> pRoutes);

   void InternalAdd(std::shared_ptr<HM::Route> pObject);

public:

   std::shared_ptr<HM::Routes> routes_;

};

OBJECT_ENTRY_AUTO(__uuidof(Routes), InterfaceRoutes)
