#include "ParticleTriangleApp.h"
#include "AetherFunctions.h"
#include "TriangulatedFunctions.h"
#include "Renders.h"
#include "toggleEvents.h"
#include "Osc.h"

//----------------------------------------------SETUP WARP----------------------------

void ParticleTriangleApp::prepareSettings(ci::app::AppBasic::Settings *settings)
{
	settings->setWindowSize(1270, 1080);
	settings->setWindowPos(0, 0);
	settings->setBorderless(true);
	settings->setFrameRate(180);
}

void ParticleTriangleApp::setup()
{
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_POINT_SIZE);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_MULTISAMPLE_ARB);

	console() << "LOADING SHADERS" << std::endl;
	setupBloom();
	setupWireFrame();

	console() << "CREATING COLORS" << std::endl;
	createGoodColors();

	//SETUP PHYSICS
	console() << "CREATING TRAER PHYSICS" << std::endl;
	setupParticleSystemTraer();

	// INIT WARP
	console() << "CREATING WARP" << std::endl;
	setupWarp();

	console() << "SETTUP OSC" << std::endl;
	setupOSC();

	colorSurfaceIndex = 0;

	// CREATE FONT
	mFont = Font("Arial", 18);
	mTextureFont = gl::TextureFont::create(mFont);

	console() << "SETTING CUES" << std::endl;
	mCueTemblor = timeline().add(bind(&ParticleTriangleApp::timerTemblor, this), timeline().getCurrentTime() + 1);

	mCueTemblor->setAutoRemove(false);
	mCueTemblor->setLoop(false);
	mCueTemblor->setDuration(1);

	mCueRotate = timeline().add(bind(&ParticleTriangleApp::timerRotate, this), timeline().getCurrentTime() + 1);

	mCueRotate->setAutoRemove(false);
	mCueRotate->setLoop(false);
	mCueRotate->setDuration(1);

	temblorVertex = new int[12];
	temblorVertex[0] = 53;
	temblorVertex[1] = 45;
	temblorVertex[2] = 38;
	temblorVertex[3] = 50;
	temblorVertex[4] = 55;
	temblorVertex[5] = 52;
	temblorVertex[6] = 31;
	temblorVertex[7] = 43;
	temblorVertex[8] = 54;
	temblorVertex[9] = 51;
	temblorVertex[10] = 32;
	temblorVertex[11] = 39;

	console() << "SETTING PARAMS" << std::endl;
	mUnnamed = ci::Surface(loadImage(loadResource(UNNAMED)));

	console() << "SETTINGS BIT PARTICLES" << std::endl;
	setupParticlesAether();

	mParams = ci::params::InterfaceGl::create(getWindow(), "params", ci::Vec2i(300, 750));
	mParams->addParam("FPS", &avgFps, "");
	mParams->addParam("Num Particles", &numParticles, "");
	mParams->addParam("numTriangles", &numTriangles, "");
	mParams->addParam("Eventos", &eventType, "min=0 max=10 step=1");
	mParams->addParam("Activate Event", &activateEvent, "");
	mParams->addParam("interactionType", &interactionTypeAether, "min=0 max=6 step=1");
	mParams->addParam("changeMovie", &changeMovie, "min=0 max=1 step=1");
	
	mParams->addSeparator();
	mParams->addParam("mRenderMode", &mRenderMode, "min=0 max=3 step=1");
	mParams->addParam("bkg", &bkg,"");
	mParams->addSeparator();
	mParams->addButton("temblor", bind(&ParticleTriangleApp::resetTemblor, this), "");
	mParams->addButton("rotate", bind(&ParticleTriangleApp::resetRotate, this), "");
	mParams->addSeparator();
	mParams->addParam("wireframe", &toggleWireframe, "");
	mParams->addParam("Color wire", &colorWireFrame, "");
	mParams->addParam("Line Width", &lineWidth, "min=1 max=10 step=1");
	mParams->addParam("drawFullWireFrame", &drawFullWireFrame, "");
	mParams->addParam("Sculpture,", &toggleSculpture, "");
	mParams->addParam("colorPicker", &colorPicker, "");
	mParams->addParam("Debug Particles", &debugParticles, "");
	mParams->addParam("bloom", &bloomFactor, "min=0 max=2.5 step=0.05");
	mParams->addParam("alpha FrameWire", &alphaFrameWire, "min=0 max=1 step=0.01");
	mParams->addParam("Trans X", &mTranslate.x, "min=-1 max=1 step=0.01");
	mParams->addParam("Trans Y", &mTranslate.y, "min=-1 max=1 step=0.01");
	mParams->addSeparator();
	mParams->addParam("color Surface", &colorSurfaceIndex, "min=0 max=12 step=1");
	mParams->addParam("color Surface Toggle", &colorSurfaceToggle, "min=0 max=4 step=1");
	mParams->addParam("Delaunay", &drawDelaunay, "");
	mParams->addParam("Draw Mesh", &drawMesh, "");
	mParams->addParam("blendMesh", &blendMesh, "");

	//mParams->addParam("activateEvent", &activateEvent, "");
	//mParams->addParam("EvenType", &eventType, "min=0 max=10 step=1");
	mParams->addSeparator();
	mParams->addParam("centerAttractcion", &centerAttraction, "min=0.001 max=1.0 step=0.02");
	mParams->addParam("activateIncCenter", &activateIncCenter, "");
	mParams->addParam("Background Particle color", &particleBkgCol, "");
	mParams->addParam("Velocity Particle color", &particleVelCol, "");
	mParams->addParam("Particle Radius", &activateRadius, "");
	mParams->addParam("Particle Draw Type", &particleMode, "min=0 max=5 step=1");
	mParams->addSeparator();
	mParams->addParam("WireFrame", &activateWireFrame, "");
	mParams->addParam("Texture Color", &colorSelector, "min=0 max=13 step=1");
	mParams->addParam("Draw Meshs", &calculateMesh, "");
	mParams->addParam("Blend Mesh", &activateBlend, "");
	mParams->addSeparator();
	mParams->addParam("alpha FrameWire", &alphaFrameWire, "min=0.0 max=1.0 step=0.01");
	mParams->addParam("addAttractionForce", &addAttractionForce, "");

	mParams->addSeparator();
	mParams->addButton("Clean", bind(&ParticleTriangleApp::clean, this), "");
	mParams->addButton("Save", bind(&ParticleTriangleApp::saveParticleXML, this), "");
	mParams->addSeparator();
	mParams->addParam("update", &updatePhysics);
	mParams->addButton("Load", bind(&ParticleTriangleApp::loadParticlesXML, this), "");
	mParams->addParam("scaleFactor", &scaleFactor, "min=0.0 max=10.0 step=0.005");

	mCurrentFrame = 450;
	

	scaleFactor = 1.0;
	counterFlip = 0;

	changePosInOut = false;
	changePosInside = false;
	readyColorCounter = 0;
	timeColor = 0;

	toggleSculpture = false;
	eventType = 0;
	activateEvent = true;
	//AETHER
	particleBkgCol = ci::ColorA(0.82, 0.2, 7);
	particleVelCol = ci::ColorA(0.0, 0.7, 0.6, 0.7);

	activateRadius = true;
	activateRadiusPrev = false;
	activateBlend = false;
	activateBlendPrev = false;

	calculateMesh = true;

	activateWireFrame = true;

	addAttractionForce = true;

	colorSelector = 0;
	colorSelectorPrev = 0;

	particleMode = 3;


	//RENDER

	mRenderMode		= 1;
	mPrevRenderMode = mRenderMode;

	updatePhysics = true;
	//INIT VALUES
	activatorRotate = false;
	mTemblor		= false;

	lineWidth = 3;
	colorWireFrame = ci::ColorA(0, 0, 0, 1);

	mTranslate = ci::Vec2f(0, 0);
	bkg = ci::ColorA(0, 0, 0, 1);

	alphaFrameWire = 0.44; //.9

	bloomFactor = 0;

	debugParticles = false;

	blendMesh = false;

	drawDelaunay = false;
	drawMesh = true;

	drawWarp = false;
	addParticleMouse = false;
	moveParticleMouse = false;
	deleteParticleMouse = false;
	toggleWireframe = false;
	colorParticle = false;
	moveIndex = -1;

	deleteTriangle = false;
	makeTriangle = false;
	mIndexBufferAdd = 0;
	mIndexTriangleTemp0 = 0;
	mIndexTriangleTemp1 = 0;
	mIndexTriangleTemp2 = 0;

	indexColorPicker = -1;
	indexDeleteTriangle = -1;

	indexColorPickerPrev = 0;

	changeMovie = 0;
}

