#pragma once
#include <cmath>
#include <glm/glm.hpp>
#include "static/wgs84.hpp"
#include <iostream>

class Position {
public:
    // Constructor
    Position(double lat, double lon, double alt)
        : latitude(lat), longitude(lon), altitude(alt) {
        updateECEF();
    }

    Position(glm::vec3 pos) {
        updateECEF(pos);
        updateGeodetic();
    }

    // Getters
    double getLatitude() const { return latitude; }
    double getLongitude() const { return longitude; }
    double getAltitude() const { return altitude; }
    void getECEF(double& x, double& y, double& z) const {
        x = ecefX; y = ecefY; z = ecefZ;
    }

    const glm::vec3 getECEF() const {
        return glm::vec3(ecefX, ecefY, ecefZ);
    }

    void getGeodetic(double& lat, double& lon, double& alt) const {
        lat = latitude; lon = longitude; alt = altitude;
    }

    const glm::vec3 getGeodetic() const {
        return glm::vec3(ecefX, ecefY, ecefZ);
    }

    void getVelocity(double& vx, double& vy, double& vz) const {
        vx = velocityX; vy = velocityY; vz = velocityZ;
    }

    const glm::vec3 getVelocity() const {
        return glm::vec3(velocityX, velocityY, velocityZ);
    }

    // Setters
    void setLatitude(double lat) {
        latitude = lat;
        updateECEF();
    }
    void setLongitude(double lon) {
        longitude = lon;
        updateECEF();
    }
    void setAltitude(double alt) {
        altitude = alt;
        updateECEF();
    }

    void setVelocity(const glm::vec3 &vel) {
        velocityX = vel.x;
        velocityY = vel.y;
        velocityZ = vel.z;
    }

    // Update Position
    void updatePosition(double deltaLat, double deltaLon, double deltaAlt) {
        latitude += deltaLat;
        longitude += deltaLon;
        altitude += deltaAlt;
        updateECEF();
    }

    // Add Force
    void addForce(double forceX, double forceY, double forceZ) {
        totalForceX += forceX;
        totalForceY += forceY;
        totalForceZ += forceZ;
    }

    // Calculate and Apply Forces
    void calculateAndApplyForces(double mass, double deltaTime) {
        applyForce(totalForceX, totalForceY, totalForceZ, mass, deltaTime);

        // Reset total forces after applying
        totalForceX = 0.0;
        totalForceY = 0.0;
        totalForceZ = 0.0;
    }

    // Apply Force
    void applyForce(double forceX, double forceY, double forceZ, double mass, double deltaTime) {
        // Update velocity based on force and mass
        velocityX += (forceX / mass) * deltaTime;
        velocityY += (forceY / mass) * deltaTime;
        velocityZ += (forceZ / mass) * deltaTime;

        // Update position based on velocity
        ecefX += velocityX * deltaTime;
        ecefY += velocityY * deltaTime;
        ecefZ += velocityZ * deltaTime;

        updateGeodetic();
    }

private:
    // Geodetic coordinates
    double latitude;
    double longitude;
    double altitude;

    // ECEF coordinates
    double ecefX;
    double ecefY;
    double ecefZ;

    // Velocity
    double velocityX = 0;
    double velocityY = 0;
    double velocityZ = 0;

    // Total forces
    double totalForceX = 0.0;
    double totalForceY = 0.0;
    double totalForceZ = 0.0;

    // Update ECEF coordinates from geodetic coordinates
    void updateECEF() {
        updateECEF(WGS84::toCartesian(latitude,longitude, altitude));
    }

    void updateECEF(glm::vec3 pos) {
        ecefX = pos.x;
        ecefY = pos.y;
        ecefZ = pos.z;
    }

    // Update geodetic coordinates from ECEF coordinates
    void updateGeodetic() {
        auto geoPos = WGS84::toGeodetic(glm::vec3(ecefX, ecefY, ecefZ));
        latitude = geoPos[0];
        longitude = geoPos[1];
        altitude = geoPos[2];
    }
};

class Force {
public:
    // Constructor
    Force(double mass) : mass(mass) {}
    
    // Virtual destructor
    virtual ~Force() = default;

    // Virtual apply method
    virtual void apply(Position& position, double deltaTime) = 0;

protected:
    double mass;
};

