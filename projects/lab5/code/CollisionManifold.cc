#include "CollisionManifold.h"

CollisionManifold::CollisionManifold()
{
    
}

CollisionManifold::CollisionManifold(const CollisionManifold &cm)
{

    this->contactPoints = cm.contactPoints;
	this->depth = cm.depth;
	this->isColliding = cm.isColliding;
	this->collisionNormal = cm.collisionNormal;
}

CollisionManifold::~CollisionManifold()
{
}

Interval CollisionManifold::getInterval(const GraphicsNode &gn, const Vector4D axis)
{
	Vector4D max = gn.maxBounds;
	Vector4D min = gn.minBounds;
	//AABB vertices
	Vector4D verts[] = {
		Vector4D(min.x(), max.y(), min.z()), // top front left
		Vector4D(min.x(), max.y(), max.z()), // top back left

		Vector4D(min.x(), min.y(), min.z()), // bottom front left
		Vector4D(min.x(), min.y(), max.z()), // bottom back left

		Vector4D(max.x(), max.y(), min.z()), // top front right
		Vector4D(max.x(), max.y(), max.z()), // top back right

		Vector4D(max.x(), min.y(), min.z()), // bottom front right
		Vector4D(max.x(), min.y(), max.z())  // bottom back right
	};
	
	Interval result;
	result.max = -__FLT_MAX__;
	result.min = __FLT_MAX__;

	for (int i = 0; i < 8; i++){
		float projection = Vector4D::dot(axis, verts[i]);
		result.min = std::min(projection, result.min);
		result.max = std::max(projection, result.max);
	}


    return result;
}

bool CollisionManifold::overlapingOnAxis(const GraphicsNode &gn, const GraphicsNode &gn2, Vector4D &axis, float& depth, bool& isNegative)
{
	Interval first = getInterval(gn, axis.norm());
	Interval second = getInterval(gn2, axis.norm());

	if (second.max < first.min) {
		return false;
	}
	if(second.min > first.max) {
		return false;
	}


	float negativePen = second.max - first.min;
	float positivePen = first.max - second.min;
	
	isNegative = negativePen < positivePen;
	depth = std::min(negativePen, positivePen);
	return true;

}

