#pragma once


#ifdef M_PI
#undef M_PI
#endif
#define M_PI 3.14159265358979323846

#ifdef INF
#undef INF
#endif
#define INF 1.0e10

inline double Clamp(double x, double low = 0, double high = 1) {
	return (x < high) ? ((x > low) ? x : low) : high;
}

inline uint8_t ToByte(double x, double gamma = 2.2) {
	return (uint8_t)Clamp(255.0 * pow(x, 1 / 1), 0.0, 255.0);
}