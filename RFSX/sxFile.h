/************************************************************************************//**
 * @file sxFile.h
 * @brief Class declaration for RFSX File library
 * @author D.R. Gunter
 *//*
 * Copyright (c) 2008 D.R. Gunter; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_FILE_H_
#define _SX_FILE_H_

#include "RFSX.h"

// sxFile class definition
/*
	This class provides support for loading and saving script files	on the fly.

	Methods:
		skTreeNode Load(skString filename);
		void Save(skString filename, skTreeNodeObject data);

	Examples:

	Load:
		MyFile.s
		{
			foo [bar];
		}

		SomeScript.s
		{
			SomeFile

			Init[()
			{
				SomeFile = File.Load("MyFile.s");
				debug(SomeFile.foo); // outputs "bar" to debug
			}]
		}

	Save:
		{
			SaveMe[(file)
			{
				File.Save(file, self); // dumps the pawn script(with current values) to file "file"
			}]
		}
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxFile

#define SXFILE_METHODS 2

namespace RFSX
{

class SX_IMPL_TYPE: public skExecutable
{
public:
	SX_IMPL_TYPE();
	virtual ~SX_IMPL_TYPE();
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual int executableType() const { return RFSXG_FILE; }

protected:
	SX_METHOD_DECL(Load);
	SX_METHOD_DECL(Save);

private:
	static sxMHT<SX_IMPL_TYPE>  h_method;
};

} // namespace RFSX

#endif // _SX_FILE_H_