void CollisionManifold::SATOnAABBs(const GraphicsNode &gn, const GraphicsNode &gn2)
{
	//CollisionManifold collisionResult;
	Vector4D testAxis[13] = {
		Vector4D(1.0f, 0.0f, 0.0f),
		Vector4D(0.0f, 1.0f, 0.0f),
		Vector4D(0.0f, 0.0f, 1.0f),
		Vector4D(1.0f, 0.0f, 0.0f),
		Vector4D(0.0f, 1.0f, 0.0f),
		Vector4D(0.0f, 0.0f, 1.0f),
	};
	for (int i = 0; i < 3; i++){
		testAxis[4 + i * 3 + 0] = Vector4D::cross(testAxis[i], testAxis[0]);
		testAxis[4 + i * 3 + 1] = Vector4D::cross(testAxis[i], testAxis[1]);
		testAxis[4 + i * 3 + 2] = Vector4D::cross(testAxis[i], testAxis[2]);
	}
	float smallestPenDepth = __FLT_MAX__;
	Vector4D smallestPenAxis;
	Vector4D collisionNormal;
	
	
	for (int i = 0; i < 13; i++){
		bool negativePenetration;
		float resultantPenDepth;
		if(Vector4D::dot(testAxis[i], testAxis[i]) < 0.001f) {
			continue;
		}
		if(!overlapingOnAxis(gn, gn2, testAxis[i], resultantPenDepth, negativePenetration)){
			std::cout << "no collision" << "\n";
			return;
		}
		if(resultantPenDepth <= 0.0f){
			return;
		}

		if(resultantPenDepth < smallestPenDepth) {
			smallestPenDepth = resultantPenDepth;
			smallestPenAxis = testAxis[i] * (negativePenetration ? -1.0f : 1.0f);

			collisionNormal = smallestPenAxis;
			this->depth = smallestPenDepth;
		}
	}
	if(collisionNormal == Vector4D(0.0f, 0.0f, 0.0f)){
		return;
	}

	//std::cout << "hit normal is X: " << collisionNormal.x() << " Y: " << collisionNormal.y() << " Z: " << collisionNormal.z() << "\n";
	Vector4D collisionAxis = collisionNormal.norm();
	this->collisionNormal = collisionAxis;

	std::vector<Vector4D> collisions1 = checkEdgesToAABB(getEdges(gn2), gn);
	std::vector<Vector4D> collisions2 = checkEdgesToAABB(getEdges(gn), gn2);
	this->contactPoints.reserve(collisions1.size() + collisions2.size());
	for(int i = 0; i < collisions2.size(); i++){
		this->contactPoints.push_back(collisions2[i]);
	}
	for(int i = 0; i < collisions1.size(); i++){
		this->contactPoints.push_back(collisions1[i]);
	}

	Interval interval = getInterval(gn, collisionAxis);
	float distance = (interval.max - interval.min) * 0.5f - this->depth * 0.5f;
	
	Vector4D planePoint = gn.AABBCenter + collisionAxis * distance;
	
	for (int i = this->contactPoints.size() - 1; i >= 0; --i){
		Vector4D contactPoint = this->contactPoints[i];
		this->contactPoints[i] = contactPoint + (collisionAxis * 
		Vector4D::dot(collisionAxis, planePoint - contactPoint));

		for (int i = this->contactPoints.size() - 1; i >= 0; --i){
			Vector4D contactPoint = this->contactPoints[i];

			//check if point is larger than the min x, y and z values and smaller than the max x, y and z values of the gn
			bool isWithinGN1 = contactPoint.x() >= gn.minBounds.get(0) && contactPoint.get(0) <= gn.maxBounds.get(0) &&
				contactPoint.get(1) >= gn.minBounds.get(1) && contactPoint.get(1) <= gn.maxBounds.get(1) &&
				contactPoint.get(2) >= gn.minBounds.get(2) && contactPoint.get(2) <= gn.maxBounds.get(2);

			bool isWithinGN2 = contactPoint.get(0) >= gn2.minBounds.get(0) && contactPoint.get(0) <= gn2.maxBounds.get(0) &&
				contactPoint.get(1) >= gn2.minBounds.get(1) && contactPoint.get(1) <= gn2.maxBounds.get(1) &&
				contactPoint.get(2) >= gn2.minBounds.get(2) && contactPoint.get(2) <= gn2.maxBounds.get(2);

			if(isWithinGN1 && isWithinGN2){

					//std::cout << "Contact point " << i << " is good" << "\n";
					continue;
				}
				this->contactPoints.erase(this->contactPoints.begin() + i);
				//std::cout << "Contact point " << i << " is not good at all" << "\n";

				//std::cout << "isWithinGN1 " << isWithinGN1 << " isWithinGN2 " << isWithinGN2 << "\n";
				
		}

	}


	this->isColliding = true;


    return;
}

std::vector<Line> CollisionManifold::getEdges(const GraphicsNode &gn)
{
	std::vector<Line> edges;
	edges.reserve(12);
	Vector4D max = gn.maxBounds;
	Vector4D min = gn.minBounds;
	Vector4D verts[] = {
		Vector4D(min.x(), max.y(), min.z()), // top front left
		Vector4D(min.x(), max.y(), max.z()), // top back left

		Vector4D(min.x(), min.y(), min.z()), // bottom front left
		Vector4D(min.x(), min.y(), max.z()), // bottom back left

		Vector4D(max.x(), max.y(), min.z()), // top front right
		Vector4D(max.x(), max.y(), max.z()), // top back right

		Vector4D(max.x(), min.y(), min.z()), // bottom front right
		Vector4D(max.x(), min.y(), max.z())  // bottom back right
	};

	int edgeIndices[][2] = {
		{0, 1}, {0, 2}, {1, 3}, {3, 2}, // left side face
		{5, 4}, {4, 6}, {5, 7}, {7, 6}, // right side face
		{0, 4}, {2, 6}, 				// front side face (the others are shared with other faces)
		{1, 5}, {3, 7}					// back side face
	};

	for (int i = 0; i < 12; ++i){
		edges.push_back(Line(verts[edgeIndices[i][0]], 
			verts[edgeIndices[i][1]]));

	}

    return edges;
}

