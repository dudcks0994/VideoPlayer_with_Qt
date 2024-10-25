# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\VideoPlayer_with_Qt_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\VideoPlayer_with_Qt_autogen.dir\\ParseCache.txt"
  "VideoPlayer_with_Qt_autogen"
  )
endif()
