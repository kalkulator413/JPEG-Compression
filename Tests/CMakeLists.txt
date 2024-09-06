include(AddCatch2)

set(SOURCE_FILES	
	MatrixTests.cpp
	ImgTests.cpp
)

set(LIBRARY_NAME Lib)

find_package(ImageMagick REQUIRED COMPONENTS Magick++)

add_executable(Tests ${SOURCE_FILES})
target_link_libraries(Tests PRIVATE Lib Catch2::Catch2WithMain ImageMagick::Magick++)

include(CTest)
include(Catch)
catch_discover_tests(Tests)
