projectName = "RayTracing"
set_project(projectName)

add_rules("mode.debug", "mode.release")
set_languages("c99", "cxx20")
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
target_end()