#--------------------------------------------------------
# The CMakeLists.txt for:                       pPrimeFactorTester2
# Author(s):                              Janille Maragh
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  PrimeFactorTester2.cpp
  PrimeFactorTester2_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(pPrimeFactorTester2 ${SRC})

TARGET_LINK_LIBRARIES(pPrimeFactorTester2
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread)

