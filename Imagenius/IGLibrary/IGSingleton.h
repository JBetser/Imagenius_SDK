#pragma once

namespace IGLibrary
{

template <class T>
class IGSingleton
{
public:
	IGSingleton()
	{
		if (!g_nRef++)
			g_pThis = new T();
	}

	~IGSingleton()
	{
		if (--g_nRef <= 0)
		{
			g_nRef = 0;
			if (g_pThis)
			{
				delete g_pThis;
				g_pThis = NULL;
			}
		}
	}

	T* operator -> () const
	{
		return g_pThis;
	}

	operator T*() const
	{
		return g_pThis;
	}

	void Release()
	{
		if (g_pThis)
			delete g_pThis;
		g_pThis = NULL;
		g_nRef = 0;
	}

private:
	static int g_nRef;
	static T *g_pThis;
};

template <class T>
int IGSingleton<T>::g_nRef = 0;

template <class T>
T *IGSingleton<T>::g_pThis = NULL;

}