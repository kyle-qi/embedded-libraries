#pragma once

// TODO: Install this library properly
//       Also update the docs to include this dependency
#include "Eigen/Dense"

class NineDOF{
    public:

    // TODO: Include all typedefs here for common matrix sizes I'm going to need
    typedef Matrix<float, 3, 3> Matrix3f;
    typedef Matrix<float, 3, 1> Vector3f;

    explicit NineDOF();

    bool updateState();
    bool initState();

    // This encodes the state transition matrix F
    // This is the extrapolation equation
    Vector3f extrapolateState(Vector3f state){
        state(0) += period*omega_x
        state(1) += period*omega_y
        state(2) += period*omega_z
    };

    Matrix3f extrapolateStateCov(){

    };

    

    // Manually set the state
    bool setState(float roll, float pitch, float yaw){
        this->x(0) = roll;
        this->x(1) = pitch;
        this->x(2) = yaw;
    };

    private:

    // Update period in ms
    float period = 10;

    /**
     * @brief column vector to store current state of pitch, yaw, and roll
     */
    Matrix<float, 3, 1> x = {0, 0, 0};

    /**
    * @brief Matrix of the squared measurement error
    */
    Matrix<float, 3, 3> R = {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
    };

};