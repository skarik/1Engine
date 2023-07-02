#include "aruuid4.h"

// Define a generator
std::mt19937_64 aruuid4::generator((std::random_device())());