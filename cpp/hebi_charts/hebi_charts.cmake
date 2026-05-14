# Used to download the binaries - this should not be used directly.
cmake_minimum_required(VERSION 3.12)

set(HEBI_CHARTS_VERSION "0.9.2")
set(HEBI_CHARTS_BUILD_NUMBER "115")

# ======== Determine target platform ========
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(HEBI_CHARTS_OS "linux")
  set(HEBI_CHARTS_LIB_NAME "libhebi_charts.so")
  set(HEBI_CHARTS_LINK_NAME "libhebi_charts.so")
  if(DEFINED ENV{XDG_CACHE_HOME})
    set(HEBI_CHARTS_CACHE_HOME "$ENV{XDG_CACHE_HOME}")
  else()
    set(HEBI_CHARTS_CACHE_HOME "$ENV{HOME}/.cache")
  endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  set(HEBI_CHARTS_OS "osx")
  set(HEBI_CHARTS_LIB_NAME "libhebi_charts.dylib")
  set(HEBI_CHARTS_LINK_NAME "libhebi_charts.dylib")
  set(HEBI_CHARTS_CACHE_HOME "$ENV{HOME}/Library/Caches")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(HEBI_CHARTS_OS "win")
  set(HEBI_CHARTS_LIB_NAME "hebi_charts.dll")
  set(HEBI_CHARTS_LINK_NAME "hebi_charts.lib")
  set(HEBI_CHARTS_CACHE_HOME "$ENV{LOCALAPPDATA}")
endif()

if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
  set(HEBI_CHARTS_ARCH "amd64") # For 64-bit
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
  set(HEBI_CHARTS_ARCH "arm64") # For ARM64
endif()

set(HEBI_CHARTS_PLATFORM "${HEBI_CHARTS_OS}_${HEBI_CHARTS_ARCH}")

# ======== Variables to be used by including projects ========
# Prioritize locally bundled lib binaries
set(HEBI_CHARTS_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include/")
set(HEBI_CHARTS_LIB_FILE "${CMAKE_CURRENT_LIST_DIR}/lib/${HEBI_CHARTS_PLATFORM}/${HEBI_CHARTS_LIB_NAME}")
set(HEBI_CHARTS_LINK_FILE "${CMAKE_CURRENT_LIST_DIR}/lib/${HEBI_CHARTS_PLATFORM}/${HEBI_CHARTS_LINK_NAME}")

# Fall back to shared downloaded version in local cache
if(NOT EXISTS "${HEBI_CHARTS_LIB_FILE}")

  set(HEBI_CHARTS_DOWNLOAD_DIR "${HEBI_CHARTS_CACHE_HOME}/hebi_charts/${HEBI_CHARTS_VERSION}-build${HEBI_CHARTS_BUILD_NUMBER}")
  set(HEBI_CHARTS_LIB_FILE "${HEBI_CHARTS_DOWNLOAD_DIR}/hebi_charts/lib/${HEBI_CHARTS_PLATFORM}/${HEBI_CHARTS_LIB_NAME}")
  set(HEBI_CHARTS_LINK_FILE "${HEBI_CHARTS_DOWNLOAD_DIR}/hebi_charts/lib/${HEBI_CHARTS_PLATFORM}/${HEBI_CHARTS_LINK_NAME}")

  # Include directory fallback, so we could bundle the cmake file without any headers
  if(NOT EXISTS "${HEBI_CHARTS_INCLUDE_DIR}")
    set(HEBI_CHARTS_INCLUDE_DIR "${HEBI_CHARTS_DOWNLOAD_DIR}/hebi_charts/include/")
  endif()

endif()

function(add_copy_hebi_charts target_name)
  add_custom_command(TARGET ${target_name} POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy_if_different
                     "${HEBI_CHARTS_LIB_FILE}"
                     $<TARGET_FILE_DIR:${target_name}>)
endfunction()

function(target_add_hebi_charts target_name)
  target_include_directories(${target_name} PRIVATE ${HEBI_CHARTS_INCLUDE_DIR})
  target_link_libraries(${target_name} PRIVATE ${HEBI_CHARTS_LINK_FILE})
  add_copy_hebi_charts(${target_name})
endfunction()

# ======== Download binaries on-demand ========
option(DOWNLOAD_HEBI_CHARTS "Download binaries for the local system if none are available" ON)
if(DOWNLOAD_HEBI_CHARTS AND NOT EXISTS "${HEBI_CHARTS_LIB_FILE}")

  # Build URL
  set(HEBI_CHARTS_DL_NAME "hebi_charts-${HEBI_CHARTS_VERSION}-${HEBI_CHARTS_PLATFORM}.zip")
  set(HEBI_CHARTS_DL_URL "https://files.hebi.us/download/hebi_charts/snapshot/${HEBI_CHARTS_BUILD_NUMBER}/${HEBI_CHARTS_DL_NAME}")

  # Make sure the directory exists
  file(MAKE_DIRECTORY "${HEBI_CHARTS_DOWNLOAD_DIR}")

  # Download full release
  message(STATUS "Downloading ${HEBI_CHARTS_DL_URL}")
  get_filename_component(HEBI_CHARTS_DL_PATH "${HEBI_CHARTS_DOWNLOAD_DIR}/${HEBI_CHARTS_DL_NAME}" REALPATH)
  file(DOWNLOAD "${HEBI_CHARTS_DL_URL}" "${HEBI_CHARTS_DL_PATH}" SHOW_PROGRESS)

  # Extract to shared cache directory
  message(STATUS "Extracting to ${HEBI_CHARTS_DOWNLOAD_DIR}")
  file(ARCHIVE_EXTRACT
       INPUT "${HEBI_CHARTS_DL_PATH}"
       DESTINATION "${HEBI_CHARTS_DOWNLOAD_DIR}")

  message(STATUS "Removing temporary files")
  file(REMOVE "${HEBI_CHARTS_DL_PATH}")

endif()