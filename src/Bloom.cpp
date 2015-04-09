//
//  Bloom.cpp
//  SintraMoons
//
//  Created by tom on 5/20/14.
//
//

#include "Bloom.h"


void Bloom::setupBlur(ci::Vec2f dims)
{
	ci::gl::Fbo::Format format;
	format.enableMipmapping(false);
	format.setWrap(GL_CLAMP_TO_BORDER_ARB, GL_CLAMP_TO_BORDER_ARB);

	mFboBlur1 = ci::gl::Fbo(dims.x / 4.0f, dims.y / 4.0f, format);
	mFboBlur2 = ci::gl::Fbo(dims.x / 4.0f, dims.y / 4.0f, format);

	format.setCoverageSamples(16);
	format.setSamples(4);

	mFboScene = ci::gl::Fbo(dims.x / 1.0f, dims.y / 1.0f, format);

	this->bloomDims = dims;
	this->bloomBounds = ci::Area(0, 0, dims.x, dims.y);
	ci::app::console() << "bloom bounds " << bloomBounds << std::endl;

}

void Bloom::compileShader(){

	try {
		mShaderBlur = ci::gl::GlslProg(ci::app::loadResource(RES_VERT_GLSL), ci::app::loadResource(RES_FRAG_GLSL));
	}
	catch (...) {
		ci::app::console() << "Can't load/compile blur shader" << std::endl;
	}
}

void Bloom::bindFboScene()
{

	mFboScene.bindFramebuffer();
	ci::gl::setViewport(mFboScene.getBounds());
}

void Bloom::unbindFboScene()
{
	mFboScene.unbindFramebuffer();
}

void Bloom::updateBlur()
{
	mShaderBlur.bind();
	mShaderBlur.uniform("tex0", 0);
	mShaderBlur.uniform("sample_offset", ci::Vec2f(1.0f / mFboBlur1.getWidth(), 0.0f));
	mShaderBlur.uniform("attenuation", attenuationBloom);

	mFboBlur1.bindFramebuffer();
	mFboScene.bindTexture(0);
	ci::gl::setViewport(mFboBlur1.getBounds());
	ci::gl::clear(ci::Color::black());
	ci::gl::drawSolidRect(bloomBounds);
	mFboScene.unbindTexture();
	mFboBlur1.unbindFramebuffer();

	mShaderBlur.uniform("sample_offset", ci::Vec2f(0.0f, 1.0f / mFboBlur2.getHeight()));
	mShaderBlur.uniform("attenuation", 1.2f);

	mFboBlur2.bindFramebuffer();
	mFboBlur1.bindTexture(0);
	ci::gl::setViewport(mFboBlur2.getBounds());
	ci::gl::clear(ci::Color::black());
	ci::gl::drawSolidRect(bloomBounds);
	mFboBlur1.unbindTexture();
	mFboBlur2.unbindFramebuffer();

	mShaderBlur.unbind();

	ci::gl::setViewport(bloomBounds);
}

void Bloom::drawBlur()
{
	ci::gl::pushModelView();
	ci::gl::translate(ci::Vec2f(0, bloomDims.y));
	ci::gl::scale(ci::Vec3f(1, -1, 1));
	ci::gl::color(ci::Color::white());
	ci::gl::draw(mFboScene.getTexture(), bloomBounds);

	ci::gl::enableAdditiveBlending();
	ci::gl::draw(mFboBlur2.getTexture(), bloomBounds);
	ci::gl::disableAlphaBlending();
	ci::gl::popModelView();
}