#pragma once

class FIGLayer : public Format
{
public:
  FIGLayer();
  FIGLayer (int fid, const std::wstring& sFormatName);
  ~FIGLayer();
  virtual int write(LPDATAOBJECT pIDataObject, std::istream& is, HWND hwnd, int bufsize=0);
  virtual int read(LPDATAOBJECT pIDataObject, std::ostream& os, int* size, HWND hwnd);
  virtual std::wstring getName() const;
  virtual Format* getCopy() const;
};

inline FIGLayer::FIGLayer (int fid, const std::wstring& sFormatName)
: Format (fid, sFormatName)
{
}

inline std::wstring FIGLayer::getName() const
{
  return L"Imagenius Layer";
}

inline Format* FIGLayer::getCopy() const
{
  return new FIGLayer();
}
