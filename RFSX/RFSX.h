/************************************************************************************//**
 * @file RFSX.h
 * @brief
 * @author D.R. Gunter
 *//*
 * Copyright (c) 2008 D.R. Gunter; All rights reserved.
 ****************************************************************************************/

#ifndef _RFSX_H_
#define _RFSX_H_

#include <CEGUI.h>
#include <stdlib.h>

#include "Simkin/skInterpreter.h"
#include "Simkin/skExecutable.h"
#include "Simkin/skTreeNodeObject.h"
#include "Simkin/skTreeNode.h"
#include "Simkin/skSHashTable.h"
#include "Simkin/skRuntimeException.h"
#include "Simkin/skRValueArray.h"

// Type constant definitions used by executableType() for sk_obj RTTI
#define RFSX_GLOBALS_START		START_USER_TYPES

// Global library objects
#define RFSXG_NEW				RFSX_GLOBALS_START
#define RFSXG_FILE				RFSX_GLOBALS_START+1
#define RFSXG_MATH				RFSX_GLOBALS_START+2
#define RFSXG_LOG				RFSX_GLOBALS_START+3
#define RFSXG_INPUT				RFSX_GLOBALS_START+4
#define RFSXG_CAMERA			RFSX_GLOBALS_START+5
#define RFSXG_GUI				RFSX_GLOBALS_START+6
#define RFSXG_DIALOGSTATE		RFSX_GLOBALS_START+7
#define RFSXG_PLAYER			RFSX_GLOBALS_START+8
#define RFSXG_INVENTORY			RFSX_GLOBALS_START+9
#define RFSXG_CONVERSATION		RFSX_GLOBALS_START+10

#define RFSX_GLOBALS_NUM		11

// User data objects
#define RFSX_UTYPES_START		(RFSX_GLOBALS_START + RFSX_GLOBALS_NUM)

// Genesis3D
#define RFSXU_CAMERA			RFSX_UTYPES_START
#define RFSXU_VECTOR 			RFSX_UTYPES_START+1

#define RFSXU_ACTION 			RFSX_UTYPES_START+2
#define RFSXU_MOUSEBUTTON		RFSX_UTYPES_START+3
#define RFSXU_INVENTORYITEM		RFSX_UTYPES_START+4

// CEGUI
#define RFSXU_COLOUR			RFSX_UTYPES_START+10
#define RFSXU_VECTOR2			RFSX_UTYPES_START+11
#define RFSXU_POINT				RFSX_UTYPES_START+11	// same as RFSXU_VECTOR2
#define RFSXU_SIZE				RFSX_UTYPES_START+12
#define RFSXU_RECT				RFSX_UTYPES_START+13
#define RFSXU_COLOURRECT		RFSX_UTYPES_START+14
#define RFSXU_UDIM				RFSX_UTYPES_START+15
#define RFSXU_UVECTOR2			RFSX_UTYPES_START+16
#define RFSXU_URECT				RFSX_UTYPES_START+17
#define RFSXU_EVENT				RFSX_UTYPES_START+18
#define RFSXU_EVENTARGS			(RFSX_UTYPES_START+19)
#define RFSXU_EVENTCONNECTION	RFSX_UTYPES_START+20
#define RFSXU_EVENTSET			RFSX_UTYPES_START+21
#define RFSXU_GLOBALEVENTSET	RFSX_UTYPES_START+22
#define RFSXU_MOUSECURSOR		RFSX_UTYPES_START+23
#define RFSXU_FONT				RFSX_UTYPES_START+24
#define RFSXU_FONTMANAGER		RFSX_UTYPES_START+25
#define RFSXU_TEXTURE			RFSX_UTYPES_START+26
#define RFSXU_IMAGE				RFSX_UTYPES_START+27
#define RFSXU_IMAGESET			RFSX_UTYPES_START+28
#define RFSXU_IMAGESETMANAGER	RFSX_UTYPES_START+29
#define RFSXU_PROPERTY			RFSX_UTYPES_START+30
#define RFSXU_PROPERTYSET		RFSX_UTYPES_START+31
#define RFSXU_PROPERTYHELPER	RFSX_UTYPES_START+32
#define RFSXU_WINDOW			RFSX_UTYPES_START+33
#define RFSXU_WINDOWMANAGER		RFSX_UTYPES_START+34
#define RFSXU_COORDCONVERTER	RFSX_UTYPES_START+35
#define RFSXU_RENDERER			RFSX_UTYPES_START+36
#define RFSXU_SYSTEM			RFSX_UTYPES_START+37
#define RFSXU_SCHEME			RFSX_UTYPES_START+38
#define RFSXU_SCHEMEMANAGER		RFSX_UTYPES_START+39
#define RFSXU_LOGGER			RFSX_UTYPES_START+40
#define RFSXU_KEY				RFSX_UTYPES_START+41

#define RFSXU_GUISHEET			RFSX_UTYPES_START+42
#define RFSXU_TREEITEM			RFSX_UTYPES_START+43
#define RFSXU_LISTBOXITEM		RFSX_UTYPES_START+44
#define RFSXU_MCLGRIDREF		RFSX_UTYPES_START+45

#define RFSX_UTYPES_NUM			46

#define CEGUIDeadException(e) &

#if defined(_MSC_VER) && !defined(snprintf)
#   define snprintf   _snprintf
#endif

// Carpal-tunnel disease prevention mechanisms
#define SX_IMPL_TYPE
#define SX_METHOD_DECL(f)		static bool M_##f(SX_IMPL_TYPE* caller, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
#define SX_METHOD_IMPL(f)		bool SX_IMPL_TYPE::M_##f(SX_IMPL_TYPE* caller, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
#define SX_METHOD_CALL(f)		M_##f(this, args, r_val, ctxt)
#define SX_ADD_METHOD(f)		h_method.Add(#f, M_##f)
#define SX_INIT_MHT(n)			sxMHT<SX_IMPL_TYPE> SX_IMPL_TYPE::h_method(n, SX_IMPL_TYPE::InitMHT)
#define SX_INSTALL_LIB(n)		Interp->addGlobalVariable(#n, skRValue(new sx##n, true))
#define SX_P_FUNC_T(t)			bool (*) (t*, skRValueArray&, skRValue&, skExecutableContext&)
#define SX_P_FUNC_IT(i, t)		bool (*i)(t*, skRValueArray&, skRValue&, skExecutableContext&)

#define Q_METHOD_DECL(f)		bool Q_##f(float timeElapsed, skRValueArray& args)
#define Q_METHOD_IMPL(f)		bool SX_IMPL_TYPE::Q_##f(float timeElapsed, skRValueArray& args)
#define Q_P_FUNC(f)				&SX_IMPL_TYPE::Q_##f


#include "sxCore.h"

#endif // _RFSX_H_
