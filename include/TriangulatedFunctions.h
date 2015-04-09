//---------PARTICLES----------
void ParticleTriangleApp::drawTraerParticles()
{
	gl::pushMatrices();
	//gl::translate(ci::Vec2f(mTranslate.x, mTranslate.y));
	if (toggleSculpture){
		gl::pushMatrices();
		gl::color(1.0, 1.0, 1.0, 0.2);
		gl::translate(ci::Vec2f(480, 30));
		gl::scale(ci::Vec2f(0.68, 0.68));
		gl::draw(mUnnamed);
		gl::popMatrices();
	}

	if (toggleWireframe){
		gl::enableWireframe();
	}
	else
		gl::disableWireframe();

	if (drawMesh){
		mMeshTriangles = generateTraerMeshTriangle();
		gl::draw(mMeshTriangles);
	}

	if (drawDelaunay){
		mMeshDelaunay = generateTraerMeshDelaunay();
		gl::draw(mMeshDelaunay);
	}

	if (drawFullWireFrame){

	}


	if (debugParticles){
		for (int i = 0; i < physics->numberOfParticles(); i++){
			traer::physics::Particle * p = physics->getParticle(i);
			gl::color(p->mCol);

			p->mPos.x = (p->mPos.x + mTranslate.x)* scaleFactor;
			p->mPos.y = (p->mPos.y + mTranslate.y)* scaleFactor;
			gl::drawSolidCircle(p->mPos.xy(), 5);
			gl::color(ci::ColorA(0.0, 0.0, 0.0, 1.0));
			gl::drawStrokedCircle(p->mPos.xy(), 5);
			//std::stringstream xvals; xvals << p->mPos.x;
			//std::stringstream yvals; yvals << p->mPos.y;

			std::stringstream id; id << i;
			gl::color(1, 0, 0, 1);
			mTextureFont->drawString("" + id.str(), Vec2f(p->mPos.x - 7, p->mPos.y - 7));
		}

		int tri = 0;
		for (auto it = mTriangles.begin(); it != mTriangles.end(); ++it){
			//if (tri == 81)
			//	continue;
			int indexVert0 = (*it)->getIndexVert0();
			int indexVert1 = (*it)->getIndexVert1();
			int indexVert2 = (*it)->getIndexVert2();

			if (debugParticles){

				ci::ColorA colr = (*it)->getColor();

				traer::physics::Particle * p0 = physics->getParticle(indexVert0);
				traer::physics::Particle * p1 = physics->getParticle(indexVert1);
				traer::physics::Particle * p2 = physics->getParticle(indexVert2);

				float x = (p0->mPos.x + p1->mPos.x + p2->mPos.x) / 3.0;
				float y = (p0->mPos.y + p1->mPos.y + p2->mPos.y) / 3.0;

				std::stringstream id; id << tri;
				gl::color(1, 1, 1, 1);
				mTextureFont->drawString("" + id.str(), Vec2f(x, y));
			}
			tri++;
		}

	}
	/*
	for (int i = 0; i < physics->numberOfSprings(); i++){
	Spring *strp = physics->getSpring(i);
	Particle * start = strp->getOneEnd();
	Particle * end = strp->getTheOtherEnd();
	gl::color(1, 1, 1, 0.5);
	gl::lineWidth(2);
	gl::begin(GL_LINES);
	gl::vertex(start->mPos);
	gl::vertex(end->mPos);
	gl::end();
	}

	*/


	//}

	gl::lineWidth(1);
	gl::disableWireframe();
	gl::disableAlphaBlending();

	gl::popMatrices();
}

