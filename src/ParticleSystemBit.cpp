#include "ParticleSystemBit.h"

namespace Bit{

	ParticleSystem::ParticleSystem() :
		timeStep(1) {
	}

	ParticleSystem::~ParticleSystem()
	{
		delete tessellationMesh;
	}

	void ParticleSystem::setup(int width, int height, int k) {
		this->width = width;
		this->height = height;
		this->k = k;
		binSize = 1 << k;
		xBins = (int)ceilf((float)width / (float)binSize);
		yBins = (int)ceilf((float)height / (float)binSize);
		bins.resize(xBins * yBins);

		tessellationMesh = new Tessellation("../resources/debula03.jpg", 144);
		tessellationMesh->setDimentions(ci::Vec2i(0, 0), ci::Vec2i(width, height));
	}

	void ParticleSystem::updateSize()
	{
		particleSize = particles.size();
	}

	void ParticleSystem::loadColors(std::string name, int num, int id)
	{
		surfaceColor.setupSurface(name, num, id);
	}

	void ParticleSystem::setTimeStep(float timeStep) {
		this->timeStep = timeStep;
	}

	void ParticleSystem::add(Particle particle) {
		particles.push_back(particle);
	}

	unsigned ParticleSystem::size() const {
		return particles.size();
	}

	void ParticleSystem::clean(){
		particles.clear();
		bins.clear();
	}


	Particle& ParticleSystem::operator[](unsigned i) {
		return particles[i];
	}


	std::vector<Particle*> ParticleSystem::getNeighbors(Particle& particle, float radius) {
		return getNeighbors(particle.x, particle.y, radius);
	}

	std::vector<Particle*> ParticleSystem::getNeighbors(float x, float y, float radius) {
		std::vector<Particle*> region = getRegion(
			(int)(x - radius),
			(int)(y - radius),
			(int)(x + radius),
			(int)(y + radius));
		std::vector<Particle*> neighbors;
		int n = region.size();
		float xd, yd, rsq, maxrsq;
		maxrsq = radius * radius;
		for (int i = 0; i < n; i++) {
			Particle& cur = *region[i];
			xd = cur.x - x;
			yd = cur.y - y;
			rsq = xd * xd + yd * yd;
			if (rsq < maxrsq)
				neighbors.push_back(region[i]);
		}
		return neighbors;
	}

	std::vector<Particle*> ParticleSystem::getRegion(unsigned minX, unsigned minY, unsigned maxX, unsigned maxY) {
		std::vector<Particle*> region;
		std::back_insert_iterator< std::vector<Particle*> > back = back_inserter(region);
		unsigned minXBin = minX >> k;
		unsigned maxXBin = maxX >> k;
		unsigned minYBin = minY >> k;
		unsigned maxYBin = maxY >> k;
		maxXBin++;
		maxYBin++;
		if (maxXBin > xBins)
			maxXBin = xBins;
		if (maxYBin > yBins)
			maxYBin = yBins;
		for (int y = minYBin; y < maxYBin; y++) {
			for (int x = minXBin; x < maxXBin; x++) {
				std::vector<Particle*>& cur = bins[y * xBins + x];
				copy(cur.begin(), cur.end(), back);
			}
		}
		return region;
	}

	void ParticleSystem::setupForces() {
		int n = bins.size();
		for (int i = 0; i < n; i++) {
			bins[i].clear();
		}
		n = particles.size();
		unsigned xBin, yBin, bin;
		for (int i = 0; i < n; i++) {
			Particle& cur = particles[i];
			cur.resetForce();
			xBin = ((unsigned)cur.x) >> k;
			yBin = ((unsigned)cur.y) >> k;
			bin = yBin * xBins + xBin;
			if (xBin < xBins && yBin < yBins)
				bins[bin].push_back(&cur);
		}
	}

	void ParticleSystem::addRepulsionForce(const Particle& particle, float radius, float scale) {
		addRepulsionForce(particle.x, particle.y, radius, scale);
	}

