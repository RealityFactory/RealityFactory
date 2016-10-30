/************************************************************************************//**
 * @file CInputMouseButton.cpp
 * @brief Implementation for RFSX Input MouseButton object.
 * @author Daniel Queteschiner
 * @date January 2014
 *//*
 * Copyright (c) 2014 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "CInputMouseButton.h"
#include <OIS.h>

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxMouseButton

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxMouseButton implementation
//////////////////////////////////////////////////////////////////////////////////////////

SX_IMPL_TYPE::SX_IMPL_TYPE()
{
	InitMouseButtonHT();
}


SX_IMPL_TYPE::~SX_IMPL_TYPE()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxMouseButton fields
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::getValue(const skString& name, const skString& /*att*/, skRValue& val)
{
	int *intValue = h_MouseButton.value(name);
	if(!intValue)
		return false;

	val = *intValue;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxMouseButton initialization
//////////////////////////////////////////////////////////////////////////////////////////

void SX_IMPL_TYPE::InitMouseButtonHT()
{
	h_MouseButton.insertKeyAndValue("Left",		new int(OIS::MB_Left));
	h_MouseButton.insertKeyAndValue("Right",	new int(OIS::MB_Right));
	h_MouseButton.insertKeyAndValue("Middle",	new int(OIS::MB_Middle));
	h_MouseButton.insertKeyAndValue("Button3",	new int(OIS::MB_Button3));
	h_MouseButton.insertKeyAndValue("Button4",	new int(OIS::MB_Button4));
	h_MouseButton.insertKeyAndValue("Button5",	new int(OIS::MB_Button5));
	h_MouseButton.insertKeyAndValue("Button6",	new int(OIS::MB_Button6));
	h_MouseButton.insertKeyAndValue("Button7",	new int(OIS::MB_Button7));
}

} // namespace RFSX

#undef SX_IMPL_TYPE // And that's it in a nutshell
