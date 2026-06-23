#SET(UseSWIG_TARGET_NAME_PREFERENCE STANDARD)
SET_PROPERTY(SOURCE ResBosPython.i PROPERTY CPLUSPLUS ON)
SET_PROPERTY(SOURCE ResBosPython.i PROPERTY SWIG_MODULE_NAME pyResbos)
SET_PROPERTY(SOURCE ResBosPython.i PROPERTY USE_LIBRARY_INCLUDE_DIRECTORIES TRUE)

set(PYTHON_INSTALL_FILES
    ${CMAKE_CURRENT_BINARY_DIR}/pyResbos.py
    ${CMAKE_CURRENT_BINARY_DIR}/_pyResbos.so)

set(SETUP_PY_IN ${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in)
set(SETUP_PY_OUT ${CMAKE_CURRENT_BINARY_DIR}/setup.py)
configure_file(${SETUP_PY_IN} ${SETUP_PY_OUT})

SWIG_ADD_LIBRARY(pyResbos
    TYPE SHARED
    LANGUAGE python
    SOURCES ResBosPython.i
    PyResBos.cc)

SWIG_LINK_LIBRARIES(pyResbos PRIVATE 
    Python::Python 
    ${RESBOS_LIBRARIES} 
    Resbos
    ResbosUtilities
    ResbosBeam 
    ResbosCalculation 
    ResbosProcess 
    ResbosUser
    )

set_target_properties(${SWIG_MODULE_pyResbos_REAL_NAME} PROPERTIES SUFFIX .so)

install(CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} ${SETUP_PY_OUT} install)")
