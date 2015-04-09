#pragma once

#include <Fade_2D.h>
#include <stdio.h>
#include <map>
#include <string>
#include <sstream>
#include <iomanip>

#include "ParticleBit.h"
#include "SurfaceColor.h"

#include "cinder/Triangulate.h"
#include "cinder/app/AppBasic.h"

#include "boost\shared_ptr.hpp"

class Tessellation{
public:

	~Tessellation();

	Tessellation(const std::string & name, int id);

	void setDimentions(ci::Vec2i top, ci::Vec2i bottom);

	ci::TriMesh generateMesh(std::vector<Bit::Particle> & particles);

	ci::TriMesh generateMesh(std::vector<ci::Vec2f> & markers, std::vector<Bit::Particle> & particles);

	void setNewSurfaceColor(SurfaceColor & color){surfaceColor = color;}

	void activateBlend() { blendFunc = !blendFunc;}

	ci::Color getSurfaceColorPoint(ci::Vec2i vec) {return surfaceColor.getColorPixel(vec); }

	ci::Color getGoodColorVel(ci::Vec2i vec, float vel){ return surfaceColor.getColorPixel(vec, vel); }

	void appendVertex(GEOM_FADE2D::Point2 * p, ci::ColorA & color );
	void appendVertex(GEOM_FADE2D::Point2 * p, ci::ColorA & color, float z);


	void setRadius(float val){ particleZ = val;}
protected:

	float	  particleZ;
	bool	  blendFunc;

	ci::Vec2i topLeft;
	ci::Vec2i topRight;
	ci::Vec2i bottomLeft;
	ci::Vec2i bottomRight;

	ci::TriMesh			mMesh;
	std::vector<float > mDepthRandom;

	SurfaceColor surfaceColor;
};