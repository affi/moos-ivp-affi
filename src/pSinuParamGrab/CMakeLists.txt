#--------------------------------------------------------
# The CMakeLists.txt for:                       pSinuParamGrab
# Author(s):                              Janille Maragh
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  SinuParamGrab.cpp
  SinuParamGrab_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(pSinuParamGrab ${SRC})

TARGET_LINK_LIBRARIES(pSinuParamGrab
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread)

