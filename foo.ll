; ModuleID = 'module'
source_filename = "module"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-darwin17.6.0"

@0 = private unnamed_addr constant [20 x i8] c"assertion failed!!!\00"

declare void @panic(i8*)

declare i8* @malloc(i64)

declare i32 @printf(i8*, ...)

declare i8* @memset(i8*, i64, i64)

declare void @free(i8*)

define i64 @main() {
entry:
  %t = alloca { i32, [20 x i8] }, align 8
  %.repack = getelementptr inbounds { i32, [20 x i8] }, { i32, [20 x i8] }* %t, i64 0, i32 0
  store i32 2, i32* %.repack, align 8
  %.repack1 = getelementptr inbounds { i32, [20 x i8] }, { i32, [20 x i8] }* %t, i64 0, i32 1
  %0 = bitcast [20 x i8]* %.repack1 to i64*
  store i64 73, i64* %0, align 4
  ret i64 73
}

!llvm.module.flags = !{!0, !1}

!0 = !{i32 2, !"Dwarf Version", i32 2}
!1 = !{i32 2, !"Debug Info Version", i32 3}
