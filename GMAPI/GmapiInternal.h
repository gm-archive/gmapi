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
  GmapiInternal.h
  - Declarations of all GMAPI components, excluding wrapped gm functions

  Written by: Snake (http://gamebaseteam.eu)
***************************************************************************/

#pragma once
#include <windows.h>

#include <string>
#include <iostream>

#include "GmapiCore.h"
#include "GmapiConsts.h"
#include "GmapiDefs.h"

namespace gm {

  /********************************************
    * CGMVariable
    *******************************************/

  /// CGMVariable
  /// Class that mimics GM's variable. It is used in few
  /// GM functions as string/unspecified (i.e. that ones which
  /// can be both "real" or "string") arguments and
  /// string/unspecified return values.
  ///
  class CGMVariable {
    public:
      friend struct GMVARIABLE;

      /// Ctor( __in_opt const int aValue = 0 ) [also default]
      ///   Initializes the variable to specified int value 
      ///   (it'll be casted to double) and sets its type
      ///   to "real".
      ///
      /// Parameters:
      ///   aValue: [optional] Initializing value. 
      ///
      CGMVariable( __in_opt const int aValue = 0 ): m_real( aValue ),
                                                    m_ppStr( NULL ),
                                                    m_stringType( false ),
                                                    m_disposeStr( true ) {}

      /// Ctor( __in const double aValue )
      ///   Initializes the variable to specified value 
      ///   and sets its type to "real".
      ///
      /// Parameters:
      ///   aValue: Initializing value.
      ///
      CGMVariable( __in const double aValue ): m_real( aValue ),
                                               m_ppStr( NULL ),
                                               m_stringType( false ),
                                               m_disposeStr( true ) {}

      /// Ctor( __in const char* aValue )
      ///   Initializes the variable with specified string 
      ///   and sets its type to "string".
      ///
      /// Parameters:
      ///   aValue: Initializing string.
      ///
      CGMVariable( __in const char* aValue ): m_real( 0.0 ),
                                              m_ppStr( NULL ), m_stringType( true ),
                                              m_disposeStr( true ) {
        StringSet( aValue );
      }

      /// Ctor( __in const std::string& aValue )
      ///   Initializes the variable with specified string 
      ///   and sets its type to "string".
      ///
      /// Parameters:
      ///   aValue: Initializing string.
      ///
      CGMVariable( __in const std::string& aValue ): m_real( 0.0 ),
                                                     m_ppStr( NULL ),
                                                     m_stringType( true ),
                                                     m_disposeStr( true ) {
        StringSet( aValue.c_str() );
      }

      /// Ctor( __in const bool aDeallocateString )
      ///   This constructor is used internally, its purpose is
      ///   to specify that variable should not dispose the memory
      ///   allocated for string. Thus, variable can be returned in 
      ///   functions as string type.
      ///
      /// Parameters:
      ///   aValue: Initializing string.
      ///
      CGMVariable( __in const bool aDeallocateString ): m_real( 0.0 ),
                                                        m_ppStr( NULL ),
                                                        m_stringType( false ),
                                                        m_disposeStr( aDeallocateString ) {}

      ~CGMVariable() {
        if ( m_stringType && m_ppStr && m_disposeStr )
          StringDeallocate();
      }

      /// Set( __in const double aValue )
      ///   Sets the variable to specified double value, thus
      ///   changing its type to "real". If the variable was previously
      ///   a "string" type - the string will be deallocated.
      ///
      /// Parameters:
      ///   aValue: New value
      ///
      void Set( __in const double aValue ) {
        if ( m_stringType ) {
          StringClear();
          m_stringType = false;
        }

        m_real = aValue;
      }

      /// Set( __in const char* aValue )
      ///   Sets variable to specified string. Thus, changes its type
      ///   to "string". If previously variable was "real" type - it'll
      ///   be set to zero.
      ///
      /// Parameters:
      ///   aValue: New value
      ///
      void Set( __in const char* aValue ) {
        if ( !m_stringType ) {
          m_real = 0.0;
          m_stringType = true;
        }

        if ( aValue )
          StringSet( aValue );
        else
          StringClear();
      }

      /// void Set( __in const std::string& aValue )
      ///   Sets variable to specified string. Thus, changes its type
      ///   to "string". If previously variable was "real" type - it'll
      ///   be set to zero.
      ///
      /// Parameters:
      ///   aValue: New value
      ///
      void Set( __in const std::string& aValue ) {
        Set( aValue.c_str() );
      }

      /// c_str()
      ///   Returns variable as c-style string.
      ///
      /// Parameters:
      ///   None
      ///
      /// Returns:
      ///   C-style string if variable's type is string, otherwise NULL.
      ///
      const char* c_str() const {
        if ( m_ppStr && m_stringType )
          return *m_ppStr;
        else
          return NULL;
      }

      /// real()
      ///   Returns variable as real value.
      ///
      /// Parameters:
      ///   None
      ///
      /// Returns:
      ///   Real value of the variable.
      ///
      double real() const {
        return m_real;
      }

      /// IsString()
      ///   Checks whether variable is string type.
      ///
      /// Parameters:
      ///   None
      ///
      /// Returns:
      ///   True if variable's type is string, otherwise returns false (real value).
      ///
      bool IsString() const {
        return m_stringType;
      }

      /**
        * Assignment operator overloading
        */

      CGMVariable& operator=( const double aValue ) {
        Set( aValue ); 
        return *this;
      }

      CGMVariable& operator=( const char* aValue ) { 
        Set( aValue ); 
        return *this;
      }

      CGMVariable& operator=( const std::string& aValue ) {
        Set( aValue.c_str() );
        return *this;
      }

      CGMVariable& operator=( const GMVARIABLE& aValue );
      CGMVariable& operator=( const CGMVariable& aValue ) {
        if ( aValue.m_stringType )
          Set( *aValue.m_ppStr );
        else
          Set( aValue.m_real );

        return *this;
      }

      /**
        * Type casting
        */

      operator double() {
        if ( !m_stringType )
          return this->m_real;

        return 0.0;
      }

      operator const char*() {
        if ( m_stringType )
          return *this->m_ppStr;

        return NULL;
      }

      /**
        * Operator overloads for real-value op.
        */

      CGMVariable& operator++() {
        if ( !m_stringType )
          ++m_real;
        return *this;
      }

      CGMVariable operator++(int) {
        CGMVariable old( *this );

        if ( !m_stringType )
          ++(*this);

        return old;
      }

      CGMVariable& operator--() {
        if ( !m_stringType )
          --m_real;

        return *this;
      }

      CGMVariable operator--(int) {
        CGMVariable old( *this );

        if ( !m_stringType )
          --(*this);

        return old;
      }

      void operator+=( const double aValue ) {
        if ( !m_stringType )
          m_real += aValue;
      }

      void operator-=( const double aValue ) {
        if ( !m_stringType )
          m_real -= aValue;
      }

    private:
      void StringSet( __in const char* aValue );
      void StringClear();
      void StringDeallocate();

      bool m_stringType, m_disposeStr;
      char** m_ppStr;
      double m_real;
  };

 /********************************************
   * Other operator overloading
   *******************************************/

  std::ostream& operator<<( std::ostream& aStream, const CGMVariable& aValue );

 /********************************************
   * CGMVariable
   *******************************************/

  /// GMVARIABLE
  /// That structure is used internally to specify
  /// arguments in GM functions
  ///
  struct GMVARIABLE {
    GMVARIABLE(): stringType( 0 ),
                  propertyType( 0 ),
                  valueReal( 0 ),
                  valueString( NULL ),
                  propertyPtr( 0 ) {}

    GMVARIABLE( __in const double aValue ): stringType( FALSE ),
                                            propertyType( 0 ),
                                            valueReal( aValue ),
                                            valueString( NULL ) {}

    GMVARIABLE( __in char* aValue ): stringType( FALSE ),
                                     propertyType( 0 ),
                                     valueString( aValue ) {}

    GMVARIABLE( __in const CGMVariable& aValue ): propertyType( 0 ) {
      stringType = aValue.m_stringType;
      if ( aValue.m_stringType )
        valueString = *aValue.m_ppStr;
      else {
        valueReal = aValue.m_real;
        valueString = NULL;
      }
    }

    GMVARIABLE& operator=( const double aValue );
    GMVARIABLE& operator=( char* aValue );
    GMVARIABLE& operator=( const CGMVariable& aValue );

    BOOL stringType;
    int propertyType;
    double valueReal;
    char* valueString;
    void* propertyPtr;
  };

 /********************************************
   * Exceptions
   *******************************************/

  class EGMAPIException: public std::exception {
    public:
      /// ShowError()
      ///   Shows message box with error message
      ///
      virtual void ShowError() const {
        MessageBox( 0, EXC_UNKNOWN, 0, MB_SYSTEMMODAL );
      }
  };

  class EGMAPIResourceException: public EGMAPIException {
    public:
      /// GetResourceID()
      ///   Returns ID of the resources that caused exception
      ///
      int GetResourceID() const {
        return m_resourceId;
      }

    protected:
      int m_resourceId;
  };

  class EGMAPISpriteNotExist: public EGMAPIResourceException {
    public:
      explicit EGMAPISpriteNotExist( const int aSprite ) {
        m_resourceId = aSprite;
      }

      /// ShowError()
      ///   Shows message box with error message
      ///
      virtual void ShowError() const;
  };

  class EGMAPIInvalidSubimage: public EGMAPIResourceException {
    public:
      explicit EGMAPIInvalidSubimage( const int aSprite, const int aSubimage ): m_subimage( aSubimage ) { 
        m_resourceId = aSprite;
      }

      /// ShowError()
      ///   Shows message box with error message
      ///
      virtual void ShowError() const;

    protected:
      int m_subimage;
  };

  class EGMAPIBackgroundNotExist: public EGMAPIResourceException {
    public:
      explicit EGMAPIBackgroundNotExist( const int aBackground ) {
        m_resourceId = aBackground;
      }

      /// ShowError()
      ///   Shows message box with error message
      ///
      virtual void ShowError() const;
  };

  class EGMAPIScriptNotExist: public EGMAPIResourceException {
    public:
      explicit EGMAPIScriptNotExist( const int aScript ) {
        m_resourceId = aScript;
      }

      /// ShowError()
      ///   Shows message box with error message
      ///
      virtual void ShowError() const;
  };

  class EGMAPISurfaceNotExist: public EGMAPIResourceException {
    public:
      explicit EGMAPISurfaceNotExist( const int aSurface ) {
        m_resourceId = aSurface;
      }

      /// ShowError()
      ///   Shows message box with error message
      ///
      virtual void ShowError() const;
  };

 /********************************************
   * GM resources accessors interfaces
   *******************************************/

  /// ISpriteSubimage
  /// Class that provides easy access to sprite subimages from game
  ///
  /// You should access this class only from an instance of
  /// CGMAPI class
  ///
  class ISpriteSubimage {
    friend class ISpriteSubimages;

    public:
      /// GetBitmap()
      ///   Returns pointer to sprite's subimage bitmap. The color
      ///   format is 32bit ARGB.
      ///
      /// Parameters:
      ///   None
      ///
      /// Returns:
      ///  Pointer to bitmap.
      ///
      inline static unsigned char* GetBitmap();

      /// GetBitmapSize()
      ///   Returns size of sprite's subimage bitmap in bytes.
      ///
      /// Parameters:
      ///   None
      ///
      /// Returns:
      ///  Sprite's subimage bitmap size in bytes
      ///
      inline static unsigned long GetBitmapSize();

      /// GetTextureID()
      ///  Gets a texture ID of this subimage.
      ///
      /// Parameters:
      ///   None
      /// 
      /// Returns:
      ///   Texture ID of this subimage.
      ///
      inline static int GetTextureID();

      /// GetTexture()
      ///  Retrives a pointer to an Direct3DTexture interface
      ///  associated with this subimage's texture
      ///
      /// Parameters:
      ///   None
      /// 
      /// Returns:
      ///   Pointer to IDirect3DTexture8 associated with subimage's texture.
      ///
      inline static IDirect3DTexture8* GetTexture();

    private:
      static int m_subimage;
  };

  /// ISpriteSubimages
  /// Class that provides easy access to sprite subimages from game
  ///
  /// You should access this class only from an instance of
  /// CGMAPI class
  ///
  class ISpriteSubimages {
    friend class ISprite;
    friend class ISprites;

    public:
      /// operator []
      ///   Provides access to sprite's subimagese from ISpriteSubimage interface,
      ///   setting current subimage number (an element) from int specified in brackets.
      ///
      /// Throws:
      ///   EGMAPIInvalidSubimage object in case when number of subimage
      ///   is not valid.
      ///
      /// Returns:
      ///   Reference to an ISpriteSubimage class instance.
      ///
      inline ISpriteSubimage& operator[]( const int aSubimage );

      /// GetCount()
      ///   Returns total number of sprite's subimages.
      ///
      /// Parameters:
      ///   None
      ///
      /// Returns:
      ///   Total number of subimages.
      ///
      inline static int GetCount();

    private:
      ISpriteSubimage m_subimage;
  };

  /// ISprite
  /// Class that provides easy access to sprites from game
  ///
  /// You should access this class only from an instance of
  /// CGMAPI class
  ///
  class ISprite {
    friend class ISprites;
    friend class ISpriteSubimages;

    public:
      /// Sprite's subimages accessor interface class instance
      ISpriteSubimages Subimages;

      /// GetName()
      ///   Gets name of this sprite.
      ///
      /// Parameters:
      ///   None
      ///
      /// Returns
      ///   Pointer to name of this sprite
      ///
      inline static const char* GetName();

      /// GetHeight()
      ///   Returns width of the sprite.
      ///
      /// Parameters:
      ///   None
      ///
      /// Returns:
      ///   Sprite's width.
      ///
      static int GetWidth() {
        return m_sprite->width;
      }

      /// GetHeight()
      ///   Returns height of the sprite.
      ///
      /// Parameters:
      ///   None
      ///
      /// Returns:
      ///   Sprite's height.
      ///
      static int GetHeight() {
        return m_sprite->height;
      }

      /// GetOffsetX()
      ///   Returns sprite's X origin.
      ///
      /// Parameters:
      ///   None
      ///
      /// Returns:
      ///   Sprite's X origin
      ///
      static int GetOffsetX() {
        return m_sprite->originX;
      }

      /// GetOffsetY()
      ///   Returns sprite's Y origin.
      ///
      /// Parameters:
      ///   None
      ///
      /// Returns:
      ///   Sprite's Y origin
      ///
      static int GetOffsetY() {
        return m_sprite->originY;
      }

      /// SetOffset( const int aOffsetX, const int aOffsetY )
      ///   Sets sprite's origins.
      ///
      /// Parameters:
      ///   aOffsetX: New X origin
      ///   aOffsetY: New Y origin
      ///
      static void SetOffset( __in const int aOffsetX, __in const int aOffsetY ) {
        m_sprite->originX = aOffsetX;
        m_sprite->originY = aOffsetY;
      }

      /// GetPreciseCollision()
      ///   Check whether sprite has enabled option "Precise collision".
      ///
      /// Parameters:
      ///   None
      ///
      /// Returns:
      ///  Precise collision option state.
      ///
      static bool GetPreciseCollision() {
        return m_sprite->preciseCollision & 1;
      }

      /// SetPreciseCollision( __in const bool aEnable )
      ///   Enables or disables precise collision checking.
      ///
      /// Parameters:
      ///   aEnable: Specifies whether to enable or disable precise
      ///   collision checking
      ///
      static void SetPreciseCollision( __in const bool aEnable ) {
        m_sprite->preciseCollision = (BOOL) aEnable;
      }

      /// GetTransparent()
      ///   Check whether sprite has enabled option "Transparent".
      ///
      /// Parameters:
      ///   None
      ///
      /// Returns:
      ///  Transparent option state.
      ///
      static bool GetTransparent() {
        return m_sprite->transparent;
      }

      /// GetSmoothEdges()
      ///   Check whether sprite has enabled option "Smooth edges".
      ///
      /// Parameters:
      ///   None
      ///
      /// Returns:
      ///   Smooth edges option state.
      ///
      static bool GetSmoothEdges() {
        return m_sprite->smoothEdges;
      }

      /// GetPreload()
      ///   Check whether sprite has enabled option "Preload".
      ///
      /// Parameters:
      ///   None
      ///
      /// Returns:
      ///   Preload option state.
      ///
      static bool GetPreload() {
        return m_sprite->preload;
      }

      /// GetBoundingBoxType()
      ///   Gets type of this sprite's bounding box
      ///
      /// Parameters:
      ///   None
      ///
      /// Returns:
      ///  Type of the bounding box
      ///
      static BoundingBoxType GetBoundingBoxType() {
        return m_sprite->bboxType;
      }

      /// GetBoundingBoxLeft()
      ///   Gets sprite's left side of the bounding box
      ///
      /// Parameters:
      ///   None
      ///
      /// Returns:
      ///   left side of the bounding box
      ///
      static int GetBoundingBoxLeft() {
        return m_sprite->bboxLeft;
      }

      /// GetBoundingBoxRight()
      ///   Gets sprite's right side of the bounding box
      ///
      /// Parameters:
      ///   None
      ///
      /// Returns:
      ///   right side of the bounding box
      ///
      static int GetBoundingBoxRight() {
        return m_sprite->bboxRight;
      }

      /// GetBoundingBoxTop()
      ///   Gets sprite's top side of the bounding box
      ///
      /// Parameters:
      ///   None
      ///
      /// Returns:
      ///   top side of the bounding box
      ///
      static int GetBoundingBoxTop() {
        return m_sprite->bboxTop;
      }

      /// GetBoundingBoxBottom()
      ///   Gets sprite's bottom side of the bounding box
      ///
      /// Parameters:
      ///   None
      ///
      /// Returns:
      ///   bottom side of the bounding box
      ///
      static int GetBoundingBoxBottom() {
        return m_sprite->bboxBottom;
      }

      /// SetBoundingBoxType( __in const BoundingBoxType aType )
      ///   Changes type of sprite's bounding box.
      ///
      /// Parameters:
      ///   aType: New type.
      ///
      static void SetBoundingBoxType( __in const BoundingBoxType aType ) {
        m_sprite->bboxType = aType;
      }

      /// SetBoundingBox(
      ///   __in const int aLeft, 
      ///   __in const int aRight,
      ///   __in const int aTop,
      ///   __in const int aBottom
      /// )
      ///   Sets sprite's bounding box.
      ///
      /// Parameters:
      ///   aLeft: New left side of bbox.
      ///   aRight: New right side of bbox.
      ///   aTop: New left side of bbox.
      ///   aBottom: New bottom side of bbox.
      ///
      static void SetBoundingBox( const int aLeft, const int aRight, const int aTop,
                                  const int aBottom ) {
        m_sprite->bboxLeft = aLeft;
        m_sprite->bboxRight = aRight;
        m_sprite->bboxTop = aTop;
        m_sprite->bboxBottom = aBottom;
      }

      /// SpritePtr()
      ///   Returns pointer to the GMSPRITE structure of this sprite
      ///
      /// Parameters:
      ///   None
      ///
      /// Returns:
      ///   Pointer to the GMSPRITE structure of this sprite
      ///
      static LPGMSPRITE SpritePtr() {
        return m_sprite;
      }

    private:
      static LPGMSPRITE m_sprite;
      static int m_spriteId;
  };

  /// ISprites
  /// Class that provides easy access to sprites from game
  ///
  /// You should access this class only from an instance of
  /// CGMAPI class
  ///
  class ISprites {
    friend class CGMAPI;

    public:
      /// operator []
      ///   Provides access to sprite resource from ISprite interface,
      ///   setting current sprite ID (an element) from int specified in brackets.
      ///
      /// Throws:
      ///   EGMAPISpriteNotExists object in case when sprite of ID
      ///   specified in brackets does not exist.
      ///
      /// Returns:
      ///   Reference to an ISprite class instance.
      ///
      inline ISprite& operator[]( const int aSpriteId );

      /// GetID( __in const char* aSpriteName )
      ///   Gets ID of given sprite (by name).
      ///
      /// Parameters:
      ///   aBackgroundName: Name of the sprite
      /// 
      /// Returns:
      ///   Given sprite's ID. If sprite with specified name doesn't
      ///   exists, then return valuie will be -1.
      ///
      static int GetID( __in const char* aSpriteName );

      /// Exists( __in const int aSpriteId )
      ///   Checks whether specified sprite (an ID) exists. 
      ///
      /// Parameters:
      ///   aSpriteId: ID of the sprite
      /// 
      /// Returns:
      ///   true if sprite exists.
      ///
      static bool Exists( __in const int aSpriteId );

      /// GetCount()
      ///   Returns total number of sprites in the game.
      ///
      /// Parameters:
      ///   None
      ///
      /// Returns:
      ///   Total number of sprites.
      ///
      static int GetCount();

      /// GetArraySize()
      ///   Returns size of dynamic arrays that holds pointers
      ///   to sprite resource data. (highest resource ID + 1)
      ///
      /// Parameters:
      ///   None
      /// 
      /// Returns:
      ///   Number of array elements
      ///
      inline static int GetArraySize();

    private:
      ISprite m_iSprite;
  };

  /// IBackground
  /// Class that provides easy access to backgrounds from game
  ///
  /// You should access this class only from an instance of
  /// CGMAPI class
  ///
  class IBackground {
    friend class IBackgrounds;

    public:
      /// GetName()
      ///   Gets name of this background.
      ///
      /// Parameters:
      ///   None
      ///
      /// Returns
      ///   Pointer to name of this background
      ///
      inline static const char* GetName();

      /// GetWidth()
      ///   Returns width of the background.
      ///
      /// Parameters:
      ///   None
      ///
      /// Returns:
      ///   Background's width.
      ///
      inline static int GetWidth();

      /// GetHeight()
      ///   Returns height of the background.
      ///
      /// Parameters:
      ///   None
      ///
      /// Returns:
      ///   Background's height.
      ///
      inline static int GetHeight();

      /// BitmapExists()
      ///   Check whether the background's bitmap pointer is valid.
      ///
      /// Parameters:
      ///   None
      ///
      /// Returns:
      ///   True if bitmap exists.
      ///
      inline static bool BitmapExists();

      /// GetBitmap()
      ///   Returns pointer to background's bitmap. The color
      ///   format is 32bit ARGB.
      ///
      /// Parameters:
      ///   None
      ///
      /// Returns:
      ///  Pointer to bitmap.
      ///
      inline static unsigned char* GetBitmap();

      /// GetBitmapSize()
      ///   Returns size of background's image bitmap in bytes.
      ///
      /// Parameters:
      ///   None
      ///
      /// Returns:
      ///  Background's bitmap size in bytes
      ///
      inline static unsigned long GetBitmapSize();

      /// GetTextureID()
      ///  Gets a texture ID of this background.
      ///
      /// Parameters:
      ///   None
      /// 
      /// Returns:
      ///   Texture ID of this background.
      ///
      inline static int GetTextureID();

      /// GetTexture()
      ///  Retrives a pointer to an Direct3DTexture interface
      ///  associated with this background's texture
      ///
      /// Parameters:
      ///   None
      /// 
      /// Returns:
      ///   Pointer to IDirect3DTexture8 associated with background's texture.
      ///
      inline static IDirect3DTexture8* GetTexture();

      /// GetTransparent()
      ///   Check whether background has enabled option "Transparent".
      ///
      /// Parameters:
      ///   None
      ///
      /// Returns:
      ///  Transparent option state.
      ///
      inline static bool GetTransparent();

      /// GetSmoothEdges()
      ///   Check whether background has enabled option "Smooth edges".
      ///
      /// Parameters:
      ///   None
      ///
      /// Returns:
      ///   Smooth edges option state.
      ///
      inline static bool GetSmoothEdges();

      /// GetPreload()
      ///   Check whether background has enabled option "Preload".
      ///
      /// Parameters:
      ///   None
      ///
      /// Returns:
      ///   Preload option state.
      ///
      inline static bool GetPreload();

    private:
      static int m_backgroundId;
      static LPGMBACKGROUND m_background;
  };

  /// IBackgrounds
  /// Class that provides easy access to backgrounds from game
  ///
  /// You should access this class only from an instance of
  /// CGMAPI class
  ///
  class IBackgrounds {
    public:
      /// operator []
      ///   Provides access to background resource from IBackground interface,
      ///   setting current background ID (an element) from int specified in brackets.
      ///
      /// Throws:
      ///   EGMAPIBackgroundNotExists object in case when background of ID
      ///   specified in brackets does not exist.
      ///
      /// Returns:
      ///   Reference to an IBackground class instance.
      ///
      inline IBackground& operator[]( int aBackgroundId );

      /// GetID( __in const char* aBackgroundName )
      ///   Gets ID of given background (by name).
      ///
      /// Parameters:
      ///   aBackgroundName: Name of the background
      /// 
      /// Returns:
      ///   Given background's ID. If background with specified name doesn't
      ///   exists, then return valuie will be -1.
      ///
      static int GetID( __in const char* aBackgroundName );

      /// Exists( __in const int aBackgroundId )
      ///   Checks whether specified background (an ID) exists. 
      ///
      /// Parameters:
      ///   aBackgroundId: ID of the background
      /// 
      /// Returns:
      ///   true if background exists.
      ///
      static bool Exists( __in const int aBackgroundId );

      /// GetCount()
      ///   Returns total number of backgrounds in the game.
      ///
      /// Parameters:
      ///   None
      ///
      /// Returns:
      ///   Total number of backgrounds.
      ///
      static int GetCount();

      /// GetArraySize()
      ///   Returns size of dynamic arrays that holds pointers
      ///   to background resource data. (highest resource ID + 1)
      ///
      /// Parameters:
      ///   None
      /// 
      /// Returns:
      ///   Number of array elements
      ///
      inline static int GetArraySize();

    private:
      IBackground m_iBackground;
  };

  /// ISurface
  /// Class that provides easy access to surfaces from game
  ///
  /// You should access this class only from an instance of
  /// CGMAPI class
  ///
  class ISurface {
    friend class ISurfaces;

    public:
      /// GetWidth()
      ///  Gets a width of the surface
      ///
      /// Parameters:
      ///   None
      /// 
      /// Returns:
      ///   Width of this surface.
      ///
      inline static int GetWidth();

      /// GetHeight()
      ///  Gets a height of the surface
      ///
      /// Parameters:
      ///   None
      /// 
      /// Returns:
      ///   Height of this surface.
      ///
      inline static int GetHeight();

      /// GetTextureID()
      ///  Gets a texture ID of this surface.
      ///
      /// Parameters:
      ///   None
      /// 
      /// Returns:
      ///   Texture ID of this surface.
      ///
      inline static int GetTextureID();

      /// GetTexture()
      ///  Retrives a pointer to an Direct3DTexture interface
      ///  associated with this surface's texture
      ///
      /// Parameters:
      ///   None
      /// 
      /// Returns:
      ///   Pointer to IDirect3DTexture8 associated with surface's texture.
      ///
      inline static IDirect3DTexture8* GetTexture();

    private:
      static int m_surfaceId;
  };

  /// ISurfaces
  /// Class that provides easy access to surfaces from game
  ///
  /// You should access this class only from an instance of
  /// CGMAPI class
  ///
  class ISurfaces {
    friend class CGMAPI;

    public:
      /// operator []
      ///   Provides access to surface resource from ISurface interface,
      ///   setting current surface ID (an element) from int specified in brackets.
      ///
      /// Throws:
      ///   EGMAPISurfaceNotExists object in case when surface of ID
      ///   specified in brackets does not exist.
      ///
      /// Returns:
      ///   Reference to an ISurface class instance.
      ///
      inline ISurface& operator[]( int aSurfaceId );

      /// Exists( __in const int aSurfaceId )
      ///   Checks whether specified surface (an ID) exists. 
      ///
      /// Parameters:
      ///   aSurfaceId: ID of the surface
      /// 
      /// Returns:
      ///   true if surface exists.
      ///
      inline static bool Exists( __in const int aSurfaceId );

      /// GetCount()
      ///   Returns total number of surfaces in the game.
      ///
      /// Parameters:
      ///   None
      ///
      /// Returns:
      ///   Total number of surfaces.
      ///
      static int GetCount();

      /// GetArraySize()
      ///   Returns numbetr of elements of dynamic arrays that holds
      ///   pointers to surface resource data. (highest resource ID + 1)
      ///
      /// Parameters:
      ///   None
      /// 
      ///  Returns:
      ///    Number of array elements
      ///
      inline static int GetArraySize();  

    private:
      ISurface m_iSurface;
  };

  /// IScript
  /// Class that provides easy access to scripts from game.
  /// 
  /// You should access this class only from an instance of
  /// CGMAPI class
  ///
  class IScript {
    friend class IScripts;

    public:
      /// GetScriptLength()
      ///   Gets length in characters of this script.
      ///
      /// Parameters:
      ///   None
      ///
      /// Returns:
      ///   Length of the script
      ///
      inline static unsigned long GetScriptLength();

      /// GetScript( __out char* aBuffer )
      ///   Gets code of this script resource.
      ///
      /// Parameters:
      ///   aBuffer: Pointer to an array of char. Buffer must
      ///   be large enough to hold whole code - use method
      ///   GetScriptLength() to find out length of the script.
      ///
      /// Returns:
      ///   Pointer specified in aBuffer parameter.
      ///
      static char* GetScript( __out char* aBuffer );

      /// GetName()
      ///   Gets name of this script.
      ///
      /// Parameters:
      ///   None
      ///
      /// Returns
      ///   Pointer to name of this script
      ///
      inline static const char* GetName();

    private:
      static int m_scriptId;
  };

  /// IScripts
  /// Class that provides easy access to scripts from game
  ///
  /// You should access this class only from an instance of
  /// CGMAPI class
  ///
  class IScripts {
    public:

      /// operator []
      ///   Provides access to script resource from IScript interface,
      ///   setting current sprite ID (an element).
      ///
      /// Throws:
      ///   EGMAPIScriptNotExists object in case when script of ID
      ///   specified in brackets does not exist.
      ///
      /// Returns:
      ///   Reference to an IScript class instance.
      ///
      inline IScript& operator[]( int aScriptId );

      /// GetCount()
      ///   Returns total number of scripts in the game.
      ///
      /// Parameters:
      ///   None
      /// 
      ///  Returns:
      ///    Total number of scripts.
      static int GetCount();

      /// GetArraySize()
      ///   Returns size of dynamic arrays that holds pointers
      ///   to script resource data. (highest resource ID + 1)
      ///
      /// Parameters:
      ///   None
      /// 
      ///  Returns:
      ///    Number of array elements
      ///
      inline static int GetArraySize();

      /// Exists( __in const int aScriptId )
      ///   Checks whether specified script (an ID) exists. 
      ///
      /// Parameters:
      ///   aScriptId: ID of the script
      /// 
      /// Returns:
      ///   true if script exists.
      ///
      inline static bool Exists( __in const int aScriptId );

      /// GetID( __in const char* aScriptName )
      ///   Gets ID of given script (by name).
      ///
      /// Parameters:
      ///   aScriptName: Name of the script
      /// 
      /// Returns:
      ///   Given script's ID. If script with specified name doesn't
      ///   exists, then return valuie will be -1.
      ///
      inline static int GetID( __in const char* aScriptName );
      
    private:
      IScript m_iScript;
  };

  /// CGMAPI class
  /// Initializes GMAPI engine and provides direct access
  /// to GM's resources.
  ///
  /// Notes:
  ///   Only one instance of this class is allowed at a runtime.
  ///   CGMAPI should be initialized before calling any GM function.
  /// 
  class CGMAPI {
    public:
      /// Create( int* aResult )
      ///   Creates an instance of CGMAPI class and initializes GMAPI engine.
      ///   Only one instance of this class is allowed at a runtime.
      ///
      /// Parameters:
      ///   aResult: pointer to a variable of type int which will receive one of
      ///   the following values:
      ///     GMAPI_INITIALIZATION_SUCCESS - GMAPI has been initialized properly
      ///     GMAPI_INITIALIZATION_FAILED - failed to initialize GMAPI
      ///     GMAPI_ALREADY_INITIALIZED - an instance of CGMAPI class already exists
      /// 
      /// Returns:
      ///   Pointer to CGMAPI class instance, or NULL if method fails to initialize.
      /// 
      static CGMAPI* Create( unsigned long* aResult );

      /// Destroy()
      ///   Destroys an instance of CGMAPI class and deinitializes GMAPI engine
      ///
      /// Parameters:
      ///   None
      /// 
      static void Destroy();

      /// Sprite resource accessor interface class instance
      ISprites Sprites;

      /// Background resource accessor interface class instance
      IBackgrounds Backgrounds;

      /// Surface resource accessor interface class instance
      ISurfaces Surfaces;

      /// Script resource accessor interface class instance
      IScripts Scripts;

      /// GetGMVersion()
      ///   Gets Game Maker version in which the game is created
      ///
      /// Parameters:
      ///   None
      /// 
      /// Returns:
      ///   Following values can be returned:
      ///     GM_VERSION_61 - Game Maker 6.1
      ///     GM_VERSION_70 - Game Maker 7.0
      ///     GM_VERSION_INCOMPATIBLE - Unknown GM version
      /// 
      unsigned long GetGMVersion() {
        return m_gmVersion;
      }

      /// GetGMFunctionAddress( __in const char* aFunctionName )
      ///   Gets address of specified GM function.
      ///
      /// Parameters:
      ///   aFunctionName: name of GM function
      ///
      /// Returns:
      ///   Address of GM function. If function cannot be found
      ///   then the return value will be NULL.
      /// 
      static void* GetGMFunctionAddress( __in const char* aFunctionName );

      /// GetD3DTexture( int aTextureId )
      ///   Retrieves pointer to Direct3D texture interface.
      ///
      /// Parameters:
      ///   aTextureId: specifies to which texture pointer should be retrieved
      /// 
      /// Returns:
      ///   If texture ID is valid then pointer to IDirect3DTexture8 will be
      ///   returned, otherwise function will return NULL.
      /// 
      static IDirect3DTexture8* GetD3DTexture( int aTextureId ) {
        return ( (*m_textures)[aTextureId].isValid == TRUE ? (*m_textures)[aTextureId].texture : NULL );
      }

      /// GetDirect3DInterface()
      ///   Returns pointer to IDirect3D8 interface, initialized by a runner.
      ///
      /// Parameters:
      ///   None
      /// 
      /// Returns:
      ///   Pointer to IDirect3D8 interface.
      /// 
      static IDirect3D8* GetDirect3DInterface() {
        return m_d3dInfo->d3dInterface;
      }

      /// GetDirect3DDevice()
      ///   Returns pointer to IDirect3DDevice8 interface, initialized by a runner.
      ///
      /// Parameters:
      ///   None
      /// 
      /// Returns:
      ///   Pointer to IDirect3DDevice8 interface.
      /// 
      static IDirect3DDevice8* GetDirect3DDevice() {
        return m_d3dInfo->d3dDevice;
      }

      /// GetBitmapSize( GMBITMAP* aBitmap )
      ///   Returns size in bytes of specified bitmap
      ///
      /// Parameters:
      ///   aBitmap: Pointer to GM's GMBITMAP structre
      ///
      /// Returns:
      ///   Size of bitmap in bytes.
      /// 
      static unsigned long GetBitmapSize( GMBITMAP* aBitmap ) {
        return aBitmap->width * aBitmap->height * 4;
      }

      /// FunctionData()
      ///   Returns pointer to runner's GMFUNCTIONINFOSTORAGE structure.
      ///
      /// Parameters:
      ///   None
      /// 
      /// Returns:
      ///   Pointer to GMFUNCTIONINFOSTORAGE structure.
      /// 
      static LPGMFUNCTIONINFOSTORAGE FunctionData() {
        return m_functionData;
      }

      /// BackgroundData()
      ///   Returns pointer to runner's GMBACKGROUNDSTORAGE structure.
      ///
      /// Parameters:
      ///   None
      /// 
      /// Returns:
      ///   Pointer to GMBACKGROUNDSTORAGE structure.
      /// 
      static LPGMBACKGROUNDSTORAGE BackgroundData() {
        return m_backgroundData;
      }

      /// SpriteData()
      ///   Returns pointer to runner's GMSPRITESTORAGE structure.
      ///
      /// Parameters:
      ///   None
      /// 
      /// Returns:
      ///   Pointer to GMSPRITESTORAGE structure.
      /// 
      static LPGMSPRITESTORAGE SpriteData() {
        return m_spriteData;
      }

      /// ScriptData()
      ///   Returns pointer to runner's GMSCRIPTSTORAGE structure.
      ///
      /// Parameters:
      ///   None
      /// 
      /// Returns:
      ///   Pointer to GMSCRIPTSTORAGE structure.
      /// 
      static LPGMSCRIPTSTORAGE ScriptData() {
        return m_scriptData;
      }

      /// ScriptData()
      ///   Returns pointer to runner's GMDIRECT3DINFO structure.
      ///
      /// Parameters:
      ///   None
      /// 
      /// Returns:
      ///   Pointer to GMDIRECT3DINFO structure.
      /// 
      static LPGMDIRECT3DINFO D3DData() {
        return m_d3dInfo;
      }

      /// GetSurfaceArray()
      ///   Returns pointer to runner's array of GMSURFACE structures.
      ///
      /// Parameters:
      ///   None
      /// 
      /// Returns:
      ///   Pointer to an array of GMSURFACE structures.
      /// 
      static GMSURFACE* GetSurfaceArray() {
        return *m_surfaces; 
      }

      /// GetTextureArray()
      ///   Returns pointer to runner's array of GMTEXTURE structures.
      ///
      /// Parameters:
      ///   None
      /// 
      /// Returns:
      ///   Pointer to an array of GMTEXTURE structures.
      /// 
      static GMTEXTURE* GetTextureArray() {
        return *m_textures;
      }

      /// ScriptSwapTable()
      ///   Returns pointer to array of bytes that is used to decrypt/encrypt
      ///   scripts displayed in error messages in GM7.
      ///
      /// Example:
      ///   const char* swapTable = GMAPI->ScriptSwapTable();
      ///
      ///   char script[1024];
      ///   GMAPI->Scripts[scriptID].GetCode( script );
      ///
      ///   int scriptLength = sizeof( script ); // 1024
      ///
      ///   // GM's encrypting/decrypting routine:
      ///   for ( unsigned long i = 0; i < scriptLength; i++ )
      ///     script[i] = swapTable[ script[i] ];
      ///
      /// Parameters:
      ///   None
      /// 
      /// Returns:
      ///   Pointer to swap table.
      /// 
      static const char* ScriptSwapTable() {
        return m_scriptSwapTable;
      }

      /// SurfaceArraySizePtr()
      ///   Returns an pointer to variable that holds the size
      ///   of dynamic arrays used to access surface resource.
      ///
      /// Parameters:
      ///   None
      /// 
      /// Returns:
      ///   Pointer to "ArraySize" variable.
      /// 
      static int* SurfaceArraySizePtr() {
        return m_surfaceArraySize;
      }

      /// GMAPIFunctionArray( const enum GMFunctionPtrID aId )
      ///   This method is used by wrapped GM functions defined in GMAPI.
      /// 
      static const void* GMAPIFunctionArray( const int aId ) {
        return m_gmFunctions[aId];
      }

      /// GetMainWindowHandle()
      ///   Returns main game window handle.
      ///
      /// Parameters:
      ///   None
      /// 
      /// Returns:
      ///   Main window handle
      ///
      HWND GetMainWindowHandle() {
        return m_mainHwnd;
      }

      /// GetDebugWindowHandle()
      ///   Returns debug window handle. 
      ///
      /// Parameters:
      ///   None
      /// 
      /// Returns:
      ///   Debug window handle. In GM7 function can return
      ///   NULL in case when game doesn't run in debug mode.
      ///
      HWND GetDebugWindowHandle() {
        return m_debugHwnd;
      }

      /// GetHighscoreWindowHandle() [GM7 only]
      ///   Returns high-score window handle.
      ///
      /// Parameters:
      ///   None
      /// 
      /// Returns:
      ///   High-score window handle if the runner version is 7.0, otherwise NULL.
      ///
      HWND GetHighscoreWindowHandle() {
        return m_hScrHwnd;
      }

      /// Ptr()
      ///   Returns pointer to an instance of CGMAPI class.      
      static CGMAPI* Ptr() {
        return m_self;
      }

    private:
      CGMAPI( bool* aSuccess );
      ~CGMAPI();

      static CGMAPI* m_self;

      void RetrieveFunctionPointers();
      void RetrieveDataPointers();

      unsigned long m_gmVersion;

      static LPGMFUNCTIONINFOSTORAGE m_functionData;
      static LPGMBACKGROUNDSTORAGE   m_backgroundData;
      static LPGMSPRITESTORAGE       m_spriteData;
      static LPGMSURFACE*            m_surfaces;
      static LPGMTEXTURE*            m_textures;
      static LPGMDIRECT3DINFO        m_d3dInfo;
      static LPGMSCRIPTSTORAGE       m_scriptData;

      static HWND m_mainHwnd;
      static HWND m_debugHwnd;
      static HWND m_hScrHwnd;

      static char* m_scriptSwapTable;
      static int*  m_surfaceArraySize;

      static void* m_gmFunctions[1000];
  };

  /********************************************
   * ISprites inlined methods
   ********************************************/

  inline ISprite& ISprites::operator[]( int aSpriteId ) {
    if ( !Exists( aSpriteId ) )
      throw EGMAPISpriteNotExist( aSpriteId );

    m_iSprite.m_spriteId = aSpriteId;
    m_iSprite.m_sprite = CGMAPI::SpriteData()->sprites[aSpriteId];

    return m_iSprite;
  }

  inline int ISprites::GetArraySize() {
    return CGMAPI::SpriteData()->arraySize;
  }

  /********************************************
   * ISprite inlined methods
   ********************************************/

  inline const char* ISprite::GetName() {
    return CGMAPI::SpriteData()->names[m_spriteId];
  }

  /********************************************
   * ISpriteSubimages inlined methods
   ********************************************/

  inline ISpriteSubimage& ISpriteSubimages::operator[]( const int aSubimage ) {
    if ( aSubimage < 0 || aSubimage > (int) ISprite::SpritePtr()->nSubimages )
      throw EGMAPIInvalidSubimage( ISprite::m_spriteId, aSubimage );

    m_subimage.m_subimage = aSubimage;

    return m_subimage;
  }

  inline int ISpriteSubimages::GetCount() {
    return ISprite::SpritePtr()->nSubimages;
  }

  /********************************************
   * ISpriteSubimage inlined methods
   ********************************************/

  inline unsigned char* ISpriteSubimage::GetBitmap() {
    return ISprite::SpritePtr()->bitmaps[m_subimage]->bitmapData;
  }

  inline unsigned long ISpriteSubimage::GetBitmapSize() {
    return CGMAPI::GetBitmapSize( ISprite::SpritePtr()->bitmaps[m_subimage] );
  }

  inline int ISpriteSubimage::GetTextureID() {
    return ISprite::SpritePtr()->textureIDs[m_subimage];
  }

  inline IDirect3DTexture8* ISpriteSubimage::GetTexture() {
    return CGMAPI::GetD3DTexture( GetTextureID() );
  }

  /********************************************
   * IBackgrounds inlined methods
   ********************************************/

  inline IBackground& IBackgrounds::operator[]( const int aBackgroundId ) {
    if ( !Exists( aBackgroundId ) )
      throw EGMAPIBackgroundNotExist( aBackgroundId );

    m_iBackground.m_backgroundId = aBackgroundId;
    m_iBackground.m_background = CGMAPI::BackgroundData()->backgrounds[aBackgroundId];

    return m_iBackground;
  }

  inline int IBackgrounds::GetArraySize() {
    return CGMAPI::BackgroundData()->arraySize;
  }

  /********************************************
   * IBackground inlined methods
   ********************************************/

  inline const char* IBackground::GetName() {
    return CGMAPI::BackgroundData()->names[m_backgroundId];
  }

  inline int IBackground::GetWidth()  {
    return m_background->width;
  }

  inline int IBackground::GetHeight() {
    return m_background->height;
  }

  inline bool IBackground::BitmapExists() {
    return ( m_background->bitmap != NULL );
  } 

  inline unsigned char* IBackground::GetBitmap() {
    return m_background->bitmap->bitmapData;
  }

  inline unsigned long IBackground::GetBitmapSize() {
    return CGMAPI::GetBitmapSize( m_background->bitmap );
  }

  inline int IBackground::GetTextureID() {
    return m_background->textureId;
  }

  inline IDirect3DTexture8* IBackground::GetTexture() {
    return CGMAPI::GetD3DTexture( GetTextureID() );
  }

  inline bool IBackground::GetTransparent() {
    return m_background->transparent;
  }

  inline bool IBackground::GetSmoothEdges() {
    return m_background->smoothEdges;
  }

  inline bool IBackground::GetPreload() {
    return m_background->preload;
  }

  /********************************************
   * ISurfaces inlined methods
   ********************************************/

  inline ISurface& ISurfaces::operator[]( const int aSurfaceId ) {
    if ( !Exists( aSurfaceId ) )
      throw EGMAPISurfaceNotExist( aSurfaceId );

    m_iSurface.m_surfaceId = aSurfaceId;

    return m_iSurface;
  }

  inline bool ISurfaces::Exists( const int aTextureId ) {
    if ( CGMAPI::GetSurfaceArray() )
      return CGMAPI::GetSurfaceArray()[aTextureId].exists & 1;

    return false;
  }

  inline int ISurfaces::GetArraySize() {
    return *CGMAPI::SurfaceArraySizePtr();
  }

  /********************************************
   * ISurface inlined methods
   ********************************************/

  inline int ISurface::GetTextureID() {
    return CGMAPI::GetSurfaceArray()[m_surfaceId].textureId;
  }

  inline IDirect3DTexture8* ISurface::GetTexture() {
    return CGMAPI::GetD3DTexture( GetTextureID() );
  }

  inline int ISurface::GetWidth() {
    return CGMAPI::GetSurfaceArray()[m_surfaceId].width;
  }

  inline int ISurface::GetHeight() {
    return CGMAPI::GetSurfaceArray()[m_surfaceId].height;
  }

  /********************************************
   * IScripts inlined methods
   ********************************************/

  inline IScript& IScripts::operator[]( const int aScriptId ) {
    if ( !Exists( aScriptId ) )
      throw EGMAPIScriptNotExist( aScriptId );

    m_iScript.m_scriptId = aScriptId;

    return m_iScript;
  }

  inline bool IScripts::Exists( const int aScriptId ) {
    if ( CGMAPI::ScriptData()->scripts )
      if ( CGMAPI::ScriptData()->scripts[aScriptId] )
        return true;

    return false;
  }

  inline int IScripts::GetArraySize() {
    return CGMAPI::ScriptData()->arraySize;
  }

  /********************************************
   * IScript inlined methods
   ********************************************/

  unsigned long IScript::GetScriptLength() {
    return *((DWORD*) CGMAPI::ScriptData()->scripts[m_scriptId]->debugInfo->code - 1);
  }

  const char* IScript::GetName() {
    return CGMAPI::ScriptData()->names[m_scriptId];
  }

}