void ParticleTriangleApp::setupBloom()
{
	gl::Fbo::Format format;
	format.enableMipmapping(false);
	format.setWrap(GL_CLAMP_TO_BORDER_ARB, GL_CLAMP_TO_BORDER_ARB);

	mFboBlur1 = gl::Fbo(getWindowWidth() / 4.0f, getWindowHeight() / 4.0f, format);
	mFboBlur2 = gl::Fbo(getWindowWidth() / 4.0f, getWindowHeight() / 4.0f, format);

	format.setCoverageSamples(16);
	format.setSamples(4);

	mFboScene = gl::Fbo(getWindowWidth() / 1.0f, getWindowHeight() / 1.0f, format);

	try {
		mShaderBlur = gl::GlslProg(loadResource(RES_VERT_GLSL), loadResource(RES_FRAG_GLSL));
	}
	catch (...) {
		ci::app::console() << "Can't load/compile blur shader" << std::endl;
		quit();
	}
}


void ParticleTriangleApp::setupWireFrame()
{
	gl::Fbo::Format format;
	format.setCoverageSamples(16);
	format.setSamples(16);

	mFboWire = gl::Fbo(getWindowWidth(), getWindowHeight(), format);
	mFboWireDelaunay = gl::Fbo(getWindowWidth(), getWindowHeight(), format);
	mFboSceneWire = gl::Fbo(getWindowWidth(), getWindowHeight(), format);

	console() << "LOADING WIREFRAME" << std::endl;
	try {
		mShaderWireFrame = gl::GlslProg(loadResource(WIREFRAME_VERT),
			loadResource(WIREFRAME_FRAG),
			loadResource(WIREFRAME_GEOM),
			GL_TRIANGLES, GL_TRIANGLE_STRIP, 3);
	}
	catch (gl::GlslProgCompileExc e) {
		ci::app::console() << e.what() << std::endl;
		quit();
	}

	console() << "LOADING BLEND SHADER" << std::endl;
	try{
		mShaderMixTexture = gl::GlslProg(loadResource(PASSTHRU_VERT), loadResource(BLEND_FRAG));
	}
	catch (...){
		ci::app::console() << "Can't load/compile BLEND shader" << std::endl;
		quit();
	}
}


