#pragma once

class FIGSelection : public Format
{
public:
  FIGSelection();
  FIGSelection (int fid, const std::wstring& sFormatName);
  ~FIGSelection();
  virtual int write(LPDATAOBJECT pIDataObject, std::istream& is, HWND hwnd, int bufsize=0);
  virtual int read(LPDATAOBJECT pIDataObject, std::ostream& os, int* size, HWND hwnd);
  virtual std::wstring getName() const;
  virtual Format* getCopy() const;
};

inline FIGSelection::FIGSelection (int fid, const std::wstring& sFormatName)
: Format (fid, sFormatName)
{
}

inline std::wstring FIGSelection::getName() const
{
  return L"Imagenius Selection";
}

inline Format* FIGSelection::getCopy() const
{
  return new FIGSelection();
}
