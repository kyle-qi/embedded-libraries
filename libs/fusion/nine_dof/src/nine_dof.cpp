// Biggest TODOs:
// Wire in the Eigen Library
// Port everything over so it's object oriented
// Wire in the sensor readings
// Finish all the stubs
// Figure out how tf we choose our covariance matrices???

#include "nine_dof.h"

// --- CONFIGURATION VARIABLES ---

// Kalman Filter Update Frequency in ms
static constexpr int period = 10;

// Construct state vector x
Matrix<float, 3, 1> x = {0, 0, 0};

// Construct and initialize the covariance matrix P
Matrix<float, 3, 3> P = {
    {0, 0, 0}
    {0, 0, 0}
    {0, 0, 0}
};

// Construct and initialize the process noise matrix Q
Matrix<float, 3, 3> Q = {
    {0, 0, 0}
    {0, 0, 0}
    {0, 0, 0}
};

// Construct and initialize the measurement noise matrix R
// For now this assumes that the noise is static but I'll probably
// turn this into a function later so that faster gyro movements means
// higher measurement noise or whatever
Matrix<float, 3, 3> R = {
    {0, 0, 0}
    {0, 0, 0}
    {0, 0, 0}
}; 

// UTILITY FUNCTIONS

// Construct matrix F
// TODO: Implement this properly
Matrix<float, 3, 3> makeF(Matrix<float, 3, 1> gyro){
    Matrix<float, 3, 3> F = {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0}
    };
    return F;
}

// Stub until I wire in the actual gyro read
Matrix<float, 3, 1> readGyro(){
    Matrix<float, 3, 1> g = {0, 0, 0};
    return g;
}

// --- PREDICT STAGE FUNCTIONS ---

// Predict state
// This function takes hat{x}_{n,n} to hat{x}_{n+1,n}
void extrapolateState(Matrix<float, 3, 1> gyro){
    x(0) += gyro(0)*period;
    x(1) += gyro(1)*period;
    x(2) += gyro(2)*period;
}

// Predict Uncertainty
// This function takes P_{n,n} to P_{n+1,n}
void extrapolateCov(Matrix<float, 3, 1> gyro){
    Matrix<float, 3, 3> F = constructF(gyro);
    Matrix<float, 3, 3> F_T = F.transpose();
    return F*P*F_T + Q
}

// --- UPDATE STAGE FUNCTIONS ---

Matrix<float, 3, 3> kalmanGain(){
    // H = I therefore the equation simplifies a lot
    Matrix<float, 3, 3> K = P*((P + R).inv())
}

void updateState(Matrix<float, 3, 3> kalman){
    // Stub for now
}

void updateCov(Matrix<float, 3, 3> kalman){
    // Stub for now
}