//
//  Bloom.h
//  SintraMoons
//
//  Created by tom on 5/20/14.
//
//
#pragma once

#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/app/AppBasic.h"

#include "Resources.h"


//BLOOM
class Bloom{
public:
	Bloom(){ attenuationBloom = 0.0; }
	void                setupBlur(ci::Vec2f dims);
	void				compileShader();

	void                updateBlur();
	void                drawBlur();

	void                bindFboScene();
	void                unbindFboScene();

	void                setAttenuation(float v){ attenuationBloom = v; }

protected:
	float               attenuationBloom;

	//FBO
	ci::gl::Fbo         mFboScene;
	ci::gl::Fbo         mFboBlur1;
	ci::gl::Fbo         mFboBlur2;

	//GLSL
	ci::gl::GlslProg    mShaderBlur;

	ci::Vec2f           bloomDims;

	ci::Area            bloomBounds;

};