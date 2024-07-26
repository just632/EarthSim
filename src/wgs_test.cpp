#include "static/wgs84.hpp"
#include <iostream>
#include <sstream>
#include <string>

int main() {
    std::ostringstream oss;
    
    // Testing geodetic conversion
    auto geoPos = WGS84::toGeodetic(glm::vec3(0.f, WGS84::A + 100/WGS84::UnitToMeterRatio, 0.f));
    oss << "geoPos(lat: " << geoPos.x << " , lon: " << geoPos.y << " , alt: " << geoPos.z << " )\n";
    
    // Testing Cartesian conversion
    auto pos = WGS84::toCartesian(geoPos.x, geoPos.y, geoPos.z);
    oss << "pos(x: " << pos.x << " , y: " << pos.y << " , z: " << pos.z << " )\n";
    
    // Testing surface normal
    auto norm = WGS84::surfaceNormal(geoPos.x, geoPos.y);
    oss << "norm(x: " << norm.x << " , y: " << norm.y << " , z: " << norm.z << " )\n";
    
    // Testing gravity models
    double lat = geoPos.y; // Latitude from geodetic position
    double height = geoPos.z; // Altitude from geodetic position

    double g_surface = WGS84::gravityOnSurface(lat);
    oss << "Gravity on surface at latitude " << lat << " : " << g_surface << " u/s^2\n";

    double g_height = WGS84::gravityAtHeight(lat, height);
    oss << "Gravity at height " << height << " m at latitude " << lat << " : " << g_height << " u/s^2\n";
    std::cout << oss.str() << std::endl;

    return 0;
}