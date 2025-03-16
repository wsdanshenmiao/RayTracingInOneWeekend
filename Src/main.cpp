#include <iostream>
#include "RayTracing.h"

using namespace DSM;

int main()
{
    RayTracing rayTracing(16.f / 9, 400);
    rayTracing.Render();
    return 0;
}