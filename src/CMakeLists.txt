set(HEADER_DIR ${LIBBINARYIO_ROOT}/include/binaryio)
set(PUBLIC_HEADERS	${HEADER_DIR}/binaryreader.hpp
					${HEADER_DIR}/binarywriter.hpp
					${HEADER_DIR}/util.hpp)

file(GLOB_RECURSE SRC_FILES
	"*.hpp"
	"*.cpp"
)

add_library(libbinaryio STATIC	${SRC_FILES}
								${PUBLIC_HEADERS})

set_property(TARGET libbinaryio PROPERTY CXX_STANDARD 17)
set_property(TARGET libbinaryio PROPERTY PREFIX "")
target_include_directories(libbinaryio PUBLIC ${LIBBINARYIO_ROOT}/include)

install(TARGETS libbinaryio DESTINATION lib)
install(FILES ${PUBLIC_HEADERS} DESTINATION include/binaryio)
install(FILES ${LIBBINARYIO_ROOT}/LICENSE.md DESTINATION share/binaryio RENAME copyright)
