// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using MySql.Data.MySqlClient;

namespace APIDocumentationCreator
{
    class HTMLGenerator
    {

        public void Generate(Parser parser, string destinationFolder)
        {
            const string connectionString = "Database=hmailserver_webpage;Data Source=hmailserver.com;User Id=user;Password=password;CharSet=utf8";
            using (MySqlConnection conn = new MySql.Data.MySqlClient.MySqlConnection(connectionString))
            {
                conn.Open();

                foreach (APIInterface apiInterface in parser.Interfaces)
                {
                    // do some pre-sorting...
                    IEnumerable<APIMethod> methods = apiInterface.Methods.OrderBy(method => method.Name);
                    IEnumerable<APIProperty> properties = apiInterface.Properties.OrderBy(prop => prop.Name);

                    string fileName = Path.Combine(destinationFolder, "com_object_" + apiInterface.Name + ".html");
                    fileName = fileName.ToLower();

                    string outputCode = "";

                    outputCode = "<h3>Description</h3>";
                    outputCode += apiInterface.HelpString  + "<br/>";

                    if (methods.Count() > 0)
                    {
                        outputCode += "<h3>Methods</h3>";

                        foreach (APIMethod method in methods)
                        {
                            string outputLine = string.Format("<div class=\"api_method_name\">{0}(", method.Name);

                            string parameters = "";
                            foreach (APIParameter parameter in method.Parameters)
                            {
                                if (!parameter.Input)
                                    continue;

                                string parameterItem = string.Format("{0} {1}", CreateLinkToType(parser, FriendlynizeType(parameter.Type)), parameter.Name);

                                if (parameters.Length > 0)
                                    parameters += ", ";

                                parameters += parameterItem;
                            }

                            outputLine += parameters;
                            outputLine += ")";

                            outputLine += "</div>";
                            outputCode += outputLine;

                            if (method.HelpString.Length > 0)
                            {
                                outputLine = string.Format("<br/><div class=\"api_description\">{0}</div>", method.HelpString);

                                outputCode += outputLine;
                            }

                            outputCode += "<br/>";
                        }
                    }

                    if (properties.Count() > 0)
                    {
                        outputCode += "<h3>Properties</h3>";

                        List<string> printedProperties = new List<string>();

                        foreach (APIProperty property in properties)
                        {
                            string outputLine = string.Format("<div class=\"api_method_name\">");

                            if (printedProperties.Contains(property.Name))
                                continue;

                            printedProperties.Add(property.Name);

                            foreach (APIParameter parameter in property.Parameters)
                            {
                                if (parameter.Input && property.Parameters.Count > 1)
                                    continue;

                                outputLine += CreateLinkToType(parser, FriendlynizeType(parameter.Type)) + " ";
                            }

                            outputLine += string.Format("{0}", property.Name);

                            string parameters = "";
                            if (property.Parameters.Count > 1)
                            {
                                

                                for (int i = 0; i < property.Parameters.Count; i++ )
                                {
                                    APIParameter parameter = property.Parameters[i];
                                    if (!parameter.Input)
                                        continue;

                                    string parameterItem = string.Format("{0} {1}", CreateLinkToType(parser, FriendlynizeType(parameter.Type)), parameter.Name);

                                    if (parameters.Length > 0)
                                        parameters += ", ";

                                    parameters += parameterItem;
                                }

                                if (parameters.Length > 0)
                                {
                                    outputLine += "(";
                                    outputLine += parameters;
                                    outputLine += ")";

                                }
                            }

                            outputLine += "</div>";
                            outputCode += outputLine;

                            if (property.HelpString.Length > 0)
                            {
                                outputLine = string.Format("<br/><div class=\"api_description\">{0}<br/>", property.HelpString);

                                if (!property.HasSet)
                                {
                                    // check if there's a set-property with same name.
                                    IEnumerable<APIProperty> setProperties = properties.Where(prop => prop.Name == property.Name && prop.HasSet == true);
                                    if (setProperties.Count() == 0)
                                        outputLine += " <i>(read-only)</i>";
                                }

                                outputLine += "</div>";
                                outputCode += outputLine;
                            }

                            outputCode += "<br/>";
                        }
                    }

                    
                    File.WriteAllText(fileName, outputCode);

                    UploadDocument(conn, apiInterface, outputCode);
                }   

                                
                // remove old documents.
                string sql = "select documentid, documentname from hm_documents where documentname like 'com\\_object\\_%'";
                MySqlCommand selectCommand = new MySqlCommand(sql, conn);

                List<int> documentsToRemove = new List<int>();
                using (MySqlDataReader reader = selectCommand.ExecuteReader())
                {
                    while (reader.Read())
                    {
                        int documentID = Convert.ToInt32(reader["documentid"]);
                        string documentName = Convert.ToString(reader["documentname"]);

                        IEnumerable<APIInterface> interfaces = parser.Interfaces.Where(apiInterface => "com_object_" + apiInterface.Name.ToLower() == documentName);
                        if (interfaces.Count() == 0)
                        {
                            documentsToRemove.Add(documentID);
                        }
                    }

                    reader.Close();
                }

                foreach (int documentToRemove in documentsToRemove)
                {
                    sql = string.Format("delete from hm_documents where documentid = {0}", documentToRemove);
                    MySqlCommand deleteCommand = new MySqlCommand(sql, conn);
                    deleteCommand.ExecuteNonQuery();
                }
            
            }
        }

