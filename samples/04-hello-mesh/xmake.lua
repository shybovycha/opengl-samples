add_requires("sfml ~2.5.1", { alias = "sfml", configs = { shared = true } })
add_requires("glm")
add_requires("vcpkg::globjects ~1.1.0", { alias = "globjects" })
add_requires("vcpkg::glbinding", { alias = "glbinding" })

target("04-hello-mesh")
  set_kind("binary")

  add_packages("sfml", "glm", "globjects", "glbinding")

  if is_plat("macosx") then
    add_frameworks("Foundation", "OpenGL", "IOKit", "Cocoa", "Carbon")
  end

  add_files("main.cpp")

  after_build(function (target)
    os.cp("$(scriptdir)/media", "$(buildir)/media")
  end)
