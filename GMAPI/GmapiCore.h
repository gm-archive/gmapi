/************************************************************************** 
  LICENSE:

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301 USA
***************************************************************************/

/*************************************************************************
  GmapiCore.h
  - GMAPICore library header

  Written by: Snake (http://gamebaseteam.eu)
***************************************************************************/

#pragma once

namespace gm {

  namespace core {
    extern "C" inline char** __stdcall GMAllocateString();
    extern "C" inline void __stdcall GMDeallocateString( __in char** aPtrString );
    extern "C" inline void __stdcall GMDeallocateResult( __deref_inout void* aPtrResult );

    extern "C" void __stdcall GMCallFunction( __in const void* aPtrFunction,
                                              __in void* aArgArray,
                                              __in const int aArgCount,
                                              __inout void* aPtrResult );

    extern "C" inline void __stdcall GMSetString( __in const char* aString, __inout char** aPtrString );
    extern "C" inline void __stdcall GMClearString( __deref_inout char** aPtrString );

    extern "C" inline unsigned long __stdcall GMAPIInitialize();

    extern "C" inline void __stdcall GMAPIHookInstall();
    extern "C" inline void __stdcall GMAPIHookUninstall();
  }
}