#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <vector>

#include "assembler.h"

template<typename T>
class Optional {
public:
    bool isPresent;
    T value;
    bool wasConstantRead;
};

class Interpreter;

template <typename T>
void interpretMathAdd(Interpreter *interp);

template <typename T>
void interpretMathSub(Interpreter *interp);

template <typename T>
void interpretMathMul(Interpreter *interp);

template <typename T>
void interpretMathDiv(Interpreter *interp);

template <typename T>
void interpretMathMod(Interpreter *interp);

template <int32_t bits, typename T>
void interpretMathBitwiseAnd(Interpreter *interp);

template <int32_t bits, typename T>
void interpretMathBitwiseOr(Interpreter *interp);

template <int32_t bits, typename T>
void interpretMathBitwiseXor(Interpreter *interp);

template <typename T>
void interpretCmpEq(Interpreter *interp);

template <typename T>
void interpretCmpNeq(Interpreter *interp);

template <typename T>
void interpretCmpGt(Interpreter *interp);

template <typename T>
void interpretCmpGte(Interpreter *interp);

template <typename T>
void interpretCmpLt(Interpreter *interp);

template <typename T>
void interpretCmpLte(Interpreter *interp);

void interpretCalli(Interpreter *interp);
void interpretCall(Interpreter *interp);
void interpretBumpSP(Interpreter *interp);
void interpretRet(Interpreter *interp);
void interpretJumpIf(Interpreter *interp);
void interpretJump(Interpreter *interp);
void interpretStore(Interpreter *interp);
void interpretStoreConst(Interpreter *interp);
void interpretExit(Interpreter *interp);
void interpretAssert(Interpreter *interp);

class Interpreter {
public:
    vector<unsigned char> instructions = {};
    unordered_map<uint32_t, uint32_t> fnTable = {};

    vector<unsigned char> stack = {};

    int32_t pc = 0;
    int32_t sp = 0;
    int32_t bp = 0;

    int32_t lastStmtPc;

    typedef void (*interpFuncType)(Interpreter *);
    vector<interpFuncType> table;

    bool terminated = false;

    SourceMap sourceMap;
    vector<unsigned long> breakpoints = {};
    bool continuing = false;

    // used for stepping 'over' functions (as opposed to normal step which goes 'into')
    uint16_t depth = 0;
    int32_t overDepth = (2 << 15) + 1;

    Interpreter(): Interpreter(1024 * 32) {}

