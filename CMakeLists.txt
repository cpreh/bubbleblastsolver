cmake_minimum_required(
	VERSION
	2.6
)

project(
	bubble_blast_solver
)

include(
	FcpptCMakeUtils
)

find_package(
	Boost
	1.44.0
	REQUIRED
)

include_directories(
	SYSTEM
	${Boost_INCLUDE_DIR}
)

find_package(
	Fcppt
	REQUIRED
)

add_definitions(
	"${Fcppt_DEFINITIONS}"
)

include_directories(
	${Fcppt_INCLUDE_DIRS}
)

add_executable(
	solver
	main.cpp
)
