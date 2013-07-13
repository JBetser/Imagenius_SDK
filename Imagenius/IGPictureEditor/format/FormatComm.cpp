#include "stdafx.h"
#include "Format.h"
#include "FormatComm.h"

FormatCommunication::FormatCommunication(Format* fmt, int cid, int mode)
{
  fmt_ = fmt;
  cid_ = cid;
  mode_ = mode;
}

FormatCommunication::~FormatCommunication()
{
}

void FormatCommunication::write(LPDATAOBJECT pIDataObject, std::istream& is, HWND hwnd, int bufsize)
{
  fmt_->write(pIDataObject, is, hwnd, bufsize);
}

void FormatCommunication::read(LPDATAOBJECT pIDataObject, std::ostream& os, int* size, HWND hwnd)
{
  fmt_->read(pIDataObject, os, size, hwnd);
}

int FormatCommunication::getKobraID() const
{
  return fmt_->getKobraID();
}

int FormatCommunication::cid() const
{
  return cid_;
}

void FormatCommunication::cid(int cid)
{
  cid_ = cid;
}

int FormatCommunication::mode() const
{
  return mode_;
}

void FormatCommunication::mode(int mode)
{
  mode_ = mode;
}

Format* FormatCommunication::format() const
{
  return fmt_;
}