ci::TriMesh ParticleTriangleApp::generateTraerMeshTriangle()
{
	ci::TriMesh mMeshTriangles;

	if (colorParticle)
		colorPicker = colorPickerTemp;

	SurfaceColor * colSurface = mManagerColors.getSurfaceColor(colorSurfaceIndex);

	readyColorCounter = 0;
	for (int i = 0; i < physics->numberOfParticles(); i++){
		traer::physics::Particle * p = physics->getParticle(i);
		ci::ColorA col = colSurface->getColorPixel(ci::Vec2i(p->mPos.x, p->mPos.y));

		//ci::ColorA col = p->mColNew;// p->mCol;
		//p->mCol = col;
		//float Y = 0.2126 * col.r + 0.7152 *col.g + 0.0722 * col.b;
		mMeshTriangles.appendVertex(ci::Vec3f(p->mPos.x, p->mPos.y, 0));
		mMeshTriangles.appendColorRgba(col);

		if (i == indexColorPicker){
			p->mCol = colorPicker;
			colorPickerTemp = colorPicker;
		}

		if (p->readyColor)
			readyColorCounter++;
	}

	int indexTri = 0;
	for (auto it = mTriangles.begin(); it != mTriangles.end(); ++it){

		TriangleVertex * tri = *it;

		int indexVert0 = tri->getIndexVert0();
		int indexVert1 = tri->getIndexVert1();
		int indexVert2 = tri->getIndexVert2();
		mMeshTriangles.appendTriangle(indexVert0, indexVert1, indexVert2);
	}


	return mMeshTriangles;
}

