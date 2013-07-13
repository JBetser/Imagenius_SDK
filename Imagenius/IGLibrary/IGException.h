#pragma once
#include <exception>
#include <string>

#define IGEXCEPTIONARGS(func, reason) __FILE__,__LINE__,(func),(reason)
#define IGEXCEPTION(instance, func, reason) instance(IGEXCEPTIONARGS(func,reason))

namespace IGLibrary
{
	class IGException :
		public std::exception
	{
	public:
		IGException(const char *pcString) : m_sWhat (pcString) {}
		virtual ~IGException(){}

		virtual const char* what(){return m_sWhat.c_str();}

		IGException& operator () (const char *pcFile, int nLine, const char *pcFunc, const char *pcReason = NULL) {
			m_sWhat.insert(0, " - ");
			char tcLine[32]; ::_itoa_s(nLine, tcLine, 10);
			m_sWhat.insert(0, tcLine); m_sWhat.insert(0, std::string(", Line: ")); 
			m_sWhat.insert(0, std::string(pcFile)); m_sWhat.insert(0, std::string("in File: "));
			m_sWhat += "::"; m_sWhat += pcFunc;
			if (pcReason){
				m_sWhat += " - ";
				m_sWhat += pcReason;
			}
			return *this;
		}

	private:
		std::string m_sWhat;
	};
}
