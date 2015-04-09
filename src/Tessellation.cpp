#include "Tessellation.h"


Tessellation::~Tessellation()
{


}

Tessellation::Tessellation(const std::string & name, int id)
{
	surfaceColor.setupSurface(name, id);
}

void Tessellation::setDimentions(ci::Vec2i top, ci::Vec2i bottom)
{
	topLeft = top;
	topRight = ci::Vec2i(bottom.x, top.y); 
	bottomLeft = ci::Vec2i(top.x, bottom.y); 
	bottomRight = bottom;

	surfaceColor.setWindowdim(bottom.x, bottom.y);

}

ci::TriMesh Tessellation::generateMesh(std::vector<Bit::Particle> & particles)
{
	mMesh.clear();
	mDepthRandom.clear();

	GEOM_FADE2D::Fade_2D * dt = new GEOM_FADE2D::Fade_2D(particles.size());

	std::vector<GEOM_FADE2D::Point2> vInputPoints;

	for(int i = 0;  i < particles.size(); i++){
		vInputPoints.push_back(GEOM_FADE2D::Point2(particles[i].x,  particles[i].y));
	}
	
	dt->insert(vInputPoints); 

	std::vector< GEOM_FADE2D::Triangle2 *> vAllDelaunayTri;
	dt->getTrianglePointers(vAllDelaunayTri);

	int i    = 0;
	int itri = 0;
	
	if(blendFunc){
		for(auto it = vAllDelaunayTri.begin(); it != vAllDelaunayTri.end(); ++it){
			GEOM_FADE2D::Point2 * p1 = (*it)->getCorner(0);
			GEOM_FADE2D::Point2 * p2 = (*it)->getCorner(1);
			GEOM_FADE2D::Point2 * p3 = (*it)->getCorner(2);

			ci::Vec2i center = ci::Vec2i((int)p1->x(), (int)p1->y())/3.0f + 
							   ci::Vec2i((int)p2->x(), (int)p2->y())/3.0f +
							   ci::Vec2i((int)p3->x(), (int)p3->y())/3.0f;

			ci::ColorA col = surfaceColor.getColorPixel(center);

			appendVertex(p1, col);
			appendVertex(p2, col);
			appendVertex(p3, col);

			mMesh.appendTriangle( mMesh.getVertices().size() - 3, mMesh.getVertices().size() - 2, mMesh.getVertices().size() - 1 );
		}
	}else{
		for(auto it = vAllDelaunayTri.begin(); it != vAllDelaunayTri.end(); ++it){
			GEOM_FADE2D::Point2 * p1 = (*it)->getCorner(0);
			GEOM_FADE2D::Point2 * p2 = (*it)->getCorner(1);
			GEOM_FADE2D::Point2 * p3 = (*it)->getCorner(2);

			appendVertex(p1, surfaceColor.getColorPixel( ci::Vec2f(p1->x(), p1->y()) ));
			appendVertex(p2, surfaceColor.getColorPixel( ci::Vec2f(p2->x(), p2->y()) ));
			appendVertex(p3, surfaceColor.getColorPixel( ci::Vec2f(p3->x(), p3->y()) ));

			mMesh.appendTriangle( mMesh.getVertices().size() - 3, mMesh.getVertices().size() - 2, mMesh.getVertices().size() - 1 );
		}

	}

	//ci::app::console()<<"NUMBER OF TRIANGLES "<<mMesh.getNumTriangles()<<" "<<mMesh.getNumVertices()<<endl;

	vAllDelaunayTri.clear();

	delete dt;
	
	return mMesh;
}


ci::TriMesh Tessellation::generateMesh(std::vector<ci::Vec2f> & markers, std::vector<Bit::Particle> & particles)
{
	mMesh.clear();
	mDepthRandom.clear();
	GEOM_FADE2D::Fade_2D * dt = new GEOM_FADE2D::Fade_2D(particles.size());

	std::vector<GEOM_FADE2D::Point2> vInputPoints;

	for(int i = 0;  i < particles.size(); i++){
		vInputPoints.push_back(GEOM_FADE2D::Point2(particles[i].x,  particles[i].y));
	}
	
	dt->insert(vInputPoints); 

	std::vector< GEOM_FADE2D::Triangle2 *> vAllDelaunayTri;
	dt->getTrianglePointers(vAllDelaunayTri);


	int i    = 0;
	int itri = 0;

	//ci::app::console() << markers.size() << std::endl;
	
	if(blendFunc){
		for(auto it = vAllDelaunayTri.begin(); it != vAllDelaunayTri.end(); ++it){
			GEOM_FADE2D::Point2 * p1 = (*it)->getCorner(0);
			GEOM_FADE2D::Point2 * p2 = (*it)->getCorner(1);
			GEOM_FADE2D::Point2 * p3 = (*it)->getCorner(2);

			ci::ColorA col;
			float radius = 0;
			ci::Vec2i center = ci::Vec2f(p1->x(), p1->y())/3.0f + 
							   ci::Vec2f(p2->x(), p2->y())/3.0f +
							   ci::Vec2f(p3->x(), p3->y())/3.0f;

			col = surfaceColor.getColorPixel(center);
			col.r = 0.0;
			col.g = 0.0;
			col.b = 0.0;


			appendVertex(p1, col, 0);
			appendVertex(p2, col, 0);
			appendVertex(p3, col, 0);

			mMesh.appendTriangle( mMesh.getVertices().size() - 3, mMesh.getVertices().size() - 2, mMesh.getVertices().size() - 1 );
		}
	}else{
		for(auto it = vAllDelaunayTri.begin(); it != vAllDelaunayTri.end(); ++it){
			GEOM_FADE2D::Point2 * p1 = (*it)->getCorner(0);
			GEOM_FADE2D::Point2 * p2 = (*it)->getCorner(1);
			GEOM_FADE2D::Point2 * p3 = (*it)->getCorner(2);

			appendVertex(p1, surfaceColor.getColorPixel( ci::Vec2f(p1->x(), p1->y()) ));
			appendVertex(p2, surfaceColor.getColorPixel( ci::Vec2f(p2->x(), p2->y()) ));
			appendVertex(p3, surfaceColor.getColorPixel( ci::Vec2f(p3->x(), p3->y()) ));

			mMesh.appendTriangle( mMesh.getVertices().size() - 3, mMesh.getVertices().size() - 2, mMesh.getVertices().size() - 1 );
		}

	}

	vAllDelaunayTri.clear();
	delete dt;
	
	return mMesh;
}


void Tessellation::appendVertex(GEOM_FADE2D::Point2 * p, ci::ColorA & color )
{
	mMesh.appendVertex(ci::Vec3f(p->x(), p->y(), 0));
	mMesh.appendTexCoord(ci::Vec2f(p->x(), p->y()) / ci::app::getWindowSize());
	mMesh.appendColorRgba(color);
}

void Tessellation::appendVertex(GEOM_FADE2D::Point2 * p, ci::ColorA & color, float z)
{
	mMesh.appendVertex(ci::Vec3f(p->x(), p->y(), z));
	mMesh.appendTexCoord(ci::Vec2f(p->x(), p->y()) / ci::app::getWindowSize());
	mMesh.appendColorRgb(color);
}