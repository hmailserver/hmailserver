// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace APIDocumentationCreator
{
    class APIInterface
    {
        public string Name { get; set; }
        public string HelpString { get; set; }
        public List<APIProperty> Properties {get;set;}
        public List<APIMethod> Methods { get; set; }


        public APIInterface()
        {
            Properties = new List<APIProperty>();
            Methods = new List<APIMethod>();
        }

        public APIProperty GetProperty(string name)
        {
            foreach (APIProperty property in Properties)
            {
                if (property.Name == name)
                    return property;
            }

            return null;
        }

    }
}
