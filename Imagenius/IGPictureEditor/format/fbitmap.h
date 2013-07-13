#pragma once

class FBitmap : public Format
{
public:
  FBitmap();
  FBitmap(int fid, const std::wstring& sFormatName);
  ~FBitmap();
  virtual int write(LPDATAOBJECT pIDataObject, std::istream& is, HWND hwnd, int bufsize=0);
  virtual int read(LPDATAOBJECT pIDataObject, std::ostream& os, int* size, HWND hwnd);
  virtual std::wstring getName() const;
  virtual Format* getCopy() const;
};

inline FBitmap::FBitmap (int fid, const std::wstring& sFormatName)
: Format (fid, sFormatName)
{
}

inline std::wstring FBitmap::getName() const
{
  return L"Bitmap";
}

inline Format* FBitmap::getCopy() const
{
  return new FBitmap();
}
