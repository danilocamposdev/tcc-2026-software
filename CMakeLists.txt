cmake_minimum_required(VERSION 3.24)
project(production-allocation-system VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# LSP
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(FETCHCONTENT_BASE_DIR ${CMAKE_SOURCE_DIR}/.deps)

# Find Qt6 installed in machine
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql)

# CPM
file(
  DOWNLOAD
  https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.8/CPM.cmake
  ${CMAKE_SOURCE_DIR}/.deps/cmake/CPM.cmake
  EXPECTED_HASH SHA256=78ba32abdf798bc616bab7c73aac32a17bbd7b06ad9e26a6add69de8f3ae4791
)
include(${CMAKE_SOURCE_DIR}/.deps/cmake/CPM.cmake)

# OR lib
CPMAddPackage(
    NAME tcc_2026_lib
    GITHUB_REPOSITORY "danilocamposdev/tcc-2026"
    GIT_TAG main
)

add_executable(${PROJECT_NAME}
	src/main.cpp
	src/ui/sections/Section.h
	src/ui/sections/SectionOptimization.h
	src/ui/dialogs/FormDialog.h
	src/ui/dialogs/OrderFormDialog.h
	src/ui/dialogs/OptimizationFormDialog.h
	src/ui/dialogs/MoldFormDialog.h
	src/ui/dialogs/ClientFormDialog.h
	src/ui/dialogs/EditClientFormDialog.h
	src/ui/dialogs/EditMoldFormDialog.h
	src/ui/dialogs/EditOrderFormDialog.h
	src/ui/dialogs/PersonalizeFormDialog.h
	src/ui/sections/SectionOrders.h
	src/ui/sections/SectionMolds.h
	src/ui/sections/SectionClients.h
	src/ui/tables/Table.h
	src/ui/tables/TableSimple.h
	src/ui/tables/TableOptimization.h
	src/ui/buttons/DialogButton.h
	src/ui/buttons/Button.h
	resources.qrc
	src/repository/OrderRepository.cpp
	src/repository/MoldRepository.cpp
	src/repository/ClientRepository.cpp
	src/repository/OptimizationConfigRepository.cpp
	src/repository/ProductionAllocationRepository.cpp
	src/database/Database.cpp
	src/database/DatabaseSeeder.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE src)

target_link_libraries(${PROJECT_NAME}
	PRIVATE
	Qt6::Core
	Qt6::Widgets
	Qt6::Sql
	tcc_2026_lib
)