ci::TriMesh ParticleTriangleApp::generateTraerMeshDelaunay()
{
	ci::TriMesh mMeshDelaunay;

	SurfaceColor * colSurface = mManagerColors.getSurfaceColor(colorSurfaceIndex);
	SurfaceColor * colSurfaceNext = mManagerColors.getSurfaceColor(colorSurfaceIndex +1);

	readyPosCounter = 0;
	for (int i = 0; i < physics->numberOfParticles(); i++){
		traer::physics::Particle * p = physics->getParticle(i);
		ci::ColorA col = p->mColNew;
		mMeshDelaunay.appendVertex(ci::Vec3f(p->mPos.x, p->mPos.y, 0));
		mMeshDelaunay.appendColorRgba(col);

		if (changePosInside)
			p->resetPosTimer();

		if (p->readyPos)
			readyPosCounter++;
	}

	if (changePosInside){
		changePosInOut = !changePosInOut;
		console() << "flip" << std::endl;
		changePosInside = false;
		counterFlip = 0;
	}

	if (readyPosCounter >= 220){
		if (counterFlip > 300){
			console() << "Ready flip" << std::endl;
			changePosInside = true;
		}
		counterFlip++;
	}

	int indexTri = 0;
	GEOM_FADE2D::Fade_2D * dt = new GEOM_FADE2D::Fade_2D(6);
	
	for (auto it = mTriangles.begin(); it != mTriangles.end(); ++it){
		std::vector<GEOM_FADE2D::Point2> vInputPoints;

		TriangleVertex * tri = *it;

		int indexVert0 = tri->getIndexVert0();
		int indexVert1 = tri->getIndexVert1();
		int indexVert2 = tri->getIndexVert2();

		traer::physics::Particle * p0 = physics->getParticle(indexVert0);
		traer::physics::Particle * p1 = physics->getParticle(indexVert1);
		traer::physics::Particle * p2 = physics->getParticle(indexVert2);

		vInputPoints.push_back(GEOM_FADE2D::Point2(p0->mPos.x, p0->mPos.y));
		vInputPoints.push_back(GEOM_FADE2D::Point2(p1->mPos.x, p1->mPos.y));
		vInputPoints.push_back(GEOM_FADE2D::Point2(p2->mPos.x, p2->mPos.y));

		if (tri->getVertexIndex().size() == 0){

			mMeshDelaunay.appendTriangle(indexVert0, indexVert1, indexVert2);
		}

		//if (indexTri < INDEXDELAUNAY){ //33
		if (tri->getVertexIndex().size() > 0){
			if (indexTri == 64 || indexTri == 63 || indexTri == 70 || indexTri == 68 || indexTri == 69 || indexTri == 70){
				std::vector<GEOM_FADE2D::Point2> inputPoints;
				int iTri = 0;
				for (auto itr = tri->getVertexIndex().begin(); itr != tri->getVertexIndex().end(); ++itr){
					int index = *itr;
					traer::physics::Particle * parE = physics->getParticle(index);
					inputPoints.push_back(GEOM_FADE2D::Point2(parE->mPos.x, parE->mPos.y));
					//.push_back(GEOM_FADE2D::Point2(parE->mPosCurr.x, parE->mPosCurr.y));
					iTri++;
				}

				dt->insert(inputPoints);
				std::vector<GEOM_FADE2D::Segment2> vSegments;

				//FILL THE POINTS WITH SEGMENTS
				for (auto vTri = vInputPoints.begin(); vTri != vInputPoints.end() - 1; ++vTri){
					GEOM_FADE2D::Segment2 seg(*vTri, *(vTri + 1));
					vSegments.push_back(seg);
				}
				vSegments.push_back(GEOM_FADE2D::Segment2(vInputPoints.back(), vInputPoints.front()));

				GEOM_FADE2D::ConstraintGraph2* pConstraint = dt->createConstraint(vSegments, GEOM_FADE2D::CIS_IGNORE_DELAUNAY);
				GEOM_FADE2D::Zone2* pZone = dt->createZone(pConstraint, GEOM_FADE2D::ZL_INSIDE);
				dt->applyConstraintsAndZones();

				std::vector<GEOM_FADE2D::Triangle2*> vAllDelaunayTri;
				pZone->getTriangles(vAllDelaunayTri);

				/*
				dt->insert(vInputPoints);

				std::vector< GEOM_FADE2D::Triangle2 *> vAllDelaunayTri;
				dt->getTrianglePointers(vAllDelaunayTri);
				*/

				if (blendMesh){
					for (auto it = vAllDelaunayTri.begin(); it != vAllDelaunayTri.end(); ++it){
						GEOM_FADE2D::Point2 * p1 = (*it)->getCorner(0);
						GEOM_FADE2D::Point2 * p2 = (*it)->getCorner(1);
						GEOM_FADE2D::Point2 * p3 = (*it)->getCorner(2);

						ci::ColorA col = colSurface->getColorPixel(ci::Vec2i((int)p1->x(), (int)p1->y()));
						ci::ColorA colN = colSurfaceNext->getColorPixel(ci::Vec2i((int)p1->x(), (int)p1->y()));

						ci::ColorA colNew = timeColor *(colN - col) + col;
						mMeshDelaunay.appendVertex(ci::Vec3f(p1->x(), p1->y(), 0));
						mMeshDelaunay.appendColorRgba(colNew);

						col = colSurface->getColorPixel(ci::Vec2i((int)p2->x(), (int)p2->y()));
						colN = colSurfaceNext->getColorPixel(ci::Vec2i((int)p2->x(), (int)p2->y()));

						colNew = timeColor *(colN - col) + col;
						col = colSurface->getColorPixel(ci::Vec2i((int)p2->x(), (int)p2->y()));
						mMeshDelaunay.appendVertex(ci::Vec3f(p2->x(), p2->y(), 0));
						mMeshDelaunay.appendColorRgba(colNew);

						col = colSurface->getColorPixel(ci::Vec2i((int)p3->x(), (int)p3->y()));
						colN = colSurfaceNext->getColorPixel(ci::Vec2i((int)p3->x(), (int)p3->y()));
						colNew = timeColor *(colN - col) + col;

						col = colSurface->getColorPixel(ci::Vec2i((int)p3->x(), (int)p3->y()));
						mMeshDelaunay.appendVertex(ci::Vec3f(p3->x(), p3->y(), 0));
						mMeshDelaunay.appendColorRgba(colNew);


						mMeshDelaunay.appendTriangle(mMeshDelaunay.getVertices().size() - 3, mMeshDelaunay.getVertices().size() - 2, mMeshDelaunay.getVertices().size() - 1);
					}
				}
				else{
					for (auto it = vAllDelaunayTri.begin(); it != vAllDelaunayTri.end(); ++it){
						GEOM_FADE2D::Point2 * p1 = (*it)->getCorner(0);
						GEOM_FADE2D::Point2 * p2 = (*it)->getCorner(1);
						GEOM_FADE2D::Point2 * p3 = (*it)->getCorner(2);

						ci::Vec2i center = ci::Vec2i((int)p1->x(), (int)p1->y()) / 3.0f +
							ci::Vec2i((int)p2->x(), (int)p2->y()) / 3.0f +
							ci::Vec2i((int)p3->x(), (int)p3->y()) / 3.0f;


						ci::ColorA col = colSurface->getColorPixel(center);
						ci::ColorA colN = colSurfaceNext->getColorPixel(center);
						ci::ColorA colNew = timeColor *(colN - col) + col;

						mMeshDelaunay.appendVertex(ci::Vec3f(p1->x(), p1->y(), 0));
						mMeshDelaunay.appendColorRgba(colNew);
						mMeshDelaunay.appendVertex(ci::Vec3f(p2->x(), p2->y(), 0));
						mMeshDelaunay.appendColorRgba(colNew);
						mMeshDelaunay.appendVertex(ci::Vec3f(p3->x(), p3->y(), 0));
						mMeshDelaunay.appendColorRgba(colNew);

						mMeshDelaunay.appendTriangle(mMeshDelaunay.getVertices().size() - 3, mMeshDelaunay.getVertices().size() - 2, mMeshDelaunay.getVertices().size() - 1);
					}
				}
				vAllDelaunayTri.clear();
			}
			else{
				std::vector<GEOM_FADE2D::Point2> inputPoints;
				int iTri = 0;
				for (auto itr = tri->getVertexIndex().begin(); itr != tri->getVertexIndex().end(); ++itr){
					int index = *itr;
					traer::physics::Particle * parE = physics->getParticle(index);

					if (iTri == 0){
						parE->mPosFinal = p0->mPos;
					}
					if (iTri == 1){
						parE->mPosFinal = p1->mPos;
					}
					if (iTri == 2){
						parE->mPosFinal = p2->mPos;
					}

					if (!changePosInOut)
						parE->interpolateNextPosIn();
					else
						parE->interpolateNextPosOut();
					//inputPoints.push_back(GEOM_FADE2D::Point2(parE->mPos.x, parE->mPos.y));
					inputPoints.push_back(GEOM_FADE2D::Point2(parE->mPosCurr.x, parE->mPosCurr.y));
					iTri++;
				}

				dt->insert(inputPoints);
				std::vector<GEOM_FADE2D::Segment2> vSegments;

				//FILL THE POINTS WITH SEGMENTS
				for (auto vTri = vInputPoints.begin(); vTri != vInputPoints.end() - 1; ++vTri){
					GEOM_FADE2D::Segment2 seg(*vTri, *(vTri + 1));
					vSegments.push_back(seg);
				}
				vSegments.push_back(GEOM_FADE2D::Segment2(vInputPoints.back(), vInputPoints.front()));

				GEOM_FADE2D::ConstraintGraph2* pConstraint = dt->createConstraint(vSegments, GEOM_FADE2D::CIS_IGNORE_DELAUNAY);
				GEOM_FADE2D::Zone2* pZone = dt->createZone(pConstraint, GEOM_FADE2D::ZL_INSIDE);
				dt->applyConstraintsAndZones();

				std::vector<GEOM_FADE2D::Triangle2*> vAllDelaunayTri;
				pZone->getTriangles(vAllDelaunayTri);

				/*
				dt->insert(vInputPoints);

				std::vector< GEOM_FADE2D::Triangle2 *> vAllDelaunayTri;
				dt->getTrianglePointers(vAllDelaunayTri);
				*/

				if (blendMesh){
					for (auto it = vAllDelaunayTri.begin(); it != vAllDelaunayTri.end(); ++it){
						GEOM_FADE2D::Point2 * p1 = (*it)->getCorner(0);
						GEOM_FADE2D::Point2 * p2 = (*it)->getCorner(1);
						GEOM_FADE2D::Point2 * p3 = (*it)->getCorner(2);

						ci::ColorA col = colSurface->getColorPixel(ci::Vec2i((int)p1->x(), (int)p1->y()));
						ci::ColorA colN = colSurfaceNext->getColorPixel(ci::Vec2i((int)p1->x(), (int)p1->y()));

						ci::ColorA colNew = timeColor *(colN - col) + col;
						mMeshDelaunay.appendVertex(ci::Vec3f(p1->x(), p1->y(), 0));
						mMeshDelaunay.appendColorRgba(colNew);

						col = colSurface->getColorPixel(ci::Vec2i((int)p2->x(), (int)p2->y()));
						colN = colSurfaceNext->getColorPixel(ci::Vec2i((int)p2->x(), (int)p2->y()));

						colNew = timeColor *(colN - col) + col;
						col = colSurface->getColorPixel(ci::Vec2i((int)p2->x(), (int)p2->y()));
						mMeshDelaunay.appendVertex(ci::Vec3f(p2->x(), p2->y(), 0));
						mMeshDelaunay.appendColorRgba(colNew);

						col = colSurface->getColorPixel(ci::Vec2i((int)p3->x(), (int)p3->y()));
						colN = colSurfaceNext->getColorPixel(ci::Vec2i((int)p3->x(), (int)p3->y()));
						colNew = timeColor *(colN - col) + col;

						col = colSurface->getColorPixel(ci::Vec2i((int)p3->x(), (int)p3->y()));
						mMeshDelaunay.appendVertex(ci::Vec3f(p3->x(), p3->y(), 0));
						mMeshDelaunay.appendColorRgba(colNew);


						mMeshDelaunay.appendTriangle(mMeshDelaunay.getVertices().size() - 3, mMeshDelaunay.getVertices().size() - 2, mMeshDelaunay.getVertices().size() - 1);
					}
				}
				else{
					for (auto it = vAllDelaunayTri.begin(); it != vAllDelaunayTri.end(); ++it){
						GEOM_FADE2D::Point2 * p1 = (*it)->getCorner(0);
						GEOM_FADE2D::Point2 * p2 = (*it)->getCorner(1);
						GEOM_FADE2D::Point2 * p3 = (*it)->getCorner(2);

						ci::Vec2i center = ci::Vec2i((int)p1->x(), (int)p1->y()) / 3.0f +
							ci::Vec2i((int)p2->x(), (int)p2->y()) / 3.0f +
							ci::Vec2i((int)p3->x(), (int)p3->y()) / 3.0f;


						ci::ColorA col = colSurface->getColorPixel(center);
						ci::ColorA colN = colSurfaceNext->getColorPixel(center);
						ci::ColorA colNew = timeColor *(colN - col) + col;

						mMeshDelaunay.appendVertex(ci::Vec3f(p1->x(), p1->y(), 0));
						mMeshDelaunay.appendColorRgba(colNew);
						mMeshDelaunay.appendVertex(ci::Vec3f(p2->x(), p2->y(), 0));
						mMeshDelaunay.appendColorRgba(colNew);
						mMeshDelaunay.appendVertex(ci::Vec3f(p3->x(), p3->y(), 0));
						mMeshDelaunay.appendColorRgba(colNew);

						mMeshDelaunay.appendTriangle(mMeshDelaunay.getVertices().size() - 3, mMeshDelaunay.getVertices().size() - 2, mMeshDelaunay.getVertices().size() - 1);
					}
				}
				vAllDelaunayTri.clear();
			}
		}
		indexTri++;
	}


	delete dt;
	return mMeshDelaunay;
}



