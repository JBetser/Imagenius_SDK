#pragma once
#include "IGConvertible.h"
#include "IGSmartPtr.h"
#include "IGThreadSafeData.h"
#include "IGSmartPixel.h"
#include <string>
#include <vector>
#include <set>

#pragma warning (push)
#pragma warning (disable : 4244)	// conversion from double to int accepted

namespace IGLibrary
{
	class IGVirtualProperty{
		BEGIN_CONVERSION_MAP()
			CONVERSION_SUPPORT(int)
			CONVERSION_SUPPORT(wstring)
			CONVERSION_SUPPORT(IGConvertible)
			CONVERSION_SUPPORT_WITH_HANDOVER(double,int)
			CONVERSION_SUPPORT_WITH_HANDOVER(IGFRAMEPROPERTY_UNIT_ENUM,int)
			CONVERSION_SUPPORT_WITH_HANDOVER(IGFRAMEPROPERTY_STRETCHING_ENUM,int)
			CONVERSION_SUPPORT_WITH_HANDOVER(IGBRUSH_TYPE_ENUM,int)
			CONVERSION_SUPPORT_WITH_HANDOVER(IGSELECTTEXTURE_TYPE_ENUM,int)
			CONVERSION_SUPPORT_WITH_HANDOVER(IGFRAMEPROPERTY_SHOWSEGMENTATION_ENUM,int)
		END_CONVERSION_MAP()

		BEGIN_RELEASE_MAP()
			RELEASE_SUPPORT(int)
			RELEASE_SUPPORT(double)
			RELEASE_SUPPORT(wstring)
			RELEASE_SUPPORT(IGConvertible)
			RELEASE_SUPPORT(IGFRAMEPROPERTY_UNIT_ENUM)
			RELEASE_SUPPORT(IGFRAMEPROPERTY_STRETCHING_ENUM)
			RELEASE_SUPPORT(IGBRUSH_TYPE_ENUM)
			RELEASE_SUPPORT(IGSELECTTEXTURE_TYPE_ENUM)
			RELEASE_SUPPORT(IGFRAMEPROPERTY_SHOWSEGMENTATION_ENUM)
		END_RELEASE_MAP()

	public:	
		IGVirtualProperty (LPCWSTR pcwName);
		IGVirtualProperty (const std::wstring& wsName);
		IGVirtualProperty (const IGVirtualProperty& propertyCopy);
		virtual ~IGVirtualProperty();

		IGVirtualProperty operator = (const IGVirtualProperty& prop);
		bool operator == (const wchar_t *pcwName) const;
		bool operator == (const std::wstring& wsName) const;
		bool operator == (const IGVirtualProperty& prop) const;
		bool operator < (const IGVirtualProperty& prop) const;

		template <typename T>
		void SetValue(const T& value) const
		{
			// allocation & copy to param
			if ((isUnkown() && !isTypeEqual(value)) ||
				(!isUnkown() && (isTypeEqual(value) || isTypeConvertibleTo(value))))
			{
				allocate (value);
				if (isTypeConvertibleTo(value))
					convertTo (value);
				else
					assignTo (value);
			}
		}

		template <typename T>
		void GetValue(T& value) const
		{
			// allocation & copy from param
			if ((isUnkown() && !isTypeEqual(value)) ||
				(!isUnkown() && (isTypeEqual(value) || isTypeConvertibleTo(value))))
			{
				allocate (value);
				value = *reinterpret_cast <T*> (*m_spData);
			}
			// conversion to string
			if ((getType (value) == ENUM_IGTYPE_wstring) && !isTypeEqual(value))
			{
				IGToString convToStr;
				convToStr (*reinterpret_cast <std::wstring*> (&value), *m_spData, m_ePropType);
			}
		}

