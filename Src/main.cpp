#include <iostream>
#include "RayTracing.h"
#include "Math/Vector.h"
#include <format>

using namespace DSM;

int main(int argc, char* argv[])
{
    if (argc != 1 && argc != 4) {
        std::cout << std::format("用法: {}{}{}", argv[0], " <宽度>", " <高度>", " <采样数>") << '\n';
        return -1;
    }
    Vector3f args{400, 400, 10};
    for (int i = 1 ; i < argc ; i++) {
        args[i - 1] = std::atof(argv[i]);
    }
    RayTracing rayTracing(args[0] / args[1], args[0], args[2]);
    rayTracing.Render();
    return 0;
}