// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace APIDocumentationCreator
{
    class APIProperty
    {
        public string Name { get; set; }
        public bool HasGet { get; set; }
        public bool HasSet { get; set; }
        public string HelpString { get; set; }

        public List<APIParameter> Parameters { get; set; }

        public APIProperty()
        {
            Parameters = new List<APIParameter>();
        }
    }
}
