// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace APIDocumentationCreator
{
    class Parser
    {
        /* Resulting objects */
        List<APIInterface> apiInterfaces = new List<APIInterface>();

        private APIInterface _currentInterface = null;
        private string _currentHelpText = "";

        public List<APIInterface> Interfaces
        {
            get
            {
                return apiInterfaces;
            }
        }

        public void ParseLine(string line)
        {
            line = line.Trim();
            line = line.Trim("\t".ToCharArray());

            if (line.StartsWith("object"))
            {
                // we're starting a new object.
                _currentInterface = null;
                _currentHelpText = "";
            }
            else if (line.StartsWith("helpstring"))
            {
                int startPos = "helpstring(\"".Length;
                int endPos = line.LastIndexOf("\"");
                int len = endPos - startPos;

                _currentHelpText = line.Substring(startPos, len);
            }
            else if (line.StartsWith("interface") && line.Contains("IDispatch"))
            {
                // extract the first word after interface.
                string [] values = line.SplitString(" ");
                string interfaceName = "";
                for (int i = 1; i < values.Length; i++ )
                {
                    string value = values[i];
                    if (value.Length > 0)
                    {
                        interfaceName = value;
                        break;
                    }

                }

                if (string.IsNullOrEmpty(interfaceName))
                {
                    throw new Exception("Could not locate interface name");
                }

                // Remove the IInterface signature. 
                interfaceName = interfaceName.Substring("IInterface".Length);

                if (interfaceName == "FetchAccount")
                {
                    int i = 0;
                }

                _currentInterface = new APIInterface();
                _currentInterface.Name = interfaceName;
                _currentInterface.HelpString = _currentHelpText;

                apiInterfaces.Add(_currentInterface);
            }
            else if (line.StartsWith("[propget"))
            {
                // [propget, id(1), helpstring("Undelivered messages")] HRESULT UndeliveredMessages([out, retval] BSTR *pVal);
                if (_currentInterface == null)
                    return;

                ParsePropLine(line, false);
            }
            else if (line.StartsWith("[propput"))
            {
                // [propput, id(34), helpstring("SMTP relayer requires authentication")] HRESULT SMTPRelayerRequiresAuthentication([in] VARIANT_BOOL newVal);
                if (_currentInterface == null)
                    return;

                ParsePropLine(line, true);
            }
            else if (line.StartsWith("[id"))
            {
                if (_currentInterface == null)
                    return;

                if (line.Contains("CreateExternalDatabase"))
                {
                    int i = 0;
                }

                ParseMethodLine(line);
            }
            
        }

        private void ParseMethodLine(string line)
        {
            int helpStringPos = line.IndexOf("helpstring");
            if (helpStringPos <= 0)
                throw new Exception("Help string could not be found");

            helpStringPos += "helpstring".Length + 2; // 2 for ("

            int helpStringPosEnd = line.IndexOf(")]", helpStringPos) - 1;
            int helpStringLen = helpStringPosEnd - helpStringPos;
            string helpString = line.Substring(helpStringPos, helpStringLen);

            int hResultPos = line.IndexOf("HRESULT");
            if (hResultPos <= 0)
                throw new Exception("HRESULT could not be found");

            int propertyStartPos = hResultPos + "HRESULT ".Length;
            int propertyEndPos = line.IndexOf("(", propertyStartPos);
            int propertyLength = propertyEndPos - propertyStartPos;

            string propertyName = line.Substring(propertyStartPos, propertyLength);

            int parametersEndPos = line.IndexOf(")", propertyEndPos);
            int parametersLength = parametersEndPos - propertyEndPos;
            string parameters = line.Substring(propertyEndPos + 1, parametersLength - 1);

            List<APIParameter> methodParameters;
            ParseGetPropertyParameters(parameters, out methodParameters);

            APIMethod method = new APIMethod();
            _currentInterface.Methods.Add(method);
            method.Name = propertyName;
            method.Parameters = methodParameters;
            method.HelpString = helpString;
        }

        private void ParsePropLine(string line, bool setProperty)
        {
            int helpStringPos = line.IndexOf("helpstring");
            if (helpStringPos <= 0)
                throw new Exception("Help string could not be found");

            helpStringPos += "helpstring".Length + 2; // 2 for ("

            int helpStringPosEnd = line.IndexOf(")]", helpStringPos) - 1;
            int helpStringLen = helpStringPosEnd - helpStringPos;
            string helpString = line.Substring(helpStringPos, helpStringLen);

            int hResultPos = line.IndexOf("HRESULT");
            if (hResultPos <= 0)
                throw new Exception("HRESULT could not be found");

            int propertyStartPos = hResultPos + "HRESULT ".Length;
            int propertyEndPos = line.IndexOf("(", propertyStartPos);
            int propertyLength = propertyEndPos - propertyStartPos;

            string propertyName = line.Substring(propertyStartPos, propertyLength);

            int parametersEndPos = line.IndexOf(")", propertyEndPos);
            int parametersLength = parametersEndPos - propertyEndPos;
            string parameters = line.Substring(propertyEndPos + 1, parametersLength - 1);

            List<APIParameter> propertyParameters;
            ParseGetPropertyParameters(parameters, out propertyParameters);

            APIProperty property = new APIProperty();
            property.Name = propertyName;
            _currentInterface.Properties.Add(property);

            if (setProperty)
                property.HasSet = true;
            else
                property.HasGet = true;

            property.Parameters = propertyParameters;
            property.HelpString = helpString;
        }

        private void ParseGetPropertyParameters(string parameters, out List<APIParameter> objectParameters)
        {
            objectParameters = new List<APIParameter>();

            if (parameters.Trim().Length == 0)
                return;
            if (parameters.Trim().ToLower() == "void")
                return;

            int parameterPosition = 0;
            while (parameterPosition >= 0)
            {
                string definition = "";
                int definitionEnd = parameters.IndexOf("]", parameterPosition) +1;
                if (definitionEnd > 0)
                {
                    int definitionLength = definitionEnd - parameterPosition;
                    definition = parameters.Substring(parameterPosition, definitionLength);
                }

                if (string.IsNullOrEmpty(definition))
                {
                    definition = "[in]";
                    definitionEnd = parameterPosition-1;
                }

                int typeStart = definitionEnd + 1;
                int typeEnd = parameters.IndexOf(" ", typeStart+1);
                int typeLen = typeEnd - typeStart;
                string type = parameters.Substring(typeStart, typeLen).Trim();

                int nameStart = typeEnd + 1;
                int nameEnd = parameters.IndexOf(",", nameStart);
                int nameLength = nameEnd - nameStart;
                if (nameLength <= 0)
                    nameLength = parameters.Length - nameStart;
                string name = parameters.Substring(nameStart, nameLength);

                type = RemoveIDLTokens(type);
                name = RemoveIDLTokens(name);

                APIParameter parameter = new APIParameter();
                parameter.Type = type;
                parameter.Name = name;

                if (definition.StartsWith("[in"))
                    parameter.Input = true;
                else if (definition.StartsWith("[out"))
                    parameter.Input = false;
                else
                    throw new Exception("Unknown type");

                objectParameters.Add(parameter);

                parameterPosition = parameters.IndexOf("[", typeEnd);
                if (parameterPosition < 0)
                {
                    parameterPosition = parameters.IndexOf(",", typeEnd);
                    if (parameterPosition > 0)
                    {
                        parameterPosition++;
                    }
                }
            }


        }

        private static string RemoveIDLTokens(string value)
        {
            value = value.Replace("*", "");
            value = value.Replace("IInterface", "");

            return value;
        }

        
    }
}
