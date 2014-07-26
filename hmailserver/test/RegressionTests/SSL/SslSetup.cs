using System;
using System.IO;
using System.Security.Cryptography.X509Certificates;
using hMailServer;

namespace RegressionTests.SSL
{
   public class SslSetup
   {
      public static void SetupSSLPorts(hMailServer.Application application, eConnectionSecurity security)
      {
         SSLCertificate sslCeritifcate = SetupSSLCertificate(application);

         TCPIPPort port = application.Settings.TCPIPPorts.Add();
         port.Address = "0.0.0.0";
         port.PortNumber = 250;
         port.ConnectionSecurity = security;
         port.SSLCertificateID = sslCeritifcate.ID;
         port.Protocol = eSessionType.eSTSMTP;
         port.Save();

         port = application.Settings.TCPIPPorts.Add();
         port.Address = "0.0.0.0";
         port.PortNumber = 11000;
         port.ConnectionSecurity = security;
         port.SSLCertificateID = sslCeritifcate.ID;
         port.Protocol = eSessionType.eSTPOP3;
         port.Save();

         port = application.Settings.TCPIPPorts.Add();
         port.Address = "0.0.0.0";
         port.PortNumber = 14300;
         port.ConnectionSecurity = security;
         port.SSLCertificateID = sslCeritifcate.ID;
         port.Protocol = eSessionType.eSTIMAP;
         port.Save();

         application.Stop();
         application.Start();
      }

      private static string GetSslCertPath()
      {
         string originalPath = Environment.CurrentDirectory;
         Environment.CurrentDirectory = Environment.CurrentDirectory + "\\..\\..\\..\\SSL examples";
         string sslPath = Environment.CurrentDirectory;
         Environment.CurrentDirectory = originalPath;

         return sslPath;
      }

      private static string GetCertificatePfx()
      {
         var sslPath = GetSslCertPath();
         return Path.Combine(sslPath, "example.pfx");
      }

      public static X509Certificate2 GetCertificate()
      {
         var pfxPath = GetCertificatePfx();
         var x509 = new X509Certificate2(pfxPath, "Secret1");

         return x509;
      }

      private static SSLCertificate SetupSSLCertificate(hMailServer.Application application)
      {
         var sslPath = GetSslCertPath();

         var exampleCert = Path.Combine(sslPath, "example.crt");
         var exampleKey = Path.Combine(sslPath, "example.key");

         if (!File.Exists(exampleCert))
            CustomAssert.Fail("Certificate " + exampleCert + " was not found");
         if (!File.Exists(exampleKey))
            CustomAssert.Fail("Private key " + exampleKey + " was not found");


         SSLCertificate sslCertificate = application.Settings.SSLCertificates.Add();
         sslCertificate.Name = "Example";
         sslCertificate.CertificateFile = exampleCert;
         sslCertificate.PrivateKeyFile = exampleKey;
         sslCertificate.Save();

         return sslCertificate;
      }



   }
}
