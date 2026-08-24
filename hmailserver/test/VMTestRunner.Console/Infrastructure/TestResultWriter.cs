using System;
using System.Collections.Generic;
using System.IO;
using Newtonsoft.Json;
using Newtonsoft.Json.Converters;
using Newtonsoft.Json.Linq;
using Newtonsoft.Json.Serialization;

namespace VMTestRunner.Console
{
   public static class TestResultWriter
   {
      public static void Write(string path, TestRunReport report)
      {
         var settings = new JsonSerializerSettings
         {
            Formatting = Formatting.Indented,
            NullValueHandling = NullValueHandling.Ignore,
            DateFormatHandling = DateFormatHandling.IsoDateFormat,
            ContractResolver = new CamelCasePropertyNamesContractResolver(),
         };

         settings.Converters.Add(new StringEnumConverter());

         File.WriteAllText(path, JsonConvert.SerializeObject(report, settings));
      }

      /// <summary>
      /// Reads a result file, or returns null if it cannot be read.
      /// </summary>
      public static TestRunReport TryRead(string path)
      {
         try
         {
            var content = JToken.Parse(File.ReadAllText(path));

            // Result files written before the run details were added contain only the tests.
            if (content.Type == JTokenType.Array)
               return new TestRunReport { Tests = content.ToObject<List<TestRunResult>>() };

            return content.ToObject<TestRunReport>();
         }
         catch (Exception)
         {
            return null;
         }
      }
   }
}
