// Checks whether the TCP ports hMailServer listens on are used by another program.

type
	SOCKADDR_IN = record
		sin_family	: word;
		sin_port	: word;
		sin_addr	: cardinal;
		sin_zero1	: cardinal;
		sin_zero2	: cardinal;
	end;

const
	AF_INET						= 2;
	SOCK_STREAM					= 1;
	INVALID_SOCKET				= $FFFFFFFF;

// lpWSAData is passed as a buffer that WSAStartup fills in.
function WSAStartup(wVersionRequested: word; lpWSAData: AnsiString): integer;
external 'WSAStartup@ws2_32.dll stdcall';

function WSACleanup(): integer;
external 'WSACleanup@ws2_32.dll stdcall';

function WsSocket(af, socketType, protocol: integer): cardinal;
external 'socket@ws2_32.dll stdcall';

function WsBind(s: cardinal; var name: SOCKADDR_IN; nameLength: integer): integer;
external 'bind@ws2_32.dll stdcall';

function WsCloseSocket(s: cardinal): integer;
external 'closesocket@ws2_32.dll stdcall';

// Returns true if the TCP port cannot be bound, meaning another program is using it.
function IsPortInUse(Port: integer): boolean;
var
	Sock: cardinal;
	Addr: SOCKADDR_IN;
begin
	Result := false;

	Sock := WsSocket(AF_INET, SOCK_STREAM, 0);
	if Sock = INVALID_SOCKET then
		Exit;

	Addr.sin_family := AF_INET;
	Addr.sin_port := ((Port and $FF) shl 8) or ((Port shr 8) and $FF); // network byte order
	Addr.sin_addr := 0; // INADDR_ANY
	Addr.sin_zero1 := 0;
	Addr.sin_zero2 := 0;

	Result := WsBind(Sock, Addr, 16) <> 0;
	WsCloseSocket(Sock);
end;

function ArePortsInUse(): boolean;
begin
	Result := false;

	if WSAStartup($0202, StringOfChar(' ', 512)) <> 0 then
		Exit;

	Result := IsPortInUse(25) or IsPortInUse(110) or IsPortInUse(143);
	WSACleanup();
end;