std::vector<Vector4D> CollisionManifold::checkEdgesToAABB(const std::vector<Line> &edges, const GraphicsNode& gn)
{
	std::vector<Vector4D> results;
	results.reserve(edges.size() * 3);
	Vector4D intersection;

	std::vector<Plane> planes = getPlanes(gn);

	for (int i = 0; i < planes.size(); ++i) {
		for (int j = 0; j < edges.size(); ++j){
			if(checkEdgesToPlane(planes[i], edges[j], intersection)){
				if(pointInAABB(intersection, gn)){
					results.push_back(intersection);
				}
			}
		}
	}
    return results;
}

std::vector<Plane> CollisionManifold::getPlanes(const GraphicsNode& gn)
{
	std::vector<Plane> planes;
	planes.resize(6);
	Vector4D axis[] = {
		Vector4D(1.0f, 0.0f, 0.0f),
		Vector4D(0.0f, 1.0f, 0.0f),
		Vector4D(0.0f, 0.0f, 1.0f)
	};

	Vector4D newCenter = gn.AABBCenter;
	Vector4D size = gn.AABBSize;
	size = size * 0.5f; // we need to use half extents here
	planes[0] = Plane(axis[0], Vector4D::dot(axis[0], (newCenter + axis[0] * size[0])));
	planes[1] = Plane(axis[0] * -1.0f, -Vector4D::dot(axis[0], (newCenter - axis[0]) * size[0]));
	planes[2] = Plane(axis[1], Vector4D::dot(axis[1], (newCenter + axis[1] * size[1])));
	planes[3] = Plane(axis[1]  * -1.0f, -Vector4D::dot(axis[1], (newCenter - axis[1] * size[1])));
	planes[4] = Plane(axis[2], Vector4D::dot(axis[2], (newCenter + axis[2] * size[2])));
	planes[5] = Plane(axis[2]  * -1.0f, -Vector4D::dot(axis[2], (newCenter - axis[2] * size[2])));

    return planes;
}

bool CollisionManifold::checkEdgesToPlane(const Plane &plane, const Line &edge, Vector4D &resultPoint)
{
	Vector4D edgeVector = edge.end - edge.start;
	
	float normalEdge = Vector4D::dot(plane.getNormal(), edge.start);
	float normalEdgeAngle = Vector4D::dot(plane.getNormal(), edgeVector);

	if(normalEdgeAngle == 0){
		return false;
	}

	float t = (plane.getDistance() - normalEdge) / normalEdgeAngle;
	if(t >= 0.0f && t <= 1.0f) {
		resultPoint = edge.start + edgeVector * t;
		return true;
	}

    return false;
}

bool CollisionManifold::pointInAABB(const Vector4D &point, const GraphicsNode &gn)
{
	Vector4D direction = point - gn.AABBCenter; //direction to point
	Vector4D axis[] = {
		Vector4D(1.0f, 0.0f, 0.0f),
		Vector4D(0.0f, 1.0f, 0.0f),
		Vector4D(0.0f, 0.0f, 1.0f)

	};
	for(int i = 0; i < 3; ++i){
		float distance = Vector4D::dot(direction, axis[i]);
		
		if(distance > gn.AABBSize.get(i)){
			return false;
		}

		if(distance < -gn.AABBSize.get(i)){
			return false;
		}

	}

    return true;
}