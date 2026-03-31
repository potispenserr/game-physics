#pragma once
#include "GraphicsNode.h"
#include "plane.h"
typedef struct Interval {
	float max;
	float min;
} Interval;

typedef struct Line {
	Vector4D start;
	Vector4D end;
	Line() {

	}
	Line(const Vector4D& newStart, const Vector4D& newEnd) : start(newStart), end(newEnd) 
	{} 
} Line;


class CollisionManifold
{
private:

public:
    CollisionManifold();
    CollisionManifold(const CollisionManifold& cm);
    ~CollisionManifold();
    
    std::vector<Vector4D> contactPoints;
	float depth;
	bool isColliding;
	Vector4D collisionNormal;

    Interval getInterval(const GraphicsNode& gn, const Vector4D axis);
	bool overlapingOnAxis(const GraphicsNode& gn, const GraphicsNode& gn2, Vector4D& axis, float& depth, bool& isNegative);
	void SATOnAABBs(const GraphicsNode& gn, const GraphicsNode& gn2);
	std::vector<Line> getEdges(const GraphicsNode& gn);
	std::vector<Vector4D> checkEdgesToAABB(const std::vector<Line>& edges, const GraphicsNode& gn);
	std::vector<Plane> getPlanes(const GraphicsNode& gn);
	bool checkEdgesToPlane(const Plane& plane, const Line& edge, Vector4D& resultPoint);
	bool pointInAABB(const Vector4D& point, const GraphicsNode& gn);
};