        private static void UploadDocument(MySqlConnection conn, APIInterface apiInterface, string text)
        {

            string documentText = text.Replace("'", "''");
            string documentName = "com_object_" + apiInterface.Name.ToLower();
            string documentTitle = apiInterface.Name + " object";
            string documentLatestChange = DateTime.Now.ToString("yyyy'-'MM'-'dd");
            int documentReads = 0;
            int documentParentID = 53;
            int documentIndex = 0;
            int documentIsBook = 0;
            int documentFirstPage = 0;

            // check if the document exists.
            string name = "select * from hm_documents where documentname = '" + documentName + "'";

            MySqlCommand command = new MySqlCommand(name, conn);
            bool exists = false;
            using (MySqlDataReader reader = command.ExecuteReader())
            {
                exists = reader.Read();
                reader.Close();
            }

            string sql = null;

            if (exists)
            {
                sql = string.Format("update hm_documents set documenttext = '{0}' where documentname = '{1}'", documentText, documentName);

            }
            else
            {
                sql = string.Format(@"insert into hm_documents (documentname, documenttitle, documenttext, documentlatestchange, documentreads, documentparentid, documentindex, documentisbook, documentfirstpage) VALUES
                                            ('{0}', '{1}', '{2}', '{3}', '{4}', {5}, {6}, {7}, {8})",
                                             documentName, documentTitle, documentText, documentLatestChange, documentReads, documentParentID, documentIndex, documentIsBook, documentFirstPage);
            }

            MySqlCommand updateCommand = new MySqlCommand(sql, conn);
            updateCommand.ExecuteNonQuery();


        }

        public string CreateLinkToType(Parser parser, string type)
        {
            IEnumerable<APIInterface> interfaces = parser.Interfaces.Where(apiInterface => apiInterface.Name == type);
            if (interfaces.Count() == 0)
                return type;

            // create a link to that type.
            string result = string.Format("<a href=\"?page=com_object_{0}\">{1}</a>", type.ToLower(), type);

            return result;

        }

        public string FriendlynizeType(string unfriendly)
        {
            unfriendly = unfriendly.Replace("BSTR", "string");
            unfriendly = unfriendly.Replace("VARIANT_BOOL", "bool");
            unfriendly = unfriendly.Replace("LONG", "long");

            return unfriendly;
        }

        /*
         * <p>The cache object contains settings that relates to the hMailServer caching functionality.</p>
            <h1>Version</h1>
            <p>The below documentation is valid for hMailServer 4.2 and later.</p>
            <h1>Properties</h1>
            <h4>
            <div class="api_method_name">Property AccountCacheTTL As Long</div>
            </h4>
            <div class="indented">
            <div class="api_description">The number of seconds hMailServer should keep accounts in the cache</div>
            </div>
            <h4>
            <div class="api_method_name">Property AccountHitRate As Long</div>
            </h4>
            <div class="indented">
            <div class="api_description">The account hit rate.</div>
            </div>
            <div class="indented">
            <h4>&nbsp;</h4>
            <h3>
            <div class="api_method_name">Property AliasCacheTTL As Long</div>
            </h3>
            <div class="indented">
            <div class="api_description">The number of seconds hMailServer should keep aliases in the cache</div>
            </div>
            <h4>
            <div class="api_method_name">Property AliasHitRate As Long</div>
            </h4>
            <div class="indented">
            <div class="api_description">The alias cache hit rate.</div>
            </div>
            <div class="indented">
            <h4>
            <div class="api_method_name">Property DistributionListCacheTTL As Long</div>
            </h4>
            <div class="indented">
            <div class="api_description">The number of seconds hMailServer should keep distribution lists in the cache</div>
            </div>
            <h4>
            <div class="api_method_name">Property DistributionListHitRate As Long</div>
            </h4>
            <div class="indented">
            <div class="api_description">The hit rate for the distribution list cache.</div>
            </div>
            </div>
            </div>
            <h4>
            <div class="api_method_name">Property DomainCacheTTL As Long</div>
            </h4>
            <div class="indented">
            <div class="api_description">The number of seconds hMailServer should keep domains in the cache</div>
            </div>
            <h4>
            <div class="api_method_name">Property DomainHitRate As Long</div>
            </h4>
            <div class="indented">
            <div class="api_description">The domain hit rate.</div>
            </div>
            <h4>
            <div class="api_method_name">Property Enabled As Boolean</div>
            </h4>
            <div class="indented">
            <div class="api_description">Cache is enabled.</div>
            </div>
            <h1>Methods</h1>
            <h4>
            <div class="api_method_name">Sub Clear()</div>
            </h4>
            <div class="indented">
            <div class="api_description">Clears the cache</div>
            </div>
            <h1>Sample code</h1>
            <h2>Enabling the cache</h2>
            <p><code>    Dim obBaseApp<br />
            Set obBaseApp = CreateObject(&quot;hMailServer.Application&quot;)<br />
            <br />
            Dim obCache<br />
            Set obCache = obBaseApp.Settings.Cache<br />
            <br />
            obCache.Enabled = True<br />
            </code></p>*/
    }
}

