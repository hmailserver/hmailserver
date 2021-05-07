// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once
#include "../hMailServer/resource.h"       // main symbols

#include "../hMailServer/hMailServer.h"


// InterfaceSettings

class ATL_NO_VTABLE InterfaceSettings : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceSettings, &CLSID_Settings>,
	public IDispatchImpl<IInterfaceSettings, &IID_IInterfaceSettings, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator,
   public ISupportErrorInfo
{
public:

   InterfaceSettings()
   {
#ifdef _DEBUG
      InterlockedIncrement(&counter);
#endif
   }

   ~InterfaceSettings()
   {
#ifdef _DEBUG
      InterlockedDecrement(&counter);
#endif
   }


DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACESETTINGS)


BEGIN_COM_MAP(InterfaceSettings)
	COM_INTERFACE_ENTRY(IInterfaceSettings)
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

   STDMETHOD(get_Scripting)(/*[out, retval]*/ IInterfaceScripting **pVal);
   STDMETHOD(get_Backup)(/*[out, retval]*/ IInterfaceBackupSettings **pVal);
   STDMETHOD(get_Directories)(/*[out, retval]*/ IInterfaceDirectories **pVal);
   STDMETHOD(get_AntiSpam)(/*[out, retval]*/ IInterfaceAntiSpam **pVal);
   STDMETHOD(get_ServerMessages)(/*[out, retval]*/ IInterfaceServerMessages **pVal);
   STDMETHOD(get_TCPIPPorts)(/*[out, retval]*/ IInterfaceTCPIPPorts **pVal);
   STDMETHOD(get_SSLCertificates)(/*[out, retval]*/ IInterfaceSSLCertificates **pVal);
   STDMETHOD(get_Groups)(/*[out, retval]*/ IInterfaceGroups * *pVal);
   STDMETHOD(get_IncomingRelays)(/*[out, retval]*/ IInterfaceIncomingRelays * *pVal);
   STDMETHOD(get_MessageIndexing)(/*[out, retval]*/ IInterfaceMessageIndexing * *pVal);

   STDMETHOD(get_ServiceIMAP)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_ServiceIMAP)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_ServicePOP3)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_ServicePOP3)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_ServiceSMTP)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_ServiceSMTP)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_WelcomeIMAP)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_WelcomeIMAP)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_WelcomePOP3)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_WelcomePOP3)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_WelcomeSMTP)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_WelcomeSMTP)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_SMTPRelayer)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_SMTPRelayer)(/*[in]*/ BSTR newVal);

   STDMETHOD(get_SMTPRelayerPort)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_SMTPRelayerPort)(/*[in]*/ long newVal);

   STDMETHOD(get_MaxMessageSize)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_MaxMessageSize)(/*[in]*/ long newVal);

   STDMETHOD(get_SendStatistics)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_SendStatistics)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_SMTPRelayerRequiresAuthentication)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_SMTPRelayerRequiresAuthentication)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_SMTPRelayerUsername)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_SMTPRelayerUsername)(/*[in]*/ BSTR newVal);

   STDMETHOD(get_UserInterfaceLanguage)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_UserInterfaceLanguage)(/*[in]*/ BSTR newVal);


   STDMETHOD(SetSMTPRelayerPassword)(/*[in]*/ BSTR newVal);

   STDMETHOD(get_HostName)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_HostName)(/*[in]*/ BSTR newVal);

   STDMETHOD(get_DenyMailFromNull)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_DenyMailFromNull)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_AllowSMTPAuthPlain)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_AllowSMTPAuthPlain)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_MirrorEMailAddress)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_MirrorEMailAddress)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_MaxPOP3Connections)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_MaxPOP3Connections)(/*[in]*/ long newVal);
   STDMETHOD(get_MaxSMTPConnections)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_MaxSMTPConnections)(/*[in]*/ long newVal);
   STDMETHOD(get_MaxIMAPConnections)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_MaxIMAPConnections)(/*[in]*/ long newVal);

   STDMETHOD(get_Logging)(/*[out, retval]*/ IInterfaceLogging** pVal);
   STDMETHOD(get_AntiVirus)(/*[out, retval]*/ IInterfaceAntiVirus** pVal);
   STDMETHOD(get_Cache)(/*[out, retval]*/ IInterfaceCache** pVal);

   STDMETHOD(get_SecurityRanges)(IInterfaceSecurityRanges **pVal);

   STDMETHOD(get_SMTPNoOfTries)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_SMTPNoOfTries)(/*[in]*/ long newVal);
   STDMETHOD(get_SMTPMinutesBetweenTry)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_SMTPMinutesBetweenTry)(/*[in]*/ long newVal);


   STDMETHOD(get_MaxDeliveryThreads)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_MaxDeliveryThreads)(/*[in]*/ long newVal);

   STDMETHOD(get_Routes)(IInterfaceRoutes **pVal);

   STDMETHOD(get_RuleLoopLimit)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_RuleLoopLimit)(/*[in]*/ long newVal);

   STDMETHOD(get_DefaultDomain)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_DefaultDomain)(/*[in]*/ BSTR newVal);

   STDMETHOD(get_IMAPPublicFolderName)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_IMAPPublicFolderName)(/*[in]*/ BSTR newVal);


   STDMETHOD(get_SMTPDeliveryBindToIP)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_SMTPDeliveryBindToIP)(/*[in]*/ BSTR newVal);

   
   STDMETHOD(get_IMAPSortEnabled)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_IMAPSortEnabled)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_IMAPQuotaEnabled)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_IMAPQuotaEnabled)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_IMAPIdleEnabled)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_IMAPIdleEnabled)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_IMAPACLEnabled)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_IMAPACLEnabled)(/*[in]*/ VARIANT_BOOL newVal);


   STDMETHOD(get_WorkerThreadPriority)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_WorkerThreadPriority)(/*[in]*/ long newVal);

   STDMETHOD(get_TCPIPThreads)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_TCPIPThreads)(/*[in]*/ long newVal);

   STDMETHOD(get_AllowIncorrectLineEndings)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_AllowIncorrectLineEndings)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_MaxSMTPRecipientsInBatch)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_MaxSMTPRecipientsInBatch)(/*[in]*/ long newVal);

   STDMETHOD(get_DisconnectInvalidClients)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_DisconnectInvalidClients)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_MaxNumberOfInvalidCommands)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_MaxNumberOfInvalidCommands)(/*[in]*/ long newVal);

   STDMETHOD(get_SMTPRelayerUseSSL)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_SMTPRelayerUseSSL)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_SMTPRelayerConnectionSecurity)(/*[out, retval]*/ eConnectionSecurity *pVal);
   STDMETHOD(put_SMTPRelayerConnectionSecurity)(/*[in]*/ eConnectionSecurity newVal);

   STDMETHOD(get_SMTPConnectionSecurity)(/*[out, retval]*/ eConnectionSecurity *pVal);
   STDMETHOD(put_SMTPConnectionSecurity)(/*[in]*/ eConnectionSecurity newVal);


   STDMETHOD(get_AddDeliveredToHeader)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_AddDeliveredToHeader)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(SetAdministratorPassword)(/*[in]*/ BSTR newVal);

   STDMETHOD(get_PublicFolders)(/*[out, retval]*/ IInterfaceIMAPFolders** pVal);

   STDMETHOD(get_PublicFolderDiskName)(/*[out, retval]*/ BSTR *pVal);

   STDMETHOD(get_AutoBanOnLogonFailure)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_AutoBanOnLogonFailure)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_MaxInvalidLogonAttempts)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_MaxInvalidLogonAttempts)(/*[in]*/ long newVal);

   STDMETHOD(get_MaxInvalidLogonAttemptsWithin)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_MaxInvalidLogonAttemptsWithin)(/*[in]*/ long newVal);

   STDMETHOD(get_AutoBanMinutes)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_AutoBanMinutes)(/*[in]*/ long newVal);

   STDMETHOD(ClearLogonFailureList)();

   STDMETHOD(get_IMAPHierarchyDelimiter)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_IMAPHierarchyDelimiter)(/*[in]*/ BSTR newVal);

   STDMETHOD(get_MaxAsynchronousThreads)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_MaxAsynchronousThreads)(/*[in]*/ long newVal);

   STDMETHOD(get_MaxNumberOfMXHosts)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_MaxNumberOfMXHosts)(/*[in]*/ long newVal);
   
   STDMETHOD(get_VerifyRemoteSslCertificate)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_VerifyRemoteSslCertificate)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_SslCipherList)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_SslCipherList)(/*[in]*/ BSTR newVal);

   STDMETHOD(get_TlsVersion10Enabled)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_TlsVersion10Enabled)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_TlsVersion11Enabled)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_TlsVersion11Enabled)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_TlsVersion12Enabled)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_TlsVersion12Enabled)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_TlsVersion13Enabled)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_TlsVersion13Enabled)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_TlsOptionPreferServerCiphersEnabled)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_TlsOptionPreferServerCiphersEnabled)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_TlsOptionPrioritizeChaChaEnabled)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_TlsOptionPrioritizeChaChaEnabled)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_CrashSimulationMode)(/*[out, retval]*/ long *pVal);
   STDMETHOD(put_CrashSimulationMode)(/*[in]*/ long newVal);

   bool LoadSettings();

private:
   

   
   HM::Configuration *config_;
   HM::IniFileSettings *ini_file_settings_;

#ifdef _DEBUG
   static long counter;
#endif
   
};

OBJECT_ENTRY_AUTO(__uuidof(Settings), InterfaceSettings)
