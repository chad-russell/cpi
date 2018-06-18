; ModuleID = 'module'
source_filename = "module"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-darwin17.6.0"

%0 = type { i8, i8, i8, i8, i8 }
%1 = type { i8 }

@printfFmtStr = private unnamed_addr constant [5 x i8] c"%.*s\00"

declare {} @panic(i8*)

declare i8* @malloc(i64)

declare i32 @printf(i8*, ...)

declare i8* @memset(i8*, i64, i64)

declare {} @free(i8*)

define {} @main() {
entry:
  %a = alloca { i8*, i32 }
  %local10_ = alloca i64
  %local53_ = alloca %0
  %local69_ = alloca %0*
  %char = alloca i8
  %foreach_index = alloca i32
  %foreach_index1 = alloca { i8*, i32 }
  %local103_ = alloca %1
  %local107_ = alloca %1*
  %local124_ = alloca i8*
  %local126_ = alloca i8*
  %local127_ = alloca i64
  %local130_ = alloca i1
  %local133_ = alloca i32
  %local138_ = alloca i32
  %local139_ = alloca i32
  %local140_ = alloca i32
  %local144_ = alloca i8*
  %local146_ = alloca i8*
  %local147_ = alloca i32
  %local148_ = alloca i32
  %local149_ = alloca i32
  store i8 0, i8* %char
  %0 = bitcast %0* %local53_ to { i8, i8, i8, i8, i8 }*
  store { i8, i8, i8, i8, i8 } { i8 104, i8 101, i8 108, i8 108, i8 111 }, { i8, i8, i8, i8, i8 }* %0
  %1 = load %0, %0* %local53_
  store %0 %1, %0* %local53_
  %2 = call i8* @malloc(i64 5)
  %3 = bitcast %0** %local69_ to i8**
  store i8* %2, i8** %3
  %4 = load %0, %0* %local53_
  %5 = bitcast i8* %2 to %0*
  store %0 %4, %0* %5
  %6 = load %0*, %0** %local69_
  %7 = bitcast %0* %6 to i8*
  %8 = insertvalue { i8*, i32 } zeroinitializer, i8* %7, 0
  %9 = insertvalue { i8*, i32 } %8, i32 5, 1
  store { i8*, i32 } %9, { i8*, i32 }* %foreach_index1
  store i32 0, i32* %foreach_index
  br label %cond

cond:                                             ; preds = %then, %entry
  %10 = getelementptr { i8*, i32 }, { i8*, i32 }* %foreach_index1, i32 0, i32 1
  %11 = load i32, i32* %foreach_index
  %12 = load i32, i32* %10
  %13 = icmp slt i32 %11, %12
  store i1 %13, i1* %local130_
  %14 = load i1, i1* %local130_
  br i1 %14, label %then, label %if_cont

then:                                             ; preds = %cond
  %15 = load { i8*, i32 }, { i8*, i32 }* %foreach_index1
  %16 = extractvalue { i8*, i32 } %15, 0
  %17 = load i32, i32* %foreach_index
  %parith = getelementptr i8, i8* %16, i32 %17
  store i8* %parith, i8** %local144_
  %18 = load i8*, i8** %local144_
  %19 = load i8, i8* %18
  store i8 %19, i8* %char
  %20 = bitcast %1* %local103_ to { i8 }*
  store { i8 } { i8 46 }, { i8 }* %20
  %21 = load %1, %1* %local103_
  store %1 %21, %1* %local103_
  %22 = call i8* @malloc(i64 1)
  %23 = bitcast %1** %local107_ to i8**
  store i8* %22, i8** %23
  %24 = load %1, %1* %local103_
  %25 = bitcast i8* %22 to %1*
  store %1 %24, %1* %25
  %26 = load %1*, %1** %local107_
  %27 = bitcast %1* %26 to i8*
  %28 = insertvalue { i8*, i32 } zeroinitializer, i8* %27, 0
  %29 = insertvalue { i8*, i32 } %28, i32 1, 1
  store { i8*, i32 } %29, { i8*, i32 }* %a
  %30 = load { i8*, i32 }, { i8*, i32 }* %a
  %31 = extractvalue { i8*, i32 } %30, 0
  store i64 0, i64* %local10_
  store i64 0, i64* %local10_
  %32 = load i64, i64* %local10_
  %parith2 = getelementptr i8, i8* %31, i64 %32
  store i8* %parith2, i8** %local124_
  %33 = load i8, i8* %char
  %34 = load i8*, i8** %local124_
  store i8 %33, i8* %34
  %35 = load { i8*, i32 }, { i8*, i32 }* %a
  %36 = extractvalue { i8*, i32 } %35, 0
  %37 = load { i8*, i32 }, { i8*, i32 }* %a
  %38 = extractvalue { i8*, i32 } %37, 1
  %39 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @printfFmtStr, i32 0, i32 0), i32 %38, i8* %36)
  %40 = load i32, i32* %foreach_index
  %41 = add i32 %40, 1
  store i32 %41, i32* %local133_
  %42 = load i32, i32* %local133_
  store i32 %42, i32* %foreach_index
  br label %cond

if_cont:                                          ; preds = %cond
  ret {} zeroinitializer
}

!llvm.module.flags = !{!0, !1}

!0 = !{i32 2, !"Dwarf Version", i32 2}
!1 = !{i32 2, !"Debug Info Version", i32 3}