void ParticleTriangleApp::setupWarp()
{
	mUseBeginEnd = true;
	updateWindowTitle();

	fs::path settings = getAssetPath("") / "warps.xml";
	if (fs::exists(settings))
	{
		// load warp settings from file if one exists
		mWarps = Warp::readSettings(loadAsset("warps.xml"));
	}
	else
	{
		// otherwise create a warp from scratch
		mWarps.push_back(WarpPerspectiveBilinear::create());
	}

	// load test image
	try
	{
		mImage = gl::Texture::create(loadImage(loadAsset("help.png")));

		mSrcArea = mImage->getBounds();

		console() << "LOADING MOVIE" << std::endl;
		mMovie = qtime::MovieGl::create(loadAsset("lineas.mov"));
		mMovie->setLoop();
		mMovie->play();

		console() << "LOADING MOVIE 2" << std::endl;
		mMovie2 = qtime::MovieGl::create(loadAsset("lineas2.mov"));
		mMovie2->setLoop();
		mMovie2->play();


		mSrcArea = mMovie->getBounds();
		Warp::setSize(mWarps, getWindowSize());
		//Warp::setSize(mWarps, mMovie->getSize());
		// adjust the content size of the warps
		//Warp::setSize(mWarps, mImage->getSize());
	}
	catch (const std::exception &e)
	{
		console() << "error "<<e.what() << std::endl;
	}
}


void ParticleTriangleApp::createGoodColors()
{
	try{
		mManagerColors.addNewColors("../resources/unnamed.jpg", getWindowWidth(), getWindowHeight(), 137);
		mManagerColors.addNewColors("../resources/chrysler_front.jpg", getWindowWidth(), getWindowHeight(), 140);
		mManagerColors.addNewColors("../resources/chrysler_front_bw.jpg", getWindowWidth(), getWindowHeight(), 141);
		mManagerColors.addNewColors("../resources/debula01.jpg", getWindowWidth(), getWindowHeight(), 142);
		mManagerColors.addNewColors("../resources/debula02.jpg", getWindowWidth(), getWindowHeight(), 143);
		mManagerColors.addNewColors("../resources/debula03.jpg", getWindowWidth(), getWindowHeight(), 144);


		mManagerColors.addNewColors("../resources/debula04.jpg", getWindowWidth(), getWindowHeight(), 145);
		mManagerColors.addNewColors("../resources/debula05.jpg", getWindowWidth(), getWindowHeight(), 146);
		mManagerColors.addNewColors("../resources/debula06.jpg", getWindowWidth(), getWindowHeight(), 147);
		mManagerColors.addNewColors("../resources/debula07.jpg", getWindowWidth(), getWindowHeight(), 148);
		mManagerColors.addNewColors("../resources/debula08.jpg", getWindowWidth(), getWindowHeight(), 149);
		mManagerColors.addNewColors("../resources/debula09.jpg", getWindowWidth(), getWindowHeight(), 150);
		mManagerColors.addNewColors("../resources/debula10.jpg", getWindowWidth(), getWindowHeight(), 151);

		numColors = mManagerColors.getSize() - 1;
	}
	catch (...){
		console() << "Creating Color Image failed" << std::endl;
		quit();
	}
}

//----------------------------------UPDATE-----------------------------------------
void ParticleTriangleApp::update()
{
	avgFps = getAverageFps();

	activateEvents();

	updateRender();

	updateOSC();

//	if (getElapsedFrames() % 120 == 0){
//		writeImage(getHomeDirectory() / "cinder" / "saveImage_" / (toString(mCurrentFrame) + ".png"), copyWindowSurface());
//		mCurrentFrame++;
//	}
}

