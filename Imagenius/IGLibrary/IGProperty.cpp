#include "StdAfx.h"
#include "IGProperty.h"

using namespace IGLibrary;
using namespace std;

unsigned int	IGVirtualProperty::mg_uiNbInstances = 0;

IGVirtualProperty::IGVirtualProperty (LPCWSTR pcwName)	: m_wsName (pcwName)
											, m_spData (new unsigned int)
											, m_ePropType (ENUM_IGTYPE_UNKNOWN)
											, m_ePropTypeHandover (ENUM_IGTYPE_UNKNOWN)
											, m_uiOrder (mg_uiNbInstances++)
{	 
	*m_spData = 0;
}

IGVirtualProperty::IGVirtualProperty (const std::wstring& wsName) : m_wsName (wsName)
													, m_spData (new unsigned int)
													, m_ePropType (ENUM_IGTYPE_UNKNOWN)
													, m_ePropTypeHandover (ENUM_IGTYPE_UNKNOWN)
													, m_uiOrder (mg_uiNbInstances++)
{
	*m_spData = 0;
}

IGVirtualProperty::IGVirtualProperty (const IGVirtualProperty& propertyCopy) : m_wsName (propertyCopy.m_wsName)
	, m_spData (propertyCopy.m_spData)
	, m_ePropType (propertyCopy.m_ePropType)
	, m_ePropTypeHandover (propertyCopy.m_ePropTypeHandover)
	, m_uiOrder (propertyCopy.m_uiOrder)
{
}

IGVirtualProperty::~IGVirtualProperty(void)
{
	// if smart ptr is about to be released, release the underlying data
	if (m_spData.use_count() == 1)
		release();
}

bool IGVirtualProperty::operator == (const wchar_t *pcwName) const
{
	return m_wsName == pcwName;
}

bool IGVirtualProperty::operator == (const std::wstring& wsName) const
{
	return m_wsName == wsName;
}

bool IGVirtualProperty::operator == (const IGVirtualProperty& prop) const
{
	return m_wsName == prop.m_wsName;
}

bool IGVirtualProperty::operator < (const IGVirtualProperty& prop) const
{
	return m_uiOrder < prop.m_uiOrder;
}

IGVirtualProperty IGVirtualProperty::operator = (const IGVirtualProperty& prop)
{
	m_wsName = prop.m_wsName;
	return m_wsName;
}