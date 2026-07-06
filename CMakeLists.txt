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
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql PrintSupport)

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
	src/view/sections/Section.h
	src/view/sections/OptimizationSection.h
	src/view/dialogs/FormDialog.h
	src/view/dialogs/OrderFormDialog.h
	src/view/dialogs/OptimizationFormDialog.h
	src/view/dialogs/MoldFormDialog.h
	src/view/dialogs/OperatorFormDialog.h
	src/view/dialogs/ClientFormDialog.h
	src/view/dialogs/EditClientFormDialog.h
	src/view/dialogs/EditMoldFormDialog.h
	src/view/dialogs/EditOperatorFormDialog.h
	src/view/dialogs/EditOrderFormDialog.h
	src/view/dialogs/PersonalizeFormDialog.h
	src/view/sections/SectionOrders.h
	src/view/sections/SectionMolds.h
	src/view/sections/SectionClients.h
	src/view/tables/Table.h
	src/view/tables/TableSimple.h
	src/view/tables/optimizationTable/OptimizationTable.h
	src/view/tables/optimizationTable/NotAllocatedsTable.h
	src/view/tables/optimizationTable/OptimizationCellWidget.h
	src/view/tables/optimizationTable/JobCardWidget.h
	src/view/buttons/DialogButton.h
	src/view/buttons/Button.h
	src/view/buttons/ExportButton.h
	resources.qrc
	src/repository/OrderRepository.cpp
	src/repository/MoldRepository.cpp
	src/repository/OperatorRepository.cpp
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
	Qt6::PrintSupport
	tcc_2026_lib
)
