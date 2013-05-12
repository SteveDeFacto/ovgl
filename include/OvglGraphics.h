/**
* @file OvglGraphics.h
* Copyright 2011 Steven Batchelor
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*       http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
* @brief None.
*/

#ifdef _WIN32
#  define DLLEXPORT __declspec( dllexport )
#else
#  define DLLEXPORT
#endif

// Forward declare external classes.
typedef struct _CGcontext *CGcontext;

namespace Ovgl
{
extern "C"
{
class Camera;
class Interface;
class Vector4;
class Texture;
class URect;
class Font;
class Event;
class Material;

class DLLEXPORT RenderTarget
{
public:
	/**
	* This is the constuctor for a window based render target.
	*/
	RenderTarget( Ovgl::Context* context, Ovgl::Window* window, const Ovgl::URect& viewport, uint32_t flags );
	/**
	* This is the constuctor for a texture based render target.
	*/
	RenderTarget( Ovgl::Context* context, Ovgl::Texture* texture, const Ovgl::URect& viewport, uint32_t flags );
	/**
    * Release render target.
    */
    ~RenderTarget();
	/**
	* This is a pointer to the context which owns this render target.
	*/
	Context* context;
	/**
	* This is a pointer to the camera which the render target will render from.
	*/
	Camera* view;
	/**
	* This frame buffer is used to provide multi sampling.
	*/
	uint32_t MultiSampleFrameBuffer;
	/**
	* This frame buffer is used to apply effects.
	*/
	uint32_t EffectFrameBuffer;
	/**
	* Depth buffer used for rendering.
	*/
	uint32_t DepthBuffer;
	/**
	* Color buffer used for rendering.
	*/
	uint32_t ColorBuffer;
	/**
	*
	*/
    uint32_t color_texture;
	/**
	*
	*/
	uint32_t depth_texture;
	/**
	* This is the primary texture.
	*/
	uint32_t PrimaryTex;
	/**
	* This is the secondary texture.
	*/
	uint32_t SecondaryTex;
	/**
	* This is the primary bloom texture.
	*/
	uint32_t PrimaryBloomTex;
	/**
	* This is the secondary bloom texture.
	*/
	uint32_t SecondaryBloomTex;
	/**
	* Pointer to window if this is a window based render target.
	*/
	Ovgl::Window* window;
	/**
	* Pointer to texture if this is a texture based render target.
	*/
	Ovgl::Texture* hTex;
	/**
	* The area of the texture or window to render to.
	*/
	Ovgl::URect rect;
	/**
	* List of user interfaces.
	*/
	std::vector< Ovgl::Interface* > Interfaces;
	/**
	* Last position of camera. Used for motion blur.
	*/
	Ovgl::Vector2 LastCamVec;
	/**
	* The area of the texture or window to render to.
	*/
    float eye_luminance;
	/**
	* Indicates if render target is to display debug information.
	*/
    bool debugMode;
    /**
	* Indicates if bloom is enabled.
	*/
    int bloom;
	/**
	* Indicates if automatic luminance is enabled.
	*/
    bool autoLuminance;
	/**
	* Indicates if multisampling is enabled.
	*/
    bool multiSample;
    /**
	* Indicates if motion blur is enabled.
	*/
    bool motionBlur;
    /**
	* Render auto luminance effect.
	*/
    void AutoLuminance();
	/**
	* Render auto luminance effect.
	*/
    void Bloom();
	/**
	* Render auto luminance effect.
	*/
    void MotionBlur( );
	/**
	* Render debug marker.
	*/
    void DrawMarker( const Matrix44& matrix );
    /**
    * Render entire scene.
    */
    void Render();
    /**
    * Update dimensions of render target.
    */
    void Update();
    /**
    * Render a single mesh.
    */
    void RenderMesh( const Ovgl::Mesh& mesh, const Matrix44& matrix, std::vector< Matrix44 >& pose, std::vector< Material* >& materials, bool PostRender );

    void do_event(Event event);
    void (*on_key_down)(char);
    void (*on_key_up)(char);
    void (*on_mouse_move)(long, long);
    void (*on_mouse_down)(long, long, int);
    void (*on_mouse_up)(long, long, int);
    void (*on_mouse_over)();
    void (*on_mouse_out)();
};

class DLLEXPORT Interface
{
public:
    Interface( Interface* parent, const URect& rect );
    Interface( RenderTarget* parent, const URect& rect );
    ~Interface();
    RenderTarget*               rendertarget;
    URect                       rect;
    Texture*                    background;
    bool                        tile_x;
    bool                        tile_y;
    bool                        wordbreak;
    Vector4                     color;
    Vector4                     text_color;
    std::string                 text;
    Font*                       font;
    uint32_t                    align;
    Interface*                  parent;
    int32_t                     hscroll;
    int32_t                     vscroll;
    std::vector<Interface*>     children;
    void set_text( const std::string& text );
    void render( const Ovgl::Rect& adjustedrect );
    void do_event( Event event, const Rect& adjustedrect);
    void (*on_key_down)(char);
    void (*on_key_up)(char);
    void (*on_mouse_move)(long, long);
    void (*on_mouse_down)(long, long, int);
    void (*on_mouse_up)(long, long, int);
    void (*on_mouse_over)();
    void (*on_mouse_out)();
};
}
}
