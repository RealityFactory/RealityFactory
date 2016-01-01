/************************************************************************************//**
 * @file RFSX.h
 * @brief
 * @author D.R. Gunter
 *//*
 * Copyright (c) 2008 D.R. Gunter; All rights reserved.
 ****************************************************************************************/

#ifndef _RFSX_H_
#define _RFSX_H_

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

#define RFSX_GLOBALS_NUM		4

// User data objects
#define RFSX_UTYPES_START		(RFSX_GLOBALS_START + RFSX_GLOBALS_NUM)

// Genesis3D
#define RFSXU_CAMERA			RFSX_UTYPES_START
#define RFSXU_VECTOR 			RFSX_UTYPES_START+1

#define RFSX_UTYPES_NUM			2

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
