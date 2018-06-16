; ModuleID = 'module'
source_filename = "module"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-darwin17.6.0"

%0 = type { i32 }
%1 = type { i8, i8, i8, i8 }
%2 = type { %3 }
%3 = type { { i8*, i32 }, { i32, [20 x i8] }* }

declare {} @panic(i8*)

declare i8* @malloc(i64)

declare i32 @printf(i8*, ...)

declare i8* @memset(i8*, i64, i64)

declare {} @free(i8*)

define i32 @main() {
entry:
  %ll = alloca %0
  %t = alloca { i32, [20 x i8] }
  %unique__431 = alloca { i32, [20 x i8] }
  %unique__543 = alloca { i32, [20 x i8] }
  %unique__656 = alloca { i32, [20 x i8] }
  %local796_ = alloca %1
  %local809_ = alloca %1*
  %local829_ = alloca %2
  %local831_ = alloca %2*
  %0 = bitcast %0* %ll to { i32 }*
  store { i32 } zeroinitializer, { i32 }* %0
  %1 = bitcast %1* %local796_ to { i8, i8, i8, i8 }*
  store { i8, i8, i8, i8 } { i8 110, i8 101, i8 120, i8 116 }, { i8, i8, i8, i8 }* %1
  %2 = load %1, %1* %local796_
  store %1 %2, %1* %local796_
  %3 = call i8* @malloc(i64 4)
  %4 = bitcast %1** %local809_ to i8**
  store i8* %3, i8** %4
  %5 = load %1, %1* %local796_
  %6 = bitcast i8* %3 to %1*
  store %1 %5, %1* %6
  %7 = load %1*, %1** %local809_
  %8 = bitcast %1* %7 to i8*
  %9 = insertvalue { i8*, i32 } zeroinitializer, i8* %8, 0
  %10 = insertvalue { i8*, i32 } %9, i32 4, 1
  %11 = bitcast { i32, [20 x i8] }* %unique__656 to <{ i32, {} }>*
  store <{ i32, {} }> <{ i32 4, {} zeroinitializer }>, <{ i32, {} }>* %11
  %12 = bitcast { i32, [20 x i8] }* %unique__656 to { i32, {}, i64, {}, {}, {}, float, {}, i1, {}, {}, { { i8*, i32 }, i8* }, { i8*, i32 }, i8*, { i8*, i32 }, i8* }*
  store { i32, {}, i64, {}, {}, {}, float, {}, i1, {}, {}, { { i8*, i32 }, i8* }, { i8*, i32 }, i8*, { i8*, i32 }, i8* } zeroinitializer, { i32, {}, i64, {}, {}, {}, float, {}, i1, {}, {}, { { i8*, i32 }, i8* }, { i8*, i32 }, i8*, { i8*, i32 }, i8* }* %12
  %13 = insertvalue { { i8*, i32 }, { i32, [20 x i8] }* } zeroinitializer, { i8*, i32 } %10, 0
  %14 = insertvalue { { i8*, i32 }, { i32, [20 x i8] }* } %13, { i32, [20 x i8] }* %unique__656, 1
  %15 = insertvalue { { { i8*, i32 }, { i32, [20 x i8] }* } } zeroinitializer, { { i8*, i32 }, { i32, [20 x i8] }* } %14, 0
  %16 = bitcast %2* %local829_ to { { { i8*, i32 }, { i32, [20 x i8] }* } }*
  store { { { i8*, i32 }, { i32, [20 x i8] }* } } %15, { { { i8*, i32 }, { i32, [20 x i8] }* } }* %16
  %17 = load %2, %2* %local829_
  store %2 %17, %2* %local829_
  %18 = call i8* @malloc(i64 24)
  %19 = bitcast %2** %local831_ to i8**
  store i8* %18, i8** %19
  %20 = load %2, %2* %local829_
  %21 = bitcast i8* %18 to %2*
  store %2 %20, %2* %21
  %22 = load %2*, %2** %local831_
  %23 = bitcast %2* %22 to %3*
  %24 = insertvalue { %3*, i32 } zeroinitializer, %3* %23, 0
  %25 = insertvalue { %3*, i32 } %24, i32 1, 1
  %26 = insertvalue <{ i32, { %3*, i32 } }> <{ i32 12, { %3*, i32 } zeroinitializer }>, { %3*, i32 } %25, 1
  %27 = bitcast { i32, [20 x i8] }* %unique__543 to <{ i32, { %3*, i32 } }>*
  store <{ i32, { %3*, i32 } }> %26, <{ i32, { %3*, i32 } }>* %27
  %28 = bitcast { i32, [20 x i8] }* %unique__543 to { i32, {}, i64, {}, {}, {}, float, {}, i1, {}, {}, { { i8*, i32 }, i8* }, { i8*, i32 }, i8*, { i8*, i32 }, i8* }*
  store { i32, {}, i64, {}, {}, {}, float, {}, i1, {}, {}, { { i8*, i32 }, i8* }, { i8*, i32 }, i8*, { i8*, i32 }, i8* } zeroinitializer, { i32, {}, i64, {}, {}, {}, float, {}, i1, {}, {}, { { i8*, i32 }, i8* }, { i8*, i32 }, i8*, { i8*, i32 }, i8* }* %28
  %29 = getelementptr { i32, [20 x i8] }, { i32, [20 x i8] }* %t, i32 0, i32 0
  %30 = load i32, i32* %29
  %realRet = alloca i32
  store i32 %30, i32* %realRet
  %31 = load i32, i32* %realRet
  ret i32 %31
}

!llvm.module.flags = !{!0, !1}

!0 = !{i32 2, !"Dwarf Version", i32 2}
!1 = !{i32 2, !"Debug Info Version", i32 3}
