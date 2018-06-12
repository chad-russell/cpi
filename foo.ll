; ModuleID = 'module'
source_filename = "module"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-darwin17.5.0"

%0 = type { i8, i8, i8, i8, i8, i8, i8, i8, i8 }

@printfFmtStr = private unnamed_addr constant [5 x i8] c"%.*s\00"
@printfFmtStr.1 = private unnamed_addr constant [5 x i8] c"%.*s\00"

declare void @panic(i8*)

declare i8* @malloc(i64)

declare i32 @printf(i8*, ...)

declare i8* @memset(i8*, i64, i64)

declare void @free(i8*)

define void @main() {
entry:
  %local77_ = alloca %0, align 8
  %local105_ = alloca %0*, align 8
  %.repack = getelementptr inbounds %0, %0* %local77_, i64 0, i32 0
  store i8 102, i8* %.repack, align 8
  %0 = getelementptr inbounds %0, %0* %local77_, i64 0, i32 1
  store i8 117, i8* %0, align 1
  %1 = getelementptr inbounds %0, %0* %local77_, i64 0, i32 2
  store i8 99, i8* %1, align 2
  %2 = getelementptr inbounds %0, %0* %local77_, i64 0, i32 3
  store i8 107, i8* %2, align 1
  %3 = getelementptr inbounds %0, %0* %local77_, i64 0, i32 4
  store i8 32, i8* %3, align 4
  %4 = getelementptr inbounds %0, %0* %local77_, i64 0, i32 5
  store i8 -16, i8* %4, align 1
  %5 = getelementptr inbounds %0, %0* %local77_, i64 0, i32 6
  store i8 -97, i8* %5, align 2
  %6 = getelementptr inbounds %0, %0* %local77_, i64 0, i32 7
  store i8 -112, i8* %6, align 1
  %7 = getelementptr inbounds %0, %0* %local77_, i64 0, i32 8
  store i8 -110, i8* %7, align 8
  store i8 102, i8* %.repack, align 8
  store i8 117, i8* %0, align 1
  store i8 99, i8* %1, align 2
  store i8 107, i8* %2, align 1
  store i8 32, i8* %3, align 4
  store i8 -16, i8* %4, align 1
  store i8 -97, i8* %5, align 2
  store i8 -112, i8* %6, align 1
  store i8 -110, i8* %7, align 8
  %8 = call i8* @malloc(i64 9)
  %9 = bitcast %0** %local105_ to i8**
  store i8* %8, i8** %9, align 8
  store i8 102, i8* %8, align 1
  %10 = getelementptr inbounds i8, i8* %8, i64 1
  store i8 117, i8* %10, align 1
  %11 = getelementptr inbounds i8, i8* %8, i64 2
  store i8 99, i8* %11, align 1
  %12 = getelementptr inbounds i8, i8* %8, i64 3
  store i8 107, i8* %12, align 1
  %13 = getelementptr inbounds i8, i8* %8, i64 4
  store i8 32, i8* %13, align 1
  %14 = getelementptr inbounds i8, i8* %8, i64 5
  store i8 -16, i8* %14, align 1
  %15 = getelementptr inbounds i8, i8* %8, i64 6
  store i8 -97, i8* %15, align 1
  %16 = getelementptr inbounds i8, i8* %8, i64 7
  store i8 -112, i8* %16, align 1
  %17 = getelementptr inbounds i8, i8* %8, i64 8
  store i8 -110, i8* %17, align 1
  %18 = insertvalue { i8*, i32 } zeroinitializer, i8* %8, 0
  %19 = insertvalue { i8*, i32 } %18, i32 9, 1
  call void @println({ i8*, i32 } %19)
  ret void
}

define void @println({ i8*, i32 }) {
entry:
  %1 = extractvalue { i8*, i32 } %0, 0
  %2 = extractvalue { i8*, i32 } %0, 1
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @printfFmtStr, i64 0, i64 0), i8* %1, i32 %2)
  %4 = call i8* @malloc(i64 1)
  store i8 10, i8* %4, align 1
  %5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @printfFmtStr.1, i64 0, i64 0), i8* %4, i32 1)
  ret void
}

!llvm.module.flags = !{!0, !1}

!0 = !{i32 2, !"Dwarf Version", i32 2}
!1 = !{i32 2, !"Debug Info Version", i32 3}
