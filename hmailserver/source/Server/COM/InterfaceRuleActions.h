// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../hMailServer/resource.h"    // main symbols
#include "../hMaiLServer/hMailServer.h"

namespace HM
{
   class RuleActions;
}


class ATL_NO_VTABLE InterfaceRuleActions : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceRuleActions, &CLSID_RuleActions>,
	public IDispatchImpl<IInterfaceRuleActions, &IID_IInterfaceRuleActions, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
	InterfaceRuleActions()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACERULEACTIONS)


BEGIN_COM_MAP(InterfaceRuleActions)
	COM_INTERFACE_ENTRY(IInterfaceRuleActions)
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
   
   void Attach(std::shared_ptr<HM::RuleActions> pActions);

   STDMETHOD(get_ItemByDBID)(/*[in]*/ long DBID, /*[out, retval]*/ IInterfaceRuleAction** pVal);
   STDMETHOD(get_Item)(/*[in]*/ long Index, /*[out, retval]*/ IInterfaceRuleAction **pVal);
   STDMETHOD(get_Count)(LONG* pVal);
   STDMETHOD(Add)(IInterfaceRuleAction** pVal);
   STDMETHOD(DeleteByDBID)(LONG DBID);
   STDMETHOD(Refresh)(void);
   STDMETHOD(Delete)(LONG DBID);

private:

   std::shared_ptr<HM::RuleActions> rule_actions_;

};

OBJECT_ENTRY_AUTO(__uuidof(RuleActions), InterfaceRuleActions)
