add_requires("sfml ~2.5.1", { alias = "sfml" })
add_requires("glm")
add_requires("vcpkg::globjects", { alias = "globjects" })
add_requires("vcpkg::glbinding", { alias = "glbinding" })
add_requires("vcpkg::assimp", { alias = "assimp" })

target("25-horizon-based-ambient-occlusion")
  set_languages("cxx20")
  set_kind("binary")

  set_pcxxheader("src/stdafx.hpp")

  add_packages("sfml", "glm", "globjects", "glbinding", "assimp")

  add_includedirs("src/")

  if is_plat("macosx") then
    -- this prevents "-[SFOpenGLView enableKeyRepeat]: unrecognized selector sent to instance 0x7fa5c2507970" runtime exception
    add_ldflags("-ObjC")

    -- this prevents linker errors
    add_frameworks("Foundation", "OpenGL", "IOKit", "Cocoa", "Carbon")
  elseif is_plat("windows") then
    add_ldflags("/LTCG")
  end

  add_files("src/main.cpp", "src/AbstractMesh.cpp", "src/AbstractSkyboxBuilder.cpp", "src/AsbtractMeshBuilder.cpp", "src/AssimpModel.cpp", "src/CubemapSkyboxBuilder.cpp", "src/main.cpp", "src/MultimeshModel.cpp", "src/SimpleSkyboxBuilder.cpp", "src/SingleMeshModel.cpp", "src/Skybox.cpp")

  after_build(function (target)
    os.cp("$(scriptdir)/../media", path.join(path.directory(target:targetfile()), "media"))
    os.cp("$(scriptdir)/media", path.join(path.directory(target:targetfile()), "media"))
  end)
