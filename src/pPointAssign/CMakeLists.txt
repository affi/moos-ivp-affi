#--------------------------------------------------------
# The CMakeLists.txt for:                       pPointAssign
# Author(s):                              Janille Maragh
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  PointAssign.cpp
  PointAssign_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(pPointAssign ${SRC})

TARGET_LINK_LIBRARIES(pPointAssign
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread)

