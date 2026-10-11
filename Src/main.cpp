#include <iostream>
#include <format>
#include <string>
#include <string_view>
#include <vector>
#include <cstdint>
#include <cstdlib>
#include <algorithm>
#include "RayTracing.h"
#include "Math/Vector.h"
#include "Image.h"
#include "InstrumentorTimer.h"
#include "print"


using namespace DSM;

namespace {
    // 判断参数是否为「纯数字」：允许前导 +/- 与小数点，且必须整串都是数字。
    // "400" / "50" / "1"  -> true
    // "out.ppm" / "1.ppm" -> false（后者因为末尾不是数字）
    bool IsNumericArg(std::string_view token)
    {
        if (token.empty()) {
            return false;
        }
        const std::string text{token};
        char* endPtr = nullptr;
        std::strtod(text.c_str(), &endPtr);
        return endPtr != nullptr && endPtr != text.c_str() && *endPtr == '\0';
    }

    void PrintUsage(const char* exeName)
    {
        std::cout << std::format(
            "Usage: {} [<Width> <Height> <SampleCount> <MaxDepth> <UseMultithread>] [OutputFilename]\n"
            "  Width / Height  : 图像分辨率                (默认 400 x 400)\n"
            "  SampleCount     : 每像素采样数              (默认 10)\n"
            "  MaxDepth        : 光线最大反弹深度          (默认 0，0 表示沿用各场景内置深度)\n"
            "  UseMultithread  : 1 开启多线程 / 0 单线程   (默认 1)\n"
            "  OutputFilename  : 输出 PPM 文件名           (默认 output.ppm)\n"
            "\n示例:\n"
            "  {} 800 600 100 50 1 cornell.ppm   # 多线程, 深度 50\n"
            "  {} 800 600 100 50 0               # 单线程, 输出 output.ppm\n"
            "  {} 400 400 10                     # 其余参数全部使用默认值\n",
            exeName, exeName, exeName, exeName);
    }
}

int main(int argc, char* argv[])
{
    // 位置参数按顺序解析为数值；第一个非数值参数视为输出文件名
    std::vector<double> numericArgs;
    std::string outputFile;
    bool hasOutputFile = false;

    for (int i = 1; i < argc; ++i) {
        const std::string_view token{argv[i]};
        if (IsNumericArg(token)) {
            numericArgs.push_back(std::atof(std::string{token}.c_str()));
        }
        else if (!hasOutputFile) {
            outputFile = std::string{token};
            hasOutputFile = true;
        }
        else {
            std::cerr << std::format("Error: 只能指定一个输出文件名，多余参数: {}\n", token);
            PrintUsage(argv[0]);
            return -1;
        }
    }

    constexpr std::size_t kMaxNumericArgs = 5;
    if (numericArgs.size() > kMaxNumericArgs) {
        std::cerr << std::format(
            "Error: 数值参数最多 {} 个 (Width Height SampleCount MaxDepth UseMultithread)，实际 {}\n",
            kMaxNumericArgs, numericArgs.size());
        PrintUsage(argv[0]);
        return -1;
    }

    const float width  = numericArgs.size() > 0 ? std::max(static_cast<float>(numericArgs[0]), 1.0f) : 400.0f;
    const float height = numericArgs.size() > 1 ? std::max(static_cast<float>(numericArgs[1]), 1.0f) : 400.0f;
    const float sample = numericArgs.size() > 2 ? std::max(static_cast<float>(numericArgs[2]), 1.0f) : 10.0f;
    const int   depth  = numericArgs.size() > 3 ? std::max(static_cast<int>(numericArgs[3]), 0) : 0;
    const bool  useMultithread = numericArgs.size() > 4 ? (numericArgs[4] != 0.0) : true;

    if (!hasOutputFile) {
        outputFile = "output.ppm";
    }

    Instrumentor::BeginSession("RayTracing");
    RayTracing rayTracing(width / height,
                          static_cast<std::uint32_t>(width),
                          static_cast<std::uint32_t>(sample),
                          depth,
                          useMultithread);
    rayTracing.Render();
    Instrumentor::EndSession();

    const auto& image = rayTracing.GetScene(0).GetCameraImage();
    image.SaveToFilePPM(outputFile);
    std::clog << std::format("Image saved to {}\n", outputFile);

    return 0;
}
