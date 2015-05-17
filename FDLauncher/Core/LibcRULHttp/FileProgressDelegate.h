/* -------------------------------------------------------------------------
//  File Name   :   FileProgressDelegate.h
//  Author      :   Li Jiahan
//  Create Time :   2012-4-17 20:05:33
//  Description :
//
// -----------------------------------------------------------------------*/

#ifndef __FILEPROGRESSDELEGATE_H__
#define __FILEPROGRESSDELEGATE_H__

#include "../Misc/FastDelegate/FastDelegate.h"
#include "../Misc/FastDelegate/FastDelegateBind.h"

//-------------------------------------------------------------------------

typedef fastdelegate::FastDelegate3<double /*total*/, double /*now*/, double /*speed*/>    FileProgressDelegate;

//--------------------------------------------------------------------------
#endif /* __FILEPROGRESSDELEGATE_H__ */