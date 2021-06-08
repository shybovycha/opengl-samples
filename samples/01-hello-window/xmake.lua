add_requires("sfml ~2.5.1", { alias = "sfml" })

target("01-hello-window")
  set_kind("binary")

  add_packages("sfml")

  if is_plat("macosx") then
    add_frameworks("Foundation", "OpenGL", "IOKit", "Cocoa", "Carbon")
  end

  add_files("main.cpp")
