/*+===================================================================
  File:      COMMON.H
  Summary:   Common header file that contains common header files and
			 macros used for the Library project of Game Graphics
			 Programming course.
  Functions:
  © 2022 Kyung Hee University
===================================================================+*/
#pragma once

#ifndef  UNICODE
#define UNICODE
#endif // ! UNICODE

#define WIN32_LEAN_AND_MEAN // C4668 ERROR
#include <windows.h>
#include <dxgi.h>
#include <d3d11_4.h>
#include <directxcolors.h>

#include "Resource.h"

constexpr LPCWSTR PSZ_COURSE_TITLE = L"Game Graphics Programming";

using namespace DirectX;