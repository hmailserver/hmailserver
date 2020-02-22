// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../hMailServer/resource.h"       // main symbols
#include "../hMailServer/hMailServer.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

namespace HM
{
   class IMAPFolder;
   class ACLPermissions;
}

// InterfaceIMAPFolderPermissions

class ATL_NO_VTABLE InterfaceIMAPFolderPermissions :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceIMAPFolderPermissions, &CLSID_IMAPFolderPermissions>,
	public IDispatchImpl<IInterfaceIMAPFolderPermissions, &IID_IInterfaceIMAPFolderPermissions, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
	InterfaceIMAPFolderPermissions()
	{
	}

   void AttachItem(std::shared_ptr<HM::IMAPFolder> folder_);
   


DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEIMAPFOLDERPERMISSIONS)


BEGIN_COM_MAP(InterfaceIMAPFolderPermissions)
	COM_INTERFACE_ENTRY(IInterfaceIMAPFolderPermissions)
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
   STDMETHOD(get_ItemByDBID)(/*[in]*/ long DBID, /*[out, retval]*/ IInterfaceIMAPFolderPermission* *pVal);
   STDMETHOD(Add)(/*[out, retval]*/ IInterfaceIMAPFolderPermission **pVal);
   STDMETHOD(get_Item)(/*[in]*/ long Index, /*[out, retval]*/ IInterfaceIMAPFolderPermission **pVal);
   STDMETHOD(Refresh)();
   STDMETHOD(Delete)(/*[in]*/ long Index);
   STDMETHOD(get_Count)(/*[out, retval]*/ long *pVal);
   STDMETHOD(DeleteByDBID)(/*[in]*/ long Index);
   STDMETHOD(get_ItemByName)(BSTR Name, IInterfaceIMAPFolderPermission **pVal);

private:

   std::shared_ptr<HM::IMAPFolder> folder_;
   std::shared_ptr<HM::ACLPermissions> acl_permission_;
};

OBJECT_ENTRY_AUTO(__uuidof(IMAPFolderPermissions), InterfaceIMAPFolderPermissions)