    Interpreter(int32_t stackSize) {
        stack.reserve(stackSize);
        for (auto i = 0; i < stackSize; i++) {
            stack.push_back(0);
        }

        table = {
            interpretMathAdd<int8_t>,
            interpretMathSub<int8_t>,
            interpretMathMul<int8_t>,
            interpretMathDiv<uint8_t>,
            interpretMathDiv<int8_t>,
            interpretMathMod<uint8_t>,
            interpretMathMod<int8_t>,
            interpretCmpEq<int8_t>,
            interpretCmpNeq<int8_t>,
            interpretCmpGt<uint8_t>,
            interpretCmpGt<int8_t>,
            interpretCmpGte<uint8_t>,
            interpretCmpGte<int8_t>,
            interpretCmpLt<uint8_t>,
            interpretCmpLt<int8_t>,
            interpretCmpLte<uint8_t>,
            interpretCmpLte<int8_t>,
            interpretMathAdd<int16_t>,
            interpretMathSub<int16_t>,
            interpretMathMul<int16_t>,
            interpretMathDiv<uint16_t>,
            interpretMathDiv<int16_t>,
            interpretMathMod<uint16_t>,
            interpretMathMod<int16_t>,
            interpretCmpEq<int16_t>,
            interpretCmpNeq<int16_t>,
            interpretCmpGt<uint16_t>,
            interpretCmpGt<int16_t>,
            interpretCmpGte<uint16_t>,
            interpretCmpGte<int16_t>,
            interpretCmpLt<uint16_t>,
            interpretCmpLt<int16_t>,
            interpretCmpLte<uint16_t>,
            interpretCmpLte<int16_t>,
            interpretMathAdd<int32_t>,
            interpretMathSub<int32_t>,
            interpretMathMul<int32_t>,
            interpretMathDiv<uint32_t>,
            interpretMathDiv<int32_t>,
            interpretMathMod<uint32_t>,
            interpretMathMod<int32_t>,
            interpretCmpEq<int32_t>,
            interpretCmpNeq<int32_t>,
            interpretCmpGt<uint32_t>,
            interpretCmpGt<int32_t>,
            interpretCmpGte<uint32_t>,
            interpretCmpGte<int32_t>,
            interpretCmpLt<uint32_t>,
            interpretCmpLt<int32_t>,
            interpretCmpLte<uint32_t>,
            interpretCmpLte<int32_t>,
            interpretMathAdd<int64_t>,
            interpretMathSub<int64_t>,
            interpretMathMul<int64_t>,
            interpretMathDiv<uint64_t>,
            interpretMathDiv<int64_t>,
            interpretMathMod<uint64_t>,
            interpretMathMod<int64_t>,
            interpretCmpEq<int64_t>,
            interpretCmpNeq<int64_t>,
            interpretCmpGt<uint64_t>,
            interpretCmpGt<int64_t>,
            interpretCmpGte<uint64_t>,
            interpretCmpGte<int64_t>,
            interpretCmpLt<uint64_t>,
            interpretCmpLt<int64_t>,
            interpretCmpLte<uint64_t>,
            interpretCmpLte<int64_t>,
            interpretMathAdd<float>,
            interpretMathSub<float>,
            interpretMathMul<float>,
            interpretMathDiv<float>,
            interpretMathMod<float>,
            interpretCmpEq<float>,
            interpretCmpNeq<float>,
            interpretCmpLt<float>,
            interpretCmpLte<float>,
            interpretCmpGt<float>,
            interpretCmpGte<float>,
            interpretMathAdd<double>,
            interpretMathSub<double>,
            interpretMathMul<double>,
            interpretMathDiv<double>,
            interpretMathMod<double>,
            interpretCmpEq<double>,
            interpretCmpNeq<double>,
            interpretCmpLt<double>,
            interpretCmpLte<double>,
            interpretCmpGt<double>,
            interpretCmpGte<double>,
            interpretMathBitwiseOr<8, int8_t>,
            interpretMathBitwiseOr<16, int16_t>,
            interpretMathBitwiseOr<32, int32_t>,
            interpretMathBitwiseOr<64, int64_t>,
            interpretMathBitwiseAnd<8, int8_t>, 
            interpretMathBitwiseAnd<16, int16_t>,
            interpretMathBitwiseAnd<32, int32_t>,
            interpretMathBitwiseAnd<64, int64_t>,
            interpretMathBitwiseXor<8, int8_t>, 
            interpretMathBitwiseXor<16, int16_t>,
            interpretMathBitwiseXor<32, int32_t>,
            interpretMathBitwiseXor<64, int64_t>,
            interpretStoreConst,
            interpretStore,
            interpretBumpSP,
            interpretJumpIf,
            interpretJump,
            interpretCalli,
            interpretCall,
            interpretRet,
            interpretExit,
            interpretAssert};
  }

  void interpret();
  void step();
  void callIndex(int32_t index);

  template <typename T>
  void copyToStack(T t, int32_t offset) {
      memcpy(&stack[offset], &t, sizeof(T));
  }

  template <typename T>
  void pushToStack(T t) {
      copyToStack(t, sp);
      sp += sizeof(T);
  }

  template <typename T>
  T readFromStack(int32_t offset) {
      return bytesTo<T>(stack, offset);
  }

  template <typename T>
  T consume() {
      auto value = bytesTo<T>(instructions, pc);
      pc += sizeof(T);
      return value;
  }

  template <typename T>
  T read() {
      auto found = tryRead<T>();
      assert(found.isPresent);
      return found.value;
  }

