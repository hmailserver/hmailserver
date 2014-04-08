// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../hMailServer/resource.h"    // main symbols
#include "../hMaiLServer/hMailServer.h"

#include "COMCollection.h"

namespace HM
{
   class RuleAction;
   class RuleActions;
}

class ATL_NO_VTABLE InterfaceRuleAction : 
   public COMCollectionItem<HM::RuleAction, HM::RuleActions>,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceRuleAction, &CLSID_RuleAction>,
	public IDispatchImpl<IInterfaceRuleAction, &IID_IInterfaceRuleAction, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
	InterfaceRuleAction()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACERULEACTION)


BEGIN_COM_MAP(InterfaceRuleAction)
	COM_INTERFACE_ENTRY(IInterfaceRuleAction)
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
   STDMETHOD(MoveUp)();
   STDMETHOD(MoveDown)();

   STDMETHOD(get_ID)(LONG* pVal);
   
   STDMETHOD(get_RuleID)(LONG* pVal);
   STDMETHOD(put_RuleID)(LONG pVal);

   STDMETHOD(get_Type)(eRuleActionType* pVal);
   STDMETHOD(put_Type)(eRuleActionType pVal);

   STDMETHOD(get_RouteID)(LONG* pVal);
   STDMETHOD(put_RouteID)(LONG pVal);

   STDMETHOD(get_Subject)(BSTR* pVal);
   STDMETHOD(put_Subject)(BSTR pVal);

   STDMETHOD(get_Body)(BSTR* pVal);
   STDMETHOD(put_Body)(BSTR pVal);

   STDMETHOD(get_FromName)(BSTR* pVal);
   STDMETHOD(put_FromName)(BSTR pVal);

   STDMETHOD(get_FromAddress)(BSTR* pVal);
   STDMETHOD(put_FromAddress)(BSTR pVal);


   STDMETHOD(get_To)(BSTR* pVal);
   STDMETHOD(put_To)(BSTR pVal);

   STDMETHOD(get_IMAPFolder)(BSTR* pVal);
   STDMETHOD(put_IMAPFolder)(BSTR pVal);

   STDMETHOD(get_Filename)(BSTR* pVal);
   STDMETHOD(put_Filename)(BSTR pVal);
   
   STDMETHOD(get_ScriptFunction)(BSTR* pVal);
   STDMETHOD(put_ScriptFunction)(BSTR pVal);

   STDMETHOD(get_HeaderName)(BSTR* pVal);
   STDMETHOD(put_HeaderName)(BSTR pVal);

   STDMETHOD(get_Value)(BSTR* pVal);
   STDMETHOD(put_Value)(BSTR pVal);
};

OBJECT_ENTRY_AUTO(__uuidof(RuleAction), InterfaceRuleAction)
