projectName = "RayTracing"
set_project(projectName)

add_rules("mode.debug", "mode.release")
set_languages("c99", "cxx23")
set_encodings("utf-8")

if is_mode("debug") then 
    binDir = path.join(os.projectdir(), "bin/debug")
else 
    binDir = path.join(os.projectdir(), "bin/release")
end 

rule("TextureCopy")
    after_build(
        function(target)
            texFiles = path.join(target:scriptdir(), "Textures")
            if(os.exists(texFiles)) then
                os.cp(texFiles, target:targetdir())
            end
        end)
rule_end()

target(projectName)
    set_kind("binary")
    set_targetdir(path.join(binDir, projectName))

    add_files("Src/**.cpp")
    add_headerfiles("Src/**.h")
    add_rules("TextureCopy")

    if is_mode("release") then
        -- 最高优化（MSVC）：
        --   /O2    全速优化（含 /Og /Oi /Ot /Oy /Ob2 /GF /Gy）
        --   /Ob3   激进的函数内联（比 /O2 自带的 /Ob2 更激进）
        --   /Oi    生成内建函数（intrinsic）
        --   /Ot    代码速度优先于体积
        --   /GL    全程序优化（Whole Program Optimization），需链接期配合 /LTCG
        --   /arch:AVX2  为 AVX2 指令集生成代码（本机 Raptor Lake 支持，提升自动向量化）
        --   /fp:fast    快速浮点模型（允许浮点重排/合并，注意可能产生细微数值差异）
        add_cxflags("-O2", "-Ob3", "-Oi", "-Ot", "-GL", "-arch:AVX2", "-fp:fast")
        add_ldflags("-LTCG", "-OPT:REF", "-OPT:ICF")
    end
target_end()

target("RayTracingTests")
    set_kind("binary")
    set_default(false)
    set_targetdir(path.join(binDir, "Tests"))
    add_files("Src/**.cpp", "Tests/**.cpp")
    remove_files("Src/main.cpp", "Src/RayTracing.cpp")
    add_includedirs("Src")
target_end()