///-----------------TEMBLOR------------------------------------------------------------
void ParticleTriangleApp::resetTemblor()
{
	mCueTemblor->reset(true);
	mTemblor = true;
	temblorAni.value() = 0;
	lineWidth = 8.0;
	 
	console() << "RESET TEMBLOR" << std::endl;
}

void ParticleTriangleApp::activateTemblor()
{
	//26 30, 15, 20, 51, 47, 54, 27, 32 1, 4, 14
	if (mTemblor){
		/*
		int  indexTemblor = 35;
		if ((int)temblorAni.value() % 2 == 0 || (int)temblorAni.value() % 3 == 0)
			indexTemblor = temblorVertex[ci::Rand::randInt(12)];
		traer::physics::Particle * p = physics->getParticle(indexTemblor);
		p->mPos = p->mPos + ci::Rand::randVec3f() * 10.0;

		for (int i = 65; i < physics->numberOfParticles(); i++){
			traer::physics::Particle * p = physics->getParticle(i);

			p->mPos = p->mPos + ci::randFloat(-1, 1)*ci::Rand::randVec3f() * 1.0;
		}
		*/
	}

	if (temblorAni.value() >= 100){
		mTemblor = false;
		lineWidth = 3.0;
	}
}

void ParticleTriangleApp::timerTemblor()
{
	timeline().apply(&temblorAni, 0.0f, 100.0f, 2, EaseOutQuart());
}

//-----------------------------ROTATE----------------------------------------------
void ParticleTriangleApp::timerRotate()
{
	timeline().apply(&rotateAni, 0.0f, 100.0f, 2, EaseOutQuart());
}
void ParticleTriangleApp::resetRotate()
{
	mCueRotate->reset(true);
	activatorRotate = true;
	rotateAni.value() = 0;
	console() << "RESET TEMBLOR" << std::endl;
}

void ParticleTriangleApp::activateRotate()
{
	if (activatorRotate){
	/*	int  indexTemblor = 30;
		for (int i = 65; i < physics->numberOfParticles(); i++){
			traer::physics::Particle * p = physics->getParticle(i);
			p->mPos.x += ci::Rand::randFloat(-0.5, 0.5);// cos(getElapsedSeconds() * 5) * 2;
			p->mPos.y += ci::Rand::randFloat(-0.5, 0.5);// sin(getElapsedSeconds() * 5) * 2;
		}
		*/
	}

	if (rotateAni.value() >= 100){
		activatorRotate = false;
	}
}


//----------------------------------DRAW------------------------------------------------
void ParticleTriangleApp::draw()
{
	drawRender();

	mParams->draw();
}


void ParticleTriangleApp::drawWarpArea()
{
	if (mImage)
	{
		// iterate over the warps and draw their content
		for (WarpConstIter itr = mWarps.begin(); itr != mWarps.end(); ++itr)
		{
			// create a readable reference to our warp, to prevent code like this: (*itr)->begin();
			WarpRef warp(*itr);

			// there are two ways you can use the warps:
			if (mUseBeginEnd)
			{
				// a) issue your draw commands between begin() and end() statements
				warp->begin();

				gl::pushModelView();
				gl::color(ci::Color::white());
				gl::draw(mFboSceneWire.getTexture(), getWindowBounds());

				gl::enableAdditiveBlending();
				gl::draw(mFboBlur2.getTexture(), getWindowBounds());
				gl::disableAlphaBlending();
				gl::popModelView();

				warp->end();
	
			}
			else
			{
				// b) simply draw a texture on them (ideal for video)
				// in this demo, we want to draw a specific area of our image,
				// but if you want to draw the whole image, you can simply use: warp->draw( mImage );
				//gl::enableAlphaBlending();
				if (changeMovie == 0){
					if (mFrameTexture)
						warp->draw(mFrameTexture);// , mSrcArea, warp->getBounds());
				}
				if (changeMovie == 1){
					if (mFrameTexture2)
						warp->draw(mFrameTexture2);// , mSrcArea, warp->getBounds());
				}
			}
		}
	}
}



//----------------------EVENTS----------------------------------------------
void ParticleTriangleApp::shutdown()
{
	// save warp settings
	fs::path settings = getAssetPath("") / "warps.xml";
	Warp::writeSettings(mWarps, writeFile(settings));

	delete temblorVertex;
}

void ParticleTriangleApp::resize()
{
	// tell the warps our window has been resized, so they properly scale up or down
	Warp::handleResize( mWarps );
}

