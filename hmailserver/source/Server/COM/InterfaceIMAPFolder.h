// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../hMailServer/resource.h"       // main symbols
#include "../hMailServer/hMailServer.h"

#include "../common/BO/IMAPFolder.h"
#include "../common/BO/IMAPFolders.h"

#include "COMCollection.h"


// InterfaceIMAPFolder

class ATL_NO_VTABLE InterfaceIMAPFolder : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceIMAPFolder, &CLSID_IMAPFolder>,
	public IDispatchImpl<IInterfaceIMAPFolder, &IID_IInterfaceIMAPFolder, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator,
   public ISupportErrorInfo,
   public COMCollectionItem<HM::IMAPFolder, HM::IMAPFolders>

{
public:
	InterfaceIMAPFolder()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEIMAPFOLDER)


BEGIN_COM_MAP(InterfaceIMAPFolder)
	COM_INTERFACE_ENTRY(IInterfaceIMAPFolder)
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

   HRESULT STDMETHODCALLTYPE get_ID(LONG* pVal)
   {
      *pVal = (long) object_->GetID();
      return S_OK;
   }

   void Attach(std::shared_ptr<HM::IMAPFolder> pFolder);

   STDMETHOD(get_ParentID)(/*[out, retval]*/ LONG *pVal);
   STDMETHOD(get_CurrentUID)(/*[out, retval]*/ LONG *pVal);
   STDMETHOD(get_CreationTime)(/*[out, retval]*/ BSTR *pVal);

   STDMETHOD(get_Name)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_Name)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_Subscribed)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_Subscribed)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_Messages)(/*[out, retval]*/ IInterfaceMessages **pVal);
   STDMETHOD(get_SubFolders)(/*[out, retval]*/ IInterfaceIMAPFolders **pVal);
   STDMETHOD(get_Permissions)(/*[out, retval]*/ IInterfaceIMAPFolderPermissions **pVal);
   
   

   STDMETHOD(Save)();
   STDMETHOD(Delete)();

public:
   std::shared_ptr<HM::IMAPFolder> object_;
};

OBJECT_ENTRY_AUTO(__uuidof(IMAPFolder), InterfaceIMAPFolder)