		bool GetVARIANT (VARIANT& var) const
		{
			int nVal = 0;
			std::wstring wsVal;
			switch (var.vt)
			{
			case VT_INT:
				GetValue (nVal); 
				var.intVal = nVal;
				return true;
			case VT_I4:
				GetValue (nVal); 
				var.lVal = nVal;
				return true;
			case VT_I8:
				GetValue (nVal); 
				var.llVal = nVal;
				return true;
			case VT_R4:
			case VT_R8:
				GetValue (var.dblVal); 
				return true;
			case VT_BSTR:
			{	GetValue (wsVal); 
				CComBSTR spbstrVal (wsVal.c_str());
				var.bstrVal = spbstrVal.Detach();
				return true;
			}
			}
			return false;
		}

		bool SetVARIANT (const VARIANT& var) const
		{
			switch (var.vt)
			{
			case VT_INT:
				SetValue ((int)var.intVal); 
				return true;
			case VT_I4:
				SetValue ((int)var.lVal); 
				return true;
			case VT_I8:
				SetValue ((int)var.llVal); 
				return true;
			case VT_R4:
				SetValue ((double)var.fltVal); 
				return true;
			case VT_R8:
				SetValue ((double)var.dblVal); 
				return true;
			case VT_BSTR:
				{	std::wstring wsVal (var.bstrVal);
					SetValue (wsVal); 
					return true;
				}
			case VT_UNKNOWN:
				{
					SetValue (dynamic_cast <IGConvertible&> (*var.punkVal));
					return true;
				}
			}
			return false;
		}

		inline ENUM_IGTYPE GetType() const;
		inline const std::wstring& GetName() const;

	protected:
		mutable IGSmartPtr <unsigned int> m_spData;
		mutable ENUM_IGTYPE m_ePropType;
		mutable ENUM_IGTYPE m_ePropTypeHandover;

	private:
		inline bool isUnkown() const {
			return (m_ePropType == ENUM_IGTYPE_UNKNOWN);
		}

		inline void convertTo (const IGConvertible& newData) const
		{
			switch (m_ePropType)
			{
			case ENUM_IGTYPE_IGConvertible:
				*reinterpret_cast <IGConvertible*> (*m_spData) = newData;
				break;
			default:
				assignTo (newData);
				break;
			}
		}

		template <typename TypeDataConvert>
		inline void convertTo (const TypeDataConvert& newData) const
		{
			switch (m_ePropType)
			{
			case ENUM_IGTYPE_double:
				*reinterpret_cast <double*> (*m_spData) = newData;
				break;
			case ENUM_IGTYPE_IGConvertible:
				*reinterpret_cast <IGConvertible*> (*m_spData) = newData;
				break;
			default:
				assignTo (newData);
				break;
			}
		}

		inline void convertTo (const std::wstring& newData) const
		{
			assignTo (newData);
		}

		template <typename TypeDataConvert>
		inline void assignTo (const TypeDataConvert& newData) const
		{
			*reinterpret_cast <TypeDataConvert*> (*m_spData) = newData;
		}

		// secondary type support (optional)
		inline void setTypeHandover(...) const {}
		inline bool isTypeConvertibleTo(...) const {return false;}

		template <typename T>
		void allocate(const T& value) const
		{
			if (!*m_spData)
			{
				*m_spData = reinterpret_cast <unsigned int> (new T ());
				setType (value);
				setTypeHandover (value, value);
			}
		}

		void allocate(const IGConvertible& value) const
		{
			if (!*m_spData)
			{
				*m_spData = reinterpret_cast <unsigned int> (new IGConvertible ());
				setType (value);
				setTypeHandover (value, value);
			}
		}

		std::wstring	m_wsName;
		unsigned int	m_uiOrder;
		static unsigned int	mg_uiNbInstances;
	};

	inline ENUM_IGTYPE IGVirtualProperty::GetType() const
	{
		return m_ePropType;
	}

	inline const std::wstring& IGVirtualProperty::GetName() const
	{
		return m_wsName;
	}

	class IGPropertySet : public std::set <IGVirtualProperty>
	{
	public:	
		const IGVirtualProperty& operator [] (const wchar_t *pcwPropId)
		{
			for(std::set<IGVirtualProperty>::const_iterator it = begin(); it != end(); ++it){
				if (*it == pcwPropId)
				{
					return *it;
				}
			}
			insert (IGVirtualProperty (pcwPropId));
			return operator [] (pcwPropId);
		}
	};

