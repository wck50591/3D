
// ========================================================================
//    Name : spline (spline.h)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2026/01/16
// ========================================================================

#ifndef SPLINE_H
#define SPLINE_H

#include <DirectXMath.h>
#include <vector>

class CatmullRomSpline {
private:
	std::vector<DirectX::XMFLOAT3> m_Points;

public:
	void AddPoint(const DirectX::XMFLOAT3& points);
	void Clear();

	DirectX::XMFLOAT3 GetPosition(float t) const;
};

#endif // !SPLINE_H
