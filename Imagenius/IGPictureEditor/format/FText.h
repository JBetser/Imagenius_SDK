#pragma once

#include "Format.h"

///////////////////////////////////
//
//   LEVEL 1 FTEXT
//
////////////////////////////////////
class FText : public Format
{
public:
  virtual int write(LPDATAOBJECT pIDataObject, std::istream& is, HWND hwnd, int bufsize=0);
  virtual int read(LPDATAOBJECT pIDataObject, std::ostream& os, int* size, HWND hwnd);
  virtual bool isUnicodeText() const;

protected :
  FText(int fid);
  FText(int fid, int sid);
  FText(int fid, const std::wstring& szFormatName);
  FText(const std::wstring& szFormatName);
};