void ParticleTriangleApp::mouseMove( MouseEvent event )
{
	// pass this mouse event to the warp editor first
	if( ! Warp::handleMouseMove( mWarps, event ) )
	{
		if (deleteTriangle){
			int i = 0;
			
			for (auto it = mTriangles.begin(); it != mTriangles.end(); ++it){
				TriangleVertex *triVert = *it;
				if (triVert->isPointInTri(ci::Vec3f(event.getPos().x, event.getPos().y, 0))){
					indexDeleteTriangle = i;
				}
				i++;
			}
		}

		isMousePressed = true;
		mouse = Vec2i(event.getPos());

	}
}

void ParticleTriangleApp::mouseDown( MouseEvent event )
{
	// pass this mouse event to the warp editor first
	if( ! Warp::handleMouseDown( mWarps, event ) )
	{
		if (addParticleMouse)
			physics->makeParticle(0.2f, event.getPos().x, event.getPos().y, 0);

		if (moveParticleMouse){
			for (int i = 0; i < physics->numberOfParticles(); i++){
				traer::physics::Particle * p = physics->getParticle(i);
				if (event.getPos().distance(p->mPos.xy()) < 12){
					p->mPos.x = event.getPos().x;
					p->mPos.y = event.getPos().y;
					moveIndex = i;
				}
			}
		}

		if (deleteTriangle){
			int i = 0;
			for (auto it = mTriangles.begin(); it != mTriangles.end(); ++it){
				TriangleVertex *triVert = *it;
				if (triVert->isPointInTri(ci::Vec3f(event.getPos().x, event.getPos().y, 0))){
					//console() << i << std::endl;
					indexDeleteTriangle = i;
				}
				i++;
			}

		}

			/*
			mParticlesSort.clear();
			for (int i = 0; i < physics->numberOfParticles(); i++){
				Particle * p = physics->getParticle(i);
				float dist = event.getPos().distance(p->mPos.xy());
				p->distance = dist;
				mParticlesSort.push_back(p);
			}
			sort(mParticlesSort.begin(), mParticlesSort.end(),
				[](const Particle * a, const Particle * b) -> bool
			{
				return a->distance < b->distance;
			});
			console() << "sorted" << std::endl;
		}
		*/
	}
}

void ParticleTriangleApp::mouseDrag( MouseEvent event )
{
	// pass this mouse event to the warp editor first
	if( ! Warp::handleMouseDrag( mWarps, event ) )
	{
		if (moveParticleMouse && moveIndex >= 0){
			traer::physics::Particle * p = physics->getParticle(moveIndex);
			p->mPos.x = event.getPos().x;
			p->mPos.y = event.getPos().y;
		}
		// let your application perform its mouseDrag handling here

		mouse = Vec2i(event.getPos());
	}
}

void ParticleTriangleApp::mouseUp( MouseEvent event )
{
	// pass this mouse event to the warp editor first
	if( ! Warp::handleMouseUp( mWarps, event ) )
	{
		isMousePressed = false;

		if (deleteParticleMouse){
			for (int i = 0; i < physics->numberOfParticles(); i++){
				traer::physics::Particle * p = physics->getParticle(i);
				if (event.getPos().distance(p->mPos.xy()) < 10){
					physics->removeParticle(p);
				}
			}
		}

		if (moveParticleMouse & moveIndex >= 0){
			traer::physics::Particle * p = physics->getParticle(moveIndex);
			p->mPos.x = event.getPos().x;
			p->mPos.y = event.getPos().y;
		}
		// let your application perform its mouseUp handling here

		if (makeTriangle){
			for (int i = 0; i < physics->numberOfParticles(); i++){
				traer::physics::Particle * p = physics->getParticle(i);
				if (event.getPos().distance(p->mPos.xy()) < 8){
					if (mIndexBufferAdd == 0)
						mIndexTriangleTemp0 = i;

					if (mIndexBufferAdd == 1)
						mIndexTriangleTemp1 = i;

					if (mIndexBufferAdd == 2){
						mIndexTriangleTemp2 = i;
						//make triangle
						TriangleVertex *tri = new TriangleVertex(mIndexTriangleTemp0, mIndexTriangleTemp1, mIndexTriangleTemp2);
						tri->setVert0(physics->getParticle(mIndexTriangleTemp0)->mPos);
						tri->setVert1(physics->getParticle(mIndexTriangleTemp1)->mPos);
						tri->setVert2(physics->getParticle(mIndexTriangleTemp2)->mPos);
						mTriangles.push_back(tri);
					}

					mIndexBufferAdd++;
					if (mIndexBufferAdd >= 3)
						mIndexBufferAdd = 0;
				}
			}
		}

		if (deleteTriangle){
			mTriangles.erase(mTriangles.begin() + indexDeleteTriangle);
		}


		if (colorParticle){
			int i = 0;
			for (int i = 0; i < physics->numberOfParticles(); i++){
				traer::physics::Particle * p = physics->getParticle(i);
				if (event.getPos().distance(p->mPos.xy()) < 12){
					indexColorPicker = i;
					colorPickerTemp = p->mCol;
				}
				i++;
			}
		}
	}
}

