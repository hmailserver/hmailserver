// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../hMailServer/resource.h"       // main symbols
#include "../hMailServer/hMailServer.h"


// InterfaceBackupManager

class ATL_NO_VTABLE InterfaceBackupManager : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceBackupManager, &CLSID_BackupManager>,
	public IDispatchImpl<IInterfaceBackupManager, &IID_IInterfaceBackupManager, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
	InterfaceBackupManager()
	{
	}

   bool LoadSettings();

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEBACKUPMANAGER)


BEGIN_COM_MAP(InterfaceBackupManager)
	COM_INTERFACE_ENTRY(IInterfaceBackupManager)
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
   STDMETHOD(StartBackup)();
   STDMETHOD(LoadBackup)(/*[in]*/ BSTR sXMLFile, /*[out, retval]*/ IInterfaceBackup **pVal);

private:
   std::shared_ptr<HM::BackupManager> backup_manager_;
};

OBJECT_ENTRY_AUTO(__uuidof(BackupManager), InterfaceBackupManager)
