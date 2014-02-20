#--------------------------------------------------------
# The CMakeLists.txt for:                       pPrimeFactor2
# Author(s):                              Janille Maragh
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  PrimeFactor2.cpp
  PrimeFactor2_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(pPrimeFactor2 ${SRC})

TARGET_LINK_LIBRARIES(pPrimeFactor2
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread)

