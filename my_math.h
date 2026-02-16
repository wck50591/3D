
// ========================================================================
//    Name : own math (my_math.h)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2026/01/16
// ========================================================================
#ifndef MY_MATH_H
#define MY_MATH_H

// get which is larger based on a number
template <typename T>
T clamp(T l, T r1, T r2) {
	if (l < r1) {
		return r1;
	}
	if (l > r2) {
		return r2;
	}
	return l;
}

// get the number with power
// O(n)
template <typename T>
T pow(T l, int r) {
	if (r == 0) return 1;
	if (r == 1) return l;

	return l * pow(l, r - 1);
}

// get the number with power using mod (fewer_step)
// O(log n)
template <typename T>
T pow_m(T l, int r) {
	if (r == 0) return T(1);

	if (r % 2 == 0) {
		T h = pow_m(l, r/2);
		return h * h;
	}
	else {
		return l * pow_m(l, r - 1);
	}
}

inline float EaseOutQuint(float t) {
	return 1.0f - pow(1.0f - t, 5);
}

#endif // !MY_MATH_H
