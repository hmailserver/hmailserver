# Builds the sources under hmailserver/source/Server that carry no Windows
# dependency, and a runner that exercises them.
#
# This is not a way to build hMailServer. The service is built from
# hmailserver/source/Server/hMailServer/hMailServer.sln with MSVC, and that
# remains the only build that produces a server. What this adds is a second
# compiler over the parts of the code that are pure logic, and with it the
# address and undefined-behaviour sanitizers, on a machine that has no Windows
# SDK. A source belongs here once it compiles against Portable/stdafx.h and
# links without pulling in anything Windows-only.
#
#   cmake -S . -B out/portable -DCMAKE_BUILD_TYPE=Debug
#   cmake --build out/portable
#   ctest --test-dir out/portable --output-on-failure

cmake_minimum_required(VERSION 3.16)

project(hmailserver-core LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(HM_SANITIZE "Build with the address and undefined-behaviour sanitizers" ON)

set(HM_SERVER "${CMAKE_CURRENT_SOURCE_DIR}/hmailserver/source/Server")

# Sources that build without the Windows SDK. Deliberately a short list: it is
# meant to grow one file at a time, each with cases added to the runner.
add_library(hmailserver-core STATIC
   "${HM_SERVER}/Common/Mime/MimeChar.cpp"
   "${HM_SERVER}/SMTP/SPF/SPFSyntax.cpp"
   "${HM_SERVER}/SMTP/SPF/SPFAddress.cpp"
   "${HM_SERVER}/SMTP/SPF/SPFRecord.cpp"
   "${HM_SERVER}/SMTP/SPF/SPFRecordLocator.cpp"
   "${HM_SERVER}/SMTP/SPF/SPFMacroExpander.cpp"
   "${HM_SERVER}/SMTP/SPF/SPFEvaluator.cpp"
)

# Portable/ holds this build's stand-in for the precompiled header. The server
# project puts its own directory on the include path so that sources find
# hMailServer/stdafx.h; this build puts Portable/ there instead, so the same
# #include "stdafx.h" resolves to a header with no Windows in it.
target_include_directories(hmailserver-core PUBLIC
   "${HM_SERVER}/Portable"
   "${HM_SERVER}"
)

# Precompiled for the same reason the server project precompiles: the header is
# included by every source and is far larger than any of them.
target_precompile_headers(hmailserver-core PUBLIC "${HM_SERVER}/Portable/stdafx.h")

target_compile_options(hmailserver-core PUBLIC -Wall -Wextra)

if(HM_SANITIZE)
   target_compile_options(hmailserver-core PUBLIC
      -fsanitize=address,undefined -fno-omit-frame-pointer -g)
   target_link_options(hmailserver-core PUBLIC -fsanitize=address,undefined)
endif()

# The test code, which the server project builds too: the same testers run from
# ClassTester under Visual Studio, where a case can be stepped through, and from
# here under the sanitizers. Test code sits on the executable rather than in the
# library above, which holds only sources the server itself is built from.
add_executable(hmailserver-core-tests
   "${HM_SERVER}/Portable/PortableCoreTests.cpp"
   "${HM_SERVER}/SMTP/SPF/Conformance/SPFConformanceSuite.cpp"
   "${HM_SERVER}/SMTP/SPF/Conformance/SPFConformanceLookup.cpp"
   "${HM_SERVER}/SMTP/SPF/Conformance/SPFConformanceTester.cpp"
   "${HM_SERVER}/SMTP/SPF/SPFTestLookup.cpp"
   "${HM_SERVER}/SMTP/SPF/SPFRecordTester.cpp"
   "${HM_SERVER}/SMTP/SPF/SPFMacroExpanderTester.cpp"
   "${HM_SERVER}/SMTP/SPF/SPFEvaluatorTester.cpp"
)
target_link_libraries(hmailserver-core-tests PRIVATE hmailserver-core)

enable_testing()
add_test(NAME hmailserver-core-tests COMMAND hmailserver-core-tests)

# Parses the records written by Portable/Fuzz/Get-SpfRecords.ps1. Not a test: the
# records come from live DNS and change over time.
add_executable(spf-parse-records "${HM_SERVER}/Portable/Fuzz/ParseRealRecords.cpp")
target_link_libraries(spf-parse-records PRIVATE hmailserver-core)

# The fuzzers, which are off by default and are not tests.
#
# A fuzzer has no pass, only a not-yet-failed, so running one for a fixed number
# of inputs would say nothing that the suites above do not already say better.
# What it is for is the input nobody thought of, and that means running it for as
# long as there is patience, against a corpus that is kept.
#
#   cmake -S . -B out/fuzz -G Ninja -DCMAKE_BUILD_TYPE=Debug -DHM_FUZZ=ON #      -DCMAKE_CXX_COMPILER=clang++
#   cmake --build out/fuzz
#   out/fuzz/fuzz-spf-record hmailserver/source/Server/Portable/Fuzz/corpus/record
#
# Needs clang and its fuzzer runtime: on Ubuntu, libclang-rt-<version>-dev.
option(HM_FUZZ "Build the libFuzzer targets. Requires clang." OFF)

if(HM_FUZZ)
   if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      message(FATAL_ERROR "HM_FUZZ needs clang: libFuzzer is part of its runtime. "
                          "Configure with -DCMAKE_CXX_COMPILER=clang++.")
   endif()

   # A library of its own rather than hmailserver-core, because the sanitizer
   # coverage instrumentation libFuzzer needs has to be compiled into the code
   # under test, not only into the entry point.
   add_library(hmailserver-core-fuzz STATIC
      "${HM_SERVER}/SMTP/SPF/SPFSyntax.cpp"
      "${HM_SERVER}/SMTP/SPF/SPFAddress.cpp"
      "${HM_SERVER}/SMTP/SPF/SPFRecord.cpp"
      "${HM_SERVER}/SMTP/SPF/SPFMacroExpander.cpp"
      "${HM_SERVER}/SMTP/SPF/SPFTestLookup.cpp"
   )

   target_include_directories(hmailserver-core-fuzz PUBLIC
      "${HM_SERVER}/Portable"
      "${HM_SERVER}"
   )

   target_precompile_headers(hmailserver-core-fuzz PUBLIC "${HM_SERVER}/Portable/stdafx.h")

   target_compile_options(hmailserver-core-fuzz PUBLIC
      -Wall -Wextra -g -fno-omit-frame-pointer
      -fsanitize=fuzzer-no-link,address,undefined)
   target_link_options(hmailserver-core-fuzz PUBLIC -fsanitize=address,undefined)

   foreach(target record macro-expander)
      if(target STREQUAL "record")
         set(source "FuzzSPFRecord.cpp")
      else()
         set(source "FuzzSPFMacroExpander.cpp")
      endif()

      add_executable("fuzz-spf-${target}" "${HM_SERVER}/Portable/Fuzz/${source}")
      target_link_libraries("fuzz-spf-${target}" PRIVATE hmailserver-core-fuzz)
      target_link_options("fuzz-spf-${target}" PRIVATE -fsanitize=fuzzer)
   endforeach()
endif()