void ParticleTriangleApp::keyDown( KeyEvent event )
{
	// pass this key event to the warp editor first
	if( ! Warp::handleKeyDown( mWarps, event ) )
	{
		// warp editor did not handle the key, so handle it here
		switch( event.getCode() )
		{
		case KeyEvent::KEY_ESCAPE:
			// quit the application
			quit();
			break;
		case KeyEvent::KEY_s:
			drawWarp = !drawWarp;
			break;
		case KeyEvent::KEY_f:
			// toggle full screen
			setFullScreen( ! isFullScreen() );
			break;
		case KeyEvent::KEY_w:
			// toggle warp edit mode
			Warp::enableEditMode( ! Warp::isEditModeEnabled() );
			break;
		case KeyEvent::KEY_a:
			// toggle drawing a random region of the image
			if( mSrcArea.getWidth() != mImage->getWidth() || mSrcArea.getHeight() != mImage->getHeight() )
				mSrcArea = mImage->getBounds();
			else
			{
				int x1 = Rand::randInt( 0, mImage->getWidth() - 150 );
				int y1 = Rand::randInt( 0, mImage->getHeight() - 150 );
				int x2 = Rand::randInt( x1 + 150, mImage->getWidth() );
				int y2 = Rand::randInt( y1 + 150, mImage->getHeight() );
				mSrcArea = Area( x1, y1, x2, y2 );
			}
			break;
		case KeyEvent::KEY_SPACE:
			// toggle drawing mode
			mUseBeginEnd = !mUseBeginEnd;
			updateWindowTitle();
			break;

		//XML
		case KeyEvent::KEY_1:
			saveParticleXML();
			break;
		case KeyEvent::KEY_2:
			loadParticlesXML();
			break;

		case  KeyEvent::KEY_8:
				mMovie->stop();
				break;
		case  KeyEvent::KEY_9:
				mMovie->play();
				break;
		case KeyEvent::KEY_7:
				timeColor = 0;
			break;
		//ADD PARTICLES
		case KeyEvent::KEY_p:
			addParticleMouse = true;
			break;
		//MOVE PARTICLE
		case KeyEvent::KEY_m:
			moveParticleMouse = true;
			break;
		//DELETE PARTICLE
		case KeyEvent::KEY_d:
			deleteParticleMouse = true;
			break;
		case KeyEvent::KEY_c:
			colorParticle = true;
			break;

		//MAKE TRIANGLE
		case KeyEvent::KEY_t:
			makeTriangle = true;
			break;
		//DELETE TRIANGLE
		case KeyEvent::KEY_y:
			deleteTriangle = true;
			break;

		}
	}
}

void ParticleTriangleApp::keyUp( KeyEvent event )
{
	// pass this key event to the warp editor first
	if( ! Warp::handleKeyUp( mWarps, event ) )
	{
		
		if (!Warp::handleKeyDown(mWarps, event))
		{
			// warp editor did not handle the key, so handle it here
			switch (event.getCode())
			{

			//ADD PARTICLES
			case KeyEvent::KEY_p:
				addParticleMouse = false;
				break;
			//MOVE PARTICLE
			case KeyEvent::KEY_m:
				moveParticleMouse = false;
				break;
			//DELETE PARTICLE
			case KeyEvent::KEY_d:
				deleteParticleMouse = false;
				break;
			case KeyEvent::KEY_c:
				colorParticle = false;
				//indexColorPicker = -1;
				break;

			//MAKE TRIANGLE
			case KeyEvent::KEY_t:
				makeTriangle = false;
				break;
			//DELETE TRIANGLE
			case KeyEvent::KEY_y:
				deleteTriangle = false;
				indexDeleteTriangle = -1;
				break;
			}
		}
	}
}

void ParticleTriangleApp::clean()
{
	physics->clear();
	mTriangles.clear();
}

void ParticleTriangleApp::updateWindowTitle()
{
	if(mUseBeginEnd)
		getWindow()->setTitle("Warping Sample - Using begin() and end()");
	else
		getWindow()->setTitle("Warping Sample - Using draw()");
}

///////////////-----------------XML-----------------------------------------
void ParticleTriangleApp::loadParticlesXML()
{

	try {
		console() << "LOADING XML" << std::endl;
		XmlTree doc(loadFile(getAppPath() / "data" / fs::path("particles.xml")));

		XmlTree::Container &childrenParticle = doc.getChild("Particles").getChildren();
		int  i = 0;
		for (auto it = childrenParticle.begin(); it != childrenParticle.end(); ++it){
			if (i == 57)
				continue;
			float x = (*it)->getChild("X").getValue<float>();
			float y = (*it)->getChild("Y").getValue<float>();

			float cr = (*it)->getChild("colorR").getValue<float>();
			float cg = (*it)->getChild("colorG").getValue<float>();
			float cb = (*it)->getChild("colorB").getValue<float>();
			float ca = (*it)->getChild("colorA").getValue<float>();

			physics->makeParticle(0.2, x, y, 0, ci::ColorA(cr, cg, cb, ca));

			if (i < 28){
				physics->getParticle(i)->setFixed(true);
				physics->getParticle(i)->setLocked(true);
			}

			i++;
		}
		numParticles = physics->numberOfParticles();

		int ae = numParticles;
		XmlTree::Container &childrenAether = doc.getChild("Aether").getChildren();
		for (auto it = childrenAether.begin(); it != childrenAether.end(); ++it){
			float x = (*it)->getChild("X").getValue<float>();
			float y = (*it)->getChild("Y").getValue<float>();

			float cr = (*it)->getChild("colorR").getValue<float>();
			float cg = (*it)->getChild("colorG").getValue<float>();
			float cb = (*it)->getChild("colorB").getValue<float>();
			float ca = (*it)->getChild("colorA").getValue<float>();

			physics->makeParticle(0.2, x, y, 0, ci::ColorA(cr, cg, cb, ca));
			ae++;
		}
		
		XmlTree::Container &childrenTriangle = doc.getChild("Triangles").getChildren();
		int tri = 0;
		int indexAether = numParticles;
		for (auto it = childrenTriangle.begin(); it != childrenTriangle.end(); ++it){
			int id0 = (*it)->getChild("index0").getValue<int>();
			int id1 = (*it)->getChild("index1").getValue<int>();
			int id2 = (*it)->getChild("index2").getValue<int>();

			float cr = (*it)->getChild("colorR").getValue<float>();
			float cg = (*it)->getChild("colorG").getValue<float>();
			float cb = (*it)->getChild("colorB").getValue<float>();
			float ca = (*it)->getChild("colorA").getValue<float>();

			ci::ColorA col = ci::ColorA(cr, cg, cb, ca);

			TriangleVertex * vert = new TriangleVertex(id0, id1, id2);
			vert->setVert0(physics->getParticle(id0)->mPos);
			vert->setVert1(physics->getParticle(id1)->mPos);
			vert->setVert2(physics->getParticle(id2)->mPos);
			vert->setColor(col);

			vert->add_index(indexAether++);
			vert->add_index(indexAether++);
			vert->add_index(indexAether++);

			mTriangles.push_back(vert);
			tri++;
		}
		
		numTriangles = mTriangles.size();
		
		console() << "COPY INSIDE " << std::endl;
		for (int i = NUMPARTICLES_TRI; i < physics->numberOfParticles(); i++){
			traer::physics::Particle * p = physics->getParticle(i);
			p->mPosInSide = p->mPos;
		}

		console() << "COPY OUTSIDE" << std::endl;
		for (auto it = mTriangles.begin(); it != mTriangles.end(); ++it){

			TriangleVertex * tri = *it;
			int indexVert0 = tri->getIndexVert0();
			int indexVert1 = tri->getIndexVert1();
			int indexVert2 = tri->getIndexVert2();

			traer::physics::Particle * p0 = physics->getParticle(indexVert0);
			traer::physics::Particle * p1 = physics->getParticle(indexVert1);
			traer::physics::Particle * p2 = physics->getParticle(indexVert2);

			if (tri->getVertexIndex().size() > 0){
				int iTri = 0;
				for (auto itr = tri->getVertexIndex().begin(); itr != tri->getVertexIndex().end(); ++itr){
					int index = *itr;
					traer::physics::Particle * parE = physics->getParticle(index);
					if (iTri == 0){
						p0->mPosOutSide = parE->mPos;
					}
					if (iTri == 1){
						p1->mPosOutSide = parE->mPos;
					}
					if (iTri == 2){
						p2->mPosOutSide = parE->mPos;
					}
					iTri++;
				}
			}
		}
	
	}
	catch (XmlTree::Exception e) {
		console() << "ERROR: loading/reading configuration file." <<
			e.what() << std::endl;
		exit(0);
	}

}

