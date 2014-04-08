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

      void _AddCodePage(const AnsiString &sName, int iCodePage);

      std::map<AnsiString, int> m_mapCodePages;
   };

}
