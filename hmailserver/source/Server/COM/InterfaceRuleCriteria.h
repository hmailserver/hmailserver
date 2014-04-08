// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../hMailServer/resource.h"    // main symbols

#include "../hMaiLServer/hMailServer.h"

#include "COMCollection.h"

namespace HM
{
   class RuleCriteria;
   class RuleCriterias;
}

class ATL_NO_VTABLE InterfaceRuleCriteria : 
   public COMCollectionItem<HM::RuleCriteria, HM::RuleCriterias>,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceRuleCriteria, &CLSID_RuleCriteria>,
	public IDispatchImpl<IInterfaceRuleCriteria, &IID_IInterfaceRuleCriteria, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
	InterfaceRuleCriteria()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACERULECRITERIA)


BEGIN_COM_MAP(InterfaceRuleCriteria)
	COM_INTERFACE_ENTRY(IInterfaceRuleCriteria)
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
   STDMETHOD(Delete)();

   STDMETHOD(get_ID)(LONG* pVal);

   STDMETHOD(get_RuleID)(LONG* pVal);
   STDMETHOD(put_RuleID)(LONG pVal);

   STDMETHOD(get_UsePredefined)(VARIANT_BOOL* pVal);
   STDMETHOD(put_UsePredefined)(VARIANT_BOOL pVal);

   STDMETHOD(get_PredefinedField)(eRulePredefinedField* pVal);
   STDMETHOD(put_PredefinedField)(eRulePredefinedField pVal);

   STDMETHOD(get_HeaderField)(BSTR* pVal);
   STDMETHOD(put_HeaderField)(BSTR pVal);

   STDMETHOD(get_MatchType)(eRuleMatchType* pVal);
   STDMETHOD(put_MatchType)(eRuleMatchType pVal);

   STDMETHOD(get_MatchValue)(BSTR* pVal);
   STDMETHOD(put_MatchValue)(BSTR pVal);


};

OBJECT_ENTRY_AUTO(__uuidof(RuleCriteria), InterfaceRuleCriteria)
