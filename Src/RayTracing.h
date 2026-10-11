#pragma once
#ifndef __RAYTRACING__H__
#define __RAYTRACING__H__

#include "Geometry/HittableList.h"
#include "Camera.h"
#include "Scene.h"
#include <cstdint>
#include <string>
#include <vector>

namespace DSM {
    class HittableList;
	class Image;
    
    
    class RayTracing
    {
    public:
        RayTracing(float aspectRatio, std::uint32_t width, std::uint32_t samplePerPixel,
                   int maxDepth = 0, bool useMultithread = true);

        void Render();
        const Scene& GetScene(std::size_t index) const; 

    private:
        // 将全局渲染设置（最大深度、多线程开关）下发到所有已构建的场景
        void ApplyRenderSettings();
        // 打印各场景耗时汇总报告
        void PrintTimingSummary() const;

        void BuildScene0();
        void BuildScene1();
        void BuildScene2();
        void BuildScene3();
        void CornellBox();
        void SampleLight();
        void CornellSmoke();
        void FinalScene();
        
    private:
        const float m_AspectRatio;
        std::uint32_t m_Width;
        std::uint32_t m_SamplePerPixel;
        int m_MaxDepth = 0;                 // 0 表示沿用各场景内置深度
        bool m_UseMultithread = true;

        std::vector<std::unique_ptr<Scene>> m_Scenes;
        std::vector<double> m_SceneTimes;   // 各场景光追耗时（毫秒）
    };

    
}

#endif
