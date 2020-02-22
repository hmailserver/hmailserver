#pragma once

namespace HM
{
   class CodePages : public Singleton<CodePages>
   {
   public:
	   CodePages();
	   virtual ~CodePages();

      void Initialize();

      int GetCodePage(const AnsiString &sName) const;

   private:

      void AddCodePage_(const AnsiString &sName, int iCodePage);

      std::map<AnsiString, int> code_pages_;
   };

}
