#include "stdafx.h"
#include "Format.h"

#include <atlbase.h>

int Format::counterType_ = Format::F_END;
std::list<Format*> Format::defaultFormats_;
std::map<std::wstring, int> Format::registeredFid_;


Format::Format()
: fid_(-1)
, sid_(static_cast<UINT>(-1))
{
}

Format::Format(int fid)
: fid_(fid)
, sid_(fid)
{
}

Format::Format(int fid, int sid)
: fid_(fid)
, sid_(sid)
{
}

Format::Format(int fid, const std::wstring& sFormatName)
: fid_(fid)
{
	sid_ = ::RegisterClipboardFormatW(sFormatName.c_str());
}

Format::Format(const std::wstring& sFormatName)
{
	fid_ = allocateKobraID(sFormatName);
	sid_ = ::RegisterClipboardFormatW(sFormatName.c_str());
}

// virtual
Format::~Format()
{
}

int Format::getKobraID() const
{
  return fid_;
}

UINT Format::getSystemID() const
{
  return sid_;
}

// virtual
bool Format::isKoData() const
{
  return false;
}

// virtual
int Format::write(LPDATAOBJECT, std::istream& is, HWND, int)
{
  return 0;
}

// virtual
int Format::read(LPDATAOBJECT, std::ostream& os, int*, HWND)
{
  return 0;
}

// virtual 
bool Format::isClipboardFormatAvailable() const
{
	return ::IsClipboardFormatAvailable(sid_) ? true : false;
}

int Format::allocateKobraID(const std::wstring& sFormatName)
{
	std::map<std::wstring, int>::const_iterator iter = Format::registeredFid_.find(sFormatName);
	if (iter != Format::registeredFid_.end())
	{
		return iter->second;
	}
	else
	{
		Format::registeredFid_.insert(std::pair<std::wstring, int>(sFormatName, counterType_));
		return counterType_++;
	}
}

#include "FBitmap.h"
#include "FDib.h"
#include "FIGLayer.h"
#include "FIGSelection.h"
#include "FIGFrame.h"

// static 
void Format::RegisterFormats()
{
	_ASSERT (defaultFormats_.size() == 0);
		
	defaultFormats_.push_back(new FBitmap());
	defaultFormats_.push_back(new FDib());	
	defaultFormats_.push_back(new FIGLayer());
	defaultFormats_.push_back(new FIGSelection());
	defaultFormats_.push_back(new FIGFrame());
}

// static 
std::list<Format*> Format::GetFormats()
{
	return defaultFormats_;
}

// static 
void Format::UnregisterFormats()
{
	while(!defaultFormats_.empty())
	{
		delete defaultFormats_.front();
		defaultFormats_.pop_front();
	}
}


// static 
Format* Format::findRegisteredFormat(int searchingFid)
{
	std::list<Format*>::const_iterator iter = defaultFormats_.begin();
	while(iter != defaultFormats_.end())
	{
		if ((*iter)->getKobraID() == searchingFid)
		{
			return (*iter);
		}
		++iter;
	}
	return 0;
}

