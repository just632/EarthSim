#include "wgs84.hpp"

glm::vec3 WGS84::toCartesian(double latitude,double longitude, double altitude) {
    double radLong = glm::radians(longitude);
    double radLat = glm::radians(latitude);

    double N = A / sqrt(1.0f - E2 * sin(radLat) * sin(radLat));

    double X = (N + altitude) * cos(radLat) * cos(radLong);
    double Y = (N + altitude) * cos(radLat) * sin(radLong);
    double Z = ((1 - E2) * N + altitude) * sin(radLat);

    return glm::vec3(X, Y, Z);
}

glm::vec3 WGS84::surfaceNormal(double longitude, double latitude) {
    double radLong = glm::radians(longitude);
    double radLat = glm::radians(latitude);

    double X = cos(radLat) * cos(radLong);
    double Y = cos(radLat) * sin(radLong);
    double Z = sin(radLat);

    glm::vec3 normal = glm::vec3(X, Y, Z);
    return glm::normalize(normal);
}

glm::vec3 WGS84::toGeodetic(const glm::vec3& position) {
    double x = position.x;
    double y = position.y;
    double z = position.z;

    double lon = std::atan2(y, x);

    double p = std::sqrt(x * x + y * y);
    double theta = std::atan2(z * A, p * B);
    double e2 = E2 / (1 - E2);

    double lat = std::atan2(z + e2 * B * std::sin(theta) * std::sin(theta) * std::sin(theta),
                           p - E2 * A * std::cos(theta) * std::cos(theta) * std::cos(theta));

    double N = A / std::sqrt(1 - E2 * std::sin(lat) * std::sin(lat));
    double height = p / std::cos(lat) - N;

    return glm::vec3(glm::degrees(lon), glm::degrees(lat), height);
}

// Gravity on the Earth's surface as a function of latitude
double WGS84::gravityOnSurface(double latitude) {
    double radLat = glm::radians(latitude);
    return 9.7803267714 * (1 + 0.00193185138639q * sin(radLat) * sin(radLat)) / 
           sqrt(1 - E2 * sin(radLat) * sin(radLat));
}

// Gravity at a altitude above the Earth's surface
double WGS84::gravityAtHeight(double latitude, double altitude) {
    double g0 = gravityOnSurface(latitude);
    return g0 * pow((A / (A + altitude)), 2);
}

