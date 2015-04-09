

void	ParticleTriangleApp::setupParticlesAether()
{
	int binPower = 4;

	particleSystem.setup(getWindowWidth(), getWindowHeight(), binPower);

	//colorPictureBR = loadImage(loadResource(IMAGE_BR));
	console() << "setting up" << std::endl;
	float padding = 0;
	float maxVelocity = .02;  //.1
	for (int i = 0; i < kParticles * 1024; i++) {
		float x = Rand::randFloat(padding, getWindowWidth() - padding);
		float y = Rand::randFloat(padding, getWindowHeight() - padding);
		float xv = Rand::randFloat(-maxVelocity, maxVelocity) + 0.01;
		float yv = Rand::randFloat(-maxVelocity, maxVelocity) + 0.01;
		float rad = Rand::randFloat(3.5f, 7.0f);

		ci::ColorA c = ci::Color::white();
		ci::ColorA cv = ci::Color::white();

		Bit::Particle particle(x, y, xv, yv);

		particle.setParticleColor(c);
		particle.setParticleColorVel(cv);

		particle.setParticleRadius(rad);
		particleSystem.add(particle);
	}

	console() << "DONE CREATING BINS PARTICLES" << std::endl;

	particleSystem.updateSize();

	timeStep = 1.0;
	lineOpacity = 0.12f;
	pointOpacity = 0.5f;
	particleNeighborhood = 22;
	particleRepulsion = 1.05;


	centerAttraction = .04;

	console() << "PARTICLES VARIABLES" << std::endl;
	//INIT VALUES

	forceInteractionLeft.x = 290;
	forceInteractionLeft.y = getWindowHeight() - 250;


	forceInteractionRight.x = getWindowWidth() - 330;
	forceInteractionRight.y = getWindowHeight() - 250;

	interactionLeftActivate = false;
	interactionRightActivate = false;

	interactionTypeAether = 0;

	//NOISE CENTER
	mSeed = clock() & 65535;
	mOctaves = 8;
	mTime = 0.0f;
	mFrequency = 1 / 200.0f;
}



void ParticleTriangleApp::updateParticlesAether()
{

	particleSystem.setupForces();

	particleSystem.setCurrColor(particleBkgCol);
	particleSystem.setVelColor(particleVelCol);

	particleSystem.setTimeStep(timeStep);

	if (colorSelectorPrev != colorSelector)
		particleSystem.setNewColorsMesh(*mManagerColors.getSurfaceColor(colorSelector));
	colorSelectorPrev = colorSelector;

	if (activateRadius != activateRadiusPrev)
		particleSystem.setNewParticleRadius(activateRadius);
	activateRadiusPrev = activateRadius;

	if (activateBlend != activateBlendPrev)
		particleSystem.blendMesh();
	activateBlendPrev = activateBlend;

	for (int i = 0; i < particleSystem.size(); i++) {
		Bit::Particle& cur = particleSystem[i];
		particleSystem.addRepulsionForce(cur, particleNeighborhood, particleRepulsion);
		cur.addDampingForce();
	}
	//glEnd();

	if (addAttractionForce)
		particleSystem.addAttractionForce(getWindowWidth() / 2, getWindowHeight() / 2, getWindowWidth(), centerAttraction);

	//if (isMousePressed)
	//	particleSystem.addRepulsionForce(mouse.x, mouse.y, 300, 40);

	//INTERACTION
	if (interactionTypeAether == 0){
		if (interactionLeftActivate){
			particleSystem.addRepulsionForce(forceInteractionLeft.x, forceInteractionLeft.y, 380, 40);
		}

		if (interactionRightActivate){
			particleSystem.addRepulsionForce(forceInteractionRight.x, forceInteractionRight.y, 380, 40);
		}
	}
	else if (interactionTypeAether == 1){
		if (interactionLeftActivate){
			particleSystem.addAttractionForce(forceInteractionLeft.x, forceInteractionLeft.y, 470, .28);
		}

		if (interactionRightActivate){
			particleSystem.addAttractionForce(forceInteractionRight.x, forceInteractionRight.y, 470, .28);
		}
	}
	else if (interactionTypeAether == 2){
		if (interactionLeftActivate){
			particleSystem.addRepulsionForce(forceInteractionLeft.x, forceInteractionLeft.y, 150, 40);
		}

		if (interactionRightActivate){
			particleSystem.addRepulsionForce(forceInteractionRight.x, forceInteractionRight.y, 150, 40);
		}
	}


	if (activateIncCenter){
		mPerlin = Perlin(mOctaves, mSeed);
		float v = (mPerlin.fBm(Vec3f(getWindowWidth() / 2.0, getWindowHeight() / 2.0, mTime) * mFrequency) + 1.0f) / 17.0f - 0.02;
		//console() << "Perling " << v << std::endl;
		mTime += 10.0f;

		centerAttraction = v;
	}

	particleSystem.update();

}

void ParticleTriangleApp::drawAetherParticles()
{
	if (calculateMesh){
		mTriMeshTessellation.clear();
		mTriMeshTessellation = particleSystem.calculateMesh();
		if (activateWireFrame)
			gl::enableWireframe();
		gl::draw(mTriMeshTessellation);
		if (activateWireFrame)
			gl::disableWireframe();
	}

	drawAetherParticleMode();

//	gl::color(0, 1, 0);
//	gl::drawSolidEllipse(forceInteractionRight, 30, 30);
//	gl::drawSolidEllipse(forceInteractionLeft, 30, 30);

	gl::disableAlphaBlending();
}

void ParticleTriangleApp::drawAetherParticleMode()
{
	switch (particleMode)
	{
	case 0:
		particleSystem.draw();
		break;
	case 1:
		particleSystem.drawOnlyVel();
		break;
	case 2:
		particleSystem.drawVelLog();
		break;
	case 3:
		particleSystem.drawBackground();
		break;
	case 4:
		particleSystem.drawVelLogBackground();
		break;
	default:
		break;
	}
}