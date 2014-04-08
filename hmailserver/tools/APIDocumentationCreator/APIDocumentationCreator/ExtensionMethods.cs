// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace APIDocumentationCreator
{
    static class ExtensionMethods
    {
        public static string[] SplitString(this string haystack, string delimiter)
        {
            return Microsoft.VisualBasic.Strings.Split(haystack, delimiter, -1, Microsoft.VisualBasic.CompareMethod.Text);
        }
    }
}
