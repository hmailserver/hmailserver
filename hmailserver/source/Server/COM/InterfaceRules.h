// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../hMailServer/resource.h"    // main symbols
#include "../hMaiLServer/hMailServer.h"



// InterfaceRules
namespace HM
{
   class Rules;
}

class ATL_NO_VTABLE InterfaceRules : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceRules, &CLSID_Rules>,
	public IDispatchImpl<IInterfaceRules, &IID_IInterfaceRules, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
	InterfaceRules()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACERULES)


BEGIN_COM_MAP(InterfaceRules)
	COM_INTERFACE_ENTRY(IInterfaceRules)
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

   void Attach(std::shared_ptr<HM::Rules> pRules);

   STDMETHOD(get_ItemByDBID)(/*[in]*/ long DBID, /*[out, retval]*/ IInterfaceRule** pVal);
   STDMETHOD(get_Item)(/*[in]*/ long Index, /*[out, retval]*/ IInterfaceRule **pVal);
   STDMETHOD(get_Count)(LONG* pVal);
   STDMETHOD(Add)(IInterfaceRule** pVal);
   STDMETHOD(DeleteByDBID)(LONG DBID);
   STDMETHOD(Refresh)(void);

private:

   std::shared_ptr<HM::Rules> rules_;
};

OBJECT_ENTRY_AUTO(__uuidof(Rules), InterfaceRules)
