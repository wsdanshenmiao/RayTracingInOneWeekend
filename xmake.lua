projectName = "RayTracing"
set_project(projectName)

add_rules("mode.debug", "mode.release")
set_languages("c99", "cxx20")
set_encodings("utf-8")

if is_mode("debug") then 
    binDir = path.join(os.projectdir(), "Bin/Debug")
else 
    binDir = path.join(os.projectdir(), "Bin/Release")
end 

target(projectName)
    set_kind("binary")
    set_targetdir(path.join(binDir, projectName))

    add_files("Src/**.cpp")
    add_headerfiles("Src/**.h")
target_end()