#----------------------------------------------------------------------------------------
# Get source files from dir
macro(GetFilesFromDir Dir Out)
    file(GLOB_RECURSE tmp ${Dir}/[^.]*.cpp)
    list(APPEND ${Out} ${tmp})
    file(GLOB_RECURSE tmp ${Dir}/[^.]*.h)
    list(APPEND ${Out} ${tmp})
endmacro()

macro(GetFilesFromDirNoRecurse Dir Out)
    file(GLOB tmp ${Dir}/[^.]*.cpp)
    list(APPEND ${Out} ${tmp})
    file(GLOB tmp ${Dir}/[^.]*.h)
    list(APPEND ${Out} ${tmp})
endmacro()


#----------------------------------------------------------------------------------------
# Compile static lib
macro(GenerateStaticLib LibName Srcs)
    add_library(${LibName} STATIC ${${Srcs}})
    target_link_libraries(${LibName} ${Deps})
    if (MOTION_OPENMP_ENABLE AND OpenMP_CXX_FOUND)
        target_link_libraries(${LibName} OpenMP::OpenMP_CXX)
    endif()
endmacro()

#----------------------------------------------------------------------------------------
# Compile share lib
macro(GenerateShareLib LibName Srcs)
    add_library(${LibName} SHARED ${${Srcs}} ${CommonSrcs})
    target_link_libraries(${LibName} ${Deps})
    if (MOTION_OPENMP_ENABLE AND OpenMP_CXX_FOUND)
        target_link_libraries(${LibName} OpenMP::OpenMP_CXX)
    endif()
    # message(STATUS "public header: ${PROJECT_SOURCE_DIR}/inc/VirtualHuman/VirtualHuman_API.hpp")
    if (IOS)
        set_target_properties(${LibName} PROPERTIES
            FRAMEWORK TRUE
            FRAMEWORK_VERSION CXX
            PUBLIC_HEADER ${PROJECT_SOURCE_DIR}/inc/VirtualHuman/VirtualHuman_API.hpp
            XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
        )
    endif()

    if (MOTION_WINDOWS)
        target_link_options(${LibName} PRIVATE ${DepsLinkOp})
    endif()
endmacro()


#----------------------------------------------------------------------------------------
# List the name of all subdirectories of a directory
macro(ListSubDirs resultDirs currentDir)
    file(GLOB _children RELATIVE ${currentDir} ${currentDir}/*)
    set(_dirlist "")
    foreach(_child ${_children})
        if(IS_DIRECTORY ${currentDir}/${_child})
            list(APPEND _dirlist ${_child})
        endif()
    endforeach()
    set(${resultDirs} ${_dirlist})
endmacro()

#----------------------------------------------------------------------------------------
# Install files preserving their relative path
macro(InstallFiles relativeBasePath destinationPath)
    foreach(_file IN ITEMS ${${ARGN}})
        if(IS_ABSOLUTE "${_file}")
            file(RELATIVE_PATH _file_rel "${relativeBasePath}" "${_file}")
        else()
            set(_file_rel "${_file}")
        endif()
        get_filename_component(_file_path "${_file_rel}" PATH)
        string(REPLACE "../" "" _file_path "${_file_path}")
        install(FILES ${_file} DESTINATION ${destinationPath}/${_file_path})
        #message(STATUS "To install: ${destinationPath}/${_file_path}/${_file}")
    endforeach()
endmacro()

macro(FindFramework framework frameworkName find_path)
    unset(FW_${frameworkName} CACHE)
    set(FW_${frameworkName})
    # if(NOT DEFINED IOS_PLATFORM)
    #     # macOS
    #     find_library(
    #         FW_${frameworkName} NAMES ${frameworkName}
    #     )
    # else()
        # iOS or simulators
    find_library(
        FW_${frameworkName} NAMES ${frameworkName}
        PATHS ${find_path} NO_DEFAULT_PATH
    )
    if(${FW_${frameworkName}} STREQUAL FW_${frameworkName}-NOTFOUND)
        find_library(
            FW_${frameworkName} NAMES ${frameworkName}
        )
    endif()
    if(${FW_${frameworkName}} STREQUAL FW_${frameworkName}-NOTFOUND)
        message(ERROR  "Framework ${frameworkName} not found")
    else()
        message(STATUS "Framework ${frameworkName} found at ${FW_${frameworkName}}")
    endif()
    set(${framework} ${FW_${frameworkName}})
endmacro()

macro(IOSInstallModels BundleName ModelPaths InstallPath)
    file(WRITE ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/dummy_source.cxx "int helloworld(){return 0;}\n")
    add_library(${BundleName} MODULE ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/dummy_source.cxx)
    target_sources(${BundleName} PUBLIC ${${ModelPaths}})
    set_target_properties(${BundleName} PROPERTIES BUNDLE TRUE)
    foreach(model_path ${${ModelPaths}})
        set_source_files_properties(${model_path} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    endforeach()
    install(TARGETS ${BundleName} DESTINATION ${InstallPath})
endmacro(IOSInstallModels)

macro(CopyIntoBundle BundleName Src Dst)
    if (IS_DIRECTORY ${Src})
        file(GLOB_RECURSE tmp ${Src}/*)
        foreach(path ${tmp})
            target_sources(${BundleName} PUBLIC ${path})
            set_target_properties(${BundleName} PROPERTIES BUNDLE TRUE)
            set_source_files_properties(${path} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/${Dst})
        endforeach()
    else()
        target_sources(${BundleName} PUBLIC ${Src})
        set_target_properties(${BundleName} PROPERTIES BUNDLE TRUE)
        set_source_files_properties(${Src} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/${Dst})
    endif()
endmacro()
# macro(CopyFileIntoBundle BundleName )
