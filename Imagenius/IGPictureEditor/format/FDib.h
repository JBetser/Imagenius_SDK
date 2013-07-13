#pragma once

class FDib : public Format
{
public:
  FDib();
  FDib (int fid, const std::wstring& sFormatName);
  ~FDib();
  virtual int write(LPDATAOBJECT pIDataObject, std::istream& is, HWND hwnd, int bufsize=0);
  virtual int read(LPDATAOBJECT pIDataObject, std::ostream& os, int* size, HWND hwnd);
  virtual std::wstring getName() const;
  virtual Format* getCopy() const;
};

inline FDib::FDib (int fid, const std::wstring& sFormatName)
: Format (fid, sFormatName)
{
}

inline std::wstring FDib::getName() const
{
  return L"Device Independent Bitmap";
}

inline Format* FDib::getCopy() const
{
  return new FDib();
}
