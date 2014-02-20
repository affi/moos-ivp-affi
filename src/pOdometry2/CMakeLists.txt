#--------------------------------------------------------
# The CMakeLists.txt for:                       pOdometry2
# Author(s):                              Janille
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  Odometry2.cpp
  Odometry2_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(pOdometry2 ${SRC})

TARGET_LINK_LIBRARIES(pOdometry2
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread)

