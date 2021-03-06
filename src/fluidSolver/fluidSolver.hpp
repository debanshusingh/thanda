//
//  fluidSolver.hpp
//  Thanda

#ifndef fluidSolver_hpp
#define fluidSolver_hpp
#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <algorithm>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include "macgriddata.h"
#include <iostream>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/Sparse>
#include <unordered_map>

using namespace glm;

enum geomtype {AIR = 0, FLUID = 1, SOLID = 2};

class Scene;
class Particle{

public:
    Particle();
    ivec3 gridIdx;
    glm::vec3 pos, speed;
    unsigned char r,g,b,a; // Color
    float size, angle, mass, density;
    float life; // Remaining life of the particle. if <0 : dead and unused.
    float cameradistance; // *Squared* distance to the camera. if dead : -1.0f

    bool operator<(const Particle& that) const {
        // Sort in reverse order : far particles drawn first.
        return this->cameradistance > that.cameradistance;
    }
};

class MACGrid{
public:
    MACGrid(const ivec3& resolution, const vec3& containerBounds, float cellSize);
    ~MACGrid();
    void initialize();
    MACGrid& operator=(const MACGrid& val);
    MACGridDataX* vel_U;
    MACGridDataY* vel_V;
    MACGridDataZ* vel_W;

    MACGridDataX* flip_vel_U;
    MACGridDataY* flip_vel_V;
    MACGridDataZ* flip_vel_W;

    MACGridDataX* save_kernel_wt_U;
    MACGridDataY* save_kernel_wt_V;
    MACGridDataZ* save_kernel_wt_W;
    MACGridData* P;

protected:
};

class FluidSolver{
public:
    FluidSolver(const ivec3& resolution, const vec3 &containerBounds);
    ~FluidSolver();
    MACGrid* grid;

    float delta;
    int num_cells;
    vec3 containerBounds;
    ivec3 resolution;
    float cellSize;

    Scene* scene;

    int LastUsedParticle; int MaxParticles;
    std::vector<Particle> ParticlesContainer;

    std::vector<Particle> particle_save;
    std::vector<Particle> particle_save_pic;//to save particle velocity

    std::map<int, ivec3> reposition_map;

    void constructMACGrid(const Scene& scene);

    void storeParticleVelocityToGrid();
    void storeCurrentGridVelocities();
    void SubtractPressureGradient();

    void step(const float &dt);
    void clearGrid();

    void initializeMarkerGrid();

    void buildMatrixA(std::vector<Eigen::Triplet<double> > &coefficients, long n);
    void buildDivergences(Eigen::VectorXd& u);
    void fillPressureGrid(Eigen::VectorXd x);

    void CalculateGravityToCell(float delta);

    void insertCoefficient(int id, int i, int j, int k, double w, std::vector<Eigen::Triplet<double> > &coeffs);

    void calculateNewGridVelocities();
    void setBoundaryVelocitiesToZero();
    void FlipSolve();
    void PicSolve();

    void ProjectPressure();

    vec3 integratePos(const vec3& pos, const vec3& speed, const float& time_step, bool RK2);

    void calculateGravityForces(Particle& p, float delta);
    void ExtrapolateVelocity();

    void genParticles(float particle_separation, float boundx, float boundy, float boundz, const std::vector<vec3> &obj);

};
#endif /* fluidSolver_hpp */
