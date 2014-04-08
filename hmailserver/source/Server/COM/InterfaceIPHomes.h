// InterfaceIPHomes.h : Declaration of the InterfaceIPHomes

#pragma once
#include "../hMailServer/resource.h"       // main symbols

#include "../hMailServer/hMailServer.h"

#include "../Common/bo/IPHomes.h"

class ATL_NO_VTABLE InterfaceIPHomes : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceIPHomes, &CLSID_IPHomes>,
	public IDispatchImpl<IInterfaceIPHomes, &IID_IInterfaceIPHomes, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
	InterfaceIPHomes()
	{
	}

   bool LoadSettings();

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEIPHOMES)


BEGIN_COM_MAP(InterfaceIPHomes)
	COM_INTERFACE_ENTRY(IInterfaceIPHomes)
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

   STDMETHOD(get_Item)(/*[in]*/ long Index, /*[out, retval]*/ IInterfaceIPHome **pVal);
   STDMETHOD(get_ItemByDBID)(/*[in]*/ long DBID, /*[out, retval]*/ IInterfaceIPHome **pVal);
   STDMETHOD(get_Count)(/*[out, retval]*/ long *pVal);
   STDMETHOD(DeleteByDBID)(/*[in]*/ long DBID);
   STDMETHOD(Refresh)();
   STDMETHOD(Add)(/*[out, retval]*/ IInterfaceIPHome** pVal);

private:


   shared_ptr<HM::IPHomes> m_pIPHomes;

};

OBJECT_ENTRY_AUTO(__uuidof(IPHomes), InterfaceIPHomes)
