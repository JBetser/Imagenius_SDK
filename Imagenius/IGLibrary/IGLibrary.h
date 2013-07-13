#pragma once

namespace IGLibrary
{
	extern "C"
	{
		// This initializes IGLibrary
		void Init (HINSTANCE hInstance);

		// This terms IGLibrary
		void Term ();

		// Get the module instance
		HINSTANCE GetInstance();
	}
}