	class IGHasProperties
	{
	public:
		IGHasProperties(){}
		virtual ~IGHasProperties(){}

		template <typename T>
		bool SetProperty (const wchar_t *pcwPropId, const T& propVal)
		{		
			if (!pcwPropId)
				return false;
			IGPropertySet& setProp = m_setProperties.Lock();
			setProp [pcwPropId].SetValue (propVal);
			m_setProperties.UnLock();
			return true;
		}

		template <typename T>
		bool GetProperty (const wchar_t *pcwPropId, T& propVal) const
		{
			if (!pcwPropId)
				return false;
			IGPropertySet& setProp = m_setProperties.Lock();
			setProp [pcwPropId].GetValue (propVal);
			m_setProperties.UnLock();
			return true;
		}

		bool GetPropertyType (const wchar_t *pcwPropId, ENUM_IGTYPE& eType) const
		{
			if (!pcwPropId)
				return false;
			IGPropertySet& setProp = m_setProperties.Lock();
			eType = setProp [pcwPropId].GetType();
			m_setProperties.UnLock();
			return true;
		}

		bool GetPropertyVARIANT (const wchar_t *pcwPropId, VARIANT& varVal) const
		{
			if (!pcwPropId)
				return false;
			IGPropertySet& setProp = m_setProperties.Lock();
			bool bRet = setProp [pcwPropId].GetVARIANT (varVal);
			m_setProperties.UnLock();
			return bRet;
		}

		bool SetPropertyVARIANT (const wchar_t *pcwPropId, VARIANT& varVal)
		{
			if (!pcwPropId)
				return false;
			IGPropertySet& setProp = m_setProperties.Lock();
			bool bRet = setProp [pcwPropId].SetVARIANT (varVal);
			m_setProperties.UnLock();
			return bRet;
		}

		std::vector <std::wstring> GetProperties() const
		{
			IGPropertySet& setProp = m_setProperties.Lock();
			std::vector <std::wstring> vProp;
			for(std::set<IGVirtualProperty>::iterator it = setProp.begin(); it != setProp.end(); ++it){
				vProp.push_back ((*it).GetName());
			}
			m_setProperties.UnLock();
			return vProp;
		}

		unsigned int GetNbProperties() const
		{
			IGPropertySet& setProp = m_setProperties.Lock();
			int nSize = setProp.size();
			m_setProperties.UnLock();
			return nSize;
		}

		std::vector <std::pair <std::wstring, std::wstring>> GetPropertiesAndValues() const
		{
			IGPropertySet& setProp = m_setProperties.Lock();
			std::vector <std::pair <std::wstring, std::wstring>> vProp;
			for(std::set<IGVirtualProperty>::iterator it = setProp.begin(); it != setProp.end(); ++it){
				static std::wstring wsVal;
				(*it).GetValue (wsVal);
				vProp.push_back (std::pair <std::wstring, std::wstring> ((*it).GetName(), wsVal));
			}
			m_setProperties.UnLock();
			return vProp;
		}

		bool GetPropertiesAndValues (std::wstring& wsProperties) const
		{
			IGPropertySet& setProp = m_setProperties.Lock();
			std::vector <std::pair <std::wstring, std::wstring>> vProp;
			for(std::set<IGVirtualProperty>::iterator it = setProp.begin(); it != setProp.end(); ++it){
				std::wstring wsName = (*it).GetName();
				if (wsName.empty())
					wsName = L"Unknown";
				std::wstring wsVal;
				(*it).GetValue (wsVal);
				if (wsVal.empty())
					wsVal = L"NoValue";
				if (it != setProp.begin())
					wsProperties += L"_";
				wsProperties += wsName;
				wsProperties += L"_";
				wsProperties += wsVal;
			}
			m_setProperties.UnLock();
			return true;
		}

		void Clear(){
			m_setProperties.Lock().clear();
			m_setProperties.UnLock();
		}

	private:
		mutable IGThreadSafeData <IGPropertySet>	m_setProperties;
	};
}

#pragma warning (pop)