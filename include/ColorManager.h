#pragma once

#include "cinder/gl/gl.h"
#include "cinder/Surface.h"
#include "cinder/ImageIo.h"
#include "Resources.h"
#include "cinder/Rand.h"

#include "SurfaceColor.h"

#include "cinder/app/App.h"

#include <string>

class ColorManager{
public:

	~ColorManager(){
		mColors.clear();
	}

	void addNewColors(const std::string & name, int num, int width, int height, int id){
		SurfaceColor *sc = new SurfaceColor();
		sc->setupSurface(name, num, id);
		sc->setWindowdim(width, height);
		mColors.push_back(sc);
	}

	void addNewColors(const std::string & name, int width, int height, int id){
		SurfaceColor *sc = new SurfaceColor();
		sc->setupSurface(name, id);
		sc->setWindowdim(width, height);
		mColors.push_back(sc);
	}

	inline SurfaceColor * getSurfaceColor(unsigned int i);

	int getSize() {return mColors.size();}

protected:

	std::vector<SurfaceColor*> mColors;
	
};

 SurfaceColor * ColorManager::getSurfaceColor(unsigned int i){
	 if(i <= mColors.size()-1)
		return mColors[i];
	 return new SurfaceColor();
}



