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
class Particle{

public:
    Particle();
    vec3 gridIdx;
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
    MACGrid();
    void initialize();
    MACGrid& operator=(const MACGrid& val);
    MACGridDataX vel_U;
    MACGridDataY vel_V;
    MACGridDataZ vel_W;
    MACGridData P;
protected:
};

class FluidSolver{
public:
    FluidSolver();

    std::vector<float> forces; // at the center of grid
    std::vector<float> vel_u; //u,v,w components of velocity
    std::vector<float> vel_v;
    std::vector<float> vel_w;

    std::unordered_map<int, std::vector<Particle*>> hash_grid; //improve here for faster particle to grid access
    std::vector<float> vel_u_current;
    std::vector<float> vel_v_current;
    std::vector<float> vel_w_current;

    std::vector<float> vel_u_change;
    std::vector<float> vel_v_change;
    std::vector<float> vel_w_change;
    std::vector<int> mark;

    MACGrid grid;
    MACGrid tmp;

    int i_size;
    int j_size;

    int num_cells;
    vec3 containerBounds;

    int LastUsedParticle; int MaxParticles;
    std::vector<Particle> ParticlesContainer;

    std::vector<Particle> particle_save;
    std::vector<Particle> particle_save_pic;//to save particle velocity

    void constructMACGrid(glm::vec3 containerBounds);
    void initMACGrid(Particle &p);
    void storeParticleVelocityToGrid();
    void storeCurrentGridVelocities();

    void clearGrid();

    void naiveNeighborSearch(Particle *p, std::vector<Particle> &neighbors);

    void initializeMarkerGrid();

    void buildMatrixA(std::vector<Eigen::Triplet<double> > &coefficients, long n);
    void buildDivergences(Eigen::VectorXd& u, int n);
    void fillPressureGrid(Eigen::VectorXd x, int n);

    void insertCoefficient(int id, int i, int j, int k, double w, std::vector<Eigen::Triplet<double> > &coeffs, int n);

    void calculateNewGridVelocities();
    void setBoundaryVelocitiesToZero(const glm::vec3 containerBounds);
    void FlipSolve();
    void PicSolve();

    void ProjectPressure();

    vec3 integratePos(const vec3 pos, const vec3 speed, float time_step, bool RK2);

    void calculateDensity(Particle &p);
    void calculateGravityForces(Particle& p, float delta);
    void ExtrapolateVelocity();
    int findUnusedParticles();
    void sortParticles();
    void particlesInit();
    void genParticles(float particle_separation, float boundx, float boundy, float boundz);

};
float Smooth(const float& r2, const float& h);
float Sharpen(const float& r2, const float& h);
float StiffKernel(const vec3 &r, const float& h);
float Sqrlength(const glm::vec3& p0, const glm::vec3& p1);
#endif /* fluidSolver_hpp */
