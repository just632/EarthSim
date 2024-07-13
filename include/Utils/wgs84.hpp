#ifndef WGS84_H
#define WGS84_H

#include <glm/glm.hpp>
#include <cmath>




class WGS84 {
public:
    static glm::vec3 toCartesian(float longitude, float latitude, float height = 0.0f);
    static glm::vec3 surfaceNormal(float longitude, float latitude);
    static glm::vec3 toGeodetic(const glm::vec3& position);
    static constexpr float A = 6378137.0f;                // Semi-major Axis : meters
    static constexpr float B = 6356752.314245f;           // Semi-minor Axis : meters
    static constexpr float F = 0.0033528106647474805f;    // Flattening Factor of the Earth
    static constexpr float GM = 3.986004418e14f;          // Geocentric Gravitational Constant in m^3/s^2
    static constexpr float W = 7292115e-11f;              // Earth’s Nominal Mean Angular Velocity in rad/s
    static constexpr float E = 0.081819190842622f;        // Ellipsoid's first eccentricity
    static constexpr float E2 = 6.6943799901414e-3;       // Ellipsoid's first eccentricity Squared
};

#endif // WGS84_H
