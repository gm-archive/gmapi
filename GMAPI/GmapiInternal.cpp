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
  GmapiInternal.cpp
  - Definitions of all GMAPI components, excluding wrapped gm functions

  Written by: Snake (http://gamebaseteam.eu)
***************************************************************************/

#include "GmapiInternal.h"
#include "GmapiMacros.h"

using namespace gm::core;

namespace gm {

  LPGMFUNCTIONINFOSTORAGE CGMAPI::m_functionData;
  LPGMBACKGROUNDSTORAGE   CGMAPI::m_backgroundData;
  LPGMSPRITESTORAGE       CGMAPI::m_spriteData;
  LPGMSURFACE*            CGMAPI::m_surfaces;
  LPGMTEXTURE*            CGMAPI::m_textures;
  LPGMDIRECT3DINFO        CGMAPI::m_d3dInfo;
  LPGMSCRIPTSTORAGE       CGMAPI::m_scriptData;
  LPGMSOUNDSTORAGE        CGMAPI::m_soundData;

  int*                    CGMAPI::m_surfaceArraySize;
  char*                   CGMAPI::m_scriptSwapTable;

  HWND                    CGMAPI::m_mainHwnd;
  HWND                    CGMAPI::m_debugHwnd;
  HWND                    CGMAPI::m_hScrHwnd;

  LPGMSPRITE              ISprite::m_sprite;
  int                     ISprite::m_spriteId;
  int                     ISpriteSubimage::m_subimage;
  LPGMBACKGROUND          IBackground::m_background;
  int                     IBackground::m_backgroundId;
  int                     ISurface::m_surfaceId;
  int                     IScript::m_scriptId;
  int                     ISound::m_soundId;

  /********************************************
   * CGMAPI class implementation
   ********************************************/

  CGMAPI* CGMAPI::m_self = NULL;
  void* CGMAPI::m_gmFunctions[1000];

  CGMAPI* CGMAPI::Create( unsigned long* aResult ) {
    if ( !m_self ) {
      bool success = false;
      m_self = new CGMAPI( &success );

      if ( !success ) {
        delete m_self;
        m_self = NULL;

        *aResult = GMAPI_INITIALIZATION_FAILED;
      } else
        *aResult = GMAPI_INITIALIZATION_SUCCESS;
    } else
      *aResult = GMAPI_ALREADY_INITIALIZED;

    return m_self;    
  }

  CGMAPI::CGMAPI( bool* aSuccess ): m_gmVersion( 0 ) {
    m_gmVersion = GMAPIInitialize();

    if ( !m_gmVersion ) {
      *aSuccess = false;
      return;
    }

    GMAPIHookInstall();
    RetrieveDataPointers();
    RetrieveFunctionPointers();

    *aSuccess = true;
  }

  void CGMAPI::Destroy() {
    if ( m_self ) {
      delete m_self;
      m_self = NULL;
    }
  }

  CGMAPI::~CGMAPI() {
    GMAPIHookUninstall();
  }

  void CGMAPI::RetrieveDataPointers() {
    if ( m_gmVersion == GM_VERSION_70 ) {
      m_functionData =                    GM70_ADDRESS_ARRAY_GMFUNCTIONS;
      m_spriteData =                      GM70_ADDRESS_STORAGE_SPRITES;
      m_backgroundData =                  GM70_ADDRESS_STORAGE_BACKGROUNDS;
      m_surfaces =         (LPGMSURFACE*) GM70_ADDRESS_ARRAY_SURFACES;
      m_textures =         (LPGMTEXTURE*) GM70_ADDRESS_ARRAY_TEXTURES;
      m_d3dInfo =                         GM70_ADDRESS_STORAGE_D3D;
      m_scriptData =                      GM70_ADDRESS_STORAGE_SCRIPTS;
      m_soundData =                       GM70_ADDRESS_SOUND_STORAGE;

      m_scriptSwapTable =  (char*)        GM70_ADDRESS_ARRAY_SWAP_BYTES;
      m_surfaceArraySize = (int*)         GM70_ADDRESS_ARRAYSIZE_SURFACES;

      m_mainHwnd =    *(HWND*) (*GM70_ADDRESS_TRUNNER_INSTANCE + 0x30);
 
      // is debug mode enabled ?
      if ( *GM70_ADDRESS_TDEBUG_INSTANCE )
        m_debugHwnd = *(HWND*) (*GM70_ADDRESS_TDEBUG_INSTANCE + 0x180);

      m_hScrHwnd =    *(HWND*) (*GM70_ADDRESS_TSCORE_INSTANCE + 0xB8);

    } else if ( m_gmVersion == GM_VERSION_61 ) {

      m_functionData =                   GM61_ADDRESS_ARRAY_GMFUNCTIONS;
      m_spriteData =                     GM61_ADDRESS_STORAGE_SPRITES;
      m_backgroundData =                 GM61_ADDRESS_STORAGE_BACKGROUNDS;
      m_surfaces =        (LPGMSURFACE*) GM61_ADDRESS_ARRAY_SURFACES;
      m_textures =        (LPGMTEXTURE*) GM61_ADDRESS_ARRAY_TEXTURES;
      m_d3dInfo =                        GM61_ADDRESS_STORAGE_D3D;
      m_scriptData =                     GM61_ADDRESS_STORAGE_SCRIPTS;
      m_soundData =                      GM61_ADDRESS_SOUND_STORAGE;

      m_surfaceArraySize = (int*) GM61_ADDRESS_ARRAYSIZE_SURFACES;

      m_mainHwnd  = *((HWND*) (*GM61_ADDRESS_TRUNNER_INSTANCE + 0x30));
      m_debugHwnd = *((HWND*) (*GM61_ADDRESS_TDEBUG_INSTANCE + 0x180));
      m_hScrHwnd = NULL; // Ptr to the handle is not valid until the highscore
                         // was showed at least once, so function table
                         // GetHighscoreWindowHandle is not supported in GM6.1
    }
  }

  void CGMAPI::RetrieveFunctionPointers() {
    for ( int i = 0; i < GM_FUNCTION_COUNT; i++ )
      m_gmFunctions[i] = GetGMFunctionAddress( GM_FUNCTION_NAMES[i] );
  }

  void* CGMAPI::GetGMFunctionAddress( const char* aFunctionName ) {
    const int NAME_MAX = sizeof( m_functionData->functions->name );

    void* address = NULL;
    int strLength = strlen( aFunctionName );

    if ( strLength <= NAME_MAX ) {
      for ( DWORD i = 0; i < m_functionData->nFunctions; i++ ) {
        if ( strLength != m_functionData->functions[i].nameLength )
          continue;

        if ( strcmp( aFunctionName, m_functionData->functions[i].name ) == 0 ) {
          address = m_functionData->functions[i].address;
          break;
        }
      }
    }

    return address;
  }

  /********************************************
   * Exceptions
   *******************************************/

  void EGMAPISpriteNotExist::ShowError() const {
    HWND hwnd = ( CGMAPI::Ptr() ? CGMAPI::Ptr()->GetMainWindowHandle() : NULL );
    char buffer[0x200];

    sprintf_s( buffer, sizeof( buffer ),
              "%s:\n%s\n\n%s:\nSprite ID: %d",
              STR_GMAPI_ERROR, EXC_SPRITENOTEXISTS, STR_GMAPI_DEBUG, m_resourceId );

    MessageBox( hwnd, buffer, 0, MB_SYSTEMMODAL | MB_ICONERROR );
  }

  void EGMAPIInvalidSubimage::ShowError() const {
    HWND hwnd = ( CGMAPI::Ptr() ? CGMAPI::Ptr()->GetMainWindowHandle() : NULL );
    char buffer[0x200];
    const char* spriteName = STR_NO_ACCESS;


    if ( CGMAPI::Ptr() )
      if ( CGMAPI::Ptr()->Sprites.Exists( m_resourceId ) )
        spriteName = CGMAPI::Ptr()->Sprites[m_resourceId].GetName();

    sprintf_s( buffer, sizeof( buffer ),
               "%s:\n%s\n\n%s:\nSprite: %s (ID: %d)\nSubimage: %d",
               STR_GMAPI_ERROR, EXC_INVALIDSUBIMAGE, STR_GMAPI_DEBUG,
               spriteName, m_resourceId, m_subimage );
    
    MessageBox( hwnd, buffer, 0, MB_SYSTEMMODAL | MB_ICONERROR );
  }

  void EGMAPIBackgroundNotExist::ShowError() const {
    HWND hwnd = ( CGMAPI::Ptr() ? CGMAPI::Ptr()->GetMainWindowHandle() : NULL );
    char buffer[0x200];

    sprintf_s( buffer, sizeof( buffer ),
              "%s:\n%s\n\n%s:\nBackground ID: %d",
              STR_GMAPI_ERROR, EXC_BACKGROUNDNOTEXISTS, STR_GMAPI_DEBUG, m_resourceId );
    
    MessageBox( hwnd, buffer, 0, MB_SYSTEMMODAL | MB_ICONERROR );
  }

  void EGMAPIScriptNotExist::ShowError() const {
    HWND hwnd = ( CGMAPI::Ptr() ? CGMAPI::Ptr()->GetMainWindowHandle() : NULL );
    char buffer[0x200];

    sprintf_s( buffer, sizeof( buffer ),
              "%s:\n%s\n\n%s:\nScript ID: %d",
              STR_GMAPI_ERROR, EXC_SCRIPTNOTEXISTS, STR_GMAPI_DEBUG, m_resourceId );
    
    MessageBox( hwnd, buffer, 0, MB_SYSTEMMODAL | MB_ICONERROR );
  }

  void EGMAPISoundNotExist::ShowError() const {
    HWND hwnd = ( CGMAPI::Ptr() ? CGMAPI::Ptr()->GetMainWindowHandle() : NULL );
    char buffer[0x200];

    sprintf_s( buffer, sizeof( buffer ),
              "%s:\n%s\n\n%s:\nSound ID: %d",
              STR_GMAPI_ERROR, EXC_SOUNDNOTEXISTS, STR_GMAPI_DEBUG, m_resourceId );
    
    MessageBox( hwnd, buffer, 0, MB_SYSTEMMODAL | MB_ICONERROR );
  }

  void EGMAPISurfaceNotExist::ShowError() const {
    HWND hwnd = ( CGMAPI::Ptr() ? CGMAPI::Ptr()->GetMainWindowHandle() : NULL );
    char buffer[0x200];

    sprintf_s( buffer, sizeof( buffer ),
              "%s:\n%s\n\n%s:\nSurface ID: %d",
              STR_GMAPI_ERROR, EXC_SURFACENOTEXISTS, STR_GMAPI_DEBUG, m_resourceId );
    
    MessageBox( hwnd, buffer, 0, MB_SYSTEMMODAL | MB_ICONERROR );
  }

  /********************************************
   * Operator overloading
   ********************************************/

  std::ostream& operator<<( std::ostream& aStream, const CGMVariable& aValue ) {
    if ( aValue.IsString() )
      aStream << aValue.c_str();
    else
      aStream << aValue.real();

    return aStream;
  }

  /********************************************
   * CGMVariable class implementation
   ********************************************/

  void CGMVariable::StringSet( __in const char* aValue ) {
    if ( !m_ppStr ) 
      m_ppStr = GMAllocateString();

    GMSetString( aValue, m_ppStr );
  }

  void CGMVariable::StringClear() {
    if ( m_ppStr )
      GMClearString( m_ppStr );
  }

  void CGMVariable::StringDeallocate() {
    if ( m_ppStr ) {
      GMDeallocateString( m_ppStr );
      m_ppStr = NULL;
    }
  }

  CGMVariable& CGMVariable::operator=( const GMVARIABLE& aValue ) {
    if ( aValue.stringType ) {
      Set( aValue.valueString );
    } else
      Set( aValue.valueReal );

    return *this;
  }

  /********************************************
   * GMVARIABLE structure operator overloading
   ********************************************/

  GMVARIABLE& GMVARIABLE::operator=( const double aValue ) {
    ZeroMemory( this, sizeof( GMVARIABLE ) );
    valueReal = aValue;

    return *this;
  }

  GMVARIABLE& GMVARIABLE::operator=( char* aValue ) {
    ZeroMemory( this, sizeof( GMVARIABLE ) );
    stringType = 1;

    valueString = aValue;

    return *this;
  }

  GMVARIABLE& GMVARIABLE::operator=( const CGMVariable& aValue ) {
    ZeroMemory( this, sizeof( GMVARIABLE ) );

    if ( aValue.m_stringType )
      valueString = *aValue.m_ppStr;
    else
      valueReal = aValue.m_real;

    stringType = aValue.m_stringType;
    return *this;
  }

  /********************************************
   * ISprites interface implementation
   ********************************************/

  bool ISprites::Exists( const int aSpriteId ) {
    if ( aSpriteId >= GetArraySize() || aSpriteId < 0 ||
         !CGMAPI::SpriteData()->sprites )
      return false;
    else
      return ( CGMAPI::SpriteData()->sprites[aSpriteId] != NULL );
  }

  int ISprites::GetID( const char* aSpriteName ) {
    int id = -1;
    DWORD strLength = strlen( aSpriteName );

    if ( CGMAPI::SpriteData()->names ) {
      for ( int i = 0; i < GetArraySize(); i++ ) {
        if ( !CGMAPI::SpriteData()->names[i] )
          continue;

        if ( strLength != *((DWORD*) CGMAPI::SpriteData()->names[i] - 1) )
          continue;

        if ( strcmp( aSpriteName, CGMAPI::SpriteData()->names[i] ) == 0 ) {
          id = i;
          break;
        }
      }
    }

    return id;
  }

  int ISprites::GetCount() {
    if ( !CGMAPI::SpriteData()->sprites )
      return 0;

    int count = 0;

    for ( int i = 0; i < GetArraySize(); i++ )
      if ( CGMAPI::SpriteData()->sprites[i] )
        count++;

    return count;
  }

  /********************************************
   * IBackgrounds interface implementation
   ********************************************/

  bool IBackgrounds::Exists( const int aBackgroundId ) {
    if ( aBackgroundId >= GetArraySize() || aBackgroundId < 0 ||
         !CGMAPI::BackgroundData()->backgrounds )
      return false;
    else
      return ( CGMAPI::BackgroundData()->backgrounds[aBackgroundId] != NULL );
  }

  int IBackgrounds::GetID( const char* aBackgroundName ) {
    int id = -1;
    DWORD strLength = strlen( aBackgroundName );

    if ( CGMAPI::BackgroundData()->names ) {
      for ( int i = 0; i < CGMAPI::BackgroundData()->arraySize; i++ ) {
        if ( !CGMAPI::BackgroundData()->names[i] )
          continue;

        if ( strLength != *((DWORD*) CGMAPI::BackgroundData()->names[i] - 1) )
          continue;

        if ( strcmp( aBackgroundName, CGMAPI::BackgroundData()->names[i] ) == 0 ) {
          id = i;
          break;
        }
      }
    }

    return id;
  }

  int IBackgrounds::GetCount() {
    if ( !CGMAPI::BackgroundData()->backgrounds )
      return 0;

    int count = 0;

    for ( int i = 0; i < GetArraySize(); i++ )
      if ( CGMAPI::BackgroundData()->backgrounds[i] )
        count++;

    return count;
  }

  /********************************************
   * ISurfaces interface implementation
   ********************************************/

  int ISurfaces::GetCount() {
    if ( !CGMAPI::GetSurfaceArray() )
      return 0;

    int count = 0;

    for ( int i = 0; i < GetArraySize(); i++ )
      if ( CGMAPI::GetSurfaceArray()[i].exists )
        count++;

    return count;
  }

  /********************************************
   * IScripts interface implementation
   ********************************************/

  int IScripts::GetCount() {
    if ( !CGMAPI::ScriptData()->scripts )
      return 0;

    int count = 0;

    for ( int i = 0; i < GetArraySize(); i++ )
      if ( CGMAPI::ScriptData()->scripts[i] )
        count++;

    return count;
  }

  int IScripts::GetID( const char* aScriptName ) {
    int id = -1;
    DWORD strLength = strlen( aScriptName );

    if ( CGMAPI::ScriptData()->names ) {
      for ( int i = 0; i < CGMAPI::ScriptData()->arraySize; i++ ) {
        if ( !CGMAPI::ScriptData()->names[i] )
          continue;

        if ( strLength != *((DWORD*) CGMAPI::ScriptData()->names[i] - 1) )
          continue;

        if ( strcmp( aScriptName, CGMAPI::ScriptData()->names[i] ) == 0 ) {
          id = i;
          break;
        }
      }
    }

    return id;
  }


  /********************************************
   * IScript interface implementation
   ********************************************/

  char* IScript::GetScript( char* aBuffer ) {
    unsigned long scrLength = GetScriptLength();

    memcpy( aBuffer,
            CGMAPI::ScriptData()->scripts[m_scriptId]->debugInfo->code,
            scrLength );

    if ( CGMAPI::Ptr()->GetGMVersion() == GM_VERSION_70 )
      for ( unsigned long i = 0; i < scrLength; i++ )
        aBuffer[i] = CGMAPI::ScriptSwapTable()[ aBuffer[i] ];

    return aBuffer;
  }

  /********************************************
   * ISounds interface implementation
   ********************************************/

  int ISounds::GetCount() {
    if ( !CGMAPI::SoundData()->sounds )
      return 0;

    int count = 0;

    for ( int i = 0; i < GetArraySize(); i++ )
      if ( CGMAPI::SoundData()->sounds[i] )
        count++;

    return count;
  }

  int ISounds::GetID( const char* aSoundName ) {
    int id = -1;
    DWORD strLength = strlen( aSoundName );

    if ( CGMAPI::SoundData()->names ) {
      for ( int i = 0; i < CGMAPI::SoundData()->arraySize; i++ ) {
        if ( !CGMAPI::SoundData()->names[i] )
          continue;

        if ( strLength != *((DWORD*) CGMAPI::SoundData()->names[i] - 1) )
          continue;

        if ( strcmp( aSoundName, CGMAPI::SoundData()->names[i] ) == 0 ) {
          id = i;
          break;
        }
      }
    }

    return id;
  }

}