	void ParticleSystem::addRepulsionForce(float x, float y, float radius, float scale) {
		addForce(x, y, radius, scale);
	}

	void ParticleSystem::addAttractionForce(const Particle& particle, float radius, float scale) {
		addAttractionForce(particle.x, particle.y, radius, scale);
	}

	void ParticleSystem::addAttractionForce(float x, float y, float radius, float scale) {
		addForce(x, y, radius, -scale);
	}

	void ParticleSystem::addForce(const Particle& particle, float radius, float scale) {
		addForce(particle.x, particle.y, radius, -scale);
	}

	void ParticleSystem::addForce(float targetX, float targetY, float radius, float scale) {
		float minX = targetX - radius;
		float minY = targetY - radius;
		float maxX = targetX + radius;
		float maxY = targetY + radius;
		if (minX < 0)
			minX = 0;
		if (minY < 0)
			minY = 0;
		unsigned minXBin = ((unsigned)minX) >> k;
		unsigned minYBin = ((unsigned)minY) >> k;
		unsigned maxXBin = ((unsigned)maxX) >> k;
		unsigned maxYBin = ((unsigned)maxY) >> k;
		maxXBin++;
		maxYBin++;
		if (maxXBin > xBins)
			maxXBin = xBins;
		if (maxYBin > yBins)
			maxYBin = yBins;
		float xd, yd, length, maxrsq;
#ifdef USE_INVSQRT
		float xhalf;
		int lengthi;
#else
		float effect;
#endif
		maxrsq = radius * radius;
		int x, y;
		for (y = minYBin; y < maxYBin; y++) {
			for (x = minXBin; x < maxXBin; x++) {
				std::vector<Particle*>& curBin = bins[y * xBins + x];
				int n = curBin.size();
				for (int i = 0; i < n; i++) {
					Particle& curParticle = *(curBin[i]);
						//				As suggested by Andrew Bell
						//				if(curParticle.x > minX && curParticle.x < maxX &&
						//					curParticle.y > minY && curParticle.y < maxY) {
						xd = curParticle.x - targetX;
						yd = curParticle.y - targetY;
						length = xd * xd + yd * yd;
						if (length > 0 && length < maxrsq) {
#ifdef DRAW_FORCES
							glVertex2f(targetX, targetY);
							glVertex2f(curParticle.x, curParticle.y);
#endif
#ifdef USE_INVSQRT
							xhalf = 0.5f * length;
							lengthi = *(int*)&length;
							lengthi = 0x5f3759df - (lengthi >> 1);
							length = *(float*)&lengthi;
							length *= 1.5f - xhalf * length * length;
							xd *= length;
							yd *= length;
							length *= radius;
							length = 1 / length;
							length = (1 - length);
							length *= scale;
							xd *= length;
							yd *= length;
							curParticle.xf += xd;
							curParticle.yf += yd;
#else
							length = sqrtf(length);
							xd /= length;
							yd /= length;
							effect = (1 - (length / radius)) * scale;
							curParticle.xf += xd * effect;
							curParticle.yf += yd * effect;
#endif
					}
					//				}
				}
			}
		}
	}

	void ParticleSystem::setNewParticleRadius(bool radius)
	{
		if (radius)
		for (int i = 0; i < particleSize; i++){
			particles[i].rad = particles[i].radOne;
		}
		else
		for (int i = 0; i < particleSize; i++){
			particles[i].rad = particles[i].radRand;
		}

	}
	void ParticleSystem::update() {
		for (int i = 0; i < particleSize; i++){
			particles[i].bounceOffWalls(0, 0, width, height);
			particles[i].updatePosition(timeStep);
		}
	}

	ci::TriMesh	ParticleSystem::calculateMesh(){
		ci::TriMesh mesh = tessellationMesh->generateMesh(particles);
		return mesh;
	}

