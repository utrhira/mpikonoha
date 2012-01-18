cmake_minimum_required(VERSION 2.6)
set(CMAKE_BUILD_TYPE ${KONOHA_BUILD_TYPE})

project(mpi)

# - set package variables
set(PACKAGE_NAME    ${PROJECT_NAME})
set(PACKAGE_VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})
set(PACKAGE_STRING  ${CMAKE_PROJECT_NAME}-${PACKAGE_VERSION})
set(PACKAGE_SCRIPT_CODE mpi.k)
set(PACKAGE_SOURCE_CODE
		mpi.c
		src/array.c
		src/coll.c
		src/data.c
		src/comm.c
		src/op.c
		src/pt2pt.c
		src/general.c
)
set(KONOHA_PACKAGE_DIR konoha/package/${KONOHA_VERSION}/konoha.${PROJECT_NAME})

# - check MPI library

set(MPI_TYPE_OMPI "on")
set(MPI_TYPE_MPICH "on")

if(MPI_TYPE)
   if("${MPI_TYPE}" STREQUAL "mpich")
     # -- disable ompi
     set(MPI_TYPE_OMPI)
   elseif("${MPI_TYPE}" STREQUAL "ompi")
     # -- disable mpich
     set(MPI_TYPE_MPICH)
   endif()
endif(MPI_TYPE)

if(MPI_ROOT_DIR)
  # -- use specified dirs
  set(MPI_INCDIR ${MPI_ROOT_DIR}/include)
  set(MPI_LIBDIR ${MPI_ROOT_DIR}/lib)
  find_library(HAVE_LIBMPICH NAMES mpich PATHS ${MPI_LIBDIR} NO_DEFAULT_PATH)
  find_library(HAVE_LIBMPI NAMES mpi PATHS ${MPI_LIBDIR} NO_DEFAULT_PATH)
else()
  find_library(HAVE_LIBMPICH mpich)
  find_library(HAVE_LIBMPI mpi)
endif(MPI_ROOT_DIR)

if(MPI_TYPE_OMPI AND HAVE_LIBMPI)
   # -- OpenMPI
   set(MessageLibs ${MessageLibs} ompi PARENT_SCOPE)
   set(INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${KONOHA_INCLUDE_DIRS} ${MPI_INCDIR})
   include_directories(${INCLUDE_DIRS})

   set(MPI_LIBS ${HAVE_LIBMPI})

# endif MPI_TYPE_OMPI AND HAVE_LIBMPI
elseif(MPI_TYPE_MPICH AND HAVE_LIBMPICH)
   # -- MPICH
   set(MessageLibs ${MessageLibs} mpich PARENT_SCOPE)
   set(INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${KONOHA_INCLUDE_DIRS} ${MPI_INCDIR})
   include_directories(${INCLUDE_DIRS})

   if (MPI_LIBDIR)
     find_library(LIB_OPENPA NAMES opa PATHS ${MPI_LIBDIR} NO_DEFAULT_PATH)
     find_library(LIB_MPL NAMES mpl PATHS ${MPI_LIBDIR} NO_DEFAULT_PATH)
     find_library(LIB_PMPICH NAMES pmpich PATHS ${MPI_LIBDIR} NO_DEFAULT_PATH)
   else()
     find_library(LIB_OPENPA opa)
     find_library(LIB_MPL mpl ${MPI_LIBDIR})
     find_library(LIB_PMPICH pmpich)
   endif()

   if (NOT EXISTS ${LIB_OPENPA})
     message(FATAL_ERROR "opa library not found.")
   endif(NOT EXISTS ${LIB_OPENPA})

   if (NOT EXISTS ${LIB_MPL})
     message(FATAL_ERROR "mpl library not found.")
   endif(NOT EXISTS ${LIB_MPL})

   set(MPI_LIBS ${HAVE_LIBMPICH} ${LIB_OPENPA} ${LIB_MPL} ${LIB_PMPICH})

# endif MPI_TYPE_MPICH AND HAVE_LIBMPICH
endif(MPI_TYPE_OMPI AND HAVE_LIBMPI)

if(MPI_LIBS)
   # -- install mpikonoha binary
   add_executable(mpikonoha src/mpikonoha.c)
   target_link_libraries(mpikonoha konoha ${MPI_LIBS})
   install(TARGETS mpikonoha RUNTIME DESTINATION bin)

   # -- install konoha.mpi package
   add_definitions(-D_SETUP)
   add_library(${PACKAGE_NAME} SHARED ${PACKAGE_SOURCE_CODE})
   set_target_properties(${PACKAGE_NAME} PROPERTIES PREFIX "")
   target_link_libraries(${PACKAGE_NAME} konoha ${MPI_LIBS})
   install(TARGETS ${PACKAGE_NAME} DESTINATION ${KONOHA_PACKAGE_DIR})
   install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${PACKAGE_SCRIPT_CODE} DESTINATION ${KONOHA_PACKAGE_DIR})

endif(MPI_LIBS)