void ParticleTriangleApp::setupParticleSystemTraer()
{
	numParticles = 0;
	physics = new traer::physics::ParticleSystem(0.0f, 0.0002f);
	physics->clear();
	//loadParticlesXML();
	console() << "PARTICLE NUMBER" << numParticles << std::endl;
	numParticles = physics->numberOfParticles();
	numTriangles = mTriangles.size();

	//CREATE SPRINGS MESH
	/*
	//CONTOUR
	makeSpring(0, 1);
	makeSpring(1, 2);
	makeSpring(2, 3);
	makeSpring(3, 4);
	makeSpring(4, 5);
	makeSpring(5, 6);
	makeSpring(6, 7);
	makeSpring(7, 8);
	makeSpring(8, 9);
	makeSpring(10, 11);
	makeSpring(11, 12);
	makeSpring(12, 13);
	makeSpring(13, 14);
	makeSpring(14, 15);
	makeSpring(15, 16);
	makeSpring(16, 17);
	makeSpring(17, 18);
	makeSpring(18, 19);
	makeSpring(19, 20);
	makeSpring(20, 21);
	makeSpring(21, 22);
	makeSpring(22, 23);
	makeSpring(23, 24);
	makeSpring(24, 25);
	makeSpring(25, 26);
	makeSpring(26, 27);
	makeSpring(27, 0);

	//LINES
	makeSpring(0, 28);
	makeSpring(28, 29);
	makeSpring(29, 30);
	makeSpring(30, 31);
	makeSpring(31, 32);
	makeSpring(32, 33);
	makeSpring(33, 21);

	makeSpring(1, 34);
	makeSpring(34, 31);
	makeSpring(31, 36);
	makeSpring(36, 55);
	makeSpring(55, 33);
	makeSpring(33, 20);

	makeSpring(1, 37);
	makeSpring(37, 35);
	makeSpring(35, 36);
	makeSpring(36, 20);
	makeSpring(36, 19);

	makeSpring(4, 51);
	makeSpring(51, 44);
	makeSpring(44, 48);
	makeSpring(48, 49);
	makeSpring(49, 50);
	makeSpring(50, 14);
	makeSpring(50, 13);

	makeSpring(5, 52);
	makeSpring(52, 53);
	makeSpring(53, 54);
	makeSpring(54, 13);

	makeSpring(3, 46);
	makeSpring(46, 47);
	makeSpring(47, 41);
	makeSpring(41, 38);
	makeSpring(38, 45);
	makeSpring(45, 42);
	makeSpring(42, 56);
	makeSpring(56, 15);

	makeSpring(2, 40);
	makeSpring(40, 43);
	makeSpring(43, 38);
	makeSpring(38, 39);
	makeSpring(39, 17);
	makeSpring(39, 16);
	makeSpring(39, 42);
	makeSpring(42, 16);

	//VERTICAL
	makeSpring(27, 28);
	makeSpring(28, 34);
	makeSpring(34, 37);
	makeSpring(37, 2);
	makeSpring(40, 3);

	makeSpring(27, 29);
	makeSpring(29, 34);
	makeSpring(26, 29);

	makeSpring(26, 30);
	makeSpring(30, 34);
	makeSpring(34, 35);
	makeSpring(35, 31);
	makeSpring(35, 39);
	makeSpring(35, 38);
	makeSpring(35, 2);
	makeSpring(35, 40);
	makeSpring(35, 43);
	makeSpring(35, 18);
	makeSpring(36, 18);
	makeSpring(39, 18);

	makeSpring(43, 3);
	makeSpring(43, 46);
	makeSpring(43, 47);
	makeSpring(43, 41);

	makeSpring(46, 4);
	makeSpring(46, 51);
	makeSpring(47, 51);
	makeSpring(51, 5);
	makeSpring(51, 52);
	makeSpring(47, 44);
	makeSpring(41, 44);
	makeSpring(41, 48);
	makeSpring(48, 38);

	makeSpring(38, 42);
	makeSpring(45, 56);
	makeSpring(56, 14);
	makeSpring(45, 14);
	makeSpring(45, 48);
	makeSpring(45, 50);
	makeSpring(44, 52);
	makeSpring(44, 53);
	makeSpring(53, 48);
	makeSpring(54, 50);
	makeSpring(54, 48);

	makeSpring(7, 5);
	makeSpring(7, 8);
	makeSpring(5, 8);
	makeSpring(9, 10);
	makeSpring(8, 53);
	makeSpring(9, 53);
	makeSpring(10, 53);
	makeSpring(10, 54);
	makeSpring(11, 54);
	makeSpring(11, 13);

	makeSpring(16, 56);
	makeSpring(32, 55);
	makeSpring(31, 55);
	makeSpring(20, 55);

	makeSpring(25, 32);
	makeSpring(24, 32);
	makeSpring(23, 32);
	makeSpring(22, 32);
	makeSpring(22, 33);

	makeSpring(28, 1);
	makeSpring(25, 30);
	makeSpring(25, 31);
	*/
}


void ParticleTriangleApp::makeSpring(int index0, int index1)
{
	float dist = physics->getParticle(index0)->mPos.distance(physics->getParticle(index1)->mPos);
	physics->makeSpring(physics->getParticle(index0), physics->getParticle(index1), SPRING_STRENGTH, SPRING_DAMPING, dist);
}