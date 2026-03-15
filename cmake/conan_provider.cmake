# CMake Conan dependency provider
# This allows CMake to automatically invoke Conan during configuration
# See: https://github.com/conan-io/cmake-conan

set(CONAN_PROVIDER_URL "https://github.com/conan-io/cmake-conan/raw/develop2/conan_provider.cmake")
set(CONAN_PROVIDER_FILE "${CMAKE_BINARY_DIR}/conan_provider.cmake")

if(NOT EXISTS "${CONAN_PROVIDER_FILE}")
    message(STATUS "Downloading conan_provider.cmake from ${CONAN_PROVIDER_URL}")
    file(DOWNLOAD "${CONAN_PROVIDER_URL}" "${CONAN_PROVIDER_FILE}"
         STATUS download_status
         TIMEOUT 60)
    list(GET download_status 0 status_code)
    if(NOT status_code EQUAL 0)
        message(FATAL_ERROR "Failed to download conan_provider.cmake")
    endif()
endif()

include("${CONAN_PROVIDER_FILE}")
