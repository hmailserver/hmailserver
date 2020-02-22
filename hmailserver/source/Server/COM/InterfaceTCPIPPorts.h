// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../hMailServer/resource.h"       // main symbols
#include "../hMailServer/hMailServer.h"

/// #include "../Common/BO/TCPIPort.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// InterfaceTCPIPPorts

class ATL_NO_VTABLE InterfaceTCPIPPorts :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<InterfaceTCPIPPorts, &CLSID_TCPIPPorts>,
	public IDispatchImpl<IInterfaceTCPIPPorts, &IID_IInterfaceTCPIPPorts, &LIBID_hMailServer, /*wMajor =*/ 1, /*wMinor =*/ 0>,
   public HM::COMAuthenticator
{
public:
	InterfaceTCPIPPorts()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_INTERFACETCPIPPORTS)


BEGIN_COM_MAP(InterfaceTCPIPPorts)
	COM_INTERFACE_ENTRY(IInterfaceTCPIPPorts)
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
   STDMETHOD(Refresh)();
   STDMETHOD(SetDefault)();

   STDMETHOD(get_Item)(/*[in]*/ long Index, /*[out, retval]*/ IInterfaceTCPIPPort **pVal);
   STDMETHOD(get_Count)(/*[out, retval]*/ long *pVal);
   STDMETHOD(get_ItemByDBID)(/*[in]*/ long lDBID, /*[out, retval]*/ IInterfaceTCPIPPort** pVal);
   STDMETHOD(DeleteByDBID)(/*[in]*/ long DBID);
   STDMETHOD(Add)(/*[out, retval]*/ IInterfaceTCPIPPort **pVal);

   void Attach(std::shared_ptr<HM::TCPIPPorts> pBlockedAttachments);

public:

   std::shared_ptr<HM::TCPIPPorts> tcpip_ports_;

};

OBJECT_ENTRY_AUTO(__uuidof(TCPIPPorts), InterfaceTCPIPPorts)

