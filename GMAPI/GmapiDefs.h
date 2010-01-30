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
  GmapiDefs.h
  - Definitions and declarations of GM's strucctures used by GMAPI

  Written by: Snake (http://gamebaseteam.eu)
***************************************************************************/

#pragma once

#ifndef GMAPI_NO_D3D
  #include <d3d8.h>
#else
  typedef void IDirect3D8, IDirect3DDevice8, IDirect3DTexture8;
#endif

namespace gm {

  enum BoundingBoxType { BBOX_UNKNOWN = -1, BBOX_AUTOMATIC, BBOX_FULLIMAGE, BBOX_MANUAL };
  enum SoundType { SND_UNKNOWN = -1, SND_NORMAL, SND_BGMUSIC, SND_3DSOUND, SND_MULTIMEDIA };

// 0 = normal snd; 1 = bg music; 2 = 3d sound; 3 = mmedia snd

  typedef struct __GMDIRECT3DINFO {
    IDirect3D8* d3dInterface;
    IDirect3DDevice8* d3dDevice;

    int renderWidth;
    int renderHeight;
  } GMDIRECT3DINFO, *LPGMDIRECT3DINFO;

  typedef struct __GMBITMAP {
    void* rttiData;

    BOOL exists;

    unsigned long width;
    unsigned long height;

    unsigned char* bitmapData;
  } GMBITMAP, *LPGMBITMAP;

  typedef struct __GMTEXTURE {
    IDirect3DTexture8* texture;

    unsigned long imageWidth;
    unsigned long imageHeight;

    unsigned long textureWidth;
    unsigned long textureHeight;

    BOOL isValid;
  } GMTEXTURE, *LPGMTEXTURE;

  typedef __declspec( align( 4 ) ) struct __GMSPRITE {
    void* rttiData;

    BoundingBoxType bboxType;
    int bboxLeft;
    int bboxTop;
    int bboxRight;
    int bboxBottom;

    unsigned long nSubimages;

    unsigned long width;
    unsigned long height;

    int originX;
    int originY;

    BOOL maskExists;
    void* collisionMask; 
    BOOL preciseCollision;

    LPGMBITMAP* bitmaps;

    bool transparent;
    bool smoothEdges;
    bool preload;

    unsigned long* textureIDs;
  } GMSPRITE, *LPGMSPRITE;

  typedef struct __GMSPRITESTORAGE {
    LPGMSPRITE* sprites;
    char** names;

    int arraySize;
  } GMSPRITESTORAGE, *LPGMSPRITESTORAGE;

  typedef __declspec( align( 4 ) ) struct __GMBACKGROUND {
    void* rttiData;

    int width;
    int height;

    GMBITMAP* bitmap;

    bool transparent;
    bool smoothEdges;
    bool preload;

    int textureId;
    unsigned long unknown;
  } GMBACKGROUND, *LPGMBACKGROUND;

  typedef struct __GMBACKGROUNDSTORAGE {
    LPGMBACKGROUND* backgrounds;
    char** names;

    int arraySize;
  } GMBACKGROUNDSTORAGE, *LPGMBACKGROUNDSTORAGE;

  typedef struct __GMSURFACE {
    int textureId;

    int width;
    int height;

    BOOL exists;
  } GMSURFACE, *LPGMSURFACE;


  typedef __declspec( align( 1 ) ) struct __GMFUNCTIONINFO {
    unsigned char nameLength;
    char name[67];

    void* address;
    char padding[8];
  } GMFUNCTIONINFO, *LPGMFUNCTIONINFO;

  typedef struct __GMFUNCTIONINFOSTORAGE {
    GMFUNCTIONINFO* functions;
    unsigned long nFunctions;
  } GMFUNCTIONINFOSTORAGE, *LPGMFUNCTIONINFOSTORAGE;

  typedef struct __GMSCRIPTCONTENT {
    void* rttiData;
    char* code;
    
    unsigned long unknown1;
    unsigned long unknown2;
    unsigned long unknown3;
  } GMSCRIPTCONTENT, *LPGMSCRIPTCONTENT;

  typedef struct __GMSCRIPTDEBUGINFO {
    void* rttiData;
    unsigned long unknown;

    BOOL isCompiled;

    char* code;

    unsigned long type;
  } GMSCRIPTDEBUGINFO, *LPGMSCRIPTDEBUGINFO;

  typedef struct __GMSCRIPT {
    void* rttiData;

    GMSCRIPTCONTENT* pScriptContent;
    GMSCRIPTDEBUGINFO* debugInfo;
  } GMSCRIPT, *LPGMSCRIPT;

  typedef struct __GMSCRIPTSTORAGE {
    char** symbols;
    unsigned long nSymbols;

    LPGMSCRIPT* scripts;
    char** names;

    int arraySize;
  } GMSCRIPTSTORAGE, *LPGMSCRIPTSTORAGE;

  typedef struct __GMSOUNDDATA {
    void* rttiData;

    unsigned char* file;
    unsigned long fileSize;
  } GMSOUNDDATA, *LPGMSOUNDDATA;

  typedef struct __GMSOUND {
    void* rttiData;

    SoundType type;

    char* fileExt;
    char* filename;

    GMSOUNDDATA* sndData;

    BOOL preload;
    unsigned long effectsBitmask;

    unsigned long unknown;

    double volume;
    double pan;

    int sndId;

    char* filePath;
  } GMSOUND, *LPGMSOUND;

  typedef struct __GMSOUNDSTORAGE {
    LPGMSOUND* sounds;
    char** names;

    int arraySize;
  } GMSOUNDSTORAGE, *LPGMSOUNDSTORAGE;

}
