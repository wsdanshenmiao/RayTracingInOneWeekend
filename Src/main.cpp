#include <iostream>
#include <format>
#include "RayTracing.h"
#include "Math/Vector.h"
#include "Image.h"
#include "InstrumentorTimer.h"

using namespace DSM;

int main(int argc, char* argv[])
{
    bool useArg = argc == 5;
    if (argc != 1 && argc != 4 && argc != 5) {
        std::cout << std::format("Usage: {} <Width> <Height> <Sample Count> <Output Filename>\n", argv[0]);
        return -1;
    }

    Vector3f args{400, 400, 10};
    for (int i = 1 ; i < argc && i < 4 ; i++) {
        args[i - 1] = std::max(std::atof(argv[i]), 1.);
    }

    Instrumentor::BeginSession("RayTracing");
    RayTracing rayTracing(args[0] / args[1], args[0], args[2]);
    auto& image = rayTracing.Render();
    Instrumentor::EndSession();
	
    if (useArg) {
		std::cout << std::format("Output Filename: {}\n", argv[4]);
        image.SaveToFilePPM(argv[4]);
    }
    else {
		image.SaveToFilePPM("output.ppm");
		std::clog << "Image saved to output.ppm\n";
    }

    return 0;
}