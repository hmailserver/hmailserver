// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace APIDocumentationCreator
{
    class APIParameter
    {
        public string Name { get; set; }
        public string Type { get; set; }
        public bool Input { get; set; }
    }
}
