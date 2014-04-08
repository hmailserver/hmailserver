// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.IO;
using System.Security.Cryptography; 

namespace hMailServer.Administrator.Utilities
{
    internal class Encryption
    {
        private static string NOT_SECRET_KEY = "THIS_KEY_IS_NOT_SECRET";

        public static string Encrypt(string plainText)
        {
            // Encryption operates on byte arrays, not on strings.
            byte[] plainTextBytes =
              System.Text.Encoding.Unicode.GetBytes(plainText);

            // Derive a key from the password.
            PasswordDeriveBytes passwordDerivedBytes = new PasswordDeriveBytes(NOT_SECRET_KEY,
                new byte[] {0x49, 0x76, 0x61, 0x6e, 0x20, 0x4d, 0x65, 0x64, 0x76, 0x65, 0x64, 0x65, 0x76});

            // Use Rijndael symmetric algorithm to do the encryption.
            Rijndael rijndaelAlgorithm = Rijndael.Create();
            rijndaelAlgorithm.Key = passwordDerivedBytes.GetBytes(32);
            rijndaelAlgorithm.IV = passwordDerivedBytes.GetBytes(16);

            MemoryStream memoryStream = new MemoryStream();

            CryptoStream cryptoStream = new CryptoStream(memoryStream, rijndaelAlgorithm.CreateEncryptor(), CryptoStreamMode.Write);
            cryptoStream.Write(plainTextBytes, 0, plainTextBytes.Length);
            cryptoStream.Close();

            byte[] encryptedBytes = memoryStream.ToArray();

            return Convert.ToBase64String(encryptedBytes);

        }

        public static string Decrypt(string encryptedText)
        {
            // Encryption operates on byte arrays, not on strings.
            byte[] cipherBytes = Convert.FromBase64String(encryptedText);

            // Derive a key from the password.
            PasswordDeriveBytes pdb = new PasswordDeriveBytes(NOT_SECRET_KEY,
                new byte[] { 0x49, 0x76, 0x61, 0x6e, 0x20, 0x4d, 0x65, 0x64, 0x76, 0x65, 0x64, 0x65, 0x76 });

            // Use Rijndael symmetric algorithm to do the decryption.
            Rijndael rijndaelAlgorithm = Rijndael.Create();
            rijndaelAlgorithm.Key = pdb.GetBytes(32);
            rijndaelAlgorithm.IV = pdb.GetBytes(16);

            MemoryStream memoryStream = new MemoryStream();
            CryptoStream cryptoStream = new CryptoStream(memoryStream, rijndaelAlgorithm.CreateDecryptor(), CryptoStreamMode.Write);
            cryptoStream.Write(cipherBytes, 0, cipherBytes.Length);
            cryptoStream.Close();

            byte[] decryptedData = memoryStream.ToArray();

            return System.Text.Encoding.Unicode.GetString(decryptedData);
        }


    }
}