	ci::TriMesh	ParticleSystem::calculateMeshPoints(std::vector<ci::Vec2f> & points){
		ci::TriMesh mesh = tessellationMesh->generateMesh(points, particles);
		return mesh;
	}

	void
		ParticleSystem::setNewColorsMesh(SurfaceColor & colors)
	{
			tessellationMesh->setNewSurfaceColor(colors);
		}

	void  ParticleSystem::setNewColorsParticle(SurfaceColor & colors)
	{
		surfaceColor = colors;
	}


	//1
	void ParticleSystem::drawOnlyVel()
	{
		for (int i = 0; i < particleSize; i++){
			if (particles[i].xd > 0.25|| particles[i].yd > 0.25){
				glPointSize(particles[i].rad * abs(particles[i].xd * 0.95 * particles[i].xd * 0.95) + 1.0);
				glBegin(GL_POINTS);
				//ci::ColorA col = tessellationMesh->getSurfaceColorPoint(ci::Vec2i(particles[i].x, particles[i].y));
				ci::ColorA col = tessellationMesh->getGoodColorVel(ci::Vec2i(particles[i].x, particles[i].y), abs(particles[i].yd + particles[i].xd)*2.0 );
				glColor4f(col);
				particles[i].draw();
				glEnd();
			}
		}
	}

	void ParticleSystem::drawBackground()
	{
		numActivePoints = 0;
		for (int i = 0; i < particleSize; i++){
			if (particles[i].xd > 0.4 && particles[i].yd > 0.4){
				glPointSize(particles[i].rad * particles[i].xd*1.24 + 2.0);
				glBegin(GL_POINTS);
				glColor4f(velColor);
				particles[i].draw();
				glEnd();
				numActivePoints++;
			}
		}
	}

	void ParticleSystem::drawVelLog()
	{
		numActivePoints = 0;
		for (int i = 0; i < particleSize; i++){
			if (particles[i].xd > 0.9 && particles[i].yd > 0.9){
				glPointSize(log(particles[i].xd*3.5 + 1.0)*11.0);
				glBegin(GL_POINTS);
				glColor4f(velColor);
				particles[i].draw();
				glEnd();
				numActivePoints++;
			}
		}
	}

	void ParticleSystem::drawVelLogBackground()
	{
		numActivePoints = 0;
		for (int i = 0; i < particleSize; i++){
			if (particles[i].xd > 0.9 && particles[i].yd > 0.9){
				glPointSize(log(particles[i].xd*5.0 + 1.0)*11.0);
				glBegin(GL_POINTS);
				ci::ColorA col = tessellationMesh->getSurfaceColorPoint(ci::Vec2i(particles[i].x, particles[i].y));
				glColor4f(abs(col.r - 1.0f), abs(col.g - 1.0f), abs(col.b - 1.0f), 1.0);
				particles[i].draw();
				glEnd();
				numActivePoints++;
			}
		}
	}

	//0
	void ParticleSystem::draw() {
		numActivePoints = 0;

		for (int i = 0; i < particleSize; i++){
			if (particles[i].xd > 0.9 || particles[i].yd > 0.9){
				glPointSize(particles[i].rad * particles[i].xd*2.45 + 2.0);
				glBegin(GL_POINTS);
				glColor4f(velColor);
				particles[i].draw();
				numActivePoints++;
			}
			else{
				glPointSize(particles[i].rad);
				glBegin(GL_POINTS);
				ci::ColorA col = tessellationMesh->getSurfaceColorPoint(ci::Vec2i(particles[i].x, particles[i].y));
				glColor4f(col);
				particles[i].draw();
			}
			glEnd();
		}
	}

	std::vector<Particle> & ParticleSystem::getParticles()
	{
		return particles;
	}

	void ParticleSystem::addAcceleration(int index, ci::Vec2f acc)
	{
		(particles[index]).addForce(acc);
	}

	void ParticleSystem::updateParticleAcc(Particle & particle, ci::Vec2f & vec)
	{
		particle.addForce(vec);//addForce(vec);
	}

}