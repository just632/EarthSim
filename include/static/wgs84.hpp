#pragma once
#include <glm/glm.hpp>
#include <cmath>




namespace WGS84 {
     glm::vec3 toCartesian(double latitude,double longitude,  double altitude = 0.0f);
     glm::vec3 surfaceNormal(double latitude,double longitude);
     glm::vec3 toGeodetic(const glm::vec3& position);
     double gravityOnSurface(double latitude);
     double gravityAtHeight(double latitude, double altitude);
     constexpr double UnitToMeterRatio = 0.0001q;
     constexpr double A = 6378137.0q * UnitToMeterRatio;           // Semi-major Axis in engine units
     constexpr double B = 6356752.314245q * UnitToMeterRatio;     // Semi-minor Axis in engine units
     constexpr double F = (A - B) / A;                            // Flattening Factor of the Earth
    
    // Geocentric Gravitational Constant in engine units³/s²
     constexpr double GM = 3.986004418e14q * (UnitToMeterRatio * UnitToMeterRatio * UnitToMeterRatio); 
     constexpr double g0 = 9.780327* (UnitToMeterRatio * UnitToMeterRatio); // Gravity at the equator
     constexpr double W = 7.292115e-5q;                           // Earth’s Nominal Mean Angular Velocity in rad/s
     constexpr double E = 0.081819190842622q;                     // Ellipsoid's first eccentricity
     constexpr double E2 = (A*A - B*B) / (A*A);                   // Ellipsoid's first eccentricity
     constexpr double M = 3986004.418e8q;                          // earths mass in kg
    
     constexpr glm::vec3 gravity = glm::vec3(0.q,-g0,0.q);
};