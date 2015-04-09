#pragma once
#include <vector>
#include <cmath>
#include "cinder/gl/gl.h"
#include "cinder\app\AppBasic.h"

namespace Bit {
	class Particle {
	public:
		float x, y;
		float xv, yv;
		float xf, yf;
		float xd, yd;
		float xp, yp;

		float xc, yc;

		float rad;
		float radOne;
		float radRand;

		ci::ColorA col;
		ci::ColorA vCol;

		Particle(float _x = 0, float _y = 0,
			float _xv = 0, float _yv = 0) :
			x(_x), y(_y),
			xv(_xv), yv(_yv) {

			xd = 0;
			yd = 0;
			xp = 0;
			yp = 0;

			xc = 0;
			yc = 0;

			xf = 0;
			yf = 0;

			radOne = 3.0;
		}

		void Particle::addForce(float x, float y){
			this->xf = x;
			this->yf = y;
		}

		void Particle::addForce(ci::Vec2f & vec){
			this->xf += vec.x;
			this->yf += vec.y;
		}

		ci::Vec2f Particle::getPosition(){
			return ci::Vec2f(x, y);
		}


		void Particle::setParticleColor(ci::ColorA col){
			this->col = col;
		}

		void Particle::setParticleColorVel(ci::ColorA vCol){
			this->vCol = vCol;
		}

		void Particle::setParticleRadius(float a){
			this->radRand = a;
		}


		void updatePosition(float timeStep = 1.0) {
			xv += xf;
			yv += yf;
			x += xv * timeStep;
			y += yv * timeStep;

			xd = x - xp;
			yd = y - yp;

			xp = x;
			yp = y;
		}

		void resetForce() {
			xf = 0;
			yf = 0;
		}

		void bounceOffWalls(float left, float top, float right, float bottom, float damping = .3) {
			bool collision = false;

			if (x > right - radRand / 2){
				x = right - radRand / 2;
				xv *= -1;
				collision = true;
			}
			else if (x < left + radRand / 2){
				x = left + radRand / 2;
				xv *= -1;
				collision = true;
			}

			if (y > bottom - radRand / 2){
				y = bottom - radRand / 2;
				yv *= -1;
				collision = true;
			}
			else if (y < top + radRand / 2){
				y = top + radRand / 2;
				yv *= -1;
				collision = true;
			}

			if (collision == true){
				xv *= damping;
				yv *= damping;
			}
		}

		void addDampingForce(float damping = .01) {
			xf = xf - xv * damping;
			yf = yf - yv * damping;
		}

		void draw() {
			//	glColor4f(col.r, col.g, col.b, col.a);
			glVertex2f(x, y);
		}
	};
}