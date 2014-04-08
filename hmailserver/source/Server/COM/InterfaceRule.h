// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once
#include "..\hMailServer\resource.h"       // main symbols

#include "..\hMailServer\hMailServer.h"

#include "..\COM\COMCollection.h"

namespace HM 
{ 
   class Rule; 
   class Rules;
}

class ATL_NO_VTABLE InterfaceRule : 
    public COMCollectionItem<HM::Rule, HM::Rules>,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceRule, &CLSID_Rule>,
	public IDispatchImpl<IInterfaceRule, &IID_IInterfaceRule, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
	InterfaceRule()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACERULE)


BEGIN_COM_MAP(InterfaceRule)
	COM_INTERFACE_ENTRY(IInterfaceRule)
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

   STDMETHOD(get_ID)(/*[out, retval]*/ long *pVal);

   STDMETHOD(Save)();
   STDMETHOD(Delete)();

   STDMETHOD(MoveUp)();
   STDMETHOD(MoveDown)();

   STDMETHOD(get_AccountID)(LONG* pVal);
   STDMETHOD(put_AccountID)(LONG pVal);
   STDMETHOD(get_Name)(BSTR* pVal);
   STDMETHOD(put_Name)(BSTR newVal);
   STDMETHOD(get_Active)(VARIANT_BOOL* pVal);
   STDMETHOD(put_Active)(VARIANT_BOOL newVal);
   STDMETHOD(get_UseAND)(VARIANT_BOOL* pVal);
   STDMETHOD(put_UseAND)(VARIANT_BOOL newVal);
   STDMETHOD(get_Criterias)(/*[out, retval]*/ IInterfaceRuleCriterias **pVal);
   STDMETHOD(get_Actions)(/*[out, retval]*/ IInterfaceRuleActions **pVal);
};


OBJECT_ENTRY_AUTO(__uuidof(Rule), InterfaceRule)
