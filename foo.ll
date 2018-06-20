; ModuleID = 'module'
source_filename = "module"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-darwin17.6.0"

@0 = private unnamed_addr constant [20 x i8] c"assertion failed!!!\00"

declare {} @panic(i8*)

declare i8* @malloc(i64)

declare i32 @printf(i8*, ...)

declare i8* @memset(i8*, i64, i64)

declare {} @free(i8*)

define i64 @main() {
entry:
  %e = alloca { i64, [8 x i8] }
  %local14_ = alloca i64
  %local41_ = alloca i1
  %local43_ = alloca i64
  %local44_ = alloca i64
  %local45_ = alloca i64
  %local48_ = alloca i1
  %local50_ = alloca i1
  %local51_ = alloca i1
  %local59_ = alloca i64
  %0 = bitcast { i64, [8 x i8] }* %e to { i64, i64 }*
  store { i64, i64 } zeroinitializer, { i64, i64 }* %0
  store i64 13, i64* %local14_
  store i64 13, i64* %local14_
  %1 = getelementptr { i64, [8 x i8] }, { i64, [8 x i8] }* %e, i32 0, i32 1
  %2 = load i64, i64* %local14_
  %3 = bitcast [8 x i8]* %1 to i64*
  store i64 %2, i64* %3
  store i64 1, i64* %local59_
  store i64 1, i64* %local59_
  %4 = getelementptr { i64, [8 x i8] }, { i64, [8 x i8] }* %e, i32 0, i32 0
  %5 = load i64, i64* %local59_
  store i64 %5, i64* %4
  %6 = getelementptr { i64, [8 x i8] }, { i64, [8 x i8] }* %e, i32 0, i32 0
  %7 = load i64, i64* %6
  %8 = icmp eq i64 %7, 1
  store i1 %8, i1* %local41_
  %9 = load i1, i1* %local41_
  %10 = icmp eq i1 %9, false
  store i1 %10, i1* %local48_
  %11 = load i1, i1* %local48_
  br i1 %11, label %then, label %else

then:                                             ; preds = %entry
  %12 = call {} @panic(i8* getelementptr inbounds ([20 x i8], [20 x i8]* @0, i32 0, i32 0))
  br label %if_cont

else:                                             ; preds = %entry
  br label %if_cont

if_cont:                                          ; preds = %else, %then
  %13 = getelementptr { i64, [8 x i8] }, { i64, [8 x i8] }* %e, i32 0, i32 1
  %14 = bitcast [8 x i8]* %13 to i64*
  %15 = load i64, i64* %14
  %realRet = alloca i64
  store i64 %15, i64* %realRet
  %16 = load i64, i64* %realRet
  ret i64 %16
}

!llvm.module.flags = !{!0, !1}

!0 = !{i32 2, !"Dwarf Version", i32 2}
!1 = !{i32 2, !"Debug Info Version", i32 3}
