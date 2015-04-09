#pragma once
#include "ParticleBit.h"
#include "SurfaceColor.h"

#include "Tessellation.h"
#include "cinder/TriMesh.h"

//#define DRAW_FORCES
#define USE_INVSQRT

namespace Bit{
	class ParticleSystem {
	protected:
		float timeStep;
		std::vector<Particle> particles;
		std::vector< std::vector<Particle*> > bins;
		int width, height, k, xBins, yBins, binSize;

		SurfaceColor surfaceColor;

		unsigned int particleSize;

		Tessellation	*tessellationMesh;

	public:
		ParticleSystem();
		~ParticleSystem();

		void updateSize();

		void setup(int width, int height, int k);
		void setTimeStep(float timeStep);
		void loadColors(std::string name, int num, int id);

		void add(Particle particle);
		std::vector<Particle*> getNeighbors(Particle& particle, float radius);
		std::vector<Particle*> getNeighbors(float x, float y, float radius);
		std::vector<Particle*> getRegion(unsigned minX, unsigned minY, unsigned maxX, unsigned maxY);
		unsigned size() const;
		Particle& operator[](unsigned i);

		void setupForces();
		void addRepulsionForce(const Particle& particle, float radius, float scale);
		void addRepulsionForce(float x, float y, float radius, float scale);
		void addAttractionForce(const Particle& particle, float radius, float scale);
		void addAttractionForce(float x, float y, float radius, float scale);
		void addForce(const Particle& particle, float radius, float scale);
		void addForce(float x, float y, float radius, float scale);
		void update();

		std::vector<Particle> & getParticles();

		void addAcceleration(int index, ci::Vec2f vec);
		void updateParticleAcc(Particle& particle, ci::Vec2f & vec);

		void draw();
		void drawOnlyVel();
		void drawVelLog();
		void drawBackground();
		void drawVelLogBackground();

		void clean();
		void setNewParticleRadius(bool radius);

		ci::TriMesh		calculateMesh();
		ci::TriMesh		calculateMeshPoints(std::vector<ci::Vec2f> & points);

		void			setNewColorsMesh(SurfaceColor & colors);
		void			setNewColorsParticle(SurfaceColor & colors);
		void			setCurrColor(ci::ColorA currColor){ this->currColor = currColor; }
		void			setVelColor(ci::ColorA velColor){ this->velColor = velColor; }

		ci::ColorA		currColor;
		ci::ColorA		velColor;

		bool			activateSurfaceColor;

		void			activateColors(){ activateSurfaceColor = !activateSurfaceColor; }

		void			blendMesh(){ tessellationMesh->activateBlend(); }


		int				numActivePoints;

		int				getNumParticlesActive(){ return numActivePoints; }


		void           setMeshZ(float val){ tessellationMesh->setRadius(val); }
	};

	inline float InvSqrt(float x){
		float xhalf = 0.5f * x;
		int i = *(int*)&x; // store floating-point bits in integer
		i = 0x5f3759d5 - (i >> 1); // initial guess for Newton's method
		x = *(float*)&i; // convert new bits into float
		x = x*(1.5f - xhalf*x*x); // One round of Newton's method
		return x;
	}

}