

void ParticleTriangleApp::activateEvents()
{
	if (activateEvent){
		switch (eventType){
		//------TRIANGLE PARTICLES
		case 0: //CHANGE COLORS INTERPOLATE
			mRenderMode = 0;
			bloomFactor = 0.2;
			drawDelaunay	  = false;
			drawFullWireFrame = true;
			blendMesh		  = true;
			drawMesh		  = true;
			alphaFrameWire    = 1.0;
			colorWireFrame.a = 0.1;
			interactionTypeAether = 5;
			break;

		case 1: //1  //interpolated colors 12
			bloomFactor = 0.2;
			drawDelaunay = true;
			drawFullWireFrame = false;
			blendMesh = true;
			drawMesh = true;
			alphaFrameWire = 1.0;
			bloomFactor    = 0.2;
			mRenderMode    = 0;
			interactionTypeAether = 5;
			break;

		case 2: //2  CHANGE WIRE FRAME

			drawDelaunay = true;
			drawFullWireFrame = false;
			blendMesh = true;
			drawMesh = true;
			alphaFrameWire = 0.3;
			bloomFactor    = 0.2;
			mRenderMode = 0;
			interactionTypeAether = 5;
			break;

		case 3: //3  ONLY WIRE FRAME
			toggleWireframe		= true;
			drawDelaunay		= true;
			drawFullWireFrame	= true;
			blendMesh			= false;
			drawMesh			= true;
			alphaFrameWire		= 1.0;
			bloomFactor			= 1.85;
			mRenderMode			= 0;
			colorWireFrame.a	= 1;
			interactionTypeAether = 5;
			break;

		case 4: // MOZAICOS
			toggleWireframe		= false;
			drawDelaunay		= true;
			drawFullWireFrame   = true;
			blendMesh		    = false;
			drawMesh			= true;
			alphaFrameWire		= 0.5;
			bloomFactor			= .04;
			mRenderMode			= 0;
			colorWireFrame.a	= 1;
			interactionTypeAether = 5;
			break;

		//------ AETHER 
		/*
		INTERACCION 10 puntos de EXPLOSION
		*/
		case 5: //5
			activateWireFrame = true;
			mRenderMode = 1;
			bloomFactor = 1.2;
			particleMode = 1;
			interactionTypeAether = 1;
			//addAttractionForce = false;
			break;

		case 6: //6
			activateWireFrame = false;
			activateBlend = false;
			bloomFactor = .7;
			mRenderMode = 1;
			particleMode = 0;
			interactionTypeAether = 1;
			break;

		case 7: //7
			activateWireFrame = false;
			activateBlend = true;
			bloomFactor = .7;
			mRenderMode = 1;
			interactionTypeAether = 1;
			break;

		//MOVIE
		case 10: //10
			//mMovie->reset();
			//mMovie->play();
			mRenderMode = 2;
			break;

		}
		activateEvent = false;
	}


	if (eventType == 0 ){

		if (readyColorCounter >= 57){
			if (getElapsedFrames() % 80 ==0){
				colorSurfaceIndex++;
				if (colorSurfaceIndex >= 12)
					colorSurfaceIndex = 0;
				colorSurfaceToggle = true;
				readyColorCounter = 0;
			}
		}
		
	}

	if(eventType == 3){
		if (getElapsedFrames() % 400 == 0){
			colorSurfaceIndex++;
			if (colorSurfaceIndex >= 11)
				colorSurfaceIndex = 0;
			colorSurfaceToggle = true;
			readyColorCounter = 0;
			console() << "changed " << colorSurfaceIndex << std::endl;
			timeColor = 0;
		}
	}

	if (eventType == 1|| eventType == 3 || eventType == 4){
		
		if (timeColor <= 1)
			timeColor += 0.005;
		else{
			if (getElapsedFrames() % 400 == 0){
				colorSurfaceIndex++;
				if (colorSurfaceIndex >= 11)
					colorSurfaceIndex = 0;
				colorSurfaceToggle = true;
				readyColorCounter = 0;
				console() << "changed " << colorSurfaceIndex << std::endl;
				timeColor = 0;
			}
		}
	}

}