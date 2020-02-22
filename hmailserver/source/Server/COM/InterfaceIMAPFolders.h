// Copyright (c) 2006 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../hMailServer/resource.h"       // main symbols
#include "../hMailServer/hMailServer.h"

namespace HM
{
   class IMAPFolders;
   class IMAPFolder;
}

class ATL_NO_VTABLE InterfaceIMAPFolders : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceIMAPFolders, &CLSID_IMAPFolders>,
	public IDispatchImpl<IInterfaceIMAPFolders, &IID_IInterfaceIMAPFolders, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator,
   public ISupportErrorInfo
{
public:
	InterfaceIMAPFolders()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEIMAPFOLDERS)


BEGIN_COM_MAP(InterfaceIMAPFolders)
	COM_INTERFACE_ENTRY(IInterfaceIMAPFolders)
	COM_INTERFACE_ENTRY(IDispatch)
   COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

   STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

   STDMETHOD(get_Item)(/*[in]*/ long Index, /*[out, retval]*/ IInterfaceIMAPFolder **pVal);
   STDMETHOD(get_Count)(/*[out, retval]*/ long *pVal);
   STDMETHOD(get_ItemByDBID)(/*[in]*/ long lDBID, /*[out, retval]*/ IInterfaceIMAPFolder** pVal);
   STDMETHOD(get_ItemByName)(BSTR Name, IInterfaceIMAPFolder **pVal);
   STDMETHOD(Add)(/*[in]*/ BSTR sName, /*[out, retval]*/ IInterfaceIMAPFolder **pVal);
   STDMETHOD(DeleteByDBID)(/*[in]*/ long lDBID);

   void Attach(std::shared_ptr<HM::IMAPFolders> pFolders);
public:
   
   std::shared_ptr<HM::IMAPFolders> object_;

};

OBJECT_ENTRY_AUTO(__uuidof(IMAPFolders), InterfaceIMAPFolders)
