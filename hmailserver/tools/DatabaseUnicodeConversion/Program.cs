// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;

namespace DatabaseUnicodeConversion
{
    class Program
    {
        static void Main(string[] args)
        {
            Parser p = new Parser();

            p.Run(args[0], args[1]);

        }
    }
}
