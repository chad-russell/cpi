<<<<<<< Updated upstream
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
  %local26_ = alloca i1
  %local28_ = alloca i64
  %local29_ = alloca i64
  %local30_ = alloca i64
  %local33_ = alloca i1
  %local35_ = alloca i1
  %local36_ = alloca i1
  %0 = bitcast { i64, [8 x i8] }* %e to { i64, i64 }*
  store { i64, i64 } zeroinitializer, { i64, i64 }* %0
  %1 = getelementptr { i64, [8 x i8] }, { i64, [8 x i8] }* %e, i32 0, i32 0
  %2 = load i64, i64* %1
  %3 = icmp eq i64 %2, 1
  store i1 %3, i1* %local26_
  %4 = load i1, i1* %local26_
  %5 = icmp eq i1 %4, false
  store i1 %5, i1* %local33_
  %6 = load i1, i1* %local33_
  br i1 %6, label %then, label %else

then:                                             ; preds = %entry
  %7 = call {} @panic(i8* getelementptr inbounds ([20 x i8], [20 x i8]* @0, i32 0, i32 0))
  br label %if_cont

else:                                             ; preds = %entry
  br label %if_cont

if_cont:                                          ; preds = %else, %then
  %8 = getelementptr { i64, [8 x i8] }, { i64, [8 x i8] }* %e, i32 0, i32 1
  %9 = bitcast [8 x i8]* %8 to i64*
  %10 = load i64, i64* %9
  %realRet = alloca i64
  store i64 %10, i64* %realRet
  %11 = load i64, i64* %realRet
  ret i64 %11
}

!llvm.module.flags = !{!0, !1}

!0 = !{i32 2, !"Dwarf Version", i32 2}
!1 = !{i32 2, !"Debug Info Version", i32 3}
=======
>>>>>>> Stashed changes
