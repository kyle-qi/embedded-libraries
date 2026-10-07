#include "nine_dof.h"

// --- MAIN ---

int main(){
    // Initialize the Kalman Filter
    // Tbh I think this is all already implemented outside of main
    while(1){
        // Read sensor
        Matrix<float, 3, 1> gyro_read = readGyro();

        // Predict
        extrapolateState(gyro_read);
        extrapolateCov(gyro_read);

        // Update
        Matrix<float, 1, 1> K = KalmanGain();
        updateState(K);
        updateCov(K);

        // Print the state and cov
        // TODO: Implement this
    }
}
