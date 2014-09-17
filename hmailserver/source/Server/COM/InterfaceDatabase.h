// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once
#include "../hMailServer/resource.h"       // main symbols

#include "../hMailServer/hMailServer.h"

namespace HM
{
   class IniFileSettings;
}

// InterfaceDatabase

class ATL_NO_VTABLE InterfaceDatabase : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceDatabase, &CLSID_Database>,
   public ISupportErrorInfo,
	public IDispatchImpl<IInterfaceDatabase, &IID_IInterfaceDatabase, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
	InterfaceDatabase();
   ~InterfaceDatabase();

   bool LoadSettings();

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACEDATABASE)


BEGIN_COM_MAP(InterfaceDatabase)
	COM_INTERFACE_ENTRY(IInterfaceDatabase)
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

   STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

   STDMETHOD(ExecuteSQL)(BSTR sSQLStatement);
   STDMETHOD(ExecuteSQLWithReturn)(BSTR sSQLStatement, long *uniqueID);
   STDMETHOD(ExecuteSQLScript)(BSTR sFilename);

   STDMETHOD(BeginTransaction)();
   STDMETHOD(CommitTransaction)();
   STDMETHOD(RollbackTransaction)();

   STDMETHOD(get_DatabaseType)(/*[out, retval]*/ eDBtype *pVal);
   STDMETHOD(get_CurrentVersion)(/*[out, retval]*/ long *pVal);
   STDMETHOD(get_RequiredVersion)(/*[out, retval]*/ long *pVal);
   STDMETHOD(get_RequiresUpgrade)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(get_IsConnected)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(get_DatabaseExists)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(get_ServerName)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(get_DatabaseName)(/*[out, retval]*/ BSTR *pVal);

   STDMETHOD(UtilGetFileNameByMessageID)(hyper lMessageID, BSTR *Output);
   
   STDMETHOD(CreateExternalDatabase)(eDBtype ServerType, BSTR ServerName, long lPort, BSTR DatabaseName, BSTR Username, BSTR Password);
   STDMETHOD(CreateInternalDatabase)();
   STDMETHOD(SetDefaultDatabase)(eDBtype ServerType, BSTR ServerName, long lPort, BSTR DatabaseName, BSTR Username, BSTR Password);

   STDMETHOD(EnsurePrerequisites)(long DBVersion);
   
private:
   HRESULT EnsureDatabaseConnectivity_();

   HM::Configuration *config_;
   HM::IniFileSettings *ini_file_settings_;
   std::shared_ptr<HM::DatabaseConnectionManager> db_manager_;

   std::shared_ptr<HM::DALConnection> conn_;
};

OBJECT_ENTRY_AUTO(__uuidof(Database), InterfaceDatabase)
