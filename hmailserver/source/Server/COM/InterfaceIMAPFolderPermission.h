// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../hMailServer/resource.h"       // main symbols
#include "../hMailServer/hMailServer.h"

#include "COMCollection.h"

#include "../Common/BO/ACLPermission.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

namespace HM
{
   class ACLPermission;
   class ACLPermissions;
}

// InterfaceIMAPFolderPermission

class ATL_NO_VTABLE InterfaceIMAPFolderPermission :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceIMAPFolderPermission, &CLSID_IMAPFolderPermission>,
	public IDispatchImpl<IInterfaceIMAPFolderPermission, &IID_IInterfaceIMAPFolderPermission, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public COMCollectionItem<HM::ACLPermission, HM::ACLPermissions>,
   public HM::COMAuthenticator,
   public ISupportErrorInfo
{
public:

   InterfaceIMAPFolderPermission()
	{
	}


DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEIMAPFOLDERPERMISSION)


BEGIN_COM_MAP(InterfaceIMAPFolderPermission)
	COM_INTERFACE_ENTRY(IInterfaceIMAPFolderPermission)
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

public:

   HRESULT STDMETHODCALLTYPE get_ID(LONG* pVal)
   {
      *pVal = (long) object_->GetID();

      return S_OK;
   }

   STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

   STDMETHOD(Save)();
   STDMETHOD(Delete)();

   STDMETHOD(get_ShareFolderID)(/*[out, retval]*/ long *pVal);
   STDMETHOD(get_PermissionGroupID)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_PermissionGroupID)(/*[in]*/ long newVal);
   STDMETHOD(get_PermissionAccountID)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_PermissionAccountID)(/*[in]*/ long newVal);
   STDMETHOD(get_PermissionType)(/*[out, retval]*/ eACLPermissionType *pVal);
   STDMETHOD(put_PermissionType)(/*[in]*/ eACLPermissionType newVal);
   STDMETHOD(get_Value)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_Value)(/*[in]*/ long newVal);
   STDMETHOD(get_Permission)(eACLPermission iFlag, VARIANT_BOOL *pVal);
   STDMETHOD(put_Permission)(eACLPermission iFlag, VARIANT_BOOL newVal);

   STDMETHOD(get_Account)(/*[out, retval]*/ IInterfaceAccount **pVal);
   STDMETHOD(get_Group)(/*[out, retval]*/ IInterfaceGroup **pVal);


private:

};

OBJECT_ENTRY_AUTO(__uuidof(IMAPFolderPermission), InterfaceIMAPFolderPermission)
