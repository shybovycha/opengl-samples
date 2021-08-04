add_requires("sfml ~2.5.1", { alias = "sfml" })
add_requires("glm")
add_requires("vcpkg::globjects ~1.1.0", { alias = "globjects" })
add_requires("vcpkg::glbinding", { alias = "glbinding" })
add_requires("vcpkg::assimp", { alias = "assimp" })

target("24-screen-space-ambient-occlusion")
  set_kind("binary")

  add_packages("sfml", "glm", "globjects", "glbinding", "assimp")

  if is_plat("macosx") then
    -- this prevents "-[SFOpenGLView enableKeyRepeat]: unrecognized selector sent to instance 0x7fa5c2507970" runtime exception
    add_ldflags("-ObjC")

    -- this prevents linker errors
    add_frameworks("Foundation", "OpenGL", "IOKit", "Cocoa", "Carbon")
  end

  add_files("main.cpp")

  after_build(function (target)
    os.cp("$(scriptdir)/media", path.join(path.directory(target:targetfile()), "media"))
  end)
