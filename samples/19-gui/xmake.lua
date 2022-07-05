add_requires("sfml ~2.5.1", { alias = "sfml" })
add_requires("glm")
add_requires("imgui")
add_requires("vcpkg::globjects", { alias = "globjects" })
add_requires("vcpkg::glbinding", { alias = "glbinding" })
add_requires("assimp")

target("19-gui")
  set_languages("cxx20")
  set_kind("binary")

  add_packages("sfml", "glm", "globjects", "glbinding", "assimp", "imgui")

  if is_plat("macosx") then
    -- this prevents "-[SFOpenGLView enableKeyRepeat]: unrecognized selector sent to instance 0x7fa5c2507970" runtime exception
    add_ldflags("-ObjC")

    -- this prevents linker errors
    add_frameworks("Foundation", "OpenGL", "IOKit", "Cocoa", "Carbon")
  end

  add_files("main.cpp", "ImGuiSfmlBackend.cpp")

  add_defines("HIGH_DPI")

  after_build(function (target)
    os.cp("$(scriptdir)/../media", path.join(path.directory(target:targetfile()), "media"))
    os.cp("$(scriptdir)/media", path.join(path.directory(target:targetfile()), "media"))
  end)
