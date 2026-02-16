
// ========================================================================
//    Name : spline (spline.h)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2026/01/16
// ========================================================================

#include "catmull_rom_spline.h"
using namespace DirectX;
#include "my_math.h"

void CatmullRomSpline::AddPoint(const XMFLOAT3& point)
{
	m_Points.push_back(point);
}

void CatmullRomSpline::Clear()
{
	m_Points.clear();
}

XMFLOAT3 CatmullRomSpline::GetPosition(float dt) const
{
	//---------------------------------
	// Get Points and Calculate Segment
	//---------------------------------

	// points is not enough for calculation, need at least 4 points
	if (m_Points.size() < 4) {
		return {};
	}

	// get the start or end?
	dt = clamp(dt, 0.0f, 1.0f);

	// for calculate the segement base on the points in vector
	// since the spline is calculated by 4 points each segment
	// 
	// the first segment is start from 0
	// 4 points = 1 segment, 5 points = 2 segment, 6 points = 3 segment...
	int segmentCount = m_Points.size() - 3;

	// get the run time based on t (elpased time (detla time))
	// to get the current segment calculated later
	// 
	// t is on 0.6 , 2 segment, and now the run time is 1.2s
	// 0.6 * 2 = 1.2
	// |----0----|----1----|
	//            ^
	// 
	// t is on 1.2 , 2 segment, and now the run time is 2.4s
	// 1.2 * 2 = 2.4
	// |----0----|----1----|--
	//                       ^
	// this will get error since there is no segment 2.
	float totalTime = dt * segmentCount;
	
	// turn totalTime into int 1.3 = 1, 2.4 = 2, 3.7 = 3...
	// to get the segment based on the runTime
	int segment = (int)totalTime;

	// get the run time in current segment, should be 0.0 to 1.0 only
	float timeInSeg = totalTime - segment;
	
	// to pervent getting OUT OF RANGE segment 
	segment = clamp(segment, 0, segmentCount - 1);

	// return 4 points based on the segmemt from totalTime 
	XMFLOAT3 P0 = m_Points[segment];
	XMFLOAT3 P1 = m_Points[segment + 1];
	XMFLOAT3 P2 = m_Points[segment + 2];
	XMFLOAT3 P3 = m_Points[segment + 3];


	//-----------------------------------
	// Calculate Position Based On Points
	//-----------------------------------

	// Based on the formula
	// 
	// p(t) = 1/2 * [t3 t2 t 1] * [-1  3 -3  1] * [P0]
	//							  [ 2 -5  4 -1] * [P1]
	//							  [-1  0  1  0] * [P2]
	// 							  [ 0  2  0  0] * [P3]
	// 
	// we need to calculate the matrix to get the position
	// Let's split it into 4 part and cal the Point and numbers first 
	XMFLOAT3 A,B,C,D;

	A.x = -1 * P0.x +  3 * P1.x + -3 * P2.x +  1 * P3.x;
	A.y = -1 * P0.y +  3 * P1.y + -3 * P2.y +  1 * P3.y;
	A.z = -1 * P0.z +  3 * P1.z + -3 * P2.z +  1 * P3.z;

	B.x =  2 * P0.x + -5 * P1.x +  4 * P2.x + -1 * P3.x;
	B.y =  2 * P0.y + -5 * P1.y +  4 * P2.y + -1 * P3.y;
	B.z =  2 * P0.z + -5 * P1.z +  4 * P2.z + -1 * P3.z;

	C.x = -1 * P0.x +  0 * P1.x +  1 * P2.x +  0 * P3.x;
	C.y = -1 * P0.y +  0 * P1.y +  1 * P2.y +  0 * P3.y;
	C.z = -1 * P0.z +  0 * P1.z +  1 * P2.z +  0 * P3.z;

	D.x =  0 * P0.x +  2 * P1.x +  0 * P2.x +  0 * P3.x;
	D.y =  0 * P0.y +  2 * P1.y +  0 * P2.y +  0 * P3.y;
	D.z =  0 * P0.z +  2 * P1.z +  0 * P2.z +  0 * P3.z;


	// now for the p(t) = 1/2 * [t3 t2 t 1] * [A B C D] part
	XMFLOAT3 position;
	float t = timeInSeg;
	float t2 = t * t;
	float t3 = t2 * t;

	position.x = 0.5 * (t3 * A.x + t2 * B.x + t * C.x + D.x);
	position.y = 0.5 * (t3 * A.y + t2 * B.y + t * C.y + D.y);
	position.z = 0.5 * (t3 * A.z + t2 * B.z + t * C.z + D.z);

	return position; //position
}