class GravityForce : public Force {
public:
    GravityForce(double mass) : Force(mass) {}

    void apply(Position& position, double deltaTime) override {
        double x, y, z;
        position.getECEF(x, y, z);
        double lat, lon, alt;
        position.getGeodetic(lat, lon, alt);

        // Using the WGS84 gravity model to calculate gravity at this height
        double g = WGS84::gravityAtHeight(lat, alt);

        // Get the surface normal at the current geodetic position
        glm::vec3 normal = WGS84::surfaceNormal(lat, lon);

        double forceMagnitude = g * mass;

        // The force is in the opposite direction of the normal
        double forceX = -forceMagnitude * normal.x;
        double forceY = -forceMagnitude * normal.y;
        double forceZ = -forceMagnitude * normal.z;
        position.addForce(forceX, forceY, forceZ);
    }
};

class DragForce : public Force {
public:
    DragForce(double mass, double dragCoefficient)
        : Force(mass), dragCoefficient(dragCoefficient) {}

    void apply(Position& position, double airDensity, double deltaTime) {
        double vx, vy, vz;
        position.getVelocity(vx, vy, vz);

        double speed = std::sqrt(vx * vx + vy * vy + vz * vz);
        double dragForceMagnitude = 0.5 * dragCoefficient * airDensity * speed * speed;

        double forceX = -dragForceMagnitude * (vx / speed);
        double forceY = -dragForceMagnitude * (vy / speed);
        double forceZ = -dragForceMagnitude * (vz / speed);

        position.addForce(forceX, forceY, forceZ);
    }

    void apply(Position& position, double deltaTime) override {
        double vx, vy, vz;
        position.getVelocity(vx, vy, vz);

        double speed = std::sqrt(vx * vx + vy * vy + vz * vz);
        double dragForceMagnitude = 0.5 * dragCoefficient * 1.225 * speed * speed;

        double forceX = -dragForceMagnitude * (vx / speed);
        double forceY = -dragForceMagnitude * (vy / speed);
        double forceZ = -dragForceMagnitude * (vz / speed);

        position.addForce(forceX, forceY, forceZ);
    }

private:
    double dragCoefficient;
};

class LiftForce : public Force {
public:
    LiftForce(double mass, double liftCoefficient,double wingArea)
        : Force(mass), liftCoefficient(liftCoefficient),wingArea(wingArea) {}

    void apply(Position& position, double airDensity, double wingArea, double deltaTime) {
        double vx, vy, vz;
        position.getVelocity(vx, vy, vz);
        double lat, lon, alt;
        position.getGeodetic(lat, lon, alt);

        double speed = std::sqrt(vx * vx + vy * vy + vz * vz);
        double liftForceMagnitude = 0.5 * liftCoefficient * airDensity * speed * speed * wingArea;

        glm::vec3 normal = WGS84::surfaceNormal(lat, lon);

        // Assuming lift acts perpendicularly to the velocity in the vertical direction
        double forceX = liftCoefficient * (vx / speed);
        double forceY = liftCoefficient * (vy / speed);
        double forceZ = liftCoefficient * (vz / speed);

        position.addForce(forceX, forceY, forceZ);
    }

        void apply(Position& position, double deltaTime) override {
        double vx, vy, vz;
        position.getVelocity(vx, vy, vz);
        double lat, lon, alt;
        position.getGeodetic(lat, lon, alt);

        double speed = std::sqrt(vx * vx + vy * vy + vz * vz);
        double liftForceMagnitude = 0.5 * liftCoefficient * 1.225 * speed * speed * wingArea;

        glm::vec3 normal = WGS84::surfaceNormal(lat, lon);

        // Assuming lift acts perpendicularly to the velocity in the vertical direction
        double forceX = liftCoefficient * (vx / speed);
        double forceY = liftCoefficient * (vy / speed);
        double forceZ = liftCoefficient * (vz / speed);

        position.addForce(forceX, forceY, forceZ);
    }

private:
    double liftCoefficient;
    double wingArea;
};

class ThrustForce : public Force {
public:
    ThrustForce(double mass) : Force(mass) {}

    void apply(Position& position, double thrustX, double thrustY, double thrustZ, double deltaTime) {
        position.addForce(thrustX, thrustY, thrustZ);
    }
};