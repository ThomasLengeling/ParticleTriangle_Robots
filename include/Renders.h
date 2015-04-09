


void ParticleTriangleApp::drawRender()
{
	if (mRenderMode == 0){  //TRIANGLES
		gl::pushModelView();
		gl::color(ci::Color::white());
		gl::draw(mFboSceneWire.getTexture(), getWindowBounds());

		gl::enableAdditiveBlending();
		gl::draw(mFboBlur2.getTexture(), getWindowBounds());
		gl::disableAlphaBlending();
		gl::popModelView();

		if (drawFullWireFrame){
			for (int i = 0; i < physics->numberOfSprings(); i++){
				Spring *strp = physics->getSpring(i);
				traer::physics::Particle * start = strp->getOneEnd();
				traer::physics::Particle * end = strp->getTheOtherEnd();
				gl::color(colorWireFrame);
				gl::lineWidth(lineWidth);
				gl::begin(GL_LINES);
				gl::vertex(start->mPos);
				gl::vertex(end->mPos);
				gl::end();
			}
		}
	}
	else if (mRenderMode == 1){ //AETHER

		gl::clear(bkg);
		drawWarpArea();

	}
	else if (mRenderMode == 2){ //MOVIE
		gl::clear(bkg);
		drawWarpArea();
	}

}


void ParticleTriangleApp::updateRender()
{

	if (mRenderMode == 0){  //TRIANGLES
		numParticles = physics->numberOfParticles();
		numTriangles = mTriangles.size();

		if (colorSurfaceToggle){
			SurfaceColor * colSurface = mManagerColors.getSurfaceColor(colorSurfaceIndex);

			for (int i = 0; i < physics->numberOfParticles(); i++){
				traer::physics::Particle * p = physics->getParticle(i);
				ci::ColorA col = colSurface->getColorPixel(ci::Vec2i(p->mPos.x, p->mPos.y));
				//p->mCol = col;
				p->mColNext = col;
				p->resetColorTimer();
			}
			colorSurfaceToggle = false;



		}

		if (numParticles > 0){
			for (int i = 0; i < NUMPARTICLES_TRI; i++){
				traer::physics::Particle * p = physics->getParticle(i);
				p->interpolateNexColor();
			}
		}

		if (updatePhysics){
			physics->tick();
		}
		offScreenRender();

		//TEMBLOR
		activateTemblor();
		activateRotate();

		if (interactionTypeAether == 5){
			if (interactionLeftActivate){
				resetTemblor();
				resetRotate();
			}

			if (interactionRightActivate){
				resetTemblor();
				resetRotate();
			}
		}
	}
	else if (mRenderMode == 1){ //AETHER
		offScreenRender();
		updateParticlesAether();
	}
	else if (mRenderMode == 2){ //MOVIE
		if (changeMovie == 0){
			if (mMovie)
				mFrameTexture = mMovie->getTexture();
		}

		if (changeMovie){
			if (mMovie2)
				mFrameTexture2 = mMovie2->getTexture();
		}
	}

	if (mPrevRenderMode != mRenderMode){
		if (mRenderMode == 1)
			mUseBeginEnd = true;
		if (mRenderMode == 2)
			mUseBeginEnd = false;
		mPrevRenderMode = mRenderMode;
	}
}


void ParticleTriangleApp::offScreenRender()
{
	float blur = 4.0 / getWindowWidth();
	Area  viewport = gl::getViewport();

	//PARTICLES FBO
	mFboScene.bindFramebuffer();
	gl::setViewport(mFboScene.getBounds());
	gl::enableAlphaBlending();
	gl::color(bkg);
	gl::drawSolidRect(ci::Rectf(0, 0, getWindowWidth(), getWindowHeight()));
	if (mRenderMode == 0)
		drawTraerParticles();
	else if (mRenderMode == 1)
		drawAetherParticles();
	mFboScene.unbindFramebuffer();


	//FRAME WIRE FBO
	mFboWire.bindFramebuffer();
	gl::setViewport(mFboWire.getBounds());
	gl::enableAlphaBlending();
	gl::color(bkg);
	gl::drawSolidRect(ci::Rectf(0, 0, getWindowWidth(), getWindowHeight()));
	if (mRenderMode == 0)
		updateWireFrame(mMeshDelaunay);
	else if (mRenderMode == 1)
		updateWireFrame(mTriMeshTessellation);
	mFboWire.unbindFramebuffer();


	//SHADER FRAME WIRE
	mFboSceneWire.bindFramebuffer();
	mShaderMixTexture.bind();
	mFboScene.bindTexture(0);
	mFboWire.bindTexture(1);
	mShaderMixTexture.uniform("texScene", 0);
	mShaderMixTexture.uniform("texWire", 1);
	mShaderMixTexture.uniform("alphaFrameWire", alphaFrameWire);
	gl::drawSolidRect(getWindowBounds());
	mFboWire.unbindTexture();
	mFboScene.unbindTexture();
	mShaderMixTexture.unbind();
	mFboSceneWire.unbindFramebuffer();
	//BLUR
	mShaderBlur.bind();
	mShaderBlur.uniform("tex0", 0);
	mShaderBlur.uniform("sample_offset", Vec2f(1.0f / mFboBlur1.getWidth(), 0.0f));
	mShaderBlur.uniform("attenuation", bloomFactor);

	mFboBlur1.bindFramebuffer();
	mFboSceneWire.bindTexture(0);
	gl::setViewport(mFboBlur1.getBounds());
	gl::clear(ci::Color::black());
	gl::drawSolidRect(getWindowBounds());
	mFboSceneWire.unbindTexture();
	mFboBlur1.unbindFramebuffer();

	mShaderBlur.uniform("sample_offset", Vec2f(0.0f, 1.0f / mFboBlur2.getHeight()));
	mShaderBlur.uniform("attenuation", bloomFactor); //1.2f

	mFboBlur2.bindFramebuffer();
	mFboBlur1.bindTexture(0);
	gl::setViewport(mFboBlur2.getBounds());
	gl::clear(ci::Color::black());
	gl::drawSolidRect(getWindowBounds());
	mFboBlur1.unbindTexture();
	mFboBlur2.unbindFramebuffer();

	mShaderBlur.unbind();

	gl::setViewport(viewport);
}

void ParticleTriangleApp::updateWireFrame(ci::TriMesh mesh)
{
	mShaderWireFrame.bind();
	mShaderWireFrame.uniform("uViewportSize", Vec2f(getWindowSize()));
	gl::draw(mesh);
	mShaderWireFrame.unbind();
}
