include(AddCatch2)

set(SOURCE_FILES	
	Tests.cpp
)

set(LIBRARY_NAME Lib)

add_executable(Tests ${SOURCE_FILES})
target_link_libraries(Tests PRIVATE Lib Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(Tests)
