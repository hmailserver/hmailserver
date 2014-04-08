// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../hMailServer/resource.h"       // main symbols
#include "../hMailServer/hMailServer.h"


// InterfaceLanguages

class ATL_NO_VTABLE InterfaceLanguages :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceLanguages, &CLSID_Languages>,
	public IDispatchImpl<IInterfaceLanguages, &IID_IInterfaceLanguages, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	InterfaceLanguages()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACELANGUAGES)


BEGIN_COM_MAP(InterfaceLanguages)
	COM_INTERFACE_ENTRY(IInterfaceLanguages)
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

   STDMETHOD(get_Item)(/*[in]*/ long Index, /*[out, retval]*/ IInterfaceLanguage* *pVal);
   STDMETHOD(get_ItemByName)(/*[in]*/ BSTR ItemName, /*[out, retval]*/ IInterfaceLanguage** pVal);
   STDMETHOD(get_Count)(/*[out, retval]*/ long *pVal);

};

OBJECT_ENTRY_AUTO(__uuidof(Languages), InterfaceLanguages)