  template <typename T>
  Optional<T> tryRead() {
      auto instructionPcString = AssemblyLexer::instructionStrings[instructions[pc]];

      string prefix = "RELCONST";
      auto testInstructionString = prefix.append(suffixForType<T>());
      if (instructionPcString == testInstructionString) {
          pc += 1;
          auto consumed = consume<T>();
          consumed += static_cast<T>(bp);
          return Optional<T>{true, consumed};
      }

      prefix = "REL";
      testInstructionString = prefix.append(suffixForType<T>());
      if (instructionPcString == testInstructionString) {
          pc += 1;
          auto consumed = consume<int32_t>();
          auto value = readFromStack<T>(consumed + bp);
          return Optional<T>{true, value};
      }

      prefix = "CONST";
      testInstructionString = prefix.append(suffixForType<T>());
      if (instructionPcString == testInstructionString) {
          pc += 1;
          auto consumed = consume<T>();
          return Optional<T>{true, consumed};
      }

      instructionPcString = AssemblyLexer::instructionStrings[instructions[pc]];
      testInstructionString = suffixForType<T>();
      if (instructionPcString == instructionPcString) {
          pc += 1;
          auto consumed = consume<int32_t>();
          auto value = readFromStack<T>(consumed);
          return Optional<T>{true, value};
      }

      return {false, {}};
  }

  auto readBits8() {
      // expect u8 or i8
      auto u8Value = tryRead<uint8_t>();
      if (u8Value.isPresent) {
          return *((int8_t *)&u8Value.value);
      }

      auto i8Value = tryRead<int8_t>();
      if (i8Value.isPresent) {
          return i8Value.value;
      }

      assert(false);
  }

  auto readBits16() {
      // expect uint16_t or int16_t
      auto uint16_tValue = tryRead<uint16_t>();
      if (uint16_tValue.isPresent) {
          return *((int16_t *)&uint16_tValue.value);
      }

      auto int16_tValue = tryRead<int16_t>();
      if (int16_tValue.isPresent) {
          return int16_tValue.value;
      }

      assert(false);
  }

  auto readBits32() {
      // expect uint32_t, int32_t, or float
      auto uint32_tValue = tryRead<uint32_t>();
      if (uint32_tValue.isPresent) {
          return *((int32_t *)&uint32_tValue.value);
      }

      auto int32_tValue = tryRead<int32_t>();
      if (int32_tValue.isPresent) {
          return int32_tValue.value;
      }

      auto floatValue = tryRead<float>();
      if (floatValue.isPresent) {
          return *((int32_t *)&floatValue.value);
      }

      assert(false);
  }

  auto readBits64() {
      // expect uint64_t, int64_t, or double
      auto uint64_tValue = tryRead<uint64_t>();
      if (uint64_tValue.isPresent) {
          return *((int64_t *)&uint64_tValue.value);
      }

      auto int64_tValue = tryRead<int64_t>();
      if (int64_tValue.isPresent) {
          return int64_tValue.value;
      }

      auto doubleValue = tryRead<double>();
      if (doubleValue.isPresent) {
          return *((int64_t *)&doubleValue.value);
      }

      assert(false);
  }

  template<int32_t bits, typename T>
  T readBits() {
      if (bits == 8) {
          return static_cast<T>(readBits8());
      }
      if (bits == 16) {
          return static_cast<T>(readBits16());
      }
      if (bits == 32) {
          return static_cast<T>(readBits32());
      }
      if (bits == 64) {
          return static_cast<T>(readBits64());
      }
  }
};

template <typename T>
void interpretMathAdd(Interpreter *interp) {
    auto a = interp->read<T>();
    auto b = interp->read<T>();
    auto result = a + b;
    auto storeOffset = interp->consume<int32_t>();
    interp->copyToStack(result, interp->bp + storeOffset);
}

template <typename T>
void interpretMathSub(Interpreter *interp) {
    auto a = interp->read<T>();
    auto b = interp->read<T>();
    auto result = a - b;
    auto storeOffset = interp->consume<int32_t>();
    interp->copyToStack(result, interp->bp + storeOffset);
}

