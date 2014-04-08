// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;
using System.Configuration;
using System.IO;
using System.Xml;
using System.Xml.Serialization;

namespace hMailServer.Administrator.Utilities.Settings
{
    public class UserSettings
    {
        private Servers serverConnections;

        private bool autoConnectOnStartup;
        private string autoConnectServer;

        public UserSettings()
        {
            serverConnections = new Servers();

            autoConnectOnStartup = false;
        }

        public Servers ServerConnections
        {
            get { return serverConnections; }
            set { serverConnections = value; }
        }

        public bool AutoConnectOnStartup
        {
            get { return autoConnectOnStartup; }
            set { autoConnectOnStartup = value; }
        }

        public string AutoConnectServer
        {
            get { return autoConnectServer; }
            set { autoConnectServer = value; }
        }


        private static string CreateSettingsFolder()
        {
            string localData = Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData);

            if (!Directory.Exists(localData))
                Directory.CreateDirectory(localData);

            string companyFolder = Path.Combine(localData, "Halvar Information");
            if (!Directory.Exists(companyFolder))
                Directory.CreateDirectory(localData);

            string appFolder = Path.Combine(companyFolder, "hMailServer");
            if (!Directory.Exists(appFolder))
                Directory.CreateDirectory(appFolder);

            return appFolder;
        }

        public static void Save(UserSettings settings)
        {
            string settingsFile = Path.Combine(CreateSettingsFolder(), "hMailAdmin.exe.config");

            XmlTextWriter writer = new XmlTextWriter(settingsFile, Encoding.UTF8);
            
            try
            {
                writer.Formatting = Formatting.Indented;

                XmlSerializer xmlSerializer = new XmlSerializer(typeof(UserSettings));
                xmlSerializer.Serialize(writer, settings);
            }
            catch (Exception)
            {

            }
            finally
            {
                writer.Close();
            }
            
        }

        public static UserSettings Load()
        {
            string settingsFile = Path.Combine(CreateSettingsFolder(), "hMailAdmin.exe.config");

            if (!File.Exists(settingsFile))
                return CreateDefault();

            XmlTextReader reader = null;

            try
            {
                reader = new XmlTextReader(settingsFile);
                XmlSerializer xmlSerializer = new XmlSerializer(typeof(UserSettings));
                UserSettings retVal = (UserSettings)xmlSerializer.Deserialize(reader);

                return retVal;
            }
            catch (Exception)
            {
                return CreateDefault();
            }
            finally
            {
                if (reader != null)
                    reader.Close();
            }
        }

        private static UserSettings CreateDefault()
        {
            UserSettings retVal = new UserSettings();

            Server server = new Server();

            server.hostName = "localhost";
            server.userName = "Administrator";
            server.encryptedPassword = "";

            retVal.ServerConnections.List.Add(server);

            return retVal;

            
        }
    }
}