void ParticleTriangleApp::loadParticlesDefaultXML()
{
	try {
		console() << "DEFAULT XML" << std::endl;
		XmlTree doc(loadFile(getAppPath() / "data" / fs::path("default.xml")));

		XmlTree::Container &childrenParticle = doc.getChild("Particles").getChildren();
		for (auto it = childrenParticle.begin(); it != childrenParticle.end(); ++it){
			float x = (*it)->getChild("X").getValue<float>();
			float y = (*it)->getChild("Y").getValue<float>();

			float cr = (*it)->getChild("colorR").getValue<float>();
			float cg = (*it)->getChild("colorG").getValue<float>();
			float cb = (*it)->getChild("colorB").getValue<float>();
			float ca = (*it)->getChild("colorA").getValue<float>();

			physics->makeParticle(0.2, x, y, 0, ci::ColorA(cr, cg, cb, ca));
		}
		numParticles = physics->numberOfParticles();

		XmlTree::Container &childrenTriangle = doc.getChild("Triangles").getChildren();
		for (auto it = childrenTriangle.begin(); it != childrenTriangle.end(); ++it){
			int id0 = (*it)->getChild("index0").getValue<int>();
			int id1 = (*it)->getChild("index1").getValue<int>();
			int id2 = (*it)->getChild("index2").getValue<int>();

			float cr = (*it)->getChild("colorR").getValue<float>();
			float cg = (*it)->getChild("colorG").getValue<float>();
			float cb = (*it)->getChild("colorB").getValue<float>();
			float ca = (*it)->getChild("colorA").getValue<float>();

			TriangleVertex * vert = new TriangleVertex(id0, id1, id2);
			vert->setVert0(physics->getParticle(id0)->mPos);
			vert->setVert1(physics->getParticle(id1)->mPos);
			vert->setVert2(physics->getParticle(id2)->mPos);
			vert->setColor(ci::ColorA(cr, cg, cb, ca));
			mTriangles.push_back(vert);
		}
		numTriangles = mTriangles.size();

	}
	catch (XmlTree::Exception e) {
		console() << "ERROR: loading/reading configuration file." <<
			e.what() << std::endl;
		exit(0);
	}
}


void ParticleTriangleApp::saveParticleXML()
{
	console() << "SAVING XML" << std::endl;
	std::string beginXmlStr("<?xml version=\"1.0\" encoding = \"UTF-8\" ?>");
	XmlTree doc(beginXmlStr);

	XmlTree particles("Particles", "");
	//65 puntos
	int maxParticlesMesh = 57;
	for (int i = 0; i < maxParticlesMesh; i++){
		traer::physics::Particle * p = physics->getParticle(i);
		XmlTree generalNode;
		generalNode.setTag("Particle");
		generalNode.push_back(XmlTree("X", toString(p->mPos.x)));
		generalNode.push_back(XmlTree("Y", toString(p->mPos.y)));

		generalNode.push_back(XmlTree("colorR", toString(p->mCol.r)));
		generalNode.push_back(XmlTree("colorG", toString(p->mCol.g)));
		generalNode.push_back(XmlTree("colorB", toString(p->mCol.b)));
		generalNode.push_back(XmlTree("colorA", toString(p->mCol.a)));

		particles.push_back(generalNode);
	}

	XmlTree aethers("Aether", "");
	SurfaceColor * colSurface = mManagerColors.getSurfaceColor(colorSurfaceIndex);
	for (int i = maxParticlesMesh; i < physics->numberOfParticles(); i++){
		traer::physics::Particle * p = physics->getParticle(i);
		ci::ColorA col = colSurface->getColorPixel(ci::Vec2i(p->mPos.x, p->mPos.y));
		XmlTree generalNode;
		generalNode.setTag("Particle");
		generalNode.push_back(XmlTree("X", toString(p->mPos.x)));
		generalNode.push_back(XmlTree("Y", toString(p->mPos.y)));

		generalNode.push_back(XmlTree("colorR", toString(col.r)));
		generalNode.push_back(XmlTree("colorG", toString(col.g)));
		generalNode.push_back(XmlTree("colorB", toString(col.b)));
		generalNode.push_back(XmlTree("colorA", toString(col.a)));

		aethers.push_back(generalNode);
	}


	XmlTree triangles("Triangles", "");
	for (auto it = mTriangles.begin(); it != mTriangles.end(); ++it){

		int indexVert0 = (*it)->getIndexVert0();
		int indexVert1 = (*it)->getIndexVert1();
		int indexVert2 = (*it)->getIndexVert2();

		ci::ColorA col = (*it)->getColor();

		XmlTree generalNode;
		generalNode.setTag("Triangle");
		generalNode.push_back(XmlTree("index0", toString(indexVert0)));
		generalNode.push_back(XmlTree("index1", toString(indexVert1)));
		generalNode.push_back(XmlTree("index2", toString(indexVert2)));

		generalNode.push_back(XmlTree("colorR", toString(col.r)));
		generalNode.push_back(XmlTree("colorG", toString(col.g)));
		generalNode.push_back(XmlTree("colorB", toString(col.b)));
		generalNode.push_back(XmlTree("colorA", toString(col.a)));

		triangles.push_back(generalNode);
	}

	doc.push_back(particles);
	doc.push_back(aethers);
	doc.push_back(triangles);

	doc.write(writeFile(getAppPath() / "data" / fs::path("particles.xml")));

}

CINDER_APP_BASIC( ParticleTriangleApp, RendererGl )
