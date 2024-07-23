#include "wgs84.hpp"

glm::vec3 WGS84::toCartesian(float longitude, float latitude, float height) {
    float radLong = glm::radians(longitude);
    float radLat = glm::radians(latitude);

    float N = A / sqrt(1.0f - E2 * sin(radLat) * sin(radLat));

    float X = (N + height) * cos(radLat) * cos(radLong);
    float Y = (N + height) * cos(radLat) * sin(radLong);
    float Z = ((1 - E2) * N + height) * sin(radLat);

    return glm::vec3(X, Y, Z);
}

glm::vec3 WGS84::surfaceNormal(float longitude, float latitude) {
    float radLong = glm::radians(longitude);
    float radLat = glm::radians(latitude);

    float X = cos(radLat) * cos(radLong);
    float Y = cos(radLat) * sin(radLong);
    float Z = (B * B / (A * A)) * sin(radLat);

    glm::vec3 normal = glm::vec3(X / (A * A), Y / (A * A), Z / (B * B));
    return glm::normalize(normal);
}

glm::vec3 WGS84::toGeodetic(const glm::vec3& position) {
    float x = position.x;
    float y = position.y;
    float z = position.z;

    float lon = std::atan2(y, x);

    float p = std::sqrt(x * x + y * y);
    float theta = std::atan2(z * A, p * (1 - F) * A);
    float sinTheta = std::sin(theta);
    float cosTheta = std::cos(theta);

    float lat = std::atan2(z + E2 * (1 - F) * A * sinTheta * sinTheta * sinTheta,
                           p - E2 * A * cosTheta * cosTheta * cosTheta);

    float N = A / std::sqrt(1 - E2 * std::sin(lat) * std::sin(lat));
    float height = p / std::cos(lat) - N;

    return glm::vec3(glm::degrees(lat), glm::degrees(lon), height);
}