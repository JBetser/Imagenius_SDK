#pragma once

class FIGFrame : public FIGLayer
{
public:
  FIGFrame();
  ~FIGFrame();
  virtual int write(LPDATAOBJECT pIDataObject, std::istream& is, HWND hwnd, int bufsize=0);
  virtual int read(LPDATAOBJECT pIDataObject, std::ostream& os, int* size, HWND hwnd);
  virtual std::wstring getName() const;
  virtual Format* getCopy() const;
};

inline std::wstring FIGFrame::getName() const
{
  return L"Imagenius Frame";
}

inline Format* FIGFrame::getCopy() const
{
  return new FIGFrame();
}
