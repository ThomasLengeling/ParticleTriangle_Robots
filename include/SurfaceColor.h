#pragma once

#include "cinder/gl/gl.h"
#include "cinder/Surface.h"
#include "cinder/ImageIo.h"
#include "Resources.h"
#include "cinder/Rand.h"

#include "cinder/app/App.h"

#include <string>

#include "cinder/CinderResources.h"


class SurfaceColor{
public:

	~SurfaceColor(){
		for (int i = 0; i < numValues; i++) {
			delete[] colors[i];
		}
		delete[] colors;
	}

	SurfaceColor(){
		numValues = 0;
	}

	ci::Surface colorSurface;

	int numValuesX;
	int numValuesY;

	int numValues;

	ci::ColorA **colors;

	int wWidth;
	int wHeight;

	int id;

	//const std::string & name
	void setupSurface(const std::string & name, int num, int id){

		colorSurface = ci::loadImage(ci::app::loadResource(name, id, "IMAGE"));

		this->numValuesX = num;
		this->numValuesY = num;

		colors = new ci::ColorA*[numValuesX];
		for (int i = 0; i < numValuesX; i++){
			colors[i] = new ci::ColorA[numValuesX];
		}

		for (int i = 0; i < numValuesX; i++){
			for (int j = 0; j < numValuesX; j++){
				ci::ColorA c = colorSurface.getPixel(ci::Vec2i(i, j));
				colors[i][j] = c;
			}
		}
	}

	void setupSurface(const std::string & name, int id){

		colorSurface = ci::loadImage(ci::app::loadResource(name, id, "IMAGE"));

		this->numValuesX = colorSurface.getWidth();
		this->numValuesY = colorSurface.getHeight();

		colors = new ci::ColorA*[numValuesX];
		for (int i = 0; i < numValuesX; i++){
			colors[i] = new ci::ColorA[numValuesY];
		}

		for (int i = 0; i < numValuesX; ++i){
			for (int j = 0; j < numValuesY; ++j){
				ci::ColorA c = colorSurface.getPixel(ci::Vec2i(i, j));
				colors[i][j] = c;
			}
		}
	}

	inline void setWindowdim(int width, int height);

	inline ci::ColorA getGoodColor();

	inline ci::ColorA getColorPixel(ci::Vec2i pix);

	inline ci::ColorA getColorPixel(ci::Vec2i pix, float vel);

};

void SurfaceColor::setWindowdim(int width, int height)
{
	this->wWidth = width;
	this->wHeight = height;
}

ci::ColorA SurfaceColor::getColorPixel(ci::Vec2i pix)
{
	// int pixX = ci::constrain((int)ci::lmap<float>(pix.x, 0, wWidth, 0, numValuesX - 1), 0,  numValuesX - 1);
	// int pixY = ci::constrain((int)ci::lmap<float>(pix.y, 0, wHeight, 0, numValuesY - 1), 0,  numValuesY - 1);

	int pixX = ci::constrain((int)ci::lmap<float>(pix.x, 0, wWidth, 0, numValuesX - 1), 0, numValuesX - 1);
	int pixY = ci::constrain((int)ci::lmap<float>(pix.y, 0, wHeight, 0, numValuesY - 1), 0, numValuesY - 1);

	return colors[pixX][pixY];
}

ci::ColorA SurfaceColor::getColorPixel(ci::Vec2i pix, float vel)
{
	// int pixX = ci::constrain((int)ci::lmap<float>(pix.x, 0, wWidth, 0, numValuesX - 1), 0,  numValuesX - 1);
	// int pixY = ci::constrain((int)ci::lmap<float>(pix.y, 0, wHeight, 0, numValuesY - 1), 0,  numValuesY - 1);

	int pixX = ci::constrain((int)ci::lmap<float>(pix.x, 0, wWidth, 0, numValuesX - 1), 0, numValuesX - 1);
	int pixY = ci::constrain((int)ci::lmap<float>(pix.y, 0, wHeight, 0, numValuesY - 1), 0, numValuesY - 1);

	return colors[pixX][pixY];
}


ci::ColorA SurfaceColor::getGoodColor()
{
	return colors[ci::Rand::randInt(0, numValuesX - 1)][ci::Rand::randInt(0, numValuesY - 1)];
}


