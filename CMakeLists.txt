cmake_minimum_required(VERSION 3.7)
project(cpi)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(/usr/local/opt/llvm@5/include)

set(CMAKE_CXX_FLAGS "-I/usr/local/Cellar/llvm/5.0.1/include -L/usr/local/Cellar/llvm/5.0.1/lib \
    -lLLVMCore -lLLVMBinaryFormat -lLLVMSupport -lLLVMDemangle \
    -lLLVMLTO -lLLVMPasses -lLLVMObjCARCOpts -lLLVMSymbolize -lLLVMDebugInfoPDB \
    -lLLVMDebugInfoDWARF -lLLVMMIRParser -lLLVMCoverage -lLLVMTableGen -lLLVMDlltoolDriver \
    -lLLVMOrcJIT -lLLVMXCoreDisassembler -lLLVMXCoreCodeGen -lLLVMXCoreDesc -lLLVMXCoreInfo \
    -lLLVMXCoreAsmPrinter -lLLVMSystemZDisassembler -lLLVMSystemZCodeGen -lLLVMSystemZAsmParser \
    -lLLVMSystemZDesc -lLLVMSystemZInfo -lLLVMSystemZAsmPrinter -lLLVMSparcDisassembler \
    -lLLVMSparcCodeGen -lLLVMSparcAsmParser -lLLVMSparcDesc -lLLVMSparcInfo -lLLVMSparcAsmPrinter \
    -lLLVMPowerPCDisassembler -lLLVMPowerPCCodeGen -lLLVMPowerPCAsmParser -lLLVMPowerPCDesc \
    -lLLVMPowerPCInfo -lLLVMPowerPCAsmPrinter -lLLVMNVPTXCodeGen -lLLVMNVPTXDesc -lLLVMNVPTXInfo \
    -lLLVMNVPTXAsmPrinter -lLLVMMSP430CodeGen -lLLVMMSP430Desc -lLLVMMSP430Info -lLLVMMSP430AsmPrinter \
    -lLLVMMipsDisassembler -lLLVMMipsCodeGen -lLLVMMipsAsmParser -lLLVMMipsDesc -lLLVMMipsInfo \
    -lLLVMMipsAsmPrinter -lLLVMLanaiDisassembler -lLLVMLanaiCodeGen -lLLVMLanaiAsmParser -lLLVMLanaiDesc \
    -lLLVMLanaiAsmPrinter -lLLVMLanaiInfo -lLLVMHexagonDisassembler -lLLVMHexagonCodeGen \
    -lLLVMHexagonAsmParser -lLLVMHexagonDesc -lLLVMHexagonInfo -lLLVMBPFDisassembler -lLLVMBPFCodeGen \
    -lLLVMBPFDesc -lLLVMBPFInfo -lLLVMBPFAsmPrinter -lLLVMARMDisassembler -lLLVMARMCodeGen -lLLVMARMAsmParser \
    -lLLVMARMDesc -lLLVMARMInfo -lLLVMARMAsmPrinter -lLLVMAMDGPUDisassembler -lLLVMAMDGPUCodeGen \
    -lLLVMAMDGPUAsmParser -lLLVMAMDGPUDesc -lLLVMAMDGPUInfo -lLLVMAMDGPUAsmPrinter -lLLVMAMDGPUUtils \
    -lLLVMAArch64Disassembler -lLLVMAArch64CodeGen -lLLVMAArch64AsmParser -lLLVMAArch64Desc -lLLVMAArch64Info \
    -lLLVMAArch64AsmPrinter -lLLVMAArch64Utils -lLLVMObjectYAML -lLLVMLibDriver -lLLVMOption -lLLVMX86Disassembler \
    -lLLVMX86AsmParser -lLLVMX86CodeGen -lLLVMGlobalISel -lLLVMSelectionDAG -lLLVMAsmPrinter -lLLVMDebugInfoCodeView \
    -lLLVMDebugInfoMSF -lLLVMX86Desc -lLLVMMCDisassembler -lLLVMX86Info -lLLVMX86AsmPrinter -lLLVMX86Utils \
    -lLLVMMCJIT -lLLVMLineEditor -lLLVMInterpreter -lLLVMExecutionEngine -lLLVMRuntimeDyld -lLLVMCodeGen \
    -lLLVMTarget -lLLVMCoroutines -lLLVMipo -lLLVMInstrumentation -lLLVMVectorize -lLLVMScalarOpts \
    -lLLVMLinker -lLLVMIRReader -lLLVMAsmParser -lLLVMInstCombine -lLLVMTransformUtils -lLLVMBitWriter \
    -lLLVMAnalysis -lLLVMProfileData -lLLVMObject -lLLVMMCParser -lLLVMMC -lLLVMBitReader -lLLVMCore \
    -lLLVMBinaryFormat -lLLVMSupport -lLLVMDemangle -lcurses -lz -lm \
    -std=c++14")

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE Release)

set(SOURCE_FILES
        src/assembler.cpp
        src/assembler.h
        src/bytecodegen.cpp
        src/bytecodegen.h
        src/interpreter.cpp
        src/interpreter.h
        src/lexer.cpp
        src/lexer.h
        src/main.cpp
        src/node.cpp
        src/node.h
        src/parser.cpp
        src/parser.h
        src/semantic.cpp
        src/semantic.h
        src/util.cpp
        src/util.h src/llvmgen.cpp)

add_executable(cpi ${SOURCE_FILES})
