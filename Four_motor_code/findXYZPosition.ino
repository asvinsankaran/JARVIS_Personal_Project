// float cornerLocations[4][3] = {{0,EW,0},{0,0,0},{NS,EW,0},{NS,0,0}};  // Coordinates of each corner
// float wireRadii[4] = {25056, 17307, 35721, 35713};           // Radii of each wire

// Define the cost function
float costFunction(float moduleLocation[3]) {
    float sumSquaredDistances = 0.0;
    for (int i = 0; i < 4; ++i) {
        float distance = sqrt(pow(moduleLocation[0] - cornerLocations[i][0], 2) +
                              pow(moduleLocation[1] - cornerLocations[i][1], 2) +
                              pow(moduleLocation[2] - cornerLocations[i][2], 2));
        sumSquaredDistances += pow(distance - wireRadii[i], 2);
    }
    return sumSquaredDistances;
}

// Gradient descent optimization
void gradientDescent(float moduleLocation[3], float alpha, int maxIterations, float epsilon) {
    for (int iteration = 0; iteration < maxIterations; ++iteration) {
        float gradients[3] = {0.0, 0.0, 0.0};

        // Calculate gradients
        for (int i = 0; i < 3; ++i) {
            float moduleLocationDelta = moduleLocation[i] * 0.001; // Small delta for numerical gradient calculation
            float moduleLocationPlusDelta[3];
            float moduleLocationMinusDelta[3];

            memcpy(moduleLocationPlusDelta, moduleLocation, 3 * sizeof(float));
            moduleLocationPlusDelta[i] += moduleLocationDelta;
            gradients[i] = (costFunction(moduleLocationPlusDelta) - costFunction(moduleLocation)) / moduleLocationDelta;
        }

        // Update module location
        for (int i = 0; i < 3; ++i) {
            moduleLocation[i] -= alpha * gradients[i];
        }

        // Check for convergence
        if (costFunction(moduleLocation) < epsilon) {
            break;
        }
    }
}

// void setup() {
//     Serial.begin(57600);
//     Serial.println("start");

//     // Set up cornerLocations and wireRadii with actual values

//     // Initial guess for the module locatin
//     float moduleLocation[3] = {100.0, 100.0, 100.0};

//     // Gradient descent parameters
//     float alpha = 0.1;
//     int maxIterations = 100;
//     float epsilon = 0.001;

//     // Perform gradient descent optimization
//     gradientDescent(moduleLocation, alpha, maxIterations, epsilon);

//     // Output the result
//     Serial.print("Module Location: ");
//     Serial.print(moduleLocation[0]);
//     Serial.print(", ");
//     Serial.print(moduleLocation[1]);
//     Serial.print(", ");
//     Serial.println(moduleLocation[2]);
// }

// void loop() {
//     // Nothing to do in the loop
// }
