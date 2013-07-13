/*
 * File:	xfile.h
 * Purpose:	General Purpose File Class 
 */
/*
  --------------------------------------------------------------------------------

	COPYRIGHT NOTICE, DISCLAIMER, and LICENSE:

	CxBrush (c)  02/March/2011 Jonathan Betser

	Covered code is provided under this license on an "as is" basis, without warranty
	of any kind, either expressed or implied, including, without limitation, warranties
	that the covered code is free of defects, merchantable, fit for a particular purpose
	or non-infringing. The entire risk as to the quality and performance of the covered
	code is with you. Should any covered code prove defective in any respect, you (not
	the initial developer or any other contributor) assume the cost of any necessary
	servicing, repair or correction. This disclaimer of warranty constitutes an essential
	part of this license. No use of any covered code is authorized hereunder except under
	this disclaimer.

	Permission is hereby granted to use, copy, modify, and distribute this
	source code, or portions hereof, for any purpose, including commercial applications,
	freely and without fee, subject to the following restrictions: 

	1. The origin of this software must not be misrepresented; you must not
	claim that you wrote the original software. If you use this software
	in a product, an acknowledgment in the product documentation would be
	appreciated but is not required.

	2. Altered source versions must be plainly marked as such, and must not be
	misrepresented as being the original software.

	3. This notice may not be removed or altered from any source distribution.
  --------------------------------------------------------------------------------
 */

#if !defined(__xbrush_h)
#define __xbrush_h

class DLL_EXP CxBrush
{
public:
	CxBrush() { };
	virtual ~CxBrush() { };

	virtual bool Apply (float x, float y, const BYTE *pOriginalBits, BYTE *pBits, BYTE *pAlpha, BYTE *pSel, const int& nWidth, const int& nHeight, const DWORD& dwEffWidth) const = 0;
};

#endif // __xbrush_h