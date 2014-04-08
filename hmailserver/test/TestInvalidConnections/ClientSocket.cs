// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Net.Sockets;
using System.Net;
using System.IO;
using System.Text;
using System.Threading;

namespace StressTest
{
	/// <summary>
	/// Summary description for ClientSocket.
	/// </summary>
	public class ClientSocket
	{
		private Socket m_oSocket;

		public ClientSocket()
		{
			//
			// TODO: Add constructor logic here
			//
		}

		public bool Connect(int iPort)
		{
			IPHostEntry iphe = null;
            //iphe = Dns.GetHostEntry("hmailserver.no-ip.org");
            iphe = Dns.GetHostEntry("localhost");
			
			foreach(IPAddress ipad in iphe.AddressList)
			{
				IPEndPoint ipe = new IPEndPoint(ipad, iPort);

				Socket tmpS = new Socket(ipe.AddressFamily, SocketType.Stream, ProtocolType.Tcp);

				try
				{
					tmpS.Connect(ipe);
				}
				catch (SocketException exp)
				{
					int i = exp.ErrorCode;
				}
				

				if(tmpS.Connected)
				{
					m_oSocket = tmpS;
					return true;
				}
				else
					continue;
			}

			return false;
		}

		public bool CanConnect(int iPort)
		{
			bool bRet = Connect(iPort);

			if (bRet)
			{
				// Wait 1 second and see if we've been able
				// to receive something.
				Thread.Sleep(1000);
	
				if (m_oSocket.Available == 0)
					bRet = false;

				Disconnect();
			}

			return bRet;

		}

		public void Disconnect()
		{
			m_oSocket.Close();
		}

		public void Send(string s)
		{
			Byte[] buf = Encoding.ASCII.GetBytes(s);
			m_oSocket.Send(buf, buf.Length, 0);
		}

		public string Receive()
		{
			byte[] bytes = new byte[1024];

			while (m_oSocket.Available == 0)
			{
				m_oSocket.Poll(10, SelectMode.SelectError);
				Thread.Sleep(10);
			}

			int iReceived = m_oSocket.Receive(bytes, 0, m_oSocket.Available, SocketFlags.None);

			char[] chars = Encoding.ASCII.GetChars(bytes);
			
			string s = new string(chars, 0, iReceived);

			return s;
		}
	}
}
