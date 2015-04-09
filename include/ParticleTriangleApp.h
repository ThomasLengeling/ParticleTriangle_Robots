/*
Copyright (c) 2010-2013, Thomas Sanchez Lengeling - All rights reserved.
This code is intended for use with the Cinder C++ library: http://libcinder.org


Chrysler-Triangles is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Chrysler-Triangles is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Chrysler-Triangles.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "cinder/app/AppBasic.h"
#include "cinder/Arcball.h"
#include "cinder/Camera.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Surface.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"
#include "cinder/params/Params.h"
#include "cinder/Rand.h"
#include "cinder/Xml.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/TextureFont.h"
#include "cinder/Timeline.h"
#include "cinder/qtime/QuickTime.h"
#include "cinder/Perlin.h"

#include "Resources.h"

#include "WarpBilinear.h"
#include "WarpPerspective.h"
#include "WarpPerspectiveBilinear.h"

#include "ParticleSystemTraer.h"
#include "ModifiedEulerIntegrator.h"
#include "ParticleTraer.h"

//AETHER
#include "ParticleBit.h"
#include "ParticleSystemBit.h"
#include "Tessellation.h"

#include "TriangleVertex.h"

#include "ColorManager.h"
#include "SurfaceColor.h"

#include "OscListener.h"

#include <Fade_2D.h>

#define SPRING_STRENGTH 0.1
#define SPRING_DAMPING	0.01

#define INDEXDELAUNAY	82

#define kParticles  2

#define NUMPARTICLES_TRI 57

using namespace ci;
using namespace ci::app;
using namespace ph::warping;
using namespace std;
using namespace traer::physics;

class ParticleTriangleApp : public AppBasic {
public:

	void prepareSettings(ci::app::AppBasic::Settings *settings);
	void setup();
	void shutdown();
	void update();
	void draw();

	void resize();

	void mouseMove(MouseEvent event);
	void mouseDown(MouseEvent event);
	void mouseDrag(MouseEvent event);
	void mouseUp(MouseEvent event);

	void keyDown(KeyEvent event);
	void keyUp(KeyEvent event);

	void updateWindowTitle();

	void setupWarp();
	void setupParticleSystemTraer();

	//XML
	void	saveParticleXML();
	void	loadParticlesXML();
	void    loadParticlesDefaultXML();

private:

	//AETHER
	void				setupParticlesAether();
	void				updateParticlesAether();
	float				timeStep;
	float				lineOpacity;
	float				pointOpacity;
	float				particleNeighborhood;
	float				particleRepulsion;
	float				centerAttraction;
	bool				addAttractionForce;

	int					particleMode;
	ci::Vec2i			mouse;
	bool				isMousePressed;

	Bit::ParticleSystem	particleSystem;

	ci::TriMesh 		mTriMeshTessellation;
	bool				calculateMesh;
	bool				activateWireFrame;

	void				drawAetherParticles();
	void				drawAetherParticleMode();

	ci::ColorA			particleBkgCol;
	ci::ColorA			particleVelCol;

	ColorManager		mManagerColors;

	int					colorSelector;
	int					colorSelectorPrev;
	bool				colorSelectorChanger;

	bool				activateRadius;
	bool				activateRadiusPrev;
	bool				activateBlend;
	bool				activateBlendPrev;


	//WARP
	gl::TextureRef		mImage;
	WarpList			mWarps;
	bool				drawWarp;

	void				drawWarpArea();
	void				drawTraerParticles();

	bool				mUseBeginEnd;

	Area				mSrcArea;

	//Traer PARTICLES SYSTEM
	traer::physics::ParticleSystem*		physics;

	vector<traer::physics::Particle*>	particles;

	void				makeSpring(int index0, int index1);

	bool				addParticleMouse;
	bool				moveParticleMouse;
	int					moveIndex;
	bool				deleteParticleMouse;

	bool				colorParticle;

	//PARAMS
	ci::params::InterfaceGlRef mParams;

	ci::Vec2f				mTranslate;

	float					avgFps;
	int						numTriangles;
	int						numParticles;
	bool					toggleWireframe;
	bool					toggleSculpture;
	void					clean();

	ci::ColorA				colorPicker;
	ci::ColorA				colorPickerTemp;

	ci::ColorA				bkg;

	//RENDER
	int						mRenderMode;
	int						mPrevRenderMode;
	void					drawRender();
	void					updateRender();
	bool					activateRender;


	//TRIANGLES
	std::vector<TriangleVertex*> mTriangles;

	int							 mIndexBufferAdd;
	int							 mIndexTriangleTemp0;
	int							 mIndexTriangleTemp1;
	int							 mIndexTriangleTemp2;

	bool						 makeTriangle;
	bool						 deleteTriangle;

	int							 indexDeleteTriangle;

	int							 indexColorPicker;
	int							 indexColorPickerPrev;

	bool						 debugParticles;

	bool						 colorSurfaceToggle;

	//TRIMESH
	ci::TriMesh					 mMeshTriangles;
	ci::TriMesh					 mMeshDelaunay;
	ci::TriMesh					 generateTraerMeshDelaunay();
	ci::TriMesh					 generateTraerMeshTriangle();

	bool						 generateMeh;
	bool						 blendMesh;
	bool						 drawFullWireFrame;

	//IMAGE
	ci::Surface					 mUnnamed;

	///BLOOM
	void						setupBloom();
	ci::gl::Fbo::Format			format;
	ci::gl::Fbo					mFboScene;
	ci::gl::Fbo					mFboBlur1;
	ci::gl::Fbo					mFboBlur2;

	ci::gl::GlslProg			mShaderBlur;

	float						bloomFactor;
	void						offScreenRender();

	//WIREFRAME
	void						setupWireFrame();
	void						updateWireFrame(ci::TriMesh mesh);
	ci::gl::GlslProg			mShaderWireFrame;
	gl::Fbo						mFboWire;
	float						alphaFrameWire;
	gl::Fbo						mFboWireDelaunay;

	ci::ColorA					colorWireFrame;
	int							lineWidth;

	//FINAL RENDER
	ci::gl::GlslProg			mShaderMixTexture;
	ci::gl::GlslProg			mMixShader;
	gl::Fbo						mFboSceneWire;

	//COLORS
	void						createGoodColors();
	int							numColors;
	int							colorSurfaceIndex;

	//draw Selector
	bool						drawDelaunay;
	bool						drawMesh;


	//Temblor
	void						activateTemblor();
	bool						mTemblor;

	//CueRef						mCueParticle;
	CueRef						mCueTemblor;
	Anim<float>					temblorAni;
	bool						activatorTemblor;
	void						timerTemblor();
	void						resetTemblor();
	int							*temblorVertex; 

	//Rotate
	CueRef						mCueRotate;
	Anim<float>					rotateAni;
	bool						activatorRotate;
	void						timerRotate();
	void						resetRotate();
	void						activateRotate();

	bool						updatePhysics;


	//MOVIES
	gl::Texture					mFrameTexture, mInfoTexture;
	qtime::MovieGlRef			mMovie;


	int changeMovie;

	gl::Texture					mFrameTexture2, mInfoTexture2;
	qtime::MovieGlRef			mMovie2;

	//Aether
	bool						aether;

	//OSC
	osc::Listener 			listener;
	void					setupOSC();
	void					updateOSC();

	//EVENTOS
	int			 eventType;
	void		 activateEvents();
	bool		 activateEvent;


	int			 readyColorCounter;
	float        timeColor;

	int			 readyPosCounter;
	bool		 changePosInside;
	bool		 changePosInOut;

	int         counterFlip;


	//FORCE INTERACTION
	ci::Vec2f	forceInteractionLeft;
	ci::Vec2f   forceInteractionRight;

	bool		interactionLeftActivate;
	bool		interactionRightActivate;

	int         interactionTypeAether;


	//AUTOMATONS

	bool	   activateAutomatonLeft;
	bool	   activateAutomatonRight;

	ci::Vec2f  automatonLeft;
	ci::Vec2f  automatonRight;

	//INCREMENT CENTER
	bool					activateIncCenter;
	int						incrementCounter;

	int						mSeed;
	int						mOctaves;
	float					mTime;
	Perlin					mPerlin;
	float					mFrequency;

	float					scaleFactor;

	int						mCurrentFrame;


	//
	Font mFont;
	gl::TextureFontRef mTextureFont;
};