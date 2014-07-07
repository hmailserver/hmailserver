using System;
using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests
{
   public class CustomAssert
   {
      public static void IsNotNull(object value)
      {
         IsNotNull(value, string.Empty);
      }

      public static void IsNotNull(object value, string message)
      {
         var completeMessage = GenerateFullAssertMessage(message);

         Assert.IsNotNull(value, completeMessage);
      }

      public static void IsTrue(bool value)
      {
         IsTrue(value, string.Empty);
      }

      public static void IsTrue(bool value, string message)
      {
         var completeMessage = GenerateFullAssertMessage(message);

         Assert.IsTrue(value, completeMessage);
      }

      public static void IsFalse(bool value)
      {
         IsFalse(value, string.Empty);
      }

      public static void IsFalse(bool value, string message)
      {
         var completeMessage = GenerateFullAssertMessage(message);

         Assert.IsFalse(value, completeMessage);
      }

      public static void AreEqual(int first, int second)
      {
         AreEqual(first, second, string.Empty);
      }

      public static void AreEqual(int first, int second, string message)
      {
         var completeMessage = GenerateFullAssertMessage(message);

         Assert.AreEqual(first, second, completeMessage);
      }

      public static void AreEqual(string first, string second)
      {
         AreEqual(first, second, string.Empty);
      }

      public static void AreEqual(string first, string second, string message)
      {
         var completeMessage = GenerateFullAssertMessage(message);

         Assert.AreEqual(first, second, completeMessage);
      }

      public static void AreEqual(object first, object second)
      {
         AreEqual(first, second, string.Empty);
      }

      public static void AreEqual(object first, object second, string message)
      {
         var completeMessage = GenerateFullAssertMessage(message);

         Assert.AreEqual(first, second, completeMessage);
      }

      public static void AreNotEqual(int first, int second)
      {
         AreNotEqual(first, second, string.Empty);
      }

      public static void AreNotEqual(int first, int second, string message)
      {
         var completeMessage = GenerateFullAssertMessage(message);

         Assert.AreNotEqual(first, second, completeMessage);
      }

      public static void AreNotEqual(string first, string second)
      {
         AreNotEqual(first, second, string.Empty);
      }

      public static void AreNotEqual(string first, string second, string message)
      {
         var completeMessage = GenerateFullAssertMessage(message);

         Assert.AreNotEqual(first, second, completeMessage);
      }

      public static void AreNotEqual(object first, object second)
      {
         AreNotEqual(first, second, string.Empty);
      }

      public static void AreNotEqual(object first, object second, string message)
      {
         var completeMessage = GenerateFullAssertMessage(message);

         Assert.AreNotEqual(first, second, completeMessage);
      }

      public static void Fail()
      {
         Fail(string.Empty);
      }

      public static void Fail(string message)
      {
         var completeMessage = GenerateFullAssertMessage(message);

         Assert.Fail(completeMessage);
      }

      public static void Greater(int greater, int less)
      {
         Greater(greater, less, string.Empty);
      }

      public static void Greater(int greater, int less, string message)
      {
         var completeMessage = GenerateFullAssertMessage(message);

         Assert.Greater(greater, less, completeMessage);
      }

      public static void Greater(double greater, double less)
      {
         Greater(greater, less, string.Empty);
      }

      public static void Greater(double greater, double less, string message)
      {
         var completeMessage = GenerateFullAssertMessage(message);

         Assert.Greater(greater, less, completeMessage);
      }


      public static void Less(int less, int greater)
      {
         Less(less, greater, string.Empty);
      }

      public static void Less(int less, int greater, string message)
      {
         var completeMessage = GenerateFullAssertMessage(message);

         Assert.Less(less, greater, completeMessage);
      }

      public static void Less(double less, double greater)
      {
         Less(less, greater, string.Empty);
      }

      public static void Less(double less, double greater, string message)
      {
         var completeMessage = GenerateFullAssertMessage(message);

         Assert.Less(less, greater, completeMessage);
      }

      public static void IsNotEmpty(string value)
      {
         IsNotEmpty(value, string.Empty);
      }

      public static void IsNotEmpty(string value, string message)
      {
         var completeMessage = GenerateFullAssertMessage(message);

         Assert.IsNotEmpty(value, completeMessage);
      }

      public static void IsEmpty(string value)
      {
         IsEmpty(value, string.Empty);
      }

      public static void IsEmpty(string value, string message)
      {
         var completeMessage = GenerateFullAssertMessage(message);

         Assert.IsEmpty(value, completeMessage);
      }

      public static void IsNullOrEmpty(string value)
      {
         IsNullOrEmpty(value, string.Empty);
      }

      public static void IsNullOrEmpty(string value, string message)
      {
         var completeMessage = GenerateFullAssertMessage(message);

         Assert.IsNullOrEmpty(value, completeMessage);
      }

      public static void Inconclusive(string message)
      {
         var completeMessage = GenerateFullAssertMessage(message);

         Assert.Inconclusive(completeMessage);
      }

      private static string GenerateFullAssertMessage(string message)
      {
         if (string.IsNullOrEmpty(message))
         {
            message = "<Emtpy>";
         }

         string completeMessage = string.Format("At {0}, Message: {1}, Log: {2}", DateTime.Now, message,
            TestSetup.ReadCurrentDefaultLog());

         return completeMessage;
      }
   }
}
