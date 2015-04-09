void ParticleTriangleApp::setupOSC()
{
	listener.setup(32000);
}

void ParticleTriangleApp::updateOSC()
{
	while (listener.hasWaitingMessages()) {
		ci::osc::Message message;
		listener.getNextMessage(&message);

		///console() << "New message received" << std::endl;
		//console() << "Address: " << message.getAddress() << std::endl;
		//console() << "Num Arg: " << message.getNumArgs() << std::endl;

		if (message.getAddress() == "/piezoL"){
			if (message.getArgType(0) == ci::osc::TYPE_INT32){
				try {
					console() << " LEFT" << std::endl;

					if (message.getArgAsInt32(0) == 1){
						interactionLeftActivate = true;
						console() << "1" << std::endl;
					}

					if (message.getArgAsInt32(0) == 0){
						console() << "0" << std::endl;
						interactionLeftActivate = false;
					}
				}
				catch (...) {
					console() << "Exception reading argument as float" << std::endl;
				}
			}
		}


		if (message.getAddress() == "/piezoR"){
			console() << "RIGHT " << std::endl;
			if (message.getArgType(0) == ci::osc::TYPE_INT32){
				try {
					console() << "RIGHT " << std::endl;

					if (message.getArgAsInt32(0) == 1){
						interactionRightActivate = true;
					}

					if (message.getArgAsInt32(0) == 0){
						interactionRightActivate = false;
					}

				}
				catch (...) {
					console() << "Exception reading argument as float" << std::endl;
				}
			}
		}

	}
}