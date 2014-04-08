// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Threading;

namespace StressTest
{
	/// <summary>
	/// Summary description for SMTPSimulator.
	/// </summary>
	public class SMTPSimulator
	{
		
		ClientSocket m_oSocket;

		public SMTPSimulator()
		{
			m_oSocket = new ClientSocket();	
		}

		public bool TestConnect()
		{
            while (!m_oSocket.Connect(25)) 
                Console.WriteLine(System.DateTime.Now + " " + "Failed to connect to server");

			// Receive welcome message.
			string sData = m_oSocket.Receive();

            m_oSocket.Disconnect();

            return false;
		}
	}
}
