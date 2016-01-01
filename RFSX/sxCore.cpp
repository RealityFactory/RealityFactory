/************************************************************************************//**
 * @file sxCore.h
 * @brief Core component implementation for RFSX
 * @author D.R. Gunter
 *//*
 * Copyright (c) 2008 D.R. Gunter; All rights reserved.
 ****************************************************************************************/

#include "RFSX.h"

#include "sxNew.h"
#include "sxFile.h"
#include "sxMath.h"

namespace RFSX
{

// Automatic initialization handler class instance
sxMHT_init inits;

//////////////////////////////////////////////////////////////////////////////////////////
// RFSX Type-checking functions
//////////////////////////////////////////////////////////////////////////////////////////

// Type-checking function to insure valid parameter data
bool CheckParams(int pmin, int pmax, skRValueArray& args, skRValue::RType types[])
{
	int p = args.entries();

	if((pmax < p) || (p < pmin))
		return false; // bad parameter number

	for(int i=0; i<p; ++i)
	{
		if(args[i].type() != types[i])
			return false; // bad parameter type
	}

	return true;
}


void Install(skInterpreter* Interp)
{
	static bool called = false;

	// Only call this once
	if(called) return;

	// Initialize the MHTs
	inits.execute();

	// Install global libs
	SX_INSTALL_LIB(New);
	SX_INSTALL_LIB(File);
	SX_INSTALL_LIB(Math);

	called = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxMHT_init class implementation
//////////////////////////////////////////////////////////////////////////////////////////

sxMHT_init::sxMHT_init()
{
}


sxMHT_init::~sxMHT_init()
{
}


void sxMHT_init::AddMHT(void (*pfunc)(void))
{
	MHT_Inits.push_back(pfunc);
}


void sxMHT_init::execute()
{
	for(std::vector<void(*)(void)>::const_iterator iter=MHT_Inits.begin(); iter!=MHT_Inits.end(); ++iter)
	{
		(*iter)();
	}
}

} // namespace RFSX