template <typename T>
void interpretMathMul(Interpreter *interp) {
    auto a = interp->read<T>();
    auto b = interp->read<T>();
    auto result = a * b;
    auto storeOffset = interp->consume<int32_t>();
    interp->copyToStack(result, interp->bp + storeOffset);
}

template <typename T>
void interpretMathDiv(Interpreter *interp) {
    auto a = interp->read<T>();
    auto b = interp->read<T>();
    auto result = a / b;
    auto storeOffset = interp->consume<int32_t>();
    interp->copyToStack(result, interp->bp + storeOffset);
}

template <typename T>
void interpretMathMod(Interpreter *interp) {
    auto a = interp->read<T>();
    auto b = interp->read<T>();
    auto result = a / b;
    auto storeOffset = interp->consume<int32_t>();
    interp->copyToStack(result, interp->bp + storeOffset);
}

// bitwise binary operation
template <int32_t bits, typename T>
void interpretMathBitwiseOr(Interpreter *interp) {
    auto a = interp->readBits<bits, T>();
    auto b = interp->readBits<bits, T>();
    auto result = a | b;
    auto storeOffset = interp->consume<int32_t>();
    interp->copyToStack(result, interp->bp + storeOffset);
}

template <int32_t bits, typename T>
void interpretMathBitwiseAnd(Interpreter *interp) {
    auto a = interp->readBits<bits, T>();
    auto b = interp->readBits<bits, T>();
    auto result = a & b;
    auto storeOffset = interp->consume<int32_t>();
    interp->copyToStack(result, interp->bp + storeOffset);
}

template <int32_t bits, typename T>
void interpretMathBitwiseXor(Interpreter *interp) {
    auto a = interp->readBits<bits, T>();
    auto b = interp->readBits<bits, T>();
    auto result = a ^ b;
    auto storeOffset = interp->consume<int32_t>();
    interp->copyToStack(result, interp->bp + storeOffset);
}

// comparison
template <typename T>
void interpretCmpEq(Interpreter *interp) {
    auto a = interp->read<T>();
    auto b = interp->read<T>();
    int32_t result = a == b ? 1 : 0;
    auto storeOffset = interp->consume<int32_t>();
    interp->copyToStack(result, interp->bp + storeOffset);
}

template <typename T>
void interpretCmpNeq(Interpreter *interp) {
    auto a = interp->read<T>();
    auto b = interp->read<T>();
    int32_t result = a != b ? 1 : 0;
    auto storeOffset = interp->consume<int32_t>();
    interp->copyToStack(result, interp->bp + storeOffset);
}

template <typename T>
void interpretCmpGt(Interpreter *interp) {
    auto a = interp->read<T>();
    auto b = interp->read<T>();
    int32_t result = a > b ? 1 : 0;
    auto storeOffset = interp->consume<int32_t>();
    interp->copyToStack(result, interp->bp + storeOffset);
}

template <typename T>
void interpretCmpGte(Interpreter *interp) {
    auto a = interp->read<T>();
    auto b = interp->read<T>();
    int32_t result = a >= b ? 1 : 0;
    auto storeOffset = interp->consume<int32_t>();
    interp->copyToStack(result, interp->bp + storeOffset);
}

template <typename T>
void interpretCmpLt(Interpreter *interp) {
    auto a = interp->read<T>();
    auto b = interp->read<T>();
    int32_t result = a < b ? 1 : 0;
    auto storeOffset = interp->consume<int32_t>();
    interp->copyToStack(result, interp->bp + storeOffset);
}

template <typename T>
void interpretCmpLte(Interpreter *interp) {
    auto a = interp->read<T>();
    auto b = interp->read<T>();
    int32_t result = a <= b ? 1 : 0;
    auto storeOffset = interp->consume<int32_t>();
    interp->copyToStack(result, interp->bp + storeOffset);
}

#endif // INTERPRETER_H