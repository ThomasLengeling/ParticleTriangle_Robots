class TriangleVertex{
public:
	TriangleVertex(ci::Vec3f vert0, ci::Vec3f vert1, ci::Vec3f vert2){
		this->vert0 = vert0;
		this->vert1 = vert1;
		this->vert2 = vert2;

		col = ci::ColorA(0.5, 0.5, 0.5, 1.0);
	}

	TriangleVertex(int indexVert0, int indexVert1, int indexVert2){
		this->indexVert0 = indexVert0;
		this->indexVert1 = indexVert1;
		this->indexVert2 = indexVert2;

		col = ci::ColorA(0.5, 0.5, 0.5, 1.0);
	}

	void setVert0(ci::Vec3f & v){ vert0 = v; }
	void setVert1(ci::Vec3f & v){ vert1 = v; }
	void setVert2(ci::Vec3f & v){ vert2 = v; }

	ci::Vec3f getVert0(){ return vert0; }
	ci::Vec3f getVert1(){ return vert1; }
	ci::Vec3f getVert2(){ return vert2; }

	int getIndexVert0(){ return indexVert0; }
	int getIndexVert1(){ return indexVert1; }
	int getIndexVert2(){ return indexVert2; }

	void	setColor(ci::ColorA  cold){ this->col = cold; }
	ci::ColorA  getColor(){ return col; }

	bool isPointInTri(ci::Vec3f point){
		bool b1, b2, b3;

		b1 = Sign(point, vert0, vert1) < 0.0f;
		b2 = Sign(point, vert1, vert2) < 0.0f;
		b3 = Sign(point, vert2, vert0) < 0.0f;

		return  ((b1 == b2) && (b2 == b3));
	}

	float Sign(ci::Vec3f v1, ci::Vec3f v2, ci::Vec3f v3){
		return (v1.x - v3.x) * (v2.y - v3.y) - (v2.x - v3.x) *(v1.y - v3.y);
	}

	void add_index(int index){
		vertexIndex.push_back(index);
	}

	std::vector<int> & getVertexIndex(){
		return vertexIndex;
	}

private:
	ci::Vec3f vert0;
	ci::Vec3f vert1;
	ci::Vec3f vert2;

	int indexVert0;
	int indexVert1;
	int indexVert2;

	ci::ColorA col;

	std::vector<int> vertexIndex;
};