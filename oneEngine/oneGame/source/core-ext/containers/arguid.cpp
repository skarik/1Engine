#include "arguid.h"

// Define a generator
std::mt19937 arguid32::generator((std::random_device())());