; ModuleID = 'module'
source_filename = "module"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-darwin17.6.0"

%0 = type { i64, i64 }
%1 = type { i64, i64, i64 }
%2 = type { i64, i64 }
%3 = type { i64, %3* }
%4 = type { i64, %4******* }
%5 = type { i32, %6* }
%6 = type { i32, %7 }
%7 = type { i32, %8** }
%8 = type { i32**, i32 ()** }
%9 = type { i32, %10** }
%10 = type { i32, %11 }
%11 = type { i32, %12***** }
%12 = type { i32, %9* }
%13 = type { i32, %14* }
%14 = type { i32, %15* }
%15 = type { i32, %16* }
%16 = type { i64** }
%17 = type { %18, i64 }
%18 = type { i64, i64 }
%19 = type { i64, i64 }
%20 = type { i64, i64, i64, i64, i64 }
%21 = type { %22, %23, %24 }
%22 = type { i64, i64 }
%23 = type { i64, i64 }
%24 = type { i64, i64 }
%25 = type { i32, i32, i32 }
%26 = type { i32, i32, i32 }
%27 = type { i64**, i64** }
%28 = type { %27, %27, %27 }
%29 = type { %30 }
%30 = type { i64*, i32 }
%31 = type { i64, i64, i64 }
%32 = type { i64, i64, i64 }
%33 = type { i64, i64, i64 }
%34 = type { i64, i64, i64 }
%35 = type { { i64*, i64 }, { i64*, i64 }, { i64*, i64 } }
%36 = type { i64, i64, i64, i64 }
%37 = type { i32, i32, i32, i32, i32 }
%38 = type { i32, i32, i32, i32, i32 }
%39 = type { i64, i64, i64 }
%40 = type { i64, i64, i64 }
%41 = type { i64, i64, i64, i64, i64 }
%42 = type { i64 }
%43 = type { i32, i32 }
%44 = type { i64, { %45*, i64 } }
%45 = type { i32, i32 }
%46 = type { { i64, [24 x i8] }, i8* }
%47 = type { i8, i8, i8, i8 }
%48 = type { %49 }
%49 = type { { i8*, i64 }, { i64, [24 x i8] }* }
%50 = type { i8, i8, i8, i8 }
%51 = type { i8, i8, i8, i8, i8, i8 }
%52 = type { i64, i64, i64 }
%53 = type { i8, i8 }
%54 = type { i64, { i8*, i64 }, i1, { i64*, i64 }, %55, { i64, [8 x i8] } }
%55 = type { i64, i64* }
%56 = type { i8, i8 }
%57 = type { { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 } }
%58 = type { i8, i8, i8, i8, i8 }
%59 = type { i8, i8, i8 }
%60 = type { i8, i8 }
%61 = type { { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 } }
%62 = type { i8, i8 }
%63 = type { i8, i8 }
%64 = type { i8, i8 }
%65 = type { i8, i8 }
%66 = type { { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 } }
%67 = type { i8, i8 }
%68 = type { { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 } }
%69 = type { i8, i8 }
%70 = type { i8, i8 }
%71 = type { i8, i8 }
%72 = type { { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 }, { i8*, i64 } }
%73 = type { i8, i8, i8, i8, i8 }

@0 = private unnamed_addr constant [20 x i8] c"assertion failed!!!\00"
@1 = private unnamed_addr constant [20 x i8] c"assertion failed!!!\00"
@2 = private unnamed_addr constant [20 x i8] c"assertion failed!!!\00"
@3 = private unnamed_addr constant [20 x i8] c"assertion failed!!!\00"
@4 = private unnamed_addr constant [20 x i8] c"assertion failed!!!\00"
@5 = private unnamed_addr constant [20 x i8] c"assertion failed!!!\00"
@6 = private unnamed_addr constant [20 x i8] c"assertion failed!!!\00"
@7 = private unnamed_addr constant [20 x i8] c"assertion failed!!!\00"
@8 = private unnamed_addr constant [20 x i8] c"assertion failed!!!\00"
@9 = private unnamed_addr constant [20 x i8] c"assertion failed!!!\00"
@10 = private unnamed_addr constant [20 x i8] c"assertion failed!!!\00"
@printfFmtStr = private unnamed_addr constant [5 x i8] c"%.*s\00"
@printfFmtStr.15 = private unnamed_addr constant [5 x i8] c"%.*s\00"
@printfFmtStr.17 = private unnamed_addr constant [5 x i8] c"%.*s\00"
@printfFmtStr.19 = private unnamed_addr constant [5 x i8] c"%.*s\00"
@printfFmtStr.21 = private unnamed_addr constant [5 x i8] c"%.*s\00"
@printfFmtStr.23 = private unnamed_addr constant [5 x i8] c"%.*s\00"
@printfFmtStr.25 = private unnamed_addr constant [5 x i8] c"%.*s\00"
@printfFmtStr.30 = private unnamed_addr constant [5 x i8] c"%.*s\00"
@printfFmtStr.31 = private unnamed_addr constant [5 x i8] c"%.*s\00"
@printfFmtStr.33 = private unnamed_addr constant [5 x i8] c"%.*s\00"
@printfFmtStr.35 = private unnamed_addr constant [5 x i8] c"%.*s\00"
@printfFmtStr.37 = private unnamed_addr constant [5 x i8] c"%.*s\00"
@printfFmtStr.39 = private unnamed_addr constant [5 x i8] c"%.*s\00"
@printfFmtStr.44 = private unnamed_addr constant [5 x i8] c"%.*s\00"
@printfFmtStr.46 = private unnamed_addr constant [5 x i8] c"%.*s\00"
@printfFmtStr.48 = private unnamed_addr constant [5 x i8] c"%.*s\00"
@printfFmtStr.50 = private unnamed_addr constant [5 x i8] c"%.*s\00"
@printfFmtStr.53 = private unnamed_addr constant [5 x i8] c"%.*s\00"
@printfFmtStr.54 = private unnamed_addr constant [5 x i8] c"%.*s\00"
@printfFmtStr.55 = private unnamed_addr constant [5 x i8] c"%.*s\00"
@printfFmtStr.56 = private unnamed_addr constant [5 x i8] c"%.*s\00"
@printfFmtStr.58 = private unnamed_addr constant [5 x i8] c"%.*s\00"
@printfFmtStr.60 = private unnamed_addr constant [5 x i8] c"%.*s\00"
@printfFmtStr.62 = private unnamed_addr constant [5 x i8] c"%.*s\00"
@printfFmtStr.64 = private unnamed_addr constant [5 x i8] c"%.*s\00"
@11 = private unnamed_addr constant [20 x i8] c"assertion failed!!!\00"
@printfFmtStr.70 = private unnamed_addr constant [5 x i8] c"%.*s\00"
@printfFmtStr.72 = private unnamed_addr constant [5 x i8] c"%.*s\00"
@12 = private unnamed_addr constant [20 x i8] c"assertion failed!!!\00"
@printfFmtStr.74 = private unnamed_addr constant [5 x i8] c"%.*s\00"
@printfFmtStr.75 = private unnamed_addr constant [5 x i8] c"%.*s\00"
@printfFmtStr.77 = private unnamed_addr constant [5 x i8] c"%.*s\00"
@printfFmtStr.78 = private unnamed_addr constant [5 x i8] c"%.*s\00"

declare {} @panic(i8*)

declare i8* @malloc(i64)

declare i32 @printf(i8*, ...)

declare i8* @memset(i8*, i64, i64)

declare {} @free(i8*)

define i64 @main() {
entry:
  %p = alloca %0, align 8
  %local373_ = alloca i64, align 8
  %local461_ = alloca %1, align 8
  %testArr1 = alloca { i64*, i64 }, align 8
  %0 = call i64 ()* ()* ()* @anon()
  %1 = call i64 ()* ()* %0()
  %2 = call i64 ()* %1()
  %3 = call i64 %2()
  %4 = icmp eq i64 %3, 3
  %5 = call {} @cpi_assert(i1 %4)
  %6 = call i64 @apply(i64 (i64)* nonnull @inc, i64 1)
  %7 = call i64 @apply_overboard(i64 (i64)* nonnull @inc, i64 0)
  %8 = add i64 %7, %6
  %9 = icmp eq i64 %8, 3
  %10 = call {} @cpi_assert(i1 %9)
  %11 = call {} @cpi_assert(i1 true)
  %12 = call {} @cpi_assert(i1 true)
  %13 = call i64 @a5_fn()
  %14 = icmp eq i64 %13, 3
  %15 = call {} @cpi_assert(i1 %14)
  %16 = call i64 @a5_fn()
  %17 = icmp eq i64 %16, 3
  %18 = call {} @cpi_assert(i1 %17)
  %19 = call i64 @anon.4()
  %20 = icmp eq i64 %19, 3
  %21 = call {} @cpi_assert(i1 %20)
  %22 = call i64 @bar()
  %23 = icmp eq i64 %22, 3
  %24 = call {} @cpi_assert(i1 %23)
  %25 = call i64 @testA9()
  %26 = icmp eq i64 %25, 3
  %27 = call {} @cpi_assert(i1 %26)
  %28 = call i64 @testStruct1()
  %29 = icmp eq i64 %28, 3
  %30 = call {} @cpi_assert(i1 %29)
  %31 = call i64 @testStruct2()
  %32 = icmp eq i64 %31, 3
  %33 = call {} @cpi_assert(i1 %32)
  %34 = call i64 @testNestedCall()
  %35 = icmp eq i64 %34, 3
  %36 = call {} @cpi_assert(i1 %35)
  %37 = call i64 @testDerefParam()
  %38 = icmp eq i64 %37, 3
  %39 = call {} @cpi_assert(i1 %38)
  %40 = call i64 @testSetter()
  %41 = icmp eq i64 %40, 3
  %42 = call {} @cpi_assert(i1 %41)
  %43 = call i64 @testLinkedList()
  %44 = icmp eq i64 %43, 3
  %45 = call {} @cpi_assert(i1 %44)
  %46 = call i64 @testWeirdLinkedList()
  %47 = icmp eq i64 %46, 9
  %48 = call {} @cpi_assert(i1 %47)
  %49 = call i64 @testMoreDotsAndDerefs()
  %50 = icmp eq i64 %49, 3
  %51 = call {} @cpi_assert(i1 %50)
  %52 = call {} @test23()
  %53 = call {} @test24()
  %54 = call {} @test25()
  %55 = call i64 @testDoubleDeref()
  %56 = icmp eq i64 %55, 3
  %57 = call {} @cpi_assert(i1 %56)
  %58 = call i64 @more_struct()
  %59 = icmp eq i64 %58, 3
  %60 = call {} @cpi_assert(i1 %59)
  %61 = call i64 @test_S2_stuff()
  %62 = icmp eq i64 %61, 3
  %63 = call {} @cpi_assert(i1 %62)
  %64 = call i64 @testSetFromParam()
  %65 = icmp eq i64 %64, 3
  %66 = call {} @cpi_assert(i1 %65)
  %67 = call i32 @fib(i32 10)
  %68 = icmp eq i32 %67, 89
  %69 = call {} @cpi_assert(i1 %68)
  %70 = call i64 @stupid_sqrt(i64 5)
  %71 = icmp eq i64 %70, 0
  %72 = call {} @cpi_assert(i1 %71)
  %73 = call i64 @stupid_sqrt(i64 9)
  %74 = icmp eq i64 %73, 3
  %75 = call {} @cpi_assert(i1 %74)
  %76 = call i32 @id(i32 3)
  %77 = icmp eq i32 %76, 3
  %78 = call {} @cpi_assert(i1 %77)
  %.repack = getelementptr inbounds %0, %0* %p, i64 0, i32 0
  store i64 0, i64* %.repack, align 8
  %79 = getelementptr inbounds %0, %0* %p, i64 0, i32 1
  store i64 0, i64* %79, align 8
  store i64 8, i64* %.repack, align 8
  %80 = call %0 @id.6(%0 { i64 8, i64 0 })
  %.elt = extractvalue %0 %80, 0
  %81 = icmp eq i64 %.elt, 8
  %82 = call {} @cpi_assert(i1 %81)
  store i64 3, i64* %local373_, align 8
  %83 = call i64 @deref_ct(i64* nonnull %local373_)
  %84 = icmp eq i64 %83, 3
  %85 = call {} @cpi_assert(i1 %84)
  %86 = call i32 @add_static(i32 5)
  %87 = icmp eq i32 %86, 12
  %88 = call {} @cpi_assert(i1 %87)
  %89 = call i32 @apply_static(i32 3)
  %90 = icmp eq i32 %89, 4
  %91 = call {} @cpi_assert(i1 %90)
  %92 = call i32 @apply_static.7(i32 3)
  %93 = icmp eq i32 %92, 6
  %94 = call {} @cpi_assert(i1 %93)
  %95 = call %0 @id.9(%0 { i64 8, i64 0 })
  %.elt94 = extractvalue %0 %95, 0
  %96 = icmp eq i64 %.elt94, 8
  %97 = call {} @cpi_assert(i1 %96)
  %98 = call {} @testArray1()
  %99 = call {} @testArray2()
  %100 = call {} @testArray3()
  %101 = call {} @testArray4()
  %102 = call {} @testArray5()
  %.repack97 = getelementptr inbounds %1, %1* %local461_, i64 0, i32 0
  store i64 1, i64* %.repack97, align 8
  %103 = getelementptr inbounds %1, %1* %local461_, i64 0, i32 1
  store i64 2, i64* %103, align 8
  %104 = getelementptr inbounds %1, %1* %local461_, i64 0, i32 2
  store i64 3, i64* %104, align 8
  store i64 1, i64* %.repack97, align 8
  store i64 2, i64* %103, align 8
  store i64 3, i64* %104, align 8
  %105 = insertvalue { %1*, i32 } zeroinitializer, %1* %local461_, 0
  %106 = insertvalue { %1*, i32 } %105, i32 3, 1
  %107 = bitcast { i64*, i64 }* %testArr1 to { %1*, i32 }*
  store { %1*, i32 } %106, { %1*, i32 }* %107, align 8
  %.elt103 = getelementptr inbounds { i64*, i64 }, { i64*, i64 }* %testArr1, i64 0, i32 0
  %.unpack104 = load i64*, i64** %.elt103, align 8
  %parith = getelementptr i64, i64* %.unpack104, i64 1
  %108 = load i64, i64* %parith, align 8
  %109 = call i64 @inc(i64 %108)
  %110 = icmp eq i64 %109, 3
  %111 = call {} @cpi_assert(i1 %110)
  %112 = call {} @testWhile1()
  %113 = call {} @cpi_assert(i1 true)
  %114 = call {} @cpi_assert(i1 true)
  %115 = call {} @cpi_assert(i1 true)
  %116 = call {} @testMalloc()
  %117 = call {} @testFor1()
  %118 = call {} @testFor2()
  %119 = call {} @testFor3()
  %120 = call {} @testFor4()
  %121 = call {} @testUnion1()
  %122 = call {} @testBuffer()
  %123 = call {} @testDefault()
  %124 = call {} @testAnySimple1()
  %125 = call {} @testPrinting()
  ret i64 0
}

define i64 ()* ()* ()* @anon() {
entry:
  ret i64 ()* ()* ()* @anon.1
}

define i64 ()* ()* @anon.1() {
entry:
  ret i64 ()* ()* @foo
}

define i64 ()* @foo() {
entry:
  ret i64 ()* @bar
}

define i64 @bar() {
entry:
  ret i64 3
}

define {} @cpi_assert(i1) {
entry:
  br i1 %0, label %if_cont, label %then

then:                                             ; preds = %entry
  %1 = call {} @panic(i8* getelementptr inbounds ([20 x i8], [20 x i8]* @0, i64 0, i64 0))
  br label %if_cont

if_cont:                                          ; preds = %entry, %then
  ret {} zeroinitializer
}

define i64 @inc(i64) {
entry:
  %1 = add i64 %0, 1
  ret i64 %1
}

define i64 @apply(i64 (i64)*, i64) {
entry:
  %2 = call i64 %0(i64 %1)
  ret i64 %2
}

define i64 @apply_overboard(i64 (i64)*, i64) {
entry:
  %2 = call i64 @anon.2(i64 (i64)* %0, i64 %1)
  ret i64 %2
}

define i64 @anon.2(i64 (i64)*, i64) {
entry:
  %2 = call i64 %0(i64 %1)
  ret i64 %2
}

define i64 @a5_fn() {
entry:
  %0 = call i64 @anon.3()
  ret i64 %0
}

define i64 @anon.3() {
entry:
  ret i64 3
}

define i64 @anon.4() {
entry:
  ret i64 3
}

define i64 @bad() {
entry:
  ret i64 999
}

define i64 @testA9() {
entry:
  %0 = call i64 @testA9Helper()
  ret i64 %0
}

define i64 @testA9Helper() {
entry:
  %0 = call i64 @bad()
  ret i64 3
}

define i64 @testStruct1() {
entry:
  ret i64 3
}

define i64 @testStruct2() {
entry:
  ret i64 3
}

define i64 @testNestedCall() {
entry:
  %0 = call i64 @bad()
  %1 = insertvalue %2 { i64 3, i64 undef }, i64 %0, 1
  %2 = call %2 @point_id(%2 %1)
  %.elt = extractvalue %2 %2, 0
  %.elt15 = extractvalue %2 %2, 1
  %3 = call %2 @point_id(%2 %2)
  %.elt25 = extractvalue %2 %3, 0
  %.elt27 = extractvalue %2 %3, 1
  %4 = call %2 @point_id(%2 %3)
  %.elt37 = extractvalue %2 %4, 0
  %.elt39 = extractvalue %2 %4, 1
  %5 = call %2 @point_id(%2 %4)
  %.elt49 = extractvalue %2 %5, 0
  %.elt51 = extractvalue %2 %5, 1
  %6 = call %2 @point_id(%2 %5)
  %.elt61 = extractvalue %2 %6, 0
  %.elt63 = extractvalue %2 %6, 1
  %7 = call %2 @point_id(%2 %6)
  %.elt73 = extractvalue %2 %7, 0
  %.elt75 = extractvalue %2 %7, 1
  %8 = call i64 @point_get_x(%2 %7)
  ret i64 %8
}

define %2 @point_id(%2) {
entry:
  %.elt = extractvalue %2 %0, 0
  %.elt2 = extractvalue %2 %0, 1
  ret %2 %0
}

define i64 @point_get_x(%2) {
entry:
  %1 = extractvalue %2 %0, 0
  ret i64 %1
}

define i64 @testDerefParam() {
entry:
  %local2432_ = alloca i64, align 8
  store i64 3, i64* %local2432_, align 8
  %0 = call i64 @deref(i64* nonnull %local2432_)
  ret i64 %0
}

define i64 @deref(i64*) {
entry:
  %1 = load i64, i64* %0, align 8
  ret i64 %1
}

define i64 @testSetter() {
entry:
  %p = alloca %2, align 8
  %.repack = getelementptr inbounds %2, %2* %p, i64 0, i32 0
  store i64 0, i64* %.repack, align 8
  %0 = getelementptr inbounds %2, %2* %p, i64 0, i32 1
  store i64 0, i64* %0, align 8
  %1 = call i64 @bad()
  store i64 %1, i64* %.repack, align 8
  %2 = call i64 @bad()
  store i64 %2, i64* %0, align 8
  %3 = call i64 @set_y(%2* nonnull %p, i64 3)
  %4 = load i64, i64* %0, align 8
  ret i64 %4
}

define i64 @set_y(%2*, i64) {
entry:
  %2 = getelementptr %2, %2* %0, i64 0, i32 1
  store i64 %1, i64* %2, align 8
  ret i64 %1
}

define i64 @testLinkedList() {
entry:
  %l = alloca %3, align 8
  %.repack = getelementptr inbounds %3, %3* %l, i64 0, i32 0
  store i64 0, i64* %.repack, align 8
  %.repack1 = getelementptr inbounds %3, %3* %l, i64 0, i32 1
  %0 = bitcast %3** %.repack1 to i8**
  store i8* null, i8** %0, align 8
  store i64 3, i64* %.repack, align 8
  %.cast = ptrtoint %3* %l to i64
  %1 = bitcast %3** %.repack1 to i64*
  store i64 %.cast, i64* %1, align 8
  ret i64 3
}

define i64 @testWeirdLinkedList() {
entry:
  %l = alloca %4, align 8
  %local1963_ = alloca %4*, align 8
  %local1962_ = alloca %4**, align 8
  %local1961_ = alloca %4***, align 8
  %local1960_ = alloca %4****, align 8
  %local1959_ = alloca %4*****, align 8
  %local1958_ = alloca %4******, align 8
  %.repack = getelementptr inbounds %4, %4* %l, i64 0, i32 0
  store i64 0, i64* %.repack, align 8
  %.repack1 = getelementptr %4, %4* %l, i64 0, i32 1
  %0 = bitcast %4******** %.repack1 to i8**
  store i8* null, i8** %0, align 8
  store i64 999, i64* %.repack, align 8
  store %4* %l, %4** %local1963_, align 8
  store %4** %local1963_, %4*** %local1962_, align 8
  store %4*** %local1962_, %4**** %local1961_, align 8
  store %4**** %local1961_, %4***** %local1960_, align 8
  store %4***** %local1960_, %4****** %local1959_, align 8
  store %4****** %local1959_, %4******* %local1958_, align 8
  %.cast = ptrtoint %4******* %local1958_ to i64
  %1 = bitcast %4******** %.repack1 to i64*
  store i64 %.cast, i64* %1, align 8
  %2 = load %4*****, %4****** %local1959_, align 8
  %3 = load %4****, %4***** %2, align 8
  %4 = load %4***, %4**** %3, align 8
  %5 = load %4**, %4*** %4, align 8
  %6 = load %4*, %4** %5, align 8
  %7 = getelementptr %4, %4* %6, i64 0, i32 1
  %8 = load %4*******, %4******** %7, align 8
  %9 = load %4******, %4******* %8, align 8
  %10 = load %4*****, %4****** %9, align 8
  %11 = load %4****, %4***** %10, align 8
  %12 = load %4***, %4**** %11, align 8
  %13 = load %4**, %4*** %12, align 8
  %14 = load %4*, %4** %13, align 8
  %15 = getelementptr %4, %4* %14, i64 0, i32 1
  %16 = load %4*******, %4******** %15, align 8
  %17 = load %4******, %4******* %16, align 8
  %18 = load %4*****, %4****** %17, align 8
  %19 = load %4****, %4***** %18, align 8
  %20 = load %4***, %4**** %19, align 8
  %21 = load %4**, %4*** %20, align 8
  %22 = load %4*, %4** %21, align 8
  %23 = getelementptr %4, %4* %22, i64 0, i32 1
  %24 = load %4*******, %4******** %23, align 8
  %25 = load %4******, %4******* %24, align 8
  %26 = load %4*****, %4****** %25, align 8
  %27 = load %4****, %4***** %26, align 8
  %28 = load %4***, %4**** %27, align 8
  %29 = load %4**, %4*** %28, align 8
  %30 = load %4*, %4** %29, align 8
  %31 = getelementptr %4, %4* %30, i64 0, i32 1
  %32 = load %4*******, %4******** %31, align 8
  %33 = load %4******, %4******* %32, align 8
  %34 = load %4*****, %4****** %33, align 8
  %35 = load %4****, %4***** %34, align 8
  %36 = load %4***, %4**** %35, align 8
  %37 = load %4**, %4*** %36, align 8
  %38 = load %4*, %4** %37, align 8
  %39 = getelementptr %4, %4* %38, i64 0, i32 1
  %40 = load %4*******, %4******** %39, align 8
  %41 = load %4******, %4******* %40, align 8
  %42 = load %4*****, %4****** %41, align 8
  %43 = load %4****, %4***** %42, align 8
  %44 = load %4***, %4**** %43, align 8
  %45 = load %4**, %4*** %44, align 8
  %46 = load %4*, %4** %45, align 8
  %47 = getelementptr %4, %4* %46, i64 0, i32 1
  %48 = load %4*******, %4******** %47, align 8
  %49 = load %4******, %4******* %48, align 8
  %50 = load %4*****, %4****** %49, align 8
  %51 = load %4****, %4***** %50, align 8
  %52 = load %4***, %4**** %51, align 8
  %53 = load %4**, %4*** %52, align 8
  %54 = load %4*, %4** %53, align 8
  %55 = getelementptr %4, %4* %54, i64 0, i32 0
  store i64 3, i64* %55, align 8
  %56 = load %4*******, %4******** %.repack1, align 8
  %57 = load %4******, %4******* %56, align 8
  %58 = load %4*****, %4****** %57, align 8
  %59 = load %4****, %4***** %58, align 8
  %60 = load %4***, %4**** %59, align 8
  %61 = load %4**, %4*** %60, align 8
  %62 = load %4*, %4** %61, align 8
  %63 = getelementptr %4, %4* %62, i64 0, i32 0
  %64 = load i64, i64* %.repack, align 8
  %65 = load i64, i64* %63, align 8
  %66 = add i64 %65, %64
  %67 = getelementptr %4, %4* %62, i64 0, i32 1
  %68 = load %4*******, %4******** %67, align 8
  %69 = load %4******, %4******* %68, align 8
  %70 = load %4*****, %4****** %69, align 8
  %71 = load %4****, %4***** %70, align 8
  %72 = load %4***, %4**** %71, align 8
  %73 = load %4**, %4*** %72, align 8
  %74 = load %4*, %4** %73, align 8
  %75 = getelementptr %4, %4* %74, i64 0, i32 1
  %76 = load %4*******, %4******** %75, align 8
  %77 = load %4******, %4******* %76, align 8
  %78 = load %4*****, %4****** %77, align 8
  %79 = load %4****, %4***** %78, align 8
  %80 = load %4***, %4**** %79, align 8
  %81 = load %4**, %4*** %80, align 8
  %82 = load %4*, %4** %81, align 8
  %83 = getelementptr %4, %4* %82, i64 0, i32 1
  %84 = load %4*******, %4******** %83, align 8
  %85 = load %4******, %4******* %84, align 8
  %86 = load %4*****, %4****** %85, align 8
  %87 = load %4****, %4***** %86, align 8
  %88 = load %4***, %4**** %87, align 8
  %89 = load %4**, %4*** %88, align 8
  %90 = load %4*, %4** %89, align 8
  %91 = getelementptr %4, %4* %90, i64 0, i32 0
  %92 = load i64, i64* %91, align 8
  %93 = add i64 %92, %66
  ret i64 %93
}

define i64 @testMoreDotsAndDerefs() {
entry:
  ret i64 3
}

define {} @test23() {
entry:
  %foo = alloca %5, align 8
  %bar = alloca %6, align 8
  %baz = alloca %7, align 8
  %qux = alloca %8, align 8
  %local2918_ = alloca i32, align 4
  %local2917_ = alloca i32*, align 8
  %local2924_ = alloca i64 ()*, align 8
  %local2932_ = alloca %8*, align 8
  %0 = bitcast %5* %foo to { i32, i8* }*
  store { i32, i8* } zeroinitializer, { i32, i8* }* %0, align 8
  %1 = bitcast %6* %bar to { i32, { i32, i8* } }*
  store { i32, { i32, i8* } } zeroinitializer, { i32, { i32, i8* } }* %1, align 8
  %2 = bitcast %7* %baz to { i32, i8* }*
  store { i32, i8* } zeroinitializer, { i32, i8* }* %2, align 8
  %.repack = bitcast %8* %qux to i8**
  store i8* null, i8** %.repack, align 8
  %.repack6 = getelementptr inbounds %8, %8* %qux, i64 0, i32 1
  %3 = bitcast i32 ()*** %.repack6 to i8**
  store i8* null, i8** %3, align 8
  %4 = getelementptr inbounds %5, %5* %foo, i64 0, i32 0
  store i32 111, i32* %4, align 8
  %5 = getelementptr inbounds %6, %6* %bar, i64 0, i32 0
  store i32 222, i32* %5, align 8
  %6 = getelementptr inbounds %7, %7* %baz, i64 0, i32 0
  store i32 333, i32* %6, align 8
  store i32 444, i32* %local2918_, align 4
  store i32* %local2918_, i32** %local2917_, align 8
  %.cast = ptrtoint i32** %local2917_ to i64
  %7 = bitcast %8* %qux to i64*
  store i64 %.cast, i64* %7, align 8
  store i64 ()* @anon.5, i64 ()** %local2924_, align 8
  %.cast7 = ptrtoint i64 ()** %local2924_ to i64
  %8 = bitcast i32 ()*** %.repack6 to i64*
  store i64 %.cast7, i64* %8, align 8
  store %8* %qux, %8** %local2932_, align 8
  %9 = getelementptr inbounds %7, %7* %baz, i64 0, i32 1
  %.cast8 = ptrtoint %8** %local2932_ to i64
  %10 = bitcast %8*** %9 to i64*
  store i64 %.cast8, i64* %10, align 8
  %11 = getelementptr inbounds %6, %6* %bar, i64 0, i32 1
  %12 = load %7, %7* %baz, align 8
  store %7 %12, %7* %11, align 8
  %13 = getelementptr inbounds %5, %5* %foo, i64 0, i32 1
  %.cast9 = ptrtoint %6* %bar to i64
  %14 = bitcast %6** %13 to i64*
  store i64 %.cast9, i64* %14, align 8
  %15 = load %6, %6* %bar, align 8
  %16 = call {} @setComplexBarFromParam(%6 %15)
  %17 = getelementptr %6, %6* %bar, i64 0, i32 1, i32 1
  %18 = load %8**, %8*** %17, align 8
  %19 = load %8*, %8** %18, align 8
  %20 = getelementptr %8, %8* %19, i64 0, i32 1
  %21 = load i32 ()**, i32 ()*** %20, align 8
  %22 = load i32 ()*, i32 ()** %21, align 8
  %23 = call i32 %22()
  %24 = icmp eq i32 %23, 123
  %25 = call {} @cpi_assert(i1 %24)
  %26 = bitcast %8*** %17 to i32*****
  %27 = load i32****, i32***** %26, align 8
  %28 = load i32***, i32**** %27, align 8
  %29 = load i32**, i32*** %28, align 8
  %30 = load i32*, i32** %29, align 8
  %31 = load i32, i32* %30, align 4
  %32 = icmp eq i32 %31, 3
  %33 = call {} @cpi_assert(i1 %32)
  %34 = load i32****, i32***** %26, align 8
  %35 = load i32***, i32**** %34, align 8
  %36 = load i32**, i32*** %35, align 8
  %37 = load i32*, i32** %36, align 8
  %38 = load i32, i32* %37, align 4
  %39 = icmp eq i32 %38, 3
  %40 = call {} @cpi_assert(i1 %39)
  %41 = bitcast %8*** %9 to i32*****
  %42 = inttoptr i64 %.cast8 to i32****
  %43 = load i32***, i32**** %42, align 8
  %44 = load i32**, i32*** %43, align 8
  %45 = load i32*, i32** %44, align 8
  %46 = load i32, i32* %45, align 4
  %47 = icmp eq i32 %46, 3
  %48 = call {} @cpi_assert(i1 %47)
  %49 = getelementptr inbounds %8, %8* %qux, i64 0, i32 0
  %50 = load i32**, i32*** %49, align 8
  %51 = load i32*, i32** %50, align 8
  %52 = load i32, i32* %51, align 4
  %53 = icmp eq i32 %52, 3
  %54 = call {} @cpi_assert(i1 %53)
  ret {} zeroinitializer
}

define i64 @anon.5() {
entry:
  ret i64 123
}

define {} @setComplexBarFromParam(%6) {
entry:
  %local2960_ = alloca %7, align 8
  %1 = extractvalue %6 %0, 1
  store %7 %1, %7* %local2960_, align 8
  %2 = getelementptr inbounds %7, %7* %local2960_, i64 0, i32 1
  %3 = bitcast %8*** %2 to i32*****
  %4 = load i32****, i32***** %3, align 8
  %5 = load i32***, i32**** %4, align 8
  %6 = load i32**, i32*** %5, align 8
  %7 = load i32*, i32** %6, align 8
  store i32 3, i32* %7, align 4
  ret {} zeroinitializer
}

define {} @test24() {
entry:
  %foo = alloca %9, align 8
  %bar = alloca %10, align 8
  %baz = alloca %11, align 8
  %qux = alloca %12, align 8
  %local1606_ = alloca %12*, align 8
  %local1605_ = alloca %12**, align 8
  %local1604_ = alloca %12***, align 8
  %local1603_ = alloca %12****, align 8
  %local1618_ = alloca %10*, align 8
  %0 = bitcast %9* %foo to { i32, i8* }*
  store { i32, i8* } zeroinitializer, { i32, i8* }* %0, align 8
  %1 = getelementptr inbounds %9, %9* %foo, i64 0, i32 0
  store i32 555, i32* %1, align 8
  %2 = bitcast %10* %bar to { i32, { i32, i8* } }*
  store { i32, { i32, i8* } } zeroinitializer, { i32, { i32, i8* } }* %2, align 8
  %3 = getelementptr inbounds %10, %10* %bar, i64 0, i32 0
  store i32 666, i32* %3, align 8
  %4 = bitcast %11* %baz to { i32, i8* }*
  store { i32, i8* } zeroinitializer, { i32, i8* }* %4, align 8
  %5 = getelementptr inbounds %11, %11* %baz, i64 0, i32 0
  store i32 777, i32* %5, align 8
  %6 = bitcast %12* %qux to { i32, i8* }*
  store { i32, i8* } zeroinitializer, { i32, i8* }* %6, align 8
  %7 = getelementptr inbounds %12, %12* %qux, i64 0, i32 0
  store i32 888, i32* %7, align 8
  %8 = getelementptr inbounds %12, %12* %qux, i64 0, i32 1
  %.cast = ptrtoint %9* %foo to i64
  %9 = bitcast %9** %8 to i64*
  store i64 %.cast, i64* %9, align 8
  store %12* %qux, %12** %local1606_, align 8
  store %12** %local1606_, %12*** %local1605_, align 8
  store %12*** %local1605_, %12**** %local1604_, align 8
  store %12**** %local1604_, %12***** %local1603_, align 8
  %10 = getelementptr inbounds %11, %11* %baz, i64 0, i32 1
  %.cast13 = ptrtoint %12***** %local1603_ to i64
  %11 = bitcast %12****** %10 to i64*
  store i64 %.cast13, i64* %11, align 8
  %12 = getelementptr inbounds %10, %10* %bar, i64 0, i32 1
  %13 = load %11, %11* %baz, align 8
  store %11 %13, %11* %12, align 8
  store %10* %bar, %10** %local1618_, align 8
  %14 = getelementptr inbounds %9, %9* %foo, i64 0, i32 1
  %.cast14 = ptrtoint %10** %local1618_ to i64
  %15 = bitcast %10*** %14 to i64*
  store i64 %.cast14, i64* %15, align 8
  %16 = getelementptr inbounds %10, %10* %bar, i64 0, i32 1, i32 1
  %17 = load %12*****, %12****** %16, align 8
  %18 = load %12****, %12***** %17, align 8
  %19 = load %12***, %12**** %18, align 8
  %20 = load %12**, %12*** %19, align 8
  %21 = load %12*, %12** %20, align 8
  %22 = getelementptr %12, %12* %21, i64 0, i32 0
  store i32 6, i32* %22, align 4
  %23 = load %10**, %10*** %14, align 8
  %24 = load %10*, %10** %23, align 8
  %25 = getelementptr %10, %10* %24, i64 0, i32 1, i32 1
  %26 = load %12*****, %12****** %25, align 8
  %27 = load %12****, %12***** %26, align 8
  %28 = load %12***, %12**** %27, align 8
  %29 = load %12**, %12*** %28, align 8
  %30 = load %12*, %12** %29, align 8
  %31 = getelementptr %12, %12* %30, i64 0, i32 0
  %32 = load i32, i32* %31, align 4
  %33 = icmp eq i32 %32, 6
  %34 = call {} @cpi_assert(i1 %33)
  %35 = load %12*****, %12****** %16, align 8
  %36 = load %12****, %12***** %35, align 8
  %37 = load %12***, %12**** %36, align 8
  %38 = load %12**, %12*** %37, align 8
  %39 = load %12*, %12** %38, align 8
  %40 = getelementptr %12, %12* %39, i64 0, i32 0
  %41 = load i32, i32* %40, align 4
  %42 = icmp eq i32 %41, 6
  %43 = call {} @cpi_assert(i1 %42)
  %44 = load %12****, %12***** %local1603_, align 8
  %45 = load %12***, %12**** %44, align 8
  %46 = load %12**, %12*** %45, align 8
  %47 = load %12*, %12** %46, align 8
  %48 = getelementptr %12, %12* %47, i64 0, i32 0
  %49 = load i32, i32* %48, align 4
  %50 = icmp eq i32 %49, 6
  %51 = call {} @cpi_assert(i1 %50)
  %52 = load i32, i32* %7, align 8
  %53 = icmp eq i32 %52, 6
  %54 = call {} @cpi_assert(i1 %53)
  %55 = load %12*****, %12****** %16, align 8
  %56 = load %12****, %12***** %55, align 8
  %57 = load %12***, %12**** %56, align 8
  %58 = load %12**, %12*** %57, align 8
  %59 = load %12*, %12** %58, align 8
  %60 = getelementptr %12, %12* %59, i64 0, i32 0
  store i32 8, i32* %60, align 4
  %61 = load %10**, %10*** %14, align 8
  %62 = load %10*, %10** %61, align 8
  %63 = getelementptr %10, %10* %62, i64 0, i32 1, i32 1
  %64 = load %12*****, %12****** %63, align 8
  %65 = load %12****, %12***** %64, align 8
  %66 = load %12***, %12**** %65, align 8
  %67 = load %12**, %12*** %66, align 8
  %68 = load %12*, %12** %67, align 8
  %69 = getelementptr %12, %12* %68, i64 0, i32 0
  %70 = load i32, i32* %69, align 4
  %71 = icmp eq i32 %70, 8
  %72 = call {} @cpi_assert(i1 %71)
  %73 = load %12*****, %12****** %16, align 8
  %74 = load %12****, %12***** %73, align 8
  %75 = load %12***, %12**** %74, align 8
  %76 = load %12**, %12*** %75, align 8
  %77 = load %12*, %12** %76, align 8
  %78 = getelementptr %12, %12* %77, i64 0, i32 0
  %79 = load i32, i32* %78, align 4
  %80 = icmp eq i32 %79, 8
  %81 = call {} @cpi_assert(i1 %80)
  %82 = load %12****, %12***** %local1603_, align 8
  %83 = load %12***, %12**** %82, align 8
  %84 = load %12**, %12*** %83, align 8
  %85 = load %12*, %12** %84, align 8
  %86 = getelementptr %12, %12* %85, i64 0, i32 0
  %87 = load i32, i32* %86, align 4
  %88 = icmp eq i32 %87, 8
  %89 = call {} @cpi_assert(i1 %88)
  %90 = load i32, i32* %7, align 8
  %91 = icmp eq i32 %90, 8
  %92 = call {} @cpi_assert(i1 %91)
  store i32 3, i32* %7, align 8
  %93 = load %10**, %10*** %14, align 8
  %94 = load %10*, %10** %93, align 8
  %95 = getelementptr %10, %10* %94, i64 0, i32 1, i32 1
  %96 = load %12*****, %12****** %95, align 8
  %97 = load %12****, %12***** %96, align 8
  %98 = load %12***, %12**** %97, align 8
  %99 = load %12**, %12*** %98, align 8
  %100 = load %12*, %12** %99, align 8
  %101 = getelementptr %12, %12* %100, i64 0, i32 0
  %102 = load i32, i32* %101, align 4
  %103 = icmp eq i32 %102, 3
  %104 = call {} @cpi_assert(i1 %103)
  %105 = load %12*****, %12****** %16, align 8
  %106 = load %12****, %12***** %105, align 8
  %107 = load %12***, %12**** %106, align 8
  %108 = load %12**, %12*** %107, align 8
  %109 = load %12*, %12** %108, align 8
  %110 = getelementptr %12, %12* %109, i64 0, i32 0
  %111 = load i32, i32* %110, align 4
  %112 = icmp eq i32 %111, 3
  %113 = call {} @cpi_assert(i1 %112)
  %114 = load %12*****, %12****** %10, align 8
  %115 = load %12****, %12***** %114, align 8
  %116 = load %12***, %12**** %115, align 8
  %117 = load %12**, %12*** %116, align 8
  %118 = load %12*, %12** %117, align 8
  %119 = getelementptr %12, %12* %118, i64 0, i32 0
  %120 = load i32, i32* %119, align 4
  %121 = icmp eq i32 %120, 3
  %122 = call {} @cpi_assert(i1 %121)
  %123 = load i32, i32* %7, align 8
  %124 = icmp eq i32 %123, 3
  %125 = call {} @cpi_assert(i1 %124)
  %126 = load %10**, %10*** %14, align 8
  %127 = load %10*, %10** %126, align 8
  %128 = getelementptr %10, %10* %127, i64 0, i32 1, i32 1
  %129 = load %12*****, %12****** %128, align 8
  %130 = load %12****, %12***** %129, align 8
  %131 = load %12***, %12**** %130, align 8
  %132 = load %12**, %12*** %131, align 8
  %133 = load %12*, %12** %132, align 8
  %134 = getelementptr %12, %12* %133, i64 0, i32 1
  %135 = load %9*, %9** %134, align 8
  %136 = getelementptr %9, %9* %135, i64 0, i32 1
  %137 = load %10**, %10*** %136, align 8
  %138 = load %10*, %10** %137, align 8
  %139 = getelementptr %10, %10* %138, i64 0, i32 1, i32 1
  %140 = load %12*****, %12****** %139, align 8
  %141 = load %12****, %12***** %140, align 8
  %142 = load %12***, %12**** %141, align 8
  %143 = load %12**, %12*** %142, align 8
  %144 = load %12*, %12** %143, align 8
  %145 = getelementptr %12, %12* %144, i64 0, i32 1
  %146 = load %9*, %9** %145, align 8
  %147 = getelementptr %9, %9* %146, i64 0, i32 1
  %148 = load %10**, %10*** %147, align 8
  %149 = load %10*, %10** %148, align 8
  %150 = getelementptr %10, %10* %149, i64 0, i32 1, i32 1
  %151 = load %12*****, %12****** %150, align 8
  %152 = load %12****, %12***** %151, align 8
  %153 = load %12***, %12**** %152, align 8
  %154 = load %12**, %12*** %153, align 8
  %155 = load %12*, %12** %154, align 8
  %156 = getelementptr %12, %12* %155, i64 0, i32 1
  %157 = load %9*, %9** %156, align 8
  %158 = getelementptr %9, %9* %157, i64 0, i32 1
  %159 = load %10**, %10*** %158, align 8
  %160 = load %10*, %10** %159, align 8
  %161 = getelementptr %10, %10* %160, i64 0, i32 1, i32 1
  %162 = load %12*****, %12****** %161, align 8
  %163 = load %12****, %12***** %162, align 8
  %164 = load %12***, %12**** %163, align 8
  %165 = load %12**, %12*** %164, align 8
  %166 = load %12*, %12** %165, align 8
  %167 = getelementptr %12, %12* %166, i64 0, i32 1
  %168 = load %9*, %9** %167, align 8
  %169 = getelementptr %9, %9* %168, i64 0, i32 1
  %170 = load %10**, %10*** %169, align 8
  %171 = load %10*, %10** %170, align 8
  %172 = getelementptr %10, %10* %171, i64 0, i32 1, i32 1
  %173 = load %12*****, %12****** %172, align 8
  %174 = load %12****, %12***** %173, align 8
  %175 = load %12***, %12**** %174, align 8
  %176 = load %12**, %12*** %175, align 8
  %177 = load %12*, %12** %176, align 8
  %178 = getelementptr %12, %12* %177, i64 0, i32 1
  %179 = load %9*, %9** %178, align 8
  %180 = getelementptr %9, %9* %179, i64 0, i32 1
  %181 = load %10**, %10*** %180, align 8
  %182 = load %10*, %10** %181, align 8
  %183 = getelementptr %10, %10* %182, i64 0, i32 1, i32 1
  %184 = load %12*****, %12****** %183, align 8
  %185 = load %12****, %12***** %184, align 8
  %186 = load %12***, %12**** %185, align 8
  %187 = load %12**, %12*** %186, align 8
  %188 = load %12*, %12** %187, align 8
  %189 = getelementptr %12, %12* %188, i64 0, i32 1
  %190 = load %9*, %9** %189, align 8
  %191 = getelementptr %9, %9* %190, i64 0, i32 1
  %192 = load %10**, %10*** %191, align 8
  %193 = load %10*, %10** %192, align 8
  %194 = getelementptr %10, %10* %193, i64 0, i32 1, i32 1
  %195 = load %12*****, %12****** %194, align 8
  %196 = load %12****, %12***** %195, align 8
  %197 = load %12***, %12**** %196, align 8
  %198 = load %12**, %12*** %197, align 8
  %199 = load %12*, %12** %198, align 8
  %200 = getelementptr %12, %12* %199, i64 0, i32 0
  store i32 39, i32* %200, align 4
  %201 = load %10**, %10*** %14, align 8
  %202 = load %10*, %10** %201, align 8
  %203 = getelementptr %10, %10* %202, i64 0, i32 1, i32 1
  %204 = load %12*****, %12****** %203, align 8
  %205 = load %12****, %12***** %204, align 8
  %206 = load %12***, %12**** %205, align 8
  %207 = load %12**, %12*** %206, align 8
  %208 = load %12*, %12** %207, align 8
  %209 = getelementptr %12, %12* %208, i64 0, i32 1
  %210 = load %9*, %9** %209, align 8
  %211 = getelementptr %9, %9* %210, i64 0, i32 1
  %212 = load %10**, %10*** %211, align 8
  %213 = load %10*, %10** %212, align 8
  %214 = getelementptr %10, %10* %213, i64 0, i32 1, i32 1
  %215 = load %12*****, %12****** %214, align 8
  %216 = load %12****, %12***** %215, align 8
  %217 = load %12***, %12**** %216, align 8
  %218 = load %12**, %12*** %217, align 8
  %219 = load %12*, %12** %218, align 8
  %220 = getelementptr %12, %12* %219, i64 0, i32 1
  %221 = load %9*, %9** %220, align 8
  %222 = getelementptr %9, %9* %221, i64 0, i32 1
  %223 = load %10**, %10*** %222, align 8
  %224 = load %10*, %10** %223, align 8
  %225 = getelementptr %10, %10* %224, i64 0, i32 1, i32 1
  %226 = load %12*****, %12****** %225, align 8
  %227 = load %12****, %12***** %226, align 8
  %228 = load %12***, %12**** %227, align 8
  %229 = load %12**, %12*** %228, align 8
  %230 = load %12*, %12** %229, align 8
  %231 = getelementptr %12, %12* %230, i64 0, i32 1
  %232 = load %9*, %9** %231, align 8
  %233 = getelementptr %9, %9* %232, i64 0, i32 1
  %234 = load %10**, %10*** %233, align 8
  %235 = load %10*, %10** %234, align 8
  %236 = getelementptr %10, %10* %235, i64 0, i32 1, i32 1
  %237 = load %12*****, %12****** %236, align 8
  %238 = load %12****, %12***** %237, align 8
  %239 = load %12***, %12**** %238, align 8
  %240 = load %12**, %12*** %239, align 8
  %241 = load %12*, %12** %240, align 8
  %242 = getelementptr %12, %12* %241, i64 0, i32 1
  %243 = load %9*, %9** %242, align 8
  %244 = getelementptr %9, %9* %243, i64 0, i32 1
  %245 = load %10**, %10*** %244, align 8
  %246 = load %10*, %10** %245, align 8
  %247 = getelementptr %10, %10* %246, i64 0, i32 1, i32 1
  %248 = load %12*****, %12****** %247, align 8
  %249 = load %12****, %12***** %248, align 8
  %250 = load %12***, %12**** %249, align 8
  %251 = load %12**, %12*** %250, align 8
  %252 = load %12*, %12** %251, align 8
  %253 = getelementptr %12, %12* %252, i64 0, i32 1
  %254 = load %9*, %9** %253, align 8
  %255 = getelementptr %9, %9* %254, i64 0, i32 1
  %256 = load %10**, %10*** %255, align 8
  %257 = load %10*, %10** %256, align 8
  %258 = getelementptr %10, %10* %257, i64 0, i32 1, i32 1
  %259 = load %12*****, %12****** %258, align 8
  %260 = load %12****, %12***** %259, align 8
  %261 = load %12***, %12**** %260, align 8
  %262 = load %12**, %12*** %261, align 8
  %263 = load %12*, %12** %262, align 8
  %264 = getelementptr %12, %12* %263, i64 0, i32 1
  %265 = load %9*, %9** %264, align 8
  %266 = getelementptr %9, %9* %265, i64 0, i32 1
  %267 = load %10**, %10*** %266, align 8
  %268 = load %10*, %10** %267, align 8
  %269 = getelementptr %10, %10* %268, i64 0, i32 1, i32 1
  %270 = load %12*****, %12****** %269, align 8
  %271 = load %12****, %12***** %270, align 8
  %272 = load %12***, %12**** %271, align 8
  %273 = load %12**, %12*** %272, align 8
  %274 = load %12*, %12** %273, align 8
  %275 = getelementptr %12, %12* %274, i64 0, i32 1
  %276 = load %9*, %9** %275, align 8
  %277 = getelementptr %9, %9* %276, i64 0, i32 1
  %278 = load %10**, %10*** %277, align 8
  %279 = load %10*, %10** %278, align 8
  %280 = getelementptr %10, %10* %279, i64 0, i32 1, i32 1
  %281 = load %12*****, %12****** %280, align 8
  %282 = load %12****, %12***** %281, align 8
  %283 = load %12***, %12**** %282, align 8
  %284 = load %12**, %12*** %283, align 8
  %285 = load %12*, %12** %284, align 8
  %286 = getelementptr %12, %12* %285, i64 0, i32 1
  %287 = load %9*, %9** %286, align 8
  %288 = getelementptr %9, %9* %287, i64 0, i32 1
  %289 = load %10**, %10*** %288, align 8
  %290 = load %10*, %10** %289, align 8
  %291 = getelementptr %10, %10* %290, i64 0, i32 1, i32 1
  %292 = load %12*****, %12****** %291, align 8
  %293 = load %12****, %12***** %292, align 8
  %294 = load %12***, %12**** %293, align 8
  %295 = load %12**, %12*** %294, align 8
  %296 = load %12*, %12** %295, align 8
  %297 = getelementptr %12, %12* %296, i64 0, i32 1
  %298 = load %9*, %9** %297, align 8
  %299 = getelementptr %9, %9* %298, i64 0, i32 1
  %300 = load %10**, %10*** %299, align 8
  %301 = load %10*, %10** %300, align 8
  %302 = getelementptr %10, %10* %301, i64 0, i32 1, i32 1
  %303 = load %12*****, %12****** %302, align 8
  %304 = load %12****, %12***** %303, align 8
  %305 = load %12***, %12**** %304, align 8
  %306 = load %12**, %12*** %305, align 8
  %307 = load %12*, %12** %306, align 8
  %308 = getelementptr %12, %12* %307, i64 0, i32 0
  %309 = load i32, i32* %308, align 4
  %310 = icmp eq i32 %309, 39
  %311 = call {} @cpi_assert(i1 %310)
  ret {} zeroinitializer
}

define {} @test25() {
entry:
  %foo = alloca %13, align 8
  %bar = alloca %14, align 8
  %baz = alloca %15, align 8
  %qux = alloca %16, align 8
  %local2675_ = alloca i64, align 8
  %local2674_ = alloca i64*, align 8
  %0 = bitcast %13* %foo to { i32, i8* }*
  store { i32, i8* } zeroinitializer, { i32, i8* }* %0, align 8
  %1 = bitcast %14* %bar to { i32, i8* }*
  store { i32, i8* } zeroinitializer, { i32, i8* }* %1, align 8
  %2 = bitcast %15* %baz to { i32, i8* }*
  store { i32, i8* } zeroinitializer, { i32, i8* }* %2, align 8
  %3 = bitcast %16* %qux to i8**
  store i8* null, i8** %3, align 8
  %4 = getelementptr inbounds %13, %13* %foo, i64 0, i32 0
  store i32 111, i32* %4, align 8
  %5 = getelementptr inbounds %14, %14* %bar, i64 0, i32 0
  store i32 222, i32* %5, align 8
  %6 = getelementptr inbounds %15, %15* %baz, i64 0, i32 0
  store i32 333, i32* %6, align 8
  store i64 444, i64* %local2675_, align 8
  store i64* %local2675_, i64** %local2674_, align 8
  %.cast = ptrtoint i64** %local2674_ to i64
  %7 = bitcast %16* %qux to i64*
  store i64 %.cast, i64* %7, align 8
  %8 = getelementptr inbounds %13, %13* %foo, i64 0, i32 1
  %.cast8 = ptrtoint %14* %bar to i64
  %9 = bitcast %14** %8 to i64*
  store i64 %.cast8, i64* %9, align 8
  %10 = getelementptr %14, %14* %bar, i64 0, i32 1
  %.cast9 = ptrtoint %15* %baz to i64
  %11 = bitcast %15** %10 to i64*
  store i64 %.cast9, i64* %11, align 8
  %12 = getelementptr %15, %15* %baz, i64 0, i32 1
  %.cast10 = ptrtoint %16* %qux to i64
  %13 = bitcast %16** %12 to i64*
  store i64 %.cast10, i64* %13, align 8
  %14 = bitcast %16** %12 to i64****
  %15 = inttoptr i64 %.cast10 to i64***
  %16 = load i64**, i64*** %15, align 8
  %17 = load i64*, i64** %16, align 8
  store i64 443, i64* %17, align 8
  %18 = load %15*, %15** %10, align 8
  %19 = getelementptr %15, %15* %18, i64 0, i32 1
  %20 = bitcast %16** %19 to i64****
  %21 = load i64***, i64**** %20, align 8
  %22 = load i64**, i64*** %21, align 8
  %23 = load i64*, i64** %22, align 8
  %24 = load i64, i64* %23, align 8
  %25 = icmp eq i64 %24, 443
  %26 = call {} @cpi_assert(i1 %25)
  %27 = load %15*, %15** %10, align 8
  %28 = getelementptr %15, %15* %27, i64 0, i32 1
  %29 = bitcast %16** %28 to i64****
  %30 = load i64***, i64**** %29, align 8
  %31 = load i64**, i64*** %30, align 8
  %32 = load i64*, i64** %31, align 8
  %33 = load i64, i64* %32, align 8
  %34 = icmp eq i64 %33, 443
  %35 = call {} @cpi_assert(i1 %34)
  %36 = load i64***, i64**** %14, align 8
  %37 = load i64**, i64*** %36, align 8
  %38 = load i64*, i64** %37, align 8
  %39 = load i64, i64* %38, align 8
  %40 = icmp eq i64 %39, 443
  %41 = call {} @cpi_assert(i1 %40)
  %42 = getelementptr inbounds %16, %16* %qux, i64 0, i32 0
  %43 = load i64**, i64*** %42, align 8
  %44 = load i64*, i64** %43, align 8
  %45 = load i64, i64* %44, align 8
  %46 = icmp eq i64 %45, 443
  %47 = call {} @cpi_assert(i1 %46)
  %48 = load %15*, %15** %10, align 8
  %49 = getelementptr %15, %15* %48, i64 0, i32 1
  %50 = bitcast %16** %49 to i64****
  %51 = load i64***, i64**** %50, align 8
  %52 = load i64**, i64*** %51, align 8
  %53 = load i64*, i64** %52, align 8
  store i64 442, i64* %53, align 8
  %54 = load %15*, %15** %10, align 8
  %55 = getelementptr %15, %15* %54, i64 0, i32 1
  %56 = bitcast %16** %55 to i64****
  %57 = load i64***, i64**** %56, align 8
  %58 = load i64**, i64*** %57, align 8
  %59 = load i64*, i64** %58, align 8
  %60 = load i64, i64* %59, align 8
  %61 = icmp eq i64 %60, 442
  %62 = call {} @cpi_assert(i1 %61)
  %63 = load %15*, %15** %10, align 8
  %64 = getelementptr %15, %15* %63, i64 0, i32 1
  %65 = bitcast %16** %64 to i64****
  %66 = load i64***, i64**** %65, align 8
  %67 = load i64**, i64*** %66, align 8
  %68 = load i64*, i64** %67, align 8
  %69 = load i64, i64* %68, align 8
  %70 = icmp eq i64 %69, 442
  %71 = call {} @cpi_assert(i1 %70)
  %72 = load i64***, i64**** %14, align 8
  %73 = load i64**, i64*** %72, align 8
  %74 = load i64*, i64** %73, align 8
  %75 = load i64, i64* %74, align 8
  %76 = icmp eq i64 %75, 442
  %77 = call {} @cpi_assert(i1 %76)
  %78 = load i64**, i64*** %42, align 8
  %79 = load i64*, i64** %78, align 8
  %80 = load i64, i64* %79, align 8
  %81 = icmp eq i64 %80, 442
  %82 = call {} @cpi_assert(i1 %81)
  ret {} zeroinitializer
}

define i64 @testDoubleDeref() {
entry:
  %p = alloca %2, align 8
  %q1 = alloca %2, align 8
  %q2 = alloca %2, align 8
  %.repack = getelementptr inbounds %2, %2* %p, i64 0, i32 0
  store i64 0, i64* %.repack, align 8
  %0 = getelementptr inbounds %2, %2* %p, i64 0, i32 1
  store i64 3, i64* %0, align 8
  %q1.repack = getelementptr inbounds %2, %2* %q1, i64 0, i32 0
  store i64 0, i64* %q1.repack, align 8
  %q1.repack4 = getelementptr inbounds %2, %2* %q1, i64 0, i32 1
  store i64 3, i64* %q1.repack4, align 8
  %q2.repack = getelementptr inbounds %2, %2* %q2, i64 0, i32 0
  store i64 0, i64* %q2.repack, align 8
  %q2.repack8 = getelementptr inbounds %2, %2* %q2, i64 0, i32 1
  store i64 3, i64* %q2.repack8, align 8
  ret i64 3
}

define i64 @more_struct() {
entry:
  %s = alloca %17, align 8
  %local2218_ = alloca %18*, align 8
  %local2217_ = alloca %18**, align 8
  %psx = alloca %18***, align 8
  %new_sx = alloca %19, align 8
  %.repack.repack = getelementptr inbounds %17, %17* %s, i64 0, i32 0, i32 0
  store i64 0, i64* %.repack.repack, align 8
  %0 = getelementptr inbounds %17, %17* %s, i64 0, i32 0, i32 1
  store i64 0, i64* %0, align 8
  %1 = getelementptr inbounds %17, %17* %s, i64 0, i32 1
  store i64 0, i64* %1, align 8
  %2 = getelementptr inbounds %17, %17* %s, i64 0, i32 0
  store %18* %2, %18** %local2218_, align 8
  store %18** %local2218_, %18*** %local2217_, align 8
  store %18*** %local2217_, %18**** %psx, align 8
  %3 = call i64 @bad()
  %4 = load %18**, %18*** %local2217_, align 8
  %5 = load %18*, %18** %4, align 8
  %6 = getelementptr %18, %18* %5, i64 0, i32 1
  store i64 %3, i64* %6, align 8
  %.repack = getelementptr inbounds %19, %19* %new_sx, i64 0, i32 0
  store i64 0, i64* %.repack, align 8
  %7 = getelementptr inbounds %19, %19* %new_sx, i64 0, i32 1
  store i64 3, i64* %7, align 8
  %8 = bitcast %18**** %psx to %19****
  %9 = bitcast %18*** %local2217_ to %19***
  %10 = load %19**, %19*** %9, align 8
  %11 = load %19*, %19** %10, align 8
  %.repack6 = getelementptr inbounds %19, %19* %11, i64 0, i32 0
  store i64 0, i64* %.repack6, align 8
  %.repack8 = getelementptr inbounds %19, %19* %11, i64 0, i32 1
  store i64 3, i64* %.repack8, align 8
  %12 = load i64, i64* %0, align 8
  ret i64 %12
}

define i64 @test_S2_stuff() {
entry:
  %0 = call i64 @bad()
  ret i64 3
}

define i64 @testSetFromParam() {
entry:
  %a = alloca i64, align 8
  store i64 999, i64* %a, align 8
  %0 = call i64 @set(i64* nonnull %a)
  %1 = load i64, i64* %a, align 8
  ret i64 %1
}

define i64 @set(i64*) {
entry:
  store i64 3, i64* %0, align 8
  ret i64 999
}

define i32 @fib(i32) {
entry:
  %switch = icmp ult i32 %0, 2
  br i1 %switch, label %then, label %else2

then:                                             ; preds = %entry
  ret i32 1

else2:                                            ; preds = %entry
  %1 = add i32 %0, -1
  %2 = call i32 @fib(i32 %1)
  %3 = add i32 %0, -2
  %4 = call i32 @fib(i32 %3)
  %5 = add i32 %4, %2
  ret i32 %5
}

define i64 @stupid_sqrt(i64) {
entry:
  switch i64 %0, label %else10 [
    i64 1, label %then
    i64 4, label %then1
    i64 9, label %then5
    i64 16, label %then9
  ]

then:                                             ; preds = %entry, %else10, %then9, %then5, %then1
  %merge = phi i64 [ 1, %entry ], [ 2, %then1 ], [ 3, %then5 ], [ 4, %then9 ], [ 0, %else10 ]
  ret i64 %merge

then1:                                            ; preds = %entry
  br label %then

then5:                                            ; preds = %entry
  br label %then

then9:                                            ; preds = %entry
  br label %then

else10:                                           ; preds = %entry
  br label %then
}

define i32 @id(i32) {
entry:
  ret i32 %0
}

define %0 @id.6(%0) {
entry:
  %.elt = extractvalue %0 %0, 0
  %.elt2 = extractvalue %0 %0, 1
  ret %0 %0
}

define i64 @deref_ct(i64*) {
entry:
  %1 = load i64, i64* %0, align 8
  ret i64 %1
}

define i32 @add_static(i32) {
entry:
  %1 = add i32 %0, 7
  ret i32 %1
}

define i32 @apply_static(i32) {
entry:
  %1 = call i32 @poly_inc(i32 %0)
  ret i32 %1
}

define i32 @poly_inc(i32) {
entry:
  %1 = add i32 %0, 1
  ret i32 %1
}

define i32 @apply_static.7(i32) {
entry:
  %1 = call i32 @anon.8(i32 %0)
  ret i32 %1
}

define i32 @anon.8(i32) {
entry:
  %1 = add i32 %0, 3
  ret i32 %1
}

define %0 @id.9(%0) {
entry:
  %.elt = extractvalue %0 %0, 0
  %.elt2 = extractvalue %0 %0, 1
  ret %0 %0
}

define {} @testArray1() {
entry:
  %local976_ = alloca %20, align 8
  %a = alloca { i64*, i64 }, align 8
  %.repack = getelementptr inbounds %20, %20* %local976_, i64 0, i32 0
  store i64 1, i64* %.repack, align 8
  %0 = getelementptr inbounds %20, %20* %local976_, i64 0, i32 1
  store i64 2, i64* %0, align 8
  %1 = getelementptr inbounds %20, %20* %local976_, i64 0, i32 2
  store i64 3, i64* %1, align 8
  %2 = getelementptr inbounds %20, %20* %local976_, i64 0, i32 3
  store i64 4, i64* %2, align 8
  %3 = getelementptr inbounds %20, %20* %local976_, i64 0, i32 4
  store i64 5, i64* %3, align 8
  store i64 1, i64* %.repack, align 8
  store i64 2, i64* %0, align 8
  store i64 3, i64* %1, align 8
  store i64 4, i64* %2, align 8
  store i64 5, i64* %3, align 8
  %4 = insertvalue { %20*, i32 } zeroinitializer, %20* %local976_, 0
  %5 = insertvalue { %20*, i32 } %4, i32 5, 1
  %6 = bitcast { i64*, i64 }* %a to { %20*, i32 }*
  store { %20*, i32 } %5, { %20*, i32 }* %6, align 8
  %.elt = getelementptr inbounds { i64*, i64 }, { i64*, i64 }* %a, i64 0, i32 0
  %.unpack = load i64*, i64** %.elt, align 8
  %parith = getelementptr i64, i64* %.unpack, i64 2
  %7 = load i64, i64* %parith, align 8
  %8 = icmp eq i64 %7, 3
  %9 = call {} @cpi_assert(i1 %8)
  %10 = load i64, i64* %parith, align 8
  %11 = icmp eq i64 %10, 3
  %12 = call {} @cpi_assert(i1 %11)
  %13 = load i64, i64* %parith, align 8
  %14 = icmp eq i64 %13, 3
  %15 = call {} @cpi_assert(i1 %14)
  store i64 19, i64* %parith, align 8
  %16 = call {} @cpi_assert(i1 true)
  store i64 23, i64* %parith, align 8
  %17 = call {} @cpi_assert(i1 true)
  ret {} zeroinitializer
}

define {} @testArray2() {
entry:
  %local1076_ = alloca %21, align 8
  %point_array = alloca { %2*, i64 }, align 8
  %local7125_ = alloca %25, align 8
  %local7129_ = alloca %25*, align 8
  %.repack.repack = getelementptr inbounds %21, %21* %local1076_, i64 0, i32 0, i32 0
  store i64 1, i64* %.repack.repack, align 8
  %0 = getelementptr inbounds %21, %21* %local1076_, i64 0, i32 0, i32 1
  store i64 1, i64* %0, align 8
  %1 = getelementptr inbounds %21, %21* %local1076_, i64 0, i32 1, i32 0
  store i64 2, i64* %1, align 8
  %2 = getelementptr inbounds %21, %21* %local1076_, i64 0, i32 1, i32 1
  store i64 3, i64* %2, align 8
  %3 = getelementptr inbounds %21, %21* %local1076_, i64 0, i32 2, i32 0
  store i64 4, i64* %3, align 8
  %4 = getelementptr inbounds %21, %21* %local1076_, i64 0, i32 2, i32 1
  store i64 5, i64* %4, align 8
  store i64 1, i64* %.repack.repack, align 8
  store i64 1, i64* %0, align 8
  store i64 2, i64* %1, align 8
  store i64 3, i64* %2, align 8
  store i64 4, i64* %3, align 8
  store i64 5, i64* %4, align 8
  %5 = insertvalue { %21*, i32 } zeroinitializer, %21* %local1076_, 0
  %6 = insertvalue { %21*, i32 } %5, i32 3, 1
  %7 = bitcast { %2*, i64 }* %point_array to { %21*, i32 }*
  store { %21*, i32 } %6, { %21*, i32 }* %7, align 8
  %.elt = getelementptr inbounds { %2*, i64 }, { %2*, i64 }* %point_array, i64 0, i32 0
  %.unpack = load %2*, %2** %.elt, align 8
  %.elt18 = getelementptr inbounds %2, %2* %.unpack, i64 1, i32 1
  %.unpack19 = load i64, i64* %.elt18, align 8
  %8 = icmp eq i64 %.unpack19, 3
  %9 = call {} @cpi_assert(i1 %8)
  %.repack = getelementptr inbounds %25, %25* %local7125_, i64 0, i32 0
  store i32 1, i32* %.repack, align 8
  %10 = getelementptr inbounds %25, %25* %local7125_, i64 0, i32 1
  store i32 2, i32* %10, align 4
  %11 = getelementptr inbounds %25, %25* %local7125_, i64 0, i32 2
  store i32 3, i32* %11, align 8
  store i32 1, i32* %.repack, align 8
  store i32 2, i32* %10, align 4
  store i32 3, i32* %11, align 8
  %12 = call i8* @malloc(i64 12)
  %13 = bitcast %25** %local7129_ to i8**
  store i8* %12, i8** %13, align 8
  %.repack39 = bitcast i8* %12 to i32*
  store i32 1, i32* %.repack39, align 4
  %.repack41 = getelementptr inbounds i8, i8* %12, i64 4
  %14 = bitcast i8* %.repack41 to i32*
  store i32 2, i32* %14, align 4
  %.repack43 = getelementptr inbounds i8, i8* %12, i64 8
  %15 = bitcast i8* %.repack43 to i32*
  store i32 3, i32* %15, align 4
  %16 = bitcast %25** %local7129_ to i32**
  %parith1 = getelementptr i32, i32* %.repack39, i64 1
  %17 = call {} @cpi_assert(i1 true)
  ret {} zeroinitializer
}

define {} @testArray3() {
entry:
  %local1166_ = alloca %26, align 8
  %arr = alloca { i32*, i64 }, align 8
  %copied = alloca { i32*, i64 }, align 8
  %.repack = getelementptr inbounds %26, %26* %local1166_, i64 0, i32 0
  store i32 1, i32* %.repack, align 8
  %0 = getelementptr inbounds %26, %26* %local1166_, i64 0, i32 1
  store i32 2, i32* %0, align 4
  %1 = getelementptr inbounds %26, %26* %local1166_, i64 0, i32 2
  store i32 3, i32* %1, align 8
  store i32 1, i32* %.repack, align 8
  store i32 2, i32* %0, align 4
  store i32 3, i32* %1, align 8
  %2 = insertvalue { %26*, i32 } zeroinitializer, %26* %local1166_, 0
  %3 = insertvalue { %26*, i32 } %2, i32 3, 1
  %4 = bitcast { i32*, i64 }* %arr to { %26*, i32 }*
  store { %26*, i32 } %3, { %26*, i32 }* %4, align 8
  %5 = bitcast { i32*, i64 }* %arr to i64*
  %.unpack23 = load i64, i64* %5, align 8
  %.elt18 = getelementptr inbounds { i32*, i64 }, { i32*, i64 }* %arr, i64 0, i32 1
  %.unpack19 = load i64, i64* %.elt18, align 8
  %6 = bitcast { i32*, i64 }* %copied to i64*
  store i64 %.unpack23, i64* %6, align 8
  %copied.repack21 = getelementptr inbounds { i32*, i64 }, { i32*, i64 }* %copied, i64 0, i32 1
  store i64 %.unpack19, i64* %copied.repack21, align 8
  %.elt = getelementptr inbounds { i32*, i64 }, { i32*, i64 }* %arr, i64 0, i32 0
  %7 = inttoptr i64 %.unpack23 to i32*
  %parith = getelementptr i32, i32* %7, i64 1
  %8 = load i32, i32* %parith, align 4
  %9 = icmp eq i32 %8, 2
  %10 = call {} @cpi_assert(i1 %9)
  %.elt26 = getelementptr inbounds { i32*, i64 }, { i32*, i64 }* %copied, i64 0, i32 0
  %11 = load i32, i32* %parith, align 4
  %12 = icmp eq i32 %11, 2
  %13 = call {} @cpi_assert(i1 %12)
  store i32 18, i32* %parith, align 4
  %14 = call {} @cpi_assert(i1 true)
  %.unpack39 = load i32*, i32** %.elt26, align 8
  %parith6 = getelementptr i32, i32* %.unpack39, i64 1
  %15 = load i32, i32* %parith6, align 4
  %16 = icmp eq i32 %15, 18
  %17 = call {} @cpi_assert(i1 %16)
  store i32 99, i32* %parith6, align 4
  %.unpack47 = load i32*, i32** %.elt, align 8
  %parith9 = getelementptr i32, i32* %.unpack47, i64 1
  %18 = load i32, i32* %parith9, align 4
  %19 = icmp eq i32 %18, 99
  %20 = call {} @cpi_assert(i1 %19)
  %21 = load i32, i32* %parith6, align 4
  %22 = icmp eq i32 %21, 99
  %23 = call {} @cpi_assert(i1 %22)
  ret {} zeroinitializer
}

define {} @testArray4() {
entry:
  %local1262_ = alloca i64, align 8
  %local1261_ = alloca i64*, align 8
  %local1266_ = alloca i64, align 8
  %local1265_ = alloca i64*, align 8
  %p = alloca %27, align 8
  %py = alloca i64**, align 8
  %local1281_ = alloca i64, align 8
  %local1280_ = alloca i64*, align 8
  %local1285_ = alloca i64, align 8
  %local1284_ = alloca i64*, align 8
  %local1291_ = alloca i64, align 8
  %local1290_ = alloca i64*, align 8
  %local1295_ = alloca i64, align 8
  %local1294_ = alloca i64*, align 8
  %local7284_ = alloca %28, align 8
  %local7288_ = alloca %28*, align 8
  store i64 0, i64* %local1262_, align 8
  store i64* %local1262_, i64** %local1261_, align 8
  store i64 0, i64* %local1266_, align 8
  store i64* %local1266_, i64** %local1265_, align 8
  %.repack = getelementptr inbounds %27, %27* %p, i64 0, i32 0
  store i64** %local1261_, i64*** %.repack, align 8
  %0 = getelementptr inbounds %27, %27* %p, i64 0, i32 1
  store i64** %local1265_, i64*** %0, align 8
  %.cast = ptrtoint i64** %local1265_ to i64
  %1 = bitcast i64*** %py to i64*
  store i64 %.cast, i64* %1, align 8
  store i64 0, i64* %local1281_, align 8
  store i64* %local1281_, i64** %local1280_, align 8
  store i64 0, i64* %local1285_, align 8
  store i64* %local1285_, i64** %local1284_, align 8
  %2 = bitcast %27* %p to i64*
  %3 = ptrtoint i64** %local1261_ to i64
  %4 = bitcast i64*** %0 to i64*
  store i64 0, i64* %local1291_, align 8
  store i64* %local1291_, i64** %local1290_, align 8
  store i64 0, i64* %local1295_, align 8
  store i64* %local1295_, i64** %local1294_, align 8
  %.repack5.repack = getelementptr inbounds %28, %28* %local7284_, i64 0, i32 0, i32 0
  store i64** %local1280_, i64*** %.repack5.repack, align 8
  %5 = getelementptr inbounds %28, %28* %local7284_, i64 0, i32 0, i32 1
  store i64** %local1284_, i64*** %5, align 8
  %6 = getelementptr inbounds %28, %28* %local7284_, i64 0, i32 1, i32 0
  %7 = bitcast i64*** %6 to i64*
  store i64 %3, i64* %7, align 8
  %8 = getelementptr inbounds %28, %28* %local7284_, i64 0, i32 1, i32 1
  %9 = bitcast i64*** %8 to i64*
  store i64 %.cast, i64* %9, align 8
  %10 = getelementptr inbounds %28, %28* %local7284_, i64 0, i32 2, i32 0
  store i64** %local1290_, i64*** %10, align 8
  %11 = getelementptr inbounds %28, %28* %local7284_, i64 0, i32 2, i32 1
  store i64** %local1294_, i64*** %11, align 8
  %12 = bitcast %28* %local7284_ to i64*
  %13 = ptrtoint i64** %local1280_ to i64
  %14 = bitcast i64*** %5 to i64*
  %15 = ptrtoint i64** %local1284_ to i64
  %16 = bitcast i64*** %10 to i64*
  %17 = ptrtoint i64** %local1290_ to i64
  %18 = bitcast i64*** %11 to i64*
  %19 = ptrtoint i64** %local1294_ to i64
  store i64 %13, i64* %12, align 8
  store i64 %15, i64* %14, align 8
  store i64 %3, i64* %7, align 8
  store i64 %.cast, i64* %9, align 8
  store i64 %17, i64* %16, align 8
  store i64 %19, i64* %18, align 8
  %20 = call i8* @malloc(i64 48)
  %21 = bitcast %28** %local7288_ to i8**
  store i8* %20, i8** %21, align 8
  %22 = bitcast i8* %20 to i64*
  store i64 %13, i64* %22, align 8
  %.repack61.repack70 = getelementptr inbounds i8, i8* %20, i64 8
  %23 = bitcast i8* %.repack61.repack70 to i64*
  store i64 %15, i64* %23, align 8
  %.repack62.repack = getelementptr inbounds i8, i8* %20, i64 16
  %24 = bitcast i8* %.repack62.repack to i64*
  store i64 %3, i64* %24, align 8
  %.repack62.repack68 = getelementptr inbounds i8, i8* %20, i64 24
  %25 = bitcast i8* %.repack62.repack68 to i64*
  store i64 %.cast, i64* %25, align 8
  %.repack64.repack = getelementptr inbounds i8, i8* %20, i64 32
  %26 = bitcast i8* %.repack64.repack to i64*
  store i64 %17, i64* %26, align 8
  %.repack64.repack66 = getelementptr inbounds i8, i8* %20, i64 40
  %27 = bitcast i8* %.repack64.repack66 to i64*
  store i64 %19, i64* %27, align 8
  %28 = bitcast %28** %local7288_ to %27**
  %29 = bitcast i8* %20 to %27*
  %.elt82 = getelementptr inbounds %27, %27* %29, i64 1, i32 1
  %30 = load i64*, i64** %local1265_, align 8
  store i64 18, i64* %30, align 8
  %31 = load i64*, i64** %local1265_, align 8
  %32 = load i64, i64* %31, align 8
  %33 = icmp eq i64 %32, 18
  %34 = call {} @cpi_assert(i1 %33)
  ret {} zeroinitializer
}

define {} @testArray5() {
entry:
  %local1345_ = alloca i64, align 8
  %local1342_ = alloca %29, align 8
  %a1 = alloca { { i64*, i64 }*, i64 }, align 8
  %local7353_ = alloca %31, align 8
  %local7357_ = alloca %31*, align 8
  %local7389_ = alloca %32, align 8
  %local7393_ = alloca %32*, align 8
  %local7414_ = alloca %33, align 8
  %local7418_ = alloca %33*, align 8
  %local7439_ = alloca %34, align 8
  %local7443_ = alloca %34*, align 8
  %local7461_ = alloca %35, align 8
  %local7465_ = alloca %35*, align 8
  store i64 3, i64* %local1345_, align 8
  %0 = insertvalue { i64*, i32 } zeroinitializer, i64* %local1345_, 0
  %1 = insertvalue { i64*, i32 } %0, i32 1, 1
  %2 = bitcast %29* %local1342_ to { i64*, i32 }*
  store { i64*, i32 } %1, { i64*, i32 }* %2, align 8
  %3 = insertvalue { %29*, i32 } zeroinitializer, %29* %local1342_, 0
  %4 = insertvalue { %29*, i32 } %3, i32 1, 1
  %5 = bitcast { { i64*, i64 }*, i64 }* %a1 to { %29*, i32 }*
  store { %29*, i32 } %4, { %29*, i32 }* %5, align 8
  %6 = bitcast { { i64*, i64 }*, i64 }* %a1 to i64***
  %.unpack9101416 = load i64**, i64*** %6, align 8
  %.unpack1315 = load i64*, i64** %.unpack9101416, align 8
  %7 = load i64, i64* %.unpack1315, align 8
  %8 = icmp eq i64 %7, 3
  %9 = call {} @cpi_assert(i1 %8)
  %.repack = getelementptr inbounds %31, %31* %local7353_, i64 0, i32 0
  store i64 1, i64* %.repack, align 8
  %10 = getelementptr inbounds %31, %31* %local7353_, i64 0, i32 1
  store i64 2, i64* %10, align 8
  %11 = getelementptr inbounds %31, %31* %local7353_, i64 0, i32 2
  store i64 3, i64* %11, align 8
  store i64 1, i64* %.repack, align 8
  store i64 2, i64* %10, align 8
  store i64 3, i64* %11, align 8
  %12 = call i8* @malloc(i64 24)
  %13 = bitcast %31** %local7357_ to i8**
  store i8* %12, i8** %13, align 8
  %.repack31 = bitcast i8* %12 to i64*
  store i64 1, i64* %.repack31, align 8
  %.repack33 = getelementptr inbounds i8, i8* %12, i64 8
  %14 = bitcast i8* %.repack33 to i64*
  store i64 2, i64* %14, align 8
  %.repack35 = getelementptr inbounds i8, i8* %12, i64 16
  %15 = bitcast i8* %.repack35 to i64*
  store i64 3, i64* %15, align 8
  %16 = bitcast %31** %local7357_ to i64**
  %parith2 = getelementptr i64, i64* %.repack31, i64 2
  %17 = call {} @cpi_assert(i1 true)
  %.repack37 = getelementptr inbounds %32, %32* %local7389_, i64 0, i32 0
  store i64 1, i64* %.repack37, align 8
  %18 = getelementptr inbounds %32, %32* %local7389_, i64 0, i32 1
  store i64 2, i64* %18, align 8
  %19 = getelementptr inbounds %32, %32* %local7389_, i64 0, i32 2
  store i64 3, i64* %19, align 8
  store i64 1, i64* %.repack37, align 8
  store i64 2, i64* %18, align 8
  store i64 3, i64* %19, align 8
  %20 = call i8* @malloc(i64 24)
  %21 = bitcast %32** %local7393_ to i8**
  store i8* %20, i8** %21, align 8
  %.repack57 = bitcast i8* %20 to i64*
  store i64 1, i64* %.repack57, align 8
  %.repack59 = getelementptr inbounds i8, i8* %20, i64 8
  %22 = bitcast i8* %.repack59 to i64*
  store i64 2, i64* %22, align 8
  %.repack61 = getelementptr inbounds i8, i8* %20, i64 16
  %23 = bitcast i8* %.repack61 to i64*
  store i64 3, i64* %23, align 8
  %24 = bitcast %32** %local7393_ to i64*
  %25 = ptrtoint i8* %20 to i64
  %.repack63 = getelementptr inbounds %33, %33* %local7414_, i64 0, i32 0
  store i64 4, i64* %.repack63, align 8
  %26 = getelementptr inbounds %33, %33* %local7414_, i64 0, i32 1
  store i64 5, i64* %26, align 8
  %27 = getelementptr inbounds %33, %33* %local7414_, i64 0, i32 2
  store i64 6, i64* %27, align 8
  store i64 4, i64* %.repack63, align 8
  store i64 5, i64* %26, align 8
  store i64 6, i64* %27, align 8
  %28 = call i8* @malloc(i64 24)
  %29 = bitcast %33** %local7418_ to i8**
  store i8* %28, i8** %29, align 8
  %.repack83 = bitcast i8* %28 to i64*
  store i64 4, i64* %.repack83, align 8
  %.repack85 = getelementptr inbounds i8, i8* %28, i64 8
  %30 = bitcast i8* %.repack85 to i64*
  store i64 5, i64* %30, align 8
  %.repack87 = getelementptr inbounds i8, i8* %28, i64 16
  %31 = bitcast i8* %.repack87 to i64*
  store i64 6, i64* %31, align 8
  %32 = bitcast %33** %local7418_ to i64*
  %33 = ptrtoint i8* %28 to i64
  %.repack89 = getelementptr inbounds %34, %34* %local7439_, i64 0, i32 0
  store i64 7, i64* %.repack89, align 8
  %34 = getelementptr inbounds %34, %34* %local7439_, i64 0, i32 1
  store i64 8, i64* %34, align 8
  %35 = getelementptr inbounds %34, %34* %local7439_, i64 0, i32 2
  store i64 9, i64* %35, align 8
  store i64 7, i64* %.repack89, align 8
  store i64 8, i64* %34, align 8
  store i64 9, i64* %35, align 8
  %36 = call i8* @malloc(i64 24)
  %37 = bitcast %34** %local7443_ to i8**
  store i8* %36, i8** %37, align 8
  %.repack109 = bitcast i8* %36 to i64*
  store i64 7, i64* %.repack109, align 8
  %.repack111 = getelementptr inbounds i8, i8* %36, i64 8
  %38 = bitcast i8* %.repack111 to i64*
  store i64 8, i64* %38, align 8
  %.repack113 = getelementptr inbounds i8, i8* %36, i64 16
  %39 = bitcast i8* %.repack113 to i64*
  store i64 9, i64* %39, align 8
  %40 = bitcast %34** %local7443_ to i64*
  %41 = ptrtoint i8* %36 to i64
  %42 = bitcast %35* %local7461_ to i64*
  store i64 %25, i64* %42, align 8
  %43 = getelementptr inbounds %35, %35* %local7461_, i64 0, i32 0, i32 1
  store i64 3, i64* %43, align 8
  %44 = getelementptr inbounds %35, %35* %local7461_, i64 0, i32 1, i32 0
  %45 = bitcast i64** %44 to i64*
  store i64 %33, i64* %45, align 8
  %46 = getelementptr inbounds %35, %35* %local7461_, i64 0, i32 1, i32 1
  store i64 3, i64* %46, align 8
  %47 = getelementptr inbounds %35, %35* %local7461_, i64 0, i32 2, i32 0
  %48 = bitcast i64** %47 to i64*
  store i64 %41, i64* %48, align 8
  %49 = getelementptr inbounds %35, %35* %local7461_, i64 0, i32 2, i32 1
  store i64 3, i64* %49, align 8
  store i64 %25, i64* %42, align 8
  store i64 3, i64* %43, align 8
  store i64 %33, i64* %45, align 8
  store i64 3, i64* %46, align 8
  store i64 %41, i64* %48, align 8
  store i64 3, i64* %49, align 8
  %50 = call i8* @malloc(i64 48)
  %51 = bitcast %35** %local7465_ to i8**
  store i8* %50, i8** %51, align 8
  %52 = bitcast i8* %50 to i64*
  store i64 %25, i64* %52, align 8
  %.repack171.repack181 = getelementptr inbounds i8, i8* %50, i64 8
  %53 = bitcast i8* %.repack171.repack181 to i64*
  store i64 3, i64* %53, align 8
  %.repack173.repack = getelementptr inbounds i8, i8* %50, i64 16
  %54 = bitcast i8* %.repack173.repack to i64*
  store i64 %33, i64* %54, align 8
  %.repack173.repack179 = getelementptr inbounds i8, i8* %50, i64 24
  %55 = bitcast i8* %.repack173.repack179 to i64*
  store i64 3, i64* %55, align 8
  %.repack175.repack = getelementptr inbounds i8, i8* %50, i64 32
  %56 = bitcast i8* %.repack175.repack to i64*
  store i64 %41, i64* %56, align 8
  %.repack175.repack177 = getelementptr inbounds i8, i8* %50, i64 40
  %57 = bitcast i8* %.repack175.repack177 to i64*
  store i64 3, i64* %57, align 8
  %58 = bitcast %35** %local7465_ to { i64*, i64 }**
  %59 = bitcast i8* %50 to { i64*, i64 }*
  %.elt193 = getelementptr inbounds { i64*, i64 }, { i64*, i64 }* %59, i64 1, i32 0
  %60 = inttoptr i64 %33 to i64*
  %parith5 = getelementptr i64, i64* %60, i64 2
  %61 = load i64, i64* %parith5, align 8
  %62 = icmp eq i64 %61, 6
  %63 = call {} @cpi_assert(i1 %62)
  ret {} zeroinitializer
}

define {} @testWhile1() {
entry:
  br label %cond

cond:                                             ; preds = %then, %entry
  %0 = phi i64 [ %2, %then ], [ 3, %entry ]
  %1 = icmp eq i64 %0, 10
  br i1 %1, label %if_cont, label %then

then:                                             ; preds = %cond
  %2 = add i64 %0, 1
  br label %cond

if_cont:                                          ; preds = %cond
  %3 = call {} @cpi_assert(i1 true)
  ret {} zeroinitializer
}

define {} @testMalloc() {
entry:
  %b = alloca %0*, align 8
  %0 = call %0* @alloc_pair()
  %.cast = ptrtoint %0* %0 to i64
  %1 = bitcast %0** %b to i64*
  store i64 %.cast, i64* %1, align 8
  %2 = getelementptr %0, %0* %0, i64 0, i32 0
  store i64 18, i64* %2, align 8
  %3 = getelementptr %0, %0* %0, i64 0, i32 1
  store i64 199, i64* %3, align 8
  %4 = call %0* @alloc_pair()
  %5 = getelementptr %0, %0* %4, i64 0, i32 0
  store i64 19, i64* %5, align 8
  %6 = getelementptr %0, %0* %4, i64 0, i32 1
  store i64 200, i64* %6, align 8
  %7 = load i64, i64* %3, align 8
  %8 = icmp eq i64 %7, 199
  %9 = call {} @cpi_assert(i1 %8)
  ret {} zeroinitializer
}

define %0* @alloc_pair() {
entry:
  %0 = call i8* @malloc(i64 16)
  %.cast = bitcast i8* %0 to %0*
  ret %0* %.cast
}

define {} @testFor1() {
entry:
  %local7580_ = alloca %36, align 8
  %local7585_ = alloca %36*, align 8
  %foreach_index1 = alloca { i64*, i64 }, align 8
  %.repack = getelementptr inbounds %36, %36* %local7580_, i64 0, i32 0
  store i64 2, i64* %.repack, align 8
  %0 = getelementptr inbounds %36, %36* %local7580_, i64 0, i32 1
  store i64 3, i64* %0, align 8
  %1 = getelementptr inbounds %36, %36* %local7580_, i64 0, i32 2
  store i64 4, i64* %1, align 8
  %2 = getelementptr inbounds %36, %36* %local7580_, i64 0, i32 3
  store i64 5, i64* %2, align 8
  store i64 2, i64* %.repack, align 8
  store i64 3, i64* %0, align 8
  store i64 4, i64* %1, align 8
  store i64 5, i64* %2, align 8
  %3 = call i8* @malloc(i64 32)
  %4 = bitcast %36** %local7585_ to i8**
  store i8* %3, i8** %4, align 8
  %.repack26 = bitcast i8* %3 to i64*
  store i64 2, i64* %.repack26, align 8
  %.repack28 = getelementptr inbounds i8, i8* %3, i64 8
  %5 = bitcast i8* %.repack28 to i64*
  store i64 3, i64* %5, align 8
  %.repack30 = getelementptr inbounds i8, i8* %3, i64 16
  %6 = bitcast i8* %.repack30 to i64*
  store i64 4, i64* %6, align 8
  %.repack32 = getelementptr inbounds i8, i8* %3, i64 24
  %7 = bitcast i8* %.repack32 to i64*
  store i64 5, i64* %7, align 8
  %8 = bitcast %36** %local7585_ to i64*
  %9 = ptrtoint i8* %3 to i64
  %10 = bitcast { i64*, i64 }* %foreach_index1 to i64*
  store i64 %9, i64* %10, align 8
  %foreach_index1.repack35 = getelementptr inbounds { i64*, i64 }, { i64*, i64 }* %foreach_index1, i64 0, i32 1
  store i64 4, i64* %foreach_index1.repack35, align 8
  %11 = inttoptr i64 %9 to i64*
  br label %cond

cond:                                             ; preds = %then, %entry
  %12 = phi i64 [ %16, %then ], [ 1, %entry ]
  %13 = phi i64 [ %17, %then ], [ 0, %entry ]
  %14 = icmp slt i64 %13, 4
  br i1 %14, label %then, label %if_cont

then:                                             ; preds = %cond
  %.elt37 = getelementptr inbounds { i64*, i64 }, { i64*, i64 }* %foreach_index1, i64 0, i32 0
  %parith = getelementptr i64, i64* %11, i64 %13
  %15 = load i64, i64* %parith, align 8
  %16 = mul i64 %12, %15
  %17 = add i64 %13, 1
  br label %cond

if_cont:                                          ; preds = %cond
  %18 = icmp eq i64 %12, 120
  %19 = call {} @cpi_assert(i1 %18)
  ret {} zeroinitializer
}

define {} @testFor2() {
entry:
  %local7679_ = alloca %37, align 8
  %local7711_ = alloca %38, align 8
  %local7717_ = alloca %38*, align 8
  %.repack = getelementptr inbounds %37, %37* %local7679_, i64 0, i32 0
  store i32 1, i32* %.repack, align 8
  %0 = getelementptr inbounds %37, %37* %local7679_, i64 0, i32 1
  store i32 2, i32* %0, align 4
  %1 = getelementptr inbounds %37, %37* %local7679_, i64 0, i32 2
  store i32 3, i32* %1, align 8
  %2 = getelementptr inbounds %37, %37* %local7679_, i64 0, i32 3
  store i32 4, i32* %2, align 4
  %3 = getelementptr inbounds %37, %37* %local7679_, i64 0, i32 4
  store i32 5, i32* %3, align 8
  store i32 1, i32* %.repack, align 8
  store i32 2, i32* %0, align 4
  store i32 3, i32* %1, align 8
  store i32 4, i32* %2, align 4
  store i32 5, i32* %3, align 8
  %.repack43 = getelementptr inbounds %38, %38* %local7711_, i64 0, i32 0
  store i32 1, i32* %.repack43, align 8
  %4 = getelementptr inbounds %38, %38* %local7711_, i64 0, i32 1
  store i32 2, i32* %4, align 4
  %5 = getelementptr inbounds %38, %38* %local7711_, i64 0, i32 2
  store i32 3, i32* %5, align 8
  %6 = getelementptr inbounds %38, %38* %local7711_, i64 0, i32 3
  store i32 4, i32* %6, align 4
  %7 = getelementptr inbounds %38, %38* %local7711_, i64 0, i32 4
  store i32 5, i32* %7, align 8
  store i32 1, i32* %.repack43, align 8
  store i32 2, i32* %4, align 4
  store i32 3, i32* %5, align 8
  store i32 4, i32* %6, align 4
  store i32 5, i32* %7, align 8
  %8 = call i8* @malloc(i64 20)
  %9 = bitcast %38** %local7717_ to i8**
  store i8* %8, i8** %9, align 8
  %.repack77 = bitcast i8* %8 to i32*
  store i32 1, i32* %.repack77, align 4
  %.repack79 = getelementptr inbounds i8, i8* %8, i64 4
  %10 = bitcast i8* %.repack79 to i32*
  store i32 2, i32* %10, align 4
  %.repack81 = getelementptr inbounds i8, i8* %8, i64 8
  %11 = bitcast i8* %.repack81 to i32*
  store i32 3, i32* %11, align 4
  %.repack83 = getelementptr inbounds i8, i8* %8, i64 12
  %12 = bitcast i8* %.repack83 to i32*
  store i32 4, i32* %12, align 4
  %.repack85 = getelementptr inbounds i8, i8* %8, i64 16
  %13 = bitcast i8* %.repack85 to i32*
  store i32 5, i32* %13, align 4
  %14 = bitcast %38** %local7717_ to i32**
  %15 = insertvalue { i32*, i64 } undef, i32* %.repack77, 0
  %16 = insertvalue { i32*, i64 } %15, i64 5, 1
  %17 = call i32 @add_array({ i32*, i64 } %16)
  %18 = icmp eq i32 %17, 15
  %19 = call {} @cpi_assert(i1 %18)
  ret {} zeroinitializer
}

define i32 @add_array({ i32*, i64 }) {
entry:
  %foreach_index1 = alloca { i32*, i64 }, align 8
  %foreach_index1.repack = getelementptr inbounds { i32*, i64 }, { i32*, i64 }* %foreach_index1, i64 0, i32 0
  %.elt = extractvalue { i32*, i64 } %0, 0
  store i32* %.elt, i32** %foreach_index1.repack, align 8
  %foreach_index1.repack2 = getelementptr inbounds { i32*, i64 }, { i32*, i64 }* %foreach_index1, i64 0, i32 1
  %.elt3 = extractvalue { i32*, i64 } %0, 1
  store i64 %.elt3, i64* %foreach_index1.repack2, align 8
  br label %cond

cond:                                             ; preds = %then, %entry
  %1 = phi i32 [ %5, %then ], [ 0, %entry ]
  %2 = phi i64 [ %6, %then ], [ 0, %entry ]
  %3 = icmp slt i64 %2, %.elt3
  br i1 %3, label %then, label %if_cont

then:                                             ; preds = %cond
  %parith = getelementptr i32, i32* %.elt, i64 %2
  %4 = load i32, i32* %parith, align 4
  %5 = add i32 %1, %4
  %6 = add i64 %2, 1
  br label %cond

if_cont:                                          ; preds = %cond
  ret i32 %1
}

define {} @testFor3() {
entry:
  %local7746_ = alloca %39, align 8
  %local7750_ = alloca %39*, align 8
  %foreach_index1 = alloca { i64*, i64 }, align 8
  %local7778_ = alloca %40, align 8
  %local7782_ = alloca %40*, align 8
  %foreach_index4 = alloca { i64*, i64 }, align 8
  %.repack = getelementptr inbounds %39, %39* %local7746_, i64 0, i32 0
  store i64 1, i64* %.repack, align 8
  %0 = getelementptr inbounds %39, %39* %local7746_, i64 0, i32 1
  store i64 2, i64* %0, align 8
  %1 = getelementptr inbounds %39, %39* %local7746_, i64 0, i32 2
  store i64 3, i64* %1, align 8
  store i64 1, i64* %.repack, align 8
  store i64 2, i64* %0, align 8
  store i64 3, i64* %1, align 8
  %2 = call i8* @malloc(i64 24)
  %3 = bitcast %39** %local7750_ to i8**
  store i8* %2, i8** %3, align 8
  %.repack23 = bitcast i8* %2 to i64*
  store i64 1, i64* %.repack23, align 8
  %.repack25 = getelementptr inbounds i8, i8* %2, i64 8
  %4 = bitcast i8* %.repack25 to i64*
  store i64 2, i64* %4, align 8
  %.repack27 = getelementptr inbounds i8, i8* %2, i64 16
  %5 = bitcast i8* %.repack27 to i64*
  store i64 3, i64* %5, align 8
  %6 = bitcast %39** %local7750_ to i64*
  %7 = ptrtoint i8* %2 to i64
  %8 = bitcast { i64*, i64 }* %foreach_index1 to i64*
  store i64 %7, i64* %8, align 8
  %foreach_index1.repack30 = getelementptr inbounds { i64*, i64 }, { i64*, i64 }* %foreach_index1, i64 0, i32 1
  store i64 3, i64* %foreach_index1.repack30, align 8
  %9 = inttoptr i64 %7 to i64*
  br label %cond

cond:                                             ; preds = %if_cont7, %entry
  %10 = phi i64 [ %32, %if_cont7 ], [ 0, %entry ]
  %11 = phi i64 [ %33, %if_cont7 ], [ 0, %entry ]
  %12 = icmp slt i64 %11, 3
  br i1 %12, label %then, label %if_cont

then:                                             ; preds = %cond
  %.elt32 = getelementptr inbounds { i64*, i64 }, { i64*, i64 }* %foreach_index1, i64 0, i32 0
  %parith = getelementptr i64, i64* %9, i64 %11
  %13 = load i64, i64* %parith, align 8
  %.repack36 = getelementptr inbounds %40, %40* %local7778_, i64 0, i32 0
  store i64 4, i64* %.repack36, align 8
  %14 = getelementptr inbounds %40, %40* %local7778_, i64 0, i32 1
  store i64 5, i64* %14, align 8
  %15 = getelementptr inbounds %40, %40* %local7778_, i64 0, i32 2
  store i64 6, i64* %15, align 8
  store i64 4, i64* %.repack36, align 8
  store i64 5, i64* %14, align 8
  store i64 6, i64* %15, align 8
  %16 = call i8* @malloc(i64 24)
  %17 = bitcast %40** %local7782_ to i8**
  store i8* %16, i8** %17, align 8
  %.repack56 = bitcast i8* %16 to i64*
  store i64 4, i64* %.repack56, align 8
  %.repack58 = getelementptr inbounds i8, i8* %16, i64 8
  %18 = bitcast i8* %.repack58 to i64*
  store i64 5, i64* %18, align 8
  %.repack60 = getelementptr inbounds i8, i8* %16, i64 16
  %19 = bitcast i8* %.repack60 to i64*
  store i64 6, i64* %19, align 8
  %20 = bitcast %40** %local7782_ to i64*
  %21 = ptrtoint i8* %16 to i64
  %22 = bitcast { i64*, i64 }* %foreach_index4 to i64*
  store i64 %21, i64* %22, align 8
  %foreach_index4.repack63 = getelementptr inbounds { i64*, i64 }, { i64*, i64 }* %foreach_index4, i64 0, i32 1
  store i64 3, i64* %foreach_index4.repack63, align 8
  %23 = inttoptr i64 %21 to i64*
  br label %cond5

if_cont:                                          ; preds = %cond
  %24 = icmp eq i64 %10, 51
  %25 = call {} @cpi_assert(i1 %24)
  ret {} zeroinitializer

cond5:                                            ; preds = %then6, %then
  %26 = phi i64 [ %30, %then6 ], [ %10, %then ]
  %27 = phi i64 [ %31, %then6 ], [ 0, %then ]
  %28 = icmp slt i64 %27, 3
  br i1 %28, label %then6, label %if_cont7

then6:                                            ; preds = %cond5
  %.elt65 = getelementptr inbounds { i64*, i64 }, { i64*, i64 }* %foreach_index4, i64 0, i32 0
  %parith8 = getelementptr i64, i64* %23, i64 %27
  %29 = load i64, i64* %parith8, align 8
  %30 = add i64 %26, %29
  %31 = add i64 %27, 1
  br label %cond5

if_cont7:                                         ; preds = %cond5
  %32 = add i64 %13, %26
  %33 = add i64 %11, 1
  br label %cond
}

define {} @testFor4() {
entry:
  %local7867_ = alloca %41, align 8
  %local7873_ = alloca %41*, align 8
  %foreach_index = alloca { i64*, i64 }, align 8
  %.repack = getelementptr inbounds %41, %41* %local7867_, i64 0, i32 0
  store i64 1, i64* %.repack, align 8
  %0 = getelementptr inbounds %41, %41* %local7867_, i64 0, i32 1
  store i64 1, i64* %0, align 8
  %1 = getelementptr inbounds %41, %41* %local7867_, i64 0, i32 2
  store i64 1, i64* %1, align 8
  %2 = getelementptr inbounds %41, %41* %local7867_, i64 0, i32 3
  store i64 1999, i64* %2, align 8
  %3 = getelementptr inbounds %41, %41* %local7867_, i64 0, i32 4
  store i64 1, i64* %3, align 8
  store i64 1, i64* %.repack, align 8
  store i64 1, i64* %0, align 8
  store i64 1, i64* %1, align 8
  store i64 1999, i64* %2, align 8
  store i64 1, i64* %3, align 8
  %4 = call i8* @malloc(i64 40)
  %5 = bitcast %41** %local7873_ to i8**
  store i8* %4, i8** %5, align 8
  %.repack32 = bitcast i8* %4 to i64*
  store i64 1, i64* %.repack32, align 8
  %.repack34 = getelementptr inbounds i8, i8* %4, i64 8
  %6 = bitcast i8* %.repack34 to i64*
  store i64 1, i64* %6, align 8
  %.repack36 = getelementptr inbounds i8, i8* %4, i64 16
  %7 = bitcast i8* %.repack36 to i64*
  store i64 1, i64* %7, align 8
  %.repack38 = getelementptr inbounds i8, i8* %4, i64 24
  %8 = bitcast i8* %.repack38 to i64*
  store i64 1999, i64* %8, align 8
  %.repack40 = getelementptr inbounds i8, i8* %4, i64 32
  %9 = bitcast i8* %.repack40 to i64*
  store i64 1, i64* %9, align 8
  %10 = bitcast %41** %local7873_ to i64*
  %11 = ptrtoint i8* %4 to i64
  %12 = bitcast { i64*, i64 }* %foreach_index to i64*
  store i64 %11, i64* %12, align 8
  %foreach_index.repack43 = getelementptr inbounds { i64*, i64 }, { i64*, i64 }* %foreach_index, i64 0, i32 1
  store i64 5, i64* %foreach_index.repack43, align 8
  br label %cond

cond:                                             ; preds = %then, %entry
  %13 = phi i64 [ %16, %then ], [ 0, %entry ]
  %14 = phi i64 [ %17, %then ], [ 0, %entry ]
  %15 = icmp slt i64 %14, 5
  br i1 %15, label %then, label %if_cont

then:                                             ; preds = %cond
  %16 = add i64 %14, %13
  %17 = add i64 %14, 1
  br label %cond

if_cont:                                          ; preds = %cond
  %18 = icmp eq i64 %13, 10
  %19 = call {} @cpi_assert(i1 %18)
  ret {} zeroinitializer
}

define {} @testUnion1() {
entry:
  %x = alloca %42, align 8
  %y = alloca i64, align 8
  %tmpcast = bitcast i64* %y to %43*
  %a = alloca { i64, [8 x i8] }, align 8
  %0 = getelementptr inbounds %42, %42* %x, i64 0, i32 0
  store i64 18, i64* %0, align 8
  %.repack = bitcast i64* %y to i32*
  store i32 0, i32* %.repack, align 8
  %1 = getelementptr inbounds %43, %43* %tmpcast, i64 0, i32 1
  store i32 0, i32* %1, align 4
  %2 = bitcast { i64, [8 x i8] }* %a to { i64, { i64 }, { i32, i32 } }*
  %.repack9 = getelementptr inbounds { i64, [8 x i8] }, { i64, [8 x i8] }* %a, i64 0, i32 0
  store i64 0, i64* %.repack9, align 8
  %3 = getelementptr inbounds { i64, [8 x i8] }, { i64, [8 x i8] }* %a, i64 0, i32 1
  %4 = bitcast [8 x i8]* %3 to i64*
  store i64 0, i64* %4, align 8
  %.repack11.repack = getelementptr inbounds { i64, [8 x i8] }, { i64, [8 x i8] }* %a, i64 1
  %5 = bitcast { i64, [8 x i8] }* %.repack11.repack to i32*
  store i32 0, i32* %5, align 8
  %.repack11.repack12 = getelementptr inbounds { i64, { i64 }, { i32, i32 } }, { i64, { i64 }, { i32, i32 } }* %2, i64 0, i32 2, i32 1
  store i32 0, i32* %.repack11.repack12, align 4
  store i64 18, i64* %4, align 8
  store i64 1, i64* %.repack9, align 8
  %6 = call {} @cpi_assert(i1 true)
  %7 = call {} @cpi_assert(i1 true)
  %8 = load i64, i64* %y, align 8
  store i64 %8, i64* %4, align 8
  store i64 2, i64* %.repack9, align 8
  %9 = call {} @cpi_assert(i1 true)
  store i64 13, i64* %4, align 8
  store i64 1, i64* %.repack9, align 8
  %10 = call {} @cpi_assert(i1 true)
  %11 = getelementptr inbounds { i64, [8 x i8] }, { i64, [8 x i8] }* %a, i64 0, i32 1, i64 4
  %12 = bitcast i8* %11 to i32*
  store i32 18, i32* %12, align 4
  store i64 2, i64* %.repack9, align 8
  %13 = call {} @cpi_assert(i1 true)
  ret {} zeroinitializer
}

define {} @testBuffer() {
entry:
  %local3275_ = alloca %44, align 8
  %b = alloca %44, align 8
  %f = alloca i64, align 8
  %tmpcast27 = bitcast i64* %f to %45*
  %0 = call %44 @buffer(i64 0)
  %local3275_.repack = getelementptr inbounds %44, %44* %local3275_, i64 0, i32 0
  %.elt = extractvalue %44 %0, 0
  store i64 %.elt, i64* %local3275_.repack, align 8
  %.elt11 = extractvalue %44 %0, 1
  %local3275_.repack10.repack = getelementptr inbounds %44, %44* %local3275_, i64 0, i32 1, i32 0
  %.elt11.elt = extractvalue { %45*, i64 } %.elt11, 0
  store %45* %.elt11.elt, %45** %local3275_.repack10.repack, align 8
  %local3275_.repack10.repack12 = getelementptr inbounds %44, %44* %local3275_, i64 0, i32 1, i32 1
  %.elt11.elt13 = extractvalue { %45*, i64 } %.elt11, 1
  store i64 %.elt11.elt13, i64* %local3275_.repack10.repack12, align 8
  %1 = bitcast %45** %local3275_.repack10.repack to i64*
  %2 = ptrtoint %45* %.elt11.elt to i64
  %b.repack = getelementptr inbounds %44, %44* %b, i64 0, i32 0
  store i64 %.elt, i64* %b.repack, align 8
  %b.repack21.repack = getelementptr inbounds %44, %44* %b, i64 0, i32 1, i32 0
  %3 = bitcast %45** %b.repack21.repack to i64*
  store i64 %2, i64* %3, align 8
  %b.repack21.repack23 = getelementptr inbounds %44, %44* %b, i64 0, i32 1, i32 1
  store i64 %.elt11.elt13, i64* %b.repack21.repack23, align 8
  %.repack = bitcast i64* %f to i32*
  store i32 0, i32* %.repack, align 8
  %4 = getelementptr inbounds %45, %45* %tmpcast27, i64 0, i32 1
  store i32 0, i32* %4, align 4
  store i32 3, i32* %.repack, align 8
  store i32 4, i32* %4, align 4
  %realParam3 = alloca i64, align 8
  %tmpcast = bitcast i64* %realParam3 to %45*
  %5 = load i64, i64* %f, align 8
  store i64 %5, i64* %realParam3, align 8
  %.elt28 = bitcast i64* %realParam3 to i32*
  %6 = trunc i64 %5 to i32
  %7 = insertvalue %45 undef, i32 %6, 0
  %.elt30 = getelementptr inbounds %45, %45* %tmpcast, i64 0, i32 1
  %8 = lshr i64 %5, 32
  %9 = trunc i64 %8 to i32
  %10 = insertvalue %45 %7, i32 %9, 1
  %11 = call i64 @buffer_set(%44* nonnull %b, i64 200, %45 %10)
  %12 = call %45 @buffer_get(%44* nonnull %b, i64 200)
  %.elt32 = extractvalue %45 %12, 0
  %13 = icmp eq i32 %.elt32, 3
  %14 = call {} @cpi_assert(i1 %13)
  %15 = call %45 @buffer_get.11(%44* nonnull %b, i64 200)
  %.elt37 = extractvalue %45 %15, 1
  %16 = icmp eq i32 %.elt37, 4
  %17 = call {} @cpi_assert(i1 %16)
  ret {} zeroinitializer
}

define %44 @buffer(i64) {
entry:
  %buf = alloca { %45*, i64 }, align 8
  %buf.repack = getelementptr inbounds { %45*, i64 }, { %45*, i64 }* %buf, i64 0, i32 0
  store %45* null, %45** %buf.repack, align 8
  %1 = getelementptr inbounds { %45*, i64 }, { %45*, i64 }* %buf, i64 0, i32 1
  store i64 0, i64* %1, align 8
  %2 = shl i64 %0, 3
  %3 = call i8* @malloc(i64 %2)
  %.cast = ptrtoint i8* %3 to i64
  %4 = bitcast { %45*, i64 }* %buf to i64*
  store i64 %.cast, i64* %4, align 8
  %realRet = alloca %44, align 8
  %.repack = getelementptr inbounds %44, %44* %realRet, i64 0, i32 0
  store i64 %0, i64* %.repack, align 8
  %5 = getelementptr inbounds %44, %44* %realRet, i64 0, i32 1, i32 0
  %6 = bitcast %45** %5 to i8**
  store i8* %3, i8** %6, align 8
  %7 = getelementptr inbounds %44, %44* %realRet, i64 0, i32 1, i32 1
  store i64 0, i64* %7, align 8
  %8 = insertvalue %44 undef, i64 %0, 0
  %9 = bitcast i8* %3 to %45*
  %10 = insertvalue { %45*, i64 } undef, %45* %9, 0
  %.unpack912 = insertvalue { %45*, i64 } %10, i64 0, 1
  %11 = insertvalue %44 %8, { %45*, i64 } %.unpack912, 1
  ret %44 %11
}

define i64 @buffer_set(%44*, i64, %45) {
entry:
  %3 = call {} @ensureCapacity(%44* %0, i64 %1)
  %.unpack6.elt = getelementptr inbounds %44, %44* %0, i64 0, i32 1, i32 0
  %.unpack6.unpack = load %45*, %45** %.unpack6.elt, align 8
  %parith.repack = getelementptr inbounds %45, %45* %.unpack6.unpack, i64 %1, i32 0
  %.elt = extractvalue %45 %2, 0
  store i32 %.elt, i32* %parith.repack, align 4
  %parith.repack10 = getelementptr inbounds %45, %45* %.unpack6.unpack, i64 %1, i32 1
  %.elt11 = extractvalue %45 %2, 1
  store i32 %.elt11, i32* %parith.repack10, align 4
  %.elt12 = getelementptr inbounds %44, %44* %0, i64 0, i32 0
  %.unpack = load i64, i64* %.elt12, align 8
  ret i64 %.unpack
}

define {} @ensureCapacity(%44*, i64) {
entry:
  %newBuf = alloca { %45*, i64 }, align 8
  %.elt = getelementptr %44, %44* %0, i64 0, i32 0
  %.unpack = load i64, i64* %.elt, align 8
  %2 = icmp eq i64 %.unpack, 0
  br i1 %2, label %then, label %if_cont

then:                                             ; preds = %entry
  store i64 1, i64* %.elt, align 8
  br label %if_cont

if_cont:                                          ; preds = %entry, %then
  %.unpack2385 = phi i64 [ 1, %then ], [ %.unpack, %entry ]
  br label %cond

cond:                                             ; preds = %if_cont15, %if_cont
  %.unpack30 = phi i64 [ %.unpack23.pre, %if_cont15 ], [ %.unpack2385, %if_cont ]
  %3 = icmp sgt i64 %.unpack30, %1
  br i1 %3, label %if_cont12, label %then11

then11:                                           ; preds = %cond
  %4 = shl i64 %.unpack30, 1
  store i64 %4, i64* %.elt, align 8
  %newBuf.repack = getelementptr inbounds { %45*, i64 }, { %45*, i64 }* %newBuf, i64 0, i32 0
  store %45* null, %45** %newBuf.repack, align 8
  %newBuf.repack36 = getelementptr inbounds { %45*, i64 }, { %45*, i64 }* %newBuf, i64 0, i32 1
  store i64 0, i64* %newBuf.repack36, align 8
  %.unpack40.elt41 = getelementptr %44, %44* %0, i64 0, i32 1, i32 1
  %.unpack40.unpack42 = load i64, i64* %.unpack40.elt41, align 8
  store i64 %.unpack40.unpack42, i64* %newBuf.repack36, align 8
  %5 = shl i64 %4, 3
  %6 = call i8* @malloc(i64 %5)
  %.cast = ptrtoint i8* %6 to i64
  %7 = bitcast { %45*, i64 }* %newBuf to i64*
  store i64 %.cast, i64* %7, align 8
  %8 = inttoptr i64 %.cast to %45*
  br label %cond13

if_cont12:                                        ; preds = %cond
  ret {} zeroinitializer

cond13:                                           ; preds = %then14, %then11
  %9 = phi i64 [ %13, %then14 ], [ 0, %then11 ]
  %.unpack57.unpack59 = load i64, i64* %.unpack40.elt41, align 8
  %10 = icmp slt i64 %9, %.unpack57.unpack59
  br i1 %10, label %then14, label %if_cont15

then14:                                           ; preds = %cond13
  %.unpack75.elt = getelementptr inbounds %44, %44* %0, i64 0, i32 1, i32 0
  %.unpack75.unpack = load %45*, %45** %.unpack75.elt, align 8
  %parith = getelementptr %45, %45* %.unpack75.unpack, i64 %9
  %11 = bitcast %45* %parith to i64*
  %12 = load i64, i64* %11, align 8
  %parith16 = getelementptr %45, %45* %8, i64 %9
  %.cast83 = bitcast %45* %parith16 to i64*
  store i64 %12, i64* %.cast83, align 8
  %13 = add i64 %9, 1
  br label %cond13

if_cont15:                                        ; preds = %cond13
  %14 = getelementptr %44, %44* %0, i64 0, i32 1
  %15 = bitcast { %45*, i64 }* %14 to i64*
  store i64 %.cast, i64* %15, align 8
  store i64 %.unpack40.unpack42, i64* %.unpack40.elt41, align 8
  %.unpack23.pre = load i64, i64* %.elt, align 8
  br label %cond
}

define %45 @buffer_get(%44*, i64) {
entry:
  %2 = call {} @ensureCapacity.10(%44* %0, i64 %1)
  %.unpack3.elt = getelementptr inbounds %44, %44* %0, i64 0, i32 1, i32 0
  %.unpack3.unpack = load %45*, %45** %.unpack3.elt, align 8
  %parith = getelementptr %45, %45* %.unpack3.unpack, i64 %1
  %3 = bitcast %45* %parith to i64*
  %4 = load i64, i64* %3, align 8
  %realRet = alloca i64, align 8
  %tmpcast = bitcast i64* %realRet to %45*
  store i64 %4, i64* %realRet, align 8
  %.elt = bitcast i64* %realRet to i32*
  %5 = trunc i64 %4 to i32
  %6 = insertvalue %45 undef, i32 %5, 0
  %.elt7 = getelementptr inbounds %45, %45* %tmpcast, i64 0, i32 1
  %7 = lshr i64 %4, 32
  %8 = trunc i64 %7 to i32
  %9 = insertvalue %45 %6, i32 %8, 1
  ret %45 %9
}

define {} @ensureCapacity.10(%44*, i64) {
entry:
  %newBuf = alloca { %45*, i64 }, align 8
  %.elt = getelementptr %44, %44* %0, i64 0, i32 0
  %.unpack = load i64, i64* %.elt, align 8
  %2 = icmp eq i64 %.unpack, 0
  br i1 %2, label %then, label %if_cont

then:                                             ; preds = %entry
  store i64 1, i64* %.elt, align 8
  br label %if_cont

if_cont:                                          ; preds = %entry, %then
  %.unpack2385 = phi i64 [ 1, %then ], [ %.unpack, %entry ]
  br label %cond

cond:                                             ; preds = %if_cont15, %if_cont
  %.unpack30 = phi i64 [ %.unpack23.pre, %if_cont15 ], [ %.unpack2385, %if_cont ]
  %3 = icmp sgt i64 %.unpack30, %1
  br i1 %3, label %if_cont12, label %then11

then11:                                           ; preds = %cond
  %4 = shl i64 %.unpack30, 1
  store i64 %4, i64* %.elt, align 8
  %newBuf.repack = getelementptr inbounds { %45*, i64 }, { %45*, i64 }* %newBuf, i64 0, i32 0
  store %45* null, %45** %newBuf.repack, align 8
  %newBuf.repack36 = getelementptr inbounds { %45*, i64 }, { %45*, i64 }* %newBuf, i64 0, i32 1
  store i64 0, i64* %newBuf.repack36, align 8
  %.unpack40.elt41 = getelementptr %44, %44* %0, i64 0, i32 1, i32 1
  %.unpack40.unpack42 = load i64, i64* %.unpack40.elt41, align 8
  store i64 %.unpack40.unpack42, i64* %newBuf.repack36, align 8
  %5 = shl i64 %4, 3
  %6 = call i8* @malloc(i64 %5)
  %.cast = ptrtoint i8* %6 to i64
  %7 = bitcast { %45*, i64 }* %newBuf to i64*
  store i64 %.cast, i64* %7, align 8
  %8 = inttoptr i64 %.cast to %45*
  br label %cond13

if_cont12:                                        ; preds = %cond
  ret {} zeroinitializer

cond13:                                           ; preds = %then14, %then11
  %9 = phi i64 [ %13, %then14 ], [ 0, %then11 ]
  %.unpack57.unpack59 = load i64, i64* %.unpack40.elt41, align 8
  %10 = icmp slt i64 %9, %.unpack57.unpack59
  br i1 %10, label %then14, label %if_cont15

then14:                                           ; preds = %cond13
  %.unpack75.elt = getelementptr inbounds %44, %44* %0, i64 0, i32 1, i32 0
  %.unpack75.unpack = load %45*, %45** %.unpack75.elt, align 8
  %parith = getelementptr %45, %45* %.unpack75.unpack, i64 %9
  %11 = bitcast %45* %parith to i64*
  %12 = load i64, i64* %11, align 8
  %parith16 = getelementptr %45, %45* %8, i64 %9
  %.cast83 = bitcast %45* %parith16 to i64*
  store i64 %12, i64* %.cast83, align 8
  %13 = add i64 %9, 1
  br label %cond13

if_cont15:                                        ; preds = %cond13
  %14 = getelementptr %44, %44* %0, i64 0, i32 1
  %15 = bitcast { %45*, i64 }* %14 to i64*
  store i64 %.cast, i64* %15, align 8
  store i64 %.unpack40.unpack42, i64* %.unpack40.elt41, align 8
  %.unpack23.pre = load i64, i64* %.elt, align 8
  br label %cond
}

define %45 @buffer_get.11(%44*, i64) {
entry:
  %2 = call {} @ensureCapacity.12(%44* %0, i64 %1)
  %.unpack3.elt = getelementptr inbounds %44, %44* %0, i64 0, i32 1, i32 0
  %.unpack3.unpack = load %45*, %45** %.unpack3.elt, align 8
  %parith = getelementptr %45, %45* %.unpack3.unpack, i64 %1
  %3 = bitcast %45* %parith to i64*
  %4 = load i64, i64* %3, align 8
  %realRet = alloca i64, align 8
  %tmpcast = bitcast i64* %realRet to %45*
  store i64 %4, i64* %realRet, align 8
  %.elt = bitcast i64* %realRet to i32*
  %5 = trunc i64 %4 to i32
  %6 = insertvalue %45 undef, i32 %5, 0
  %.elt7 = getelementptr inbounds %45, %45* %tmpcast, i64 0, i32 1
  %7 = lshr i64 %4, 32
  %8 = trunc i64 %7 to i32
  %9 = insertvalue %45 %6, i32 %8, 1
  ret %45 %9
}

define {} @ensureCapacity.12(%44*, i64) {
entry:
  %newBuf = alloca { %45*, i64 }, align 8
  %.elt = getelementptr %44, %44* %0, i64 0, i32 0
  %.unpack = load i64, i64* %.elt, align 8
  %2 = icmp eq i64 %.unpack, 0
  br i1 %2, label %then, label %if_cont

then:                                             ; preds = %entry
  store i64 1, i64* %.elt, align 8
  br label %if_cont

if_cont:                                          ; preds = %entry, %then
  %.unpack2385 = phi i64 [ 1, %then ], [ %.unpack, %entry ]
  br label %cond

cond:                                             ; preds = %if_cont15, %if_cont
  %.unpack30 = phi i64 [ %.unpack23.pre, %if_cont15 ], [ %.unpack2385, %if_cont ]
  %3 = icmp sgt i64 %.unpack30, %1
  br i1 %3, label %if_cont12, label %then11

then11:                                           ; preds = %cond
  %4 = shl i64 %.unpack30, 1
  store i64 %4, i64* %.elt, align 8
  %newBuf.repack = getelementptr inbounds { %45*, i64 }, { %45*, i64 }* %newBuf, i64 0, i32 0
  store %45* null, %45** %newBuf.repack, align 8
  %newBuf.repack36 = getelementptr inbounds { %45*, i64 }, { %45*, i64 }* %newBuf, i64 0, i32 1
  store i64 0, i64* %newBuf.repack36, align 8
  %.unpack40.elt41 = getelementptr %44, %44* %0, i64 0, i32 1, i32 1
  %.unpack40.unpack42 = load i64, i64* %.unpack40.elt41, align 8
  store i64 %.unpack40.unpack42, i64* %newBuf.repack36, align 8
  %5 = shl i64 %4, 3
  %6 = call i8* @malloc(i64 %5)
  %.cast = ptrtoint i8* %6 to i64
  %7 = bitcast { %45*, i64 }* %newBuf to i64*
  store i64 %.cast, i64* %7, align 8
  %8 = inttoptr i64 %.cast to %45*
  br label %cond13

if_cont12:                                        ; preds = %cond
  ret {} zeroinitializer

cond13:                                           ; preds = %then14, %then11
  %9 = phi i64 [ %13, %then14 ], [ 0, %then11 ]
  %.unpack57.unpack59 = load i64, i64* %.unpack40.elt41, align 8
  %10 = icmp slt i64 %9, %.unpack57.unpack59
  br i1 %10, label %then14, label %if_cont15

then14:                                           ; preds = %cond13
  %.unpack75.elt = getelementptr inbounds %44, %44* %0, i64 0, i32 1, i32 0
  %.unpack75.unpack = load %45*, %45** %.unpack75.elt, align 8
  %parith = getelementptr %45, %45* %.unpack75.unpack, i64 %9
  %11 = bitcast %45* %parith to i64*
  %12 = load i64, i64* %11, align 8
  %parith16 = getelementptr %45, %45* %8, i64 %9
  %.cast83 = bitcast %45* %parith16 to i64*
  store i64 %12, i64* %.cast83, align 8
  %13 = add i64 %9, 1
  br label %cond13

if_cont15:                                        ; preds = %cond13
  %14 = getelementptr %44, %44* %0, i64 0, i32 1
  %15 = bitcast { %45*, i64 }* %14 to i64*
  store i64 %.cast, i64* %15, align 8
  store i64 %.unpack40.unpack42, i64* %.unpack40.elt41, align 8
  %.unpack23.pre = load i64, i64* %.elt, align 8
  br label %cond
}

define {} @testDefault() {
entry:
  %0 = call {} @cpi_assert(i1 true)
  %1 = call {} @cpi_assert(i1 true)
  %2 = call {} @cpi_assert(i1 true)
  %3 = call {} @cpi_assert(i1 true)
  ret {} zeroinitializer
}

define {} @testAnySimple1() {
entry:
  %local3493_ = alloca { i64, [24 x i8] }, align 8
  %local3506_ = alloca i64, align 8
  %a = alloca %46, align 8
  %local8895_ = alloca i32, align 8
  %tmpcast = bitcast i32* %local8895_ to %47*
  %local8931_ = alloca %48, align 8
  %local8933_ = alloca %48*, align 8
  %local9136_ = alloca i32, align 8
  %tmpcast96 = bitcast i32* %local9136_ to %50*
  %.repack = bitcast i32* %local8895_ to i8*
  store i8 102, i8* %.repack, align 8
  %0 = getelementptr inbounds %47, %47* %tmpcast, i64 0, i32 1
  store i8 117, i8* %0, align 1
  %1 = getelementptr inbounds %47, %47* %tmpcast, i64 0, i32 2
  store i8 99, i8* %1, align 2
  %2 = getelementptr inbounds %47, %47* %tmpcast, i64 0, i32 3
  store i8 107, i8* %2, align 1
  %3 = call i8* @malloc(i64 4)
  %4 = load i32, i32* %local8895_, align 8
  %5 = bitcast i8* %3 to i32*
  store i32 %4, i32* %5, align 4
  %.repack31 = getelementptr inbounds { i64, [24 x i8] }, { i64, [24 x i8] }* %local3493_, i64 0, i32 0
  store i64 3, i64* %.repack31, align 8
  %.repack33.repack = getelementptr inbounds %48, %48* %local8931_, i64 0, i32 0, i32 0, i32 0
  store i8* %3, i8** %.repack33.repack, align 8
  %6 = getelementptr inbounds %48, %48* %local8931_, i64 0, i32 0, i32 0, i32 1
  store i64 4, i64* %6, align 8
  %7 = getelementptr inbounds %48, %48* %local8931_, i64 0, i32 0, i32 1
  store { i64, [24 x i8] }* %local3493_, { i64, [24 x i8] }** %7, align 8
  %.unpack.unpack.unpack49.cast = ptrtoint i8* %3 to i64
  %8 = bitcast %48* %local8931_ to i64*
  store i64 %.unpack.unpack.unpack49.cast, i64* %8, align 8
  store i64 4, i64* %6, align 8
  store { i64, [24 x i8] }* %local3493_, { i64, [24 x i8] }** %7, align 8
  %9 = call i8* @malloc(i64 24)
  %10 = bitcast %48** %local8933_ to i8**
  store i8* %9, i8** %10, align 8
  %11 = bitcast { i64, [24 x i8] }** %7 to i64*
  %12 = ptrtoint { i64, [24 x i8] }* %local3493_ to i64
  %13 = bitcast i8* %9 to i64*
  store i64 %.unpack.unpack.unpack49.cast, i64* %13, align 8
  %.repack56.repack59 = getelementptr inbounds i8, i8* %9, i64 8
  %14 = bitcast i8* %.repack56.repack59 to i64*
  store i64 4, i64* %14, align 8
  %.repack57 = getelementptr inbounds i8, i8* %9, i64 16
  %15 = bitcast i8* %.repack57 to i64*
  store i64 %12, i64* %15, align 8
  %16 = bitcast %48** %local8933_ to i64*
  %17 = ptrtoint i8* %9 to i64
  store i64 77, i64* %local3506_, align 8
  %.repack63.repack = getelementptr inbounds %46, %46* %a, i64 0, i32 0, i32 0
  store i64 11, i64* %.repack63.repack, align 8
  %.repack63.repack66 = getelementptr inbounds %46, %46* %a, i64 0, i32 0, i32 1
  %18 = bitcast [24 x i8]* %.repack63.repack66 to i64*
  store i64 %17, i64* %18, align 8
  %19 = getelementptr inbounds %46, %46* %a, i64 0, i32 0, i32 1, i64 8
  %20 = bitcast i8* %19 to i64*
  store i64 1, i64* %20, align 8
  %.repack64 = getelementptr inbounds %46, %46* %a, i64 0, i32 0, i32 1, i64 16
  %21 = bitcast i8* %.repack64 to i64**
  store i64* %local3506_, i64** %21, align 8
  %22 = inttoptr i64 %17 to %49*
  %.elt76.phi.trans.insert = getelementptr inbounds %49, %49* %22, i64 0, i32 1
  %.unpack778182.pre = load { i64, [24 x i8] }*, { i64, [24 x i8] }** %.elt76.phi.trans.insert, align 8
  %.phi.trans.insert = getelementptr { i64, [24 x i8] }, { i64, [24 x i8] }* %.unpack778182.pre, i64 0, i32 0
  %.pre = load i64, i64* %.phi.trans.insert, align 8
  %23 = bitcast [24 x i8]* %.repack63.repack66 to %49**
  %24 = getelementptr { i64, [24 x i8] }, { i64, [24 x i8] }* %.unpack778182.pre, i64 0, i32 0
  %25 = icmp eq i64 %.pre, 3
  %26 = call {} @cpi_assert(i1 %25)
  %.unpack.elt.phi.trans.insert = getelementptr inbounds %49, %49* %22, i64 0, i32 0, i32 0
  %.unpack.unpack.pre = load i8*, i8** %.unpack.elt.phi.trans.insert, align 8
  %parith23.phi.trans.insert = getelementptr i8, i8* %.unpack.unpack.pre, i64 3
  %.pre99 = load i8, i8* %parith23.phi.trans.insert, align 1
  %parith23 = getelementptr i8, i8* %.unpack.unpack.pre, i64 3
  %.repack92 = bitcast i32* %local9136_ to i8*
  store i8 100, i8* %.repack92, align 8
  %27 = getelementptr inbounds %50, %50* %tmpcast96, i64 0, i32 1
  store i8 117, i8* %27, align 1
  %28 = getelementptr inbounds %50, %50* %tmpcast96, i64 0, i32 2
  store i8 99, i8* %28, align 2
  %29 = getelementptr inbounds %50, %50* %tmpcast96, i64 0, i32 3
  store i8 107, i8* %29, align 1
  %30 = call i8* @malloc(i64 4)
  %31 = load i32, i32* %local9136_, align 8
  %32 = bitcast i8* %30 to i32*
  store i32 %31, i32* %32, align 4
  %parith24 = getelementptr i8, i8* %30, i64 3
  %33 = lshr i32 %31, 24
  %34 = trunc i32 %33 to i8
  %35 = icmp eq i8 %.pre99, %34
  %36 = call {} @cpi_assert(i1 %35)
  ret {} zeroinitializer
}

define {} @testPrinting() {
entry:
  %e = alloca { i64, [8 x i8] }, align 8
  %local572_ = alloca i64, align 8
  %local9210_ = alloca %51, align 8
  %local9229_ = alloca %51*, align 8
  %local9254_ = alloca %52, align 8
  %local9258_ = alloca %52*, align 8
  %local19639_ = alloca i16, align 8
  %tmpcast = bitcast i16* %local19639_ to %53*
  %.repack = getelementptr inbounds { i64, [8 x i8] }, { i64, [8 x i8] }* %e, i64 0, i32 0
  store i64 2, i64* %.repack, align 8
  %.repack1 = getelementptr inbounds { i64, [8 x i8] }, { i64, [8 x i8] }* %e, i64 0, i32 1
  %0 = bitcast [8 x i8]* %.repack1 to i1*
  store i1 false, i1* %0, align 8
  %.repack2 = getelementptr inbounds %51, %51* %local9210_, i64 0, i32 0
  store i8 115, i8* %.repack2, align 8
  %1 = getelementptr inbounds %51, %51* %local9210_, i64 0, i32 1
  store i8 116, i8* %1, align 1
  %2 = getelementptr inbounds %51, %51* %local9210_, i64 0, i32 2
  store i8 114, i8* %2, align 2
  %3 = getelementptr inbounds %51, %51* %local9210_, i64 0, i32 3
  store i8 105, i8* %3, align 1
  %4 = getelementptr inbounds %51, %51* %local9210_, i64 0, i32 4
  store i8 110, i8* %4, align 4
  %5 = getelementptr inbounds %51, %51* %local9210_, i64 0, i32 5
  store i8 103, i8* %5, align 1
  store i8 115, i8* %.repack2, align 8
  store i8 116, i8* %1, align 1
  store i8 114, i8* %2, align 2
  store i8 105, i8* %3, align 1
  store i8 110, i8* %4, align 4
  store i8 103, i8* %5, align 1
  %6 = call i8* @malloc(i64 6)
  %7 = bitcast %51** %local9229_ to i8**
  store i8* %6, i8** %7, align 8
  store i8 115, i8* %6, align 1
  %8 = getelementptr inbounds i8, i8* %6, i64 1
  store i8 116, i8* %8, align 1
  %9 = getelementptr inbounds i8, i8* %6, i64 2
  store i8 114, i8* %9, align 1
  %10 = getelementptr inbounds i8, i8* %6, i64 3
  store i8 105, i8* %10, align 1
  %11 = getelementptr inbounds i8, i8* %6, i64 4
  store i8 110, i8* %11, align 1
  %12 = getelementptr inbounds i8, i8* %6, i64 5
  store i8 103, i8* %12, align 1
  %13 = insertvalue { i8*, i64 } zeroinitializer, i8* %6, 0
  %14 = insertvalue { i8*, i64 } %13, i64 6, 1
  %.repack53 = getelementptr inbounds %52, %52* %local9254_, i64 0, i32 0
  store i64 1, i64* %.repack53, align 8
  %15 = getelementptr inbounds %52, %52* %local9254_, i64 0, i32 1
  store i64 2, i64* %15, align 8
  %16 = getelementptr inbounds %52, %52* %local9254_, i64 0, i32 2
  store i64 3, i64* %16, align 8
  store i64 1, i64* %.repack53, align 8
  store i64 2, i64* %15, align 8
  store i64 3, i64* %16, align 8
  %17 = call i8* @malloc(i64 24)
  %18 = bitcast %52** %local9258_ to i8**
  store i8* %17, i8** %18, align 8
  %.repack73 = bitcast i8* %17 to i64*
  store i64 1, i64* %.repack73, align 8
  %.repack75 = getelementptr inbounds i8, i8* %17, i64 8
  %19 = bitcast i8* %.repack75 to i64*
  store i64 2, i64* %19, align 8
  %.repack77 = getelementptr inbounds i8, i8* %17, i64 16
  %20 = bitcast i8* %.repack77 to i64*
  store i64 3, i64* %20, align 8
  %21 = bitcast %52** %local9258_ to i64**
  %22 = insertvalue { i64*, i64 } zeroinitializer, i64* %.repack73, 0
  %23 = insertvalue { i64*, i64 } %22, i64 3, 1
  store i64 8767, i64* %local572_, align 8
  %24 = insertvalue { i64, i64* } { i64 3, i64* null }, i64* %local572_, 1
  %.unpack82.elt = getelementptr inbounds { i64, [8 x i8] }, { i64, [8 x i8] }* %e, i64 0, i32 1, i64 0
  %.unpack82.unpack = load i8, i8* %.unpack82.elt, align 8
  %25 = insertvalue [8 x i8] undef, i8 %.unpack82.unpack, 0
  %.unpack82.elt83 = getelementptr inbounds { i64, [8 x i8] }, { i64, [8 x i8] }* %e, i64 0, i32 1, i64 1
  %.unpack82.elt85 = getelementptr inbounds { i64, [8 x i8] }, { i64, [8 x i8] }* %e, i64 0, i32 1, i64 2
  %.unpack82.elt87 = getelementptr inbounds { i64, [8 x i8] }, { i64, [8 x i8] }* %e, i64 0, i32 1, i64 3
  %.unpack82.elt89 = getelementptr inbounds { i64, [8 x i8] }, { i64, [8 x i8] }* %e, i64 0, i32 1, i64 4
  %.unpack82.elt91 = getelementptr inbounds { i64, [8 x i8] }, { i64, [8 x i8] }* %e, i64 0, i32 1, i64 5
  %.unpack82.elt93 = getelementptr inbounds { i64, [8 x i8] }, { i64, [8 x i8] }* %e, i64 0, i32 1, i64 6
  %.unpack82.elt95 = getelementptr inbounds { i64, [8 x i8] }, { i64, [8 x i8] }* %e, i64 0, i32 1, i64 7
  %26 = insertvalue { i64, [8 x i8] } { i64 2, [8 x i8] undef }, [8 x i8] %25, 1
  %27 = insertvalue { i64, { i8*, i64 }, i1, { i64*, i64 }, { i64, i64* }, { i64, [8 x i8] } } { i64 123, { i8*, i64 } zeroinitializer, i1 false, { i64*, i64 } zeroinitializer, { i64, i64* } zeroinitializer, { i64, [8 x i8] } zeroinitializer }, { i8*, i64 } %14, 1
  %28 = insertvalue { i64, { i8*, i64 }, i1, { i64*, i64 }, { i64, i64* }, { i64, [8 x i8] } } %27, i1 false, 2
  %29 = insertvalue { i64, { i8*, i64 }, i1, { i64*, i64 }, { i64, i64* }, { i64, [8 x i8] } } %28, { i64*, i64 } %23, 3
  %30 = insertvalue { i64, { i8*, i64 }, i1, { i64*, i64 }, { i64, i64* }, { i64, [8 x i8] } } %29, { i64, i64* } %24, 4
  %31 = insertvalue { i64, { i8*, i64 }, i1, { i64*, i64 }, { i64, i64* }, { i64, [8 x i8] } } %30, { i64, [8 x i8] } %26, 5
  %realParam = alloca %54, align 8
  %32 = bitcast %54* %realParam to { i64, { i8*, i64 }, i1, { i64*, i64 }, { i64, i64* }, { i64, [8 x i8] } }*
  store { i64, { i8*, i64 }, i1, { i64*, i64 }, { i64, i64* }, { i64, [8 x i8] } } %31, { i64, { i8*, i64 }, i1, { i64*, i64 }, { i64, i64* }, { i64, [8 x i8] } }* %32, align 8
  %33 = load %54, %54* %realParam, align 8
  %34 = call {} @printWithSpaces(%54 %33)
  %.repack98 = bitcast i16* %local19639_ to i8*
  store i8 10, i8* %.repack98, align 8
  %35 = getelementptr inbounds %53, %53* %tmpcast, i64 0, i32 1
  store i8 10, i8* %35, align 1
  %36 = call i8* @malloc(i64 2)
  %37 = load i16, i16* %local19639_, align 8
  %38 = bitcast i8* %36 to i16*
  store i16 %37, i16* %38, align 2
  %39 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @printfFmtStr.78, i64 0, i64 0), i64 2, i8* %36)
  ret {} zeroinitializer
}

define {} @printWithSpaces(%54) {
entry:
  %1 = call {} @printWithSeparator(%54 %0)
  ret {} zeroinitializer
}

define {} @printWithSeparator(%54) {
entry:
  %local9345_ = alloca i16, align 8
  %tmpcast = bitcast i16* %local9345_ to %56*
  %local9305_ = alloca %54, align 8
  %.repack = bitcast i16* %local9345_ to i8*
  store i8 44, i8* %.repack, align 8
  %1 = getelementptr inbounds %56, %56* %tmpcast, i64 0, i32 1
  store i8 32, i8* %1, align 1
  %2 = call i8* @malloc(i64 2)
  %3 = load i16, i16* %local9345_, align 8
  %4 = bitcast i8* %2 to i16*
  store i16 %3, i16* %4, align 2
  store %54 %0, %54* %local9305_, align 8
  %5 = getelementptr inbounds %54, %54* %local9305_, i64 0, i32 0
  %6 = load i64, i64* %5, align 8
  %7 = call {} @print(i64 %6)
  %8 = insertvalue { i8*, i64 } undef, i8* %2, 0
  %9 = insertvalue { i8*, i64 } %8, i64 2, 1
  %10 = call {} @print.14({ i8*, i64 } %9)
  %.elt = getelementptr inbounds %54, %54* %local9305_, i64 0, i32 1, i32 0
  %.unpack3337 = load i8*, i8** %.elt, align 8
  %.elt28 = getelementptr inbounds %54, %54* %local9305_, i64 0, i32 1, i32 1
  %.unpack29 = load i64, i64* %.elt28, align 8
  %11 = insertvalue { i8*, i64 } undef, i8* %.unpack3337, 0
  %12 = insertvalue { i8*, i64 } %11, i64 %.unpack29, 1
  %13 = call {} @print.16({ i8*, i64 } %12)
  %14 = insertvalue { i8*, i64 } undef, i8* %2, 0
  %15 = insertvalue { i8*, i64 } %14, i64 2, 1
  %16 = call {} @print.18({ i8*, i64 } %15)
  %17 = getelementptr inbounds %54, %54* %local9305_, i64 0, i32 2
  %18 = load i1, i1* %17, align 8
  %19 = call {} @print.20(i1 %18)
  %20 = insertvalue { i8*, i64 } undef, i8* %2, 0
  %21 = insertvalue { i8*, i64 } %20, i64 2, 1
  %22 = call {} @print.22({ i8*, i64 } %21)
  %.elt38 = getelementptr inbounds %54, %54* %local9305_, i64 0, i32 3, i32 0
  %.unpack4448 = load i64*, i64** %.elt38, align 8
  %.elt39 = getelementptr inbounds %54, %54* %local9305_, i64 0, i32 3, i32 1
  %.unpack40 = load i64, i64* %.elt39, align 8
  %23 = insertvalue { i64*, i64 } undef, i64* %.unpack4448, 0
  %24 = insertvalue { i64*, i64 } %23, i64 %.unpack40, 1
  %25 = call {} @print.24({ i64*, i64 } %24)
  %26 = insertvalue { i8*, i64 } undef, i8* %2, 0
  %27 = insertvalue { i8*, i64 } %26, i64 2, 1
  %28 = call {} @print.32({ i8*, i64 } %27)
  %.elt49 = getelementptr inbounds %54, %54* %local9305_, i64 0, i32 4, i32 0
  %.unpack = load i64, i64* %.elt49, align 8
  %.elt50 = getelementptr inbounds %54, %54* %local9305_, i64 0, i32 4, i32 1
  %.unpack515560 = load i64*, i64** %.elt50, align 8
  %29 = insertvalue %55 undef, i64 %.unpack, 0
  %30 = insertvalue %55 %29, i64* %.unpack515560, 1
  %31 = call {} @print.34(%55 %30)
  %32 = insertvalue { i8*, i64 } undef, i8* %2, 0
  %33 = insertvalue { i8*, i64 } %32, i64 2, 1
  %34 = call {} @print.57({ i8*, i64 } %33)
  %realParam22 = alloca { i64, [8 x i8] }, align 8
  %.elt61 = getelementptr inbounds %54, %54* %local9305_, i64 0, i32 5, i32 0
  %.unpack62 = load i64, i64* %.elt61, align 8
  %.unpack64.elt = getelementptr inbounds %54, %54* %local9305_, i64 0, i32 5, i32 1, i64 0
  %.unpack64.unpack = load i8, i8* %.unpack64.elt, align 8
  %.unpack64.elt65 = getelementptr inbounds %54, %54* %local9305_, i64 0, i32 5, i32 1, i64 1
  %.unpack64.unpack66 = load i8, i8* %.unpack64.elt65, align 1
  %.unpack64.elt67 = getelementptr inbounds %54, %54* %local9305_, i64 0, i32 5, i32 1, i64 2
  %.unpack64.unpack68 = load i8, i8* %.unpack64.elt67, align 2
  %.unpack64.elt69 = getelementptr inbounds %54, %54* %local9305_, i64 0, i32 5, i32 1, i64 3
  %.unpack64.unpack70 = load i8, i8* %.unpack64.elt69, align 1
  %.unpack64.elt71 = getelementptr inbounds %54, %54* %local9305_, i64 0, i32 5, i32 1, i64 4
  %.unpack64.unpack72 = load i8, i8* %.unpack64.elt71, align 4
  %.unpack64.elt73 = getelementptr inbounds %54, %54* %local9305_, i64 0, i32 5, i32 1, i64 5
  %.unpack64.unpack74 = load i8, i8* %.unpack64.elt73, align 1
  %.unpack64.elt75 = getelementptr inbounds %54, %54* %local9305_, i64 0, i32 5, i32 1, i64 6
  %.unpack64.unpack76 = load i8, i8* %.unpack64.elt75, align 2
  %.unpack64.elt77 = getelementptr inbounds %54, %54* %local9305_, i64 0, i32 5, i32 1, i64 7
  %.unpack64.unpack78 = load i8, i8* %.unpack64.elt77, align 1
  %realParam22.repack = getelementptr inbounds { i64, [8 x i8] }, { i64, [8 x i8] }* %realParam22, i64 0, i32 0
  store i64 %.unpack62, i64* %realParam22.repack, align 8
  %realParam22.repack81.repack = getelementptr inbounds { i64, [8 x i8] }, { i64, [8 x i8] }* %realParam22, i64 0, i32 1, i64 0
  store i8 %.unpack64.unpack, i8* %realParam22.repack81.repack, align 8
  %realParam22.repack81.repack83 = getelementptr inbounds { i64, [8 x i8] }, { i64, [8 x i8] }* %realParam22, i64 0, i32 1, i64 1
  store i8 %.unpack64.unpack66, i8* %realParam22.repack81.repack83, align 1
  %realParam22.repack81.repack85 = getelementptr inbounds { i64, [8 x i8] }, { i64, [8 x i8] }* %realParam22, i64 0, i32 1, i64 2
  store i8 %.unpack64.unpack68, i8* %realParam22.repack81.repack85, align 2
  %realParam22.repack81.repack87 = getelementptr inbounds { i64, [8 x i8] }, { i64, [8 x i8] }* %realParam22, i64 0, i32 1, i64 3
  store i8 %.unpack64.unpack70, i8* %realParam22.repack81.repack87, align 1
  %realParam22.repack81.repack89 = getelementptr inbounds { i64, [8 x i8] }, { i64, [8 x i8] }* %realParam22, i64 0, i32 1, i64 4
  store i8 %.unpack64.unpack72, i8* %realParam22.repack81.repack89, align 4
  %realParam22.repack81.repack91 = getelementptr inbounds { i64, [8 x i8] }, { i64, [8 x i8] }* %realParam22, i64 0, i32 1, i64 5
  store i8 %.unpack64.unpack74, i8* %realParam22.repack81.repack91, align 1
  %realParam22.repack81.repack93 = getelementptr inbounds { i64, [8 x i8] }, { i64, [8 x i8] }* %realParam22, i64 0, i32 1, i64 6
  store i8 %.unpack64.unpack76, i8* %realParam22.repack81.repack93, align 2
  %realParam22.repack81.repack95 = getelementptr inbounds { i64, [8 x i8] }, { i64, [8 x i8] }* %realParam22, i64 0, i32 1, i64 7
  store i8 %.unpack64.unpack78, i8* %realParam22.repack81.repack95, align 1
  %35 = insertvalue { i64, [8 x i8] } undef, i64 %.unpack62, 0
  %36 = insertvalue [8 x i8] undef, i8 %.unpack64.unpack, 0
  %37 = insertvalue [8 x i8] %36, i8 %.unpack64.unpack66, 1
  %38 = insertvalue [8 x i8] %37, i8 %.unpack64.unpack68, 2
  %39 = insertvalue [8 x i8] %38, i8 %.unpack64.unpack70, 3
  %40 = insertvalue [8 x i8] %39, i8 %.unpack64.unpack72, 4
  %41 = insertvalue [8 x i8] %40, i8 %.unpack64.unpack74, 5
  %42 = insertvalue [8 x i8] %41, i8 %.unpack64.unpack76, 6
  %.unpack100115 = insertvalue [8 x i8] %42, i8 %.unpack64.unpack78, 7
  %43 = insertvalue { i64, [8 x i8] } %35, [8 x i8] %.unpack100115, 1
  %44 = call {} @print.59({ i64, [8 x i8] } %43)
  ret {} zeroinitializer
}

define {} @print(i64) {
entry:
  %1 = call {} @printInt(i64 %0)
  ret {} zeroinitializer
}

define {} @printInt(i64) {
entry:
  %table = alloca { { i8*, i64 }*, i64 }, align 8
  %b = alloca { i8*, i64 }, align 8
  %local10237_ = alloca %57, align 8
  %local10248_ = alloca %57*, align 8
  %1 = call i8* @malloc(i64 1)
  store i8 48, i8* %1, align 1
  %2 = call i8* @malloc(i64 1)
  store i8 49, i8* %2, align 1
  %3 = call i8* @malloc(i64 1)
  store i8 50, i8* %3, align 1
  %4 = call i8* @malloc(i64 1)
  store i8 51, i8* %4, align 1
  %5 = call i8* @malloc(i64 1)
  store i8 52, i8* %5, align 1
  %6 = call i8* @malloc(i64 1)
  store i8 53, i8* %6, align 1
  %7 = call i8* @malloc(i64 1)
  store i8 54, i8* %7, align 1
  %8 = call i8* @malloc(i64 1)
  store i8 55, i8* %8, align 1
  %9 = call i8* @malloc(i64 1)
  store i8 56, i8* %9, align 1
  %10 = call i8* @malloc(i64 1)
  store i8 57, i8* %10, align 1
  %.repack.repack = getelementptr inbounds %57, %57* %local10237_, i64 0, i32 0, i32 0
  store i8* %1, i8** %.repack.repack, align 8
  %11 = getelementptr inbounds %57, %57* %local10237_, i64 0, i32 0, i32 1
  store i64 1, i64* %11, align 8
  %12 = getelementptr inbounds %57, %57* %local10237_, i64 0, i32 1, i32 0
  store i8* %2, i8** %12, align 8
  %13 = getelementptr inbounds %57, %57* %local10237_, i64 0, i32 1, i32 1
  store i64 1, i64* %13, align 8
  %14 = getelementptr inbounds %57, %57* %local10237_, i64 0, i32 2, i32 0
  store i8* %3, i8** %14, align 8
  %15 = getelementptr inbounds %57, %57* %local10237_, i64 0, i32 2, i32 1
  store i64 1, i64* %15, align 8
  %16 = getelementptr inbounds %57, %57* %local10237_, i64 0, i32 3, i32 0
  store i8* %4, i8** %16, align 8
  %17 = getelementptr inbounds %57, %57* %local10237_, i64 0, i32 3, i32 1
  store i64 1, i64* %17, align 8
  %18 = getelementptr inbounds %57, %57* %local10237_, i64 0, i32 4, i32 0
  store i8* %5, i8** %18, align 8
  %19 = getelementptr inbounds %57, %57* %local10237_, i64 0, i32 4, i32 1
  store i64 1, i64* %19, align 8
  %20 = getelementptr inbounds %57, %57* %local10237_, i64 0, i32 5, i32 0
  store i8* %6, i8** %20, align 8
  %21 = getelementptr inbounds %57, %57* %local10237_, i64 0, i32 5, i32 1
  store i64 1, i64* %21, align 8
  %22 = getelementptr inbounds %57, %57* %local10237_, i64 0, i32 6, i32 0
  store i8* %7, i8** %22, align 8
  %23 = getelementptr inbounds %57, %57* %local10237_, i64 0, i32 6, i32 1
  store i64 1, i64* %23, align 8
  %24 = getelementptr inbounds %57, %57* %local10237_, i64 0, i32 7, i32 0
  store i8* %8, i8** %24, align 8
  %25 = getelementptr inbounds %57, %57* %local10237_, i64 0, i32 7, i32 1
  store i64 1, i64* %25, align 8
  %26 = getelementptr inbounds %57, %57* %local10237_, i64 0, i32 8, i32 0
  store i8* %9, i8** %26, align 8
  %27 = getelementptr inbounds %57, %57* %local10237_, i64 0, i32 8, i32 1
  store i64 1, i64* %27, align 8
  %28 = getelementptr inbounds %57, %57* %local10237_, i64 0, i32 9, i32 0
  store i8* %10, i8** %28, align 8
  %29 = getelementptr inbounds %57, %57* %local10237_, i64 0, i32 9, i32 1
  store i64 1, i64* %29, align 8
  %30 = bitcast %57* %local10237_ to i64*
  %31 = ptrtoint i8* %1 to i64
  %32 = bitcast i8** %12 to i64*
  %33 = ptrtoint i8* %2 to i64
  %34 = bitcast i8** %14 to i64*
  %35 = ptrtoint i8* %3 to i64
  %36 = bitcast i8** %16 to i64*
  %37 = ptrtoint i8* %4 to i64
  %38 = bitcast i8** %18 to i64*
  %39 = ptrtoint i8* %5 to i64
  %40 = bitcast i8** %20 to i64*
  %41 = ptrtoint i8* %6 to i64
  %42 = bitcast i8** %22 to i64*
  %43 = ptrtoint i8* %7 to i64
  %44 = bitcast i8** %24 to i64*
  %45 = ptrtoint i8* %8 to i64
  %46 = bitcast i8** %26 to i64*
  %47 = ptrtoint i8* %9 to i64
  %48 = bitcast i8** %28 to i64*
  %49 = ptrtoint i8* %10 to i64
  store i64 %31, i64* %30, align 8
  store i64 1, i64* %11, align 8
  store i64 %33, i64* %32, align 8
  store i64 1, i64* %13, align 8
  store i64 %35, i64* %34, align 8
  store i64 1, i64* %15, align 8
  store i64 %37, i64* %36, align 8
  store i64 1, i64* %17, align 8
  store i64 %39, i64* %38, align 8
  store i64 1, i64* %19, align 8
  store i64 %41, i64* %40, align 8
  store i64 1, i64* %21, align 8
  store i64 %43, i64* %42, align 8
  store i64 1, i64* %23, align 8
  store i64 %45, i64* %44, align 8
  store i64 1, i64* %25, align 8
  store i64 %47, i64* %46, align 8
  store i64 1, i64* %27, align 8
  store i64 %49, i64* %48, align 8
  store i64 1, i64* %29, align 8
  %50 = call i8* @malloc(i64 160)
  %51 = bitcast %57** %local10248_ to i8**
  store i8* %50, i8** %51, align 8
  %.unpack.unpack245 = load i64, i64* %30, align 8
  %.unpack.unpack195 = load i64, i64* %11, align 8
  %.unpack150.unpack244 = load i64, i64* %32, align 8
  %.unpack150.unpack192 = load i64, i64* %13, align 8
  %.unpack152.unpack243 = load i64, i64* %34, align 8
  %.unpack152.unpack189 = load i64, i64* %15, align 8
  %.unpack154.unpack242 = load i64, i64* %36, align 8
  %.unpack154.unpack186 = load i64, i64* %17, align 8
  %.unpack156.unpack241 = load i64, i64* %38, align 8
  %.unpack156.unpack183 = load i64, i64* %19, align 8
  %.unpack158.unpack240 = load i64, i64* %40, align 8
  %.unpack158.unpack180 = load i64, i64* %21, align 8
  %.unpack160.unpack239 = load i64, i64* %42, align 8
  %.unpack160.unpack177 = load i64, i64* %23, align 8
  %.unpack162.unpack238 = load i64, i64* %44, align 8
  %.unpack162.unpack174 = load i64, i64* %25, align 8
  %.unpack164.unpack237 = load i64, i64* %46, align 8
  %.unpack164.unpack171 = load i64, i64* %27, align 8
  %.unpack166.unpack236 = load i64, i64* %48, align 8
  %.unpack166.unpack168 = load i64, i64* %29, align 8
  %52 = bitcast i8* %50 to i64*
  store i64 %.unpack.unpack245, i64* %52, align 8
  %.repack.repack234 = getelementptr inbounds i8, i8* %50, i64 8
  %53 = bitcast i8* %.repack.repack234 to i64*
  store i64 %.unpack.unpack195, i64* %53, align 8
  %.repack197.repack = getelementptr inbounds i8, i8* %50, i64 16
  %54 = bitcast i8* %.repack197.repack to i64*
  store i64 %.unpack150.unpack244, i64* %54, align 8
  %.repack197.repack231 = getelementptr inbounds i8, i8* %50, i64 24
  %55 = bitcast i8* %.repack197.repack231 to i64*
  store i64 %.unpack150.unpack192, i64* %55, align 8
  %.repack199.repack = getelementptr inbounds i8, i8* %50, i64 32
  %56 = bitcast i8* %.repack199.repack to i64*
  store i64 %.unpack152.unpack243, i64* %56, align 8
  %.repack199.repack229 = getelementptr inbounds i8, i8* %50, i64 40
  %57 = bitcast i8* %.repack199.repack229 to i64*
  store i64 %.unpack152.unpack189, i64* %57, align 8
  %.repack201.repack = getelementptr inbounds i8, i8* %50, i64 48
  %58 = bitcast i8* %.repack201.repack to i64*
  store i64 %.unpack154.unpack242, i64* %58, align 8
  %.repack201.repack227 = getelementptr inbounds i8, i8* %50, i64 56
  %59 = bitcast i8* %.repack201.repack227 to i64*
  store i64 %.unpack154.unpack186, i64* %59, align 8
  %.repack203.repack = getelementptr inbounds i8, i8* %50, i64 64
  %60 = bitcast i8* %.repack203.repack to i64*
  store i64 %.unpack156.unpack241, i64* %60, align 8
  %.repack203.repack225 = getelementptr inbounds i8, i8* %50, i64 72
  %61 = bitcast i8* %.repack203.repack225 to i64*
  store i64 %.unpack156.unpack183, i64* %61, align 8
  %.repack205.repack = getelementptr inbounds i8, i8* %50, i64 80
  %62 = bitcast i8* %.repack205.repack to i64*
  store i64 %.unpack158.unpack240, i64* %62, align 8
  %.repack205.repack223 = getelementptr inbounds i8, i8* %50, i64 88
  %63 = bitcast i8* %.repack205.repack223 to i64*
  store i64 %.unpack158.unpack180, i64* %63, align 8
  %.repack207.repack = getelementptr inbounds i8, i8* %50, i64 96
  %64 = bitcast i8* %.repack207.repack to i64*
  store i64 %.unpack160.unpack239, i64* %64, align 8
  %.repack207.repack221 = getelementptr inbounds i8, i8* %50, i64 104
  %65 = bitcast i8* %.repack207.repack221 to i64*
  store i64 %.unpack160.unpack177, i64* %65, align 8
  %.repack209.repack = getelementptr inbounds i8, i8* %50, i64 112
  %66 = bitcast i8* %.repack209.repack to i64*
  store i64 %.unpack162.unpack238, i64* %66, align 8
  %.repack209.repack219 = getelementptr inbounds i8, i8* %50, i64 120
  %67 = bitcast i8* %.repack209.repack219 to i64*
  store i64 %.unpack162.unpack174, i64* %67, align 8
  %.repack211.repack = getelementptr inbounds i8, i8* %50, i64 128
  %68 = bitcast i8* %.repack211.repack to i64*
  store i64 %.unpack164.unpack237, i64* %68, align 8
  %.repack211.repack217 = getelementptr inbounds i8, i8* %50, i64 136
  %69 = bitcast i8* %.repack211.repack217 to i64*
  store i64 %.unpack164.unpack171, i64* %69, align 8
  %.repack213.repack = getelementptr inbounds i8, i8* %50, i64 144
  %70 = bitcast i8* %.repack213.repack to i64*
  store i64 %.unpack166.unpack236, i64* %70, align 8
  %.repack213.repack215 = getelementptr inbounds i8, i8* %50, i64 152
  %71 = bitcast i8* %.repack213.repack215 to i64*
  store i64 %.unpack166.unpack168, i64* %71, align 8
  %72 = bitcast %57** %local10248_ to i64*
  %73 = ptrtoint i8* %50 to i64
  %74 = bitcast { { i8*, i64 }*, i64 }* %table to i64*
  store i64 %73, i64* %74, align 8
  %table.repack246 = getelementptr inbounds { { i8*, i64 }*, i64 }, { { i8*, i64 }*, i64 }* %table, i64 0, i32 1
  store i64 10, i64* %table.repack246, align 8
  %b.repack = getelementptr inbounds { i8*, i64 }, { i8*, i64 }* %b, i64 0, i32 0
  store i8* null, i8** %b.repack, align 8
  %b.repack248 = getelementptr inbounds { i8*, i64 }, { i8*, i64 }* %b, i64 0, i32 1
  store i64 0, i64* %b.repack248, align 8
  %75 = call i8* @malloc(i64 20)
  %.cast = ptrtoint i8* %75 to i64
  %76 = bitcast { i8*, i64 }* %b to i64*
  store i64 %.cast, i64* %76, align 8
  %77 = inttoptr i64 %73 to { i8*, i64 }*
  br label %cond

cond:                                             ; preds = %then, %entry
  %78 = phi i64 [ %82, %then ], [ %0, %entry ]
  %79 = phi i32 [ %85, %then ], [ 0, %entry ]
  %80 = icmp sgt i64 %78, 10
  br i1 %80, label %then, label %if_cont

then:                                             ; preds = %cond
  %81 = call i64 @mod(i64 %78, i64 10)
  %82 = sdiv i64 %78, 10
  %.elt266 = getelementptr inbounds { { i8*, i64 }*, i64 }, { { i8*, i64 }*, i64 }* %table, i64 0, i32 0
  %.elt270 = getelementptr inbounds { i8*, i64 }, { i8*, i64 }* %77, i64 %81, i32 0
  %.unpack271274276 = load i8*, i8** %.elt270, align 8
  %83 = load i8, i8* %.unpack271274276, align 1
  %84 = sext i32 %79 to i64
  %parith4 = getelementptr i8, i8* %75, i64 %84
  store i8 %83, i8* %parith4, align 1
  %85 = add i32 %79, 1
  br label %cond

if_cont:                                          ; preds = %cond
  %86 = call i64 @mod.13(i64 %78, i64 10)
  %87 = sdiv i64 %78, 10
  %.elt = getelementptr inbounds { { i8*, i64 }*, i64 }, { { i8*, i64 }*, i64 }* %table, i64 0, i32 0
  %.elt251 = getelementptr inbounds { i8*, i64 }, { i8*, i64 }* %77, i64 %86, i32 0
  %.unpack252255257 = load i8*, i8** %.elt251, align 8
  %88 = load i8, i8* %.unpack252255257, align 1
  %89 = sext i32 %79 to i64
  %parith9 = getelementptr i8, i8* %75, i64 %89
  store i8 %88, i8* %parith9, align 1
  br label %cond10

cond10:                                           ; preds = %then11, %if_cont
  %90 = phi i32 [ %95, %then11 ], [ %79, %if_cont ]
  %91 = icmp sgt i32 %90, -1
  br i1 %91, label %then11, label %if_cont12

then11:                                           ; preds = %cond10
  %92 = sext i32 %90 to i64
  %parith13 = getelementptr i8, i8* %75, i64 %92
  %93 = load i8, i8* %parith13, align 1
  %94 = call {} @putchar(i8 %93)
  %95 = add i32 %90, -1
  br label %cond10

if_cont12:                                        ; preds = %cond10
  ret {} zeroinitializer
}

define i64 @mod(i64, i64) {
entry:
  %2 = srem i64 %0, %1
  ret i64 %2
}

define i64 @mod.13(i64, i64) {
entry:
  %2 = srem i64 %0, %1
  ret i64 %2
}

define {} @putchar(i8) {
entry:
  %1 = call i8* @malloc(i64 1)
  store i8 %0, i8* %1, align 1
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @printfFmtStr, i64 0, i64 0), i64 1, i8* %1)
  ret {} zeroinitializer
}

define {} @print.14({ i8*, i64 }) {
entry:
  %1 = extractvalue { i8*, i64 } %0, 0
  %2 = extractvalue { i8*, i64 } %0, 1
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @printfFmtStr.15, i64 0, i64 0), i64 %2, i8* %1)
  ret {} zeroinitializer
}

define {} @print.16({ i8*, i64 }) {
entry:
  %1 = extractvalue { i8*, i64 } %0, 0
  %2 = extractvalue { i8*, i64 } %0, 1
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @printfFmtStr.17, i64 0, i64 0), i64 %2, i8* %1)
  ret {} zeroinitializer
}

define {} @print.18({ i8*, i64 }) {
entry:
  %1 = extractvalue { i8*, i64 } %0, 0
  %2 = extractvalue { i8*, i64 } %0, 1
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @printfFmtStr.19, i64 0, i64 0), i64 %2, i8* %1)
  ret {} zeroinitializer
}

define {} @print.20(i1) {
entry:
  %local11409_ = alloca %58, align 8
  %local11425_ = alloca %58*, align 8
  br i1 %0, label %if_cont18, label %else17

else17:                                           ; preds = %entry
  %.repack = getelementptr inbounds %58, %58* %local11409_, i64 0, i32 0
  store i8 102, i8* %.repack, align 8
  %1 = getelementptr inbounds %58, %58* %local11409_, i64 0, i32 1
  store i8 97, i8* %1, align 1
  %2 = getelementptr inbounds %58, %58* %local11409_, i64 0, i32 2
  store i8 108, i8* %2, align 2
  %3 = getelementptr inbounds %58, %58* %local11409_, i64 0, i32 3
  store i8 115, i8* %3, align 1
  %4 = getelementptr inbounds %58, %58* %local11409_, i64 0, i32 4
  store i8 101, i8* %4, align 4
  store i8 102, i8* %.repack, align 8
  store i8 97, i8* %1, align 1
  store i8 108, i8* %2, align 2
  store i8 115, i8* %3, align 1
  store i8 101, i8* %4, align 4
  %5 = call i8* @malloc(i64 5)
  %6 = bitcast %58** %local11425_ to i8**
  store i8* %5, i8** %6, align 8
  store i8 102, i8* %5, align 1
  %7 = getelementptr inbounds i8, i8* %5, i64 1
  store i8 97, i8* %7, align 1
  %8 = getelementptr inbounds i8, i8* %5, i64 2
  store i8 108, i8* %8, align 1
  %9 = getelementptr inbounds i8, i8* %5, i64 3
  store i8 115, i8* %9, align 1
  %10 = getelementptr inbounds i8, i8* %5, i64 4
  store i8 101, i8* %10, align 1
  %11 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @printfFmtStr.21, i64 0, i64 0), i64 5, i8* %5)
  br label %if_cont18

if_cont18:                                        ; preds = %entry, %else17
  ret {} zeroinitializer
}

define {} @print.22({ i8*, i64 }) {
entry:
  %1 = extractvalue { i8*, i64 } %0, 0
  %2 = extractvalue { i8*, i64 } %0, 1
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @printfFmtStr.23, i64 0, i64 0), i64 %2, i8* %1)
  ret {} zeroinitializer
}

define {} @print.24({ i64*, i64 }) {
entry:
  %local11934_ = alloca %59, align 8
  %local11944_ = alloca %59*, align 8
  %foreach_index = alloca { i64*, i64 }, align 8
  %local12769_ = alloca i16, align 8
  %tmpcast = bitcast i16* %local12769_ to %60*
  %.repack = getelementptr inbounds %59, %59* %local11934_, i64 0, i32 0
  store i8 91, i8* %.repack, align 8
  %1 = getelementptr inbounds %59, %59* %local11934_, i64 0, i32 1
  store i8 93, i8* %1, align 1
  %2 = getelementptr inbounds %59, %59* %local11934_, i64 0, i32 2
  store i8 123, i8* %2, align 2
  store i8 91, i8* %.repack, align 8
  store i8 93, i8* %1, align 1
  store i8 123, i8* %2, align 2
  %3 = call i8* @malloc(i64 3)
  %4 = bitcast %59** %local11944_ to i8**
  store i8* %3, i8** %4, align 8
  store i8 91, i8* %3, align 1
  %5 = getelementptr inbounds i8, i8* %3, i64 1
  store i8 93, i8* %5, align 1
  %6 = getelementptr inbounds i8, i8* %3, i64 2
  store i8 123, i8* %6, align 1
  %7 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @printfFmtStr.25, i64 0, i64 0), i64 3, i8* %3)
  %foreach_index.repack = getelementptr inbounds { i64*, i64 }, { i64*, i64 }* %foreach_index, i64 0, i32 0
  %.elt47 = extractvalue { i64*, i64 } %0, 0
  store i64* %.elt47, i64** %foreach_index.repack, align 8
  %foreach_index.repack48 = getelementptr inbounds { i64*, i64 }, { i64*, i64 }* %foreach_index, i64 0, i32 1
  %.elt49 = extractvalue { i64*, i64 } %0, 1
  store i64 %.elt49, i64* %foreach_index.repack48, align 8
  br label %cond

cond:                                             ; preds = %if_cont26, %entry
  %8 = phi i64 [ %21, %if_cont26 ], [ 0, %entry ]
  %9 = icmp slt i64 %8, %.elt49
  br i1 %9, label %then22, label %if_cont23

then22:                                           ; preds = %cond
  %parith = getelementptr i64, i64* %.elt47, i64 %8
  %10 = load i64, i64* %parith, align 8
  %11 = call {} @print.26(i64 %10)
  %12 = add i64 %.elt49, -1
  %13 = icmp slt i64 %8, %12
  br i1 %13, label %then24, label %if_cont26

if_cont23:                                        ; preds = %cond
  %14 = call i8* @malloc(i64 1)
  store i8 125, i8* %14, align 1
  %15 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @printfFmtStr.31, i64 0, i64 0), i64 1, i8* %14)
  ret {} zeroinitializer

then24:                                           ; preds = %then22
  %.repack54 = bitcast i16* %local12769_ to i8*
  store i8 44, i8* %.repack54, align 8
  %16 = getelementptr inbounds %60, %60* %tmpcast, i64 0, i32 1
  store i8 32, i8* %16, align 1
  %17 = call i8* @malloc(i64 2)
  %18 = load i16, i16* %local12769_, align 8
  %19 = bitcast i8* %17 to i16*
  store i16 %18, i16* %19, align 2
  %20 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @printfFmtStr.30, i64 0, i64 0), i64 2, i8* %17)
  br label %if_cont26

if_cont26:                                        ; preds = %then22, %then24
  %21 = add i64 %8, 1
  br label %cond
}

define {} @print.26(i64) {
entry:
  %1 = call {} @printInt.27(i64 %0)
  ret {} zeroinitializer
}

define {} @printInt.27(i64) {
entry:
  %table = alloca { { i8*, i64 }*, i64 }, align 8
  %b = alloca { i8*, i64 }, align 8
  %local12578_ = alloca %61, align 8
  %local12589_ = alloca %61*, align 8
  %1 = call i8* @malloc(i64 1)
  store i8 48, i8* %1, align 1
  %2 = call i8* @malloc(i64 1)
  store i8 49, i8* %2, align 1
  %3 = call i8* @malloc(i64 1)
  store i8 50, i8* %3, align 1
  %4 = call i8* @malloc(i64 1)
  store i8 51, i8* %4, align 1
  %5 = call i8* @malloc(i64 1)
  store i8 52, i8* %5, align 1
  %6 = call i8* @malloc(i64 1)
  store i8 53, i8* %6, align 1
  %7 = call i8* @malloc(i64 1)
  store i8 54, i8* %7, align 1
  %8 = call i8* @malloc(i64 1)
  store i8 55, i8* %8, align 1
  %9 = call i8* @malloc(i64 1)
  store i8 56, i8* %9, align 1
  %10 = call i8* @malloc(i64 1)
  store i8 57, i8* %10, align 1
  %.repack.repack = getelementptr inbounds %61, %61* %local12578_, i64 0, i32 0, i32 0
  store i8* %1, i8** %.repack.repack, align 8
  %11 = getelementptr inbounds %61, %61* %local12578_, i64 0, i32 0, i32 1
  store i64 1, i64* %11, align 8
  %12 = getelementptr inbounds %61, %61* %local12578_, i64 0, i32 1, i32 0
  store i8* %2, i8** %12, align 8
  %13 = getelementptr inbounds %61, %61* %local12578_, i64 0, i32 1, i32 1
  store i64 1, i64* %13, align 8
  %14 = getelementptr inbounds %61, %61* %local12578_, i64 0, i32 2, i32 0
  store i8* %3, i8** %14, align 8
  %15 = getelementptr inbounds %61, %61* %local12578_, i64 0, i32 2, i32 1
  store i64 1, i64* %15, align 8
  %16 = getelementptr inbounds %61, %61* %local12578_, i64 0, i32 3, i32 0
  store i8* %4, i8** %16, align 8
  %17 = getelementptr inbounds %61, %61* %local12578_, i64 0, i32 3, i32 1
  store i64 1, i64* %17, align 8
  %18 = getelementptr inbounds %61, %61* %local12578_, i64 0, i32 4, i32 0
  store i8* %5, i8** %18, align 8
  %19 = getelementptr inbounds %61, %61* %local12578_, i64 0, i32 4, i32 1
  store i64 1, i64* %19, align 8
  %20 = getelementptr inbounds %61, %61* %local12578_, i64 0, i32 5, i32 0
  store i8* %6, i8** %20, align 8
  %21 = getelementptr inbounds %61, %61* %local12578_, i64 0, i32 5, i32 1
  store i64 1, i64* %21, align 8
  %22 = getelementptr inbounds %61, %61* %local12578_, i64 0, i32 6, i32 0
  store i8* %7, i8** %22, align 8
  %23 = getelementptr inbounds %61, %61* %local12578_, i64 0, i32 6, i32 1
  store i64 1, i64* %23, align 8
  %24 = getelementptr inbounds %61, %61* %local12578_, i64 0, i32 7, i32 0
  store i8* %8, i8** %24, align 8
  %25 = getelementptr inbounds %61, %61* %local12578_, i64 0, i32 7, i32 1
  store i64 1, i64* %25, align 8
  %26 = getelementptr inbounds %61, %61* %local12578_, i64 0, i32 8, i32 0
  store i8* %9, i8** %26, align 8
  %27 = getelementptr inbounds %61, %61* %local12578_, i64 0, i32 8, i32 1
  store i64 1, i64* %27, align 8
  %28 = getelementptr inbounds %61, %61* %local12578_, i64 0, i32 9, i32 0
  store i8* %10, i8** %28, align 8
  %29 = getelementptr inbounds %61, %61* %local12578_, i64 0, i32 9, i32 1
  store i64 1, i64* %29, align 8
  %30 = bitcast %61* %local12578_ to i64*
  %31 = ptrtoint i8* %1 to i64
  %32 = bitcast i8** %12 to i64*
  %33 = ptrtoint i8* %2 to i64
  %34 = bitcast i8** %14 to i64*
  %35 = ptrtoint i8* %3 to i64
  %36 = bitcast i8** %16 to i64*
  %37 = ptrtoint i8* %4 to i64
  %38 = bitcast i8** %18 to i64*
  %39 = ptrtoint i8* %5 to i64
  %40 = bitcast i8** %20 to i64*
  %41 = ptrtoint i8* %6 to i64
  %42 = bitcast i8** %22 to i64*
  %43 = ptrtoint i8* %7 to i64
  %44 = bitcast i8** %24 to i64*
  %45 = ptrtoint i8* %8 to i64
  %46 = bitcast i8** %26 to i64*
  %47 = ptrtoint i8* %9 to i64
  %48 = bitcast i8** %28 to i64*
  %49 = ptrtoint i8* %10 to i64
  store i64 %31, i64* %30, align 8
  store i64 1, i64* %11, align 8
  store i64 %33, i64* %32, align 8
  store i64 1, i64* %13, align 8
  store i64 %35, i64* %34, align 8
  store i64 1, i64* %15, align 8
  store i64 %37, i64* %36, align 8
  store i64 1, i64* %17, align 8
  store i64 %39, i64* %38, align 8
  store i64 1, i64* %19, align 8
  store i64 %41, i64* %40, align 8
  store i64 1, i64* %21, align 8
  store i64 %43, i64* %42, align 8
  store i64 1, i64* %23, align 8
  store i64 %45, i64* %44, align 8
  store i64 1, i64* %25, align 8
  store i64 %47, i64* %46, align 8
  store i64 1, i64* %27, align 8
  store i64 %49, i64* %48, align 8
  store i64 1, i64* %29, align 8
  %50 = call i8* @malloc(i64 160)
  %51 = bitcast %61** %local12589_ to i8**
  store i8* %50, i8** %51, align 8
  %.unpack.unpack245 = load i64, i64* %30, align 8
  %.unpack.unpack195 = load i64, i64* %11, align 8
  %.unpack150.unpack244 = load i64, i64* %32, align 8
  %.unpack150.unpack192 = load i64, i64* %13, align 8
  %.unpack152.unpack243 = load i64, i64* %34, align 8
  %.unpack152.unpack189 = load i64, i64* %15, align 8
  %.unpack154.unpack242 = load i64, i64* %36, align 8
  %.unpack154.unpack186 = load i64, i64* %17, align 8
  %.unpack156.unpack241 = load i64, i64* %38, align 8
  %.unpack156.unpack183 = load i64, i64* %19, align 8
  %.unpack158.unpack240 = load i64, i64* %40, align 8
  %.unpack158.unpack180 = load i64, i64* %21, align 8
  %.unpack160.unpack239 = load i64, i64* %42, align 8
  %.unpack160.unpack177 = load i64, i64* %23, align 8
  %.unpack162.unpack238 = load i64, i64* %44, align 8
  %.unpack162.unpack174 = load i64, i64* %25, align 8
  %.unpack164.unpack237 = load i64, i64* %46, align 8
  %.unpack164.unpack171 = load i64, i64* %27, align 8
  %.unpack166.unpack236 = load i64, i64* %48, align 8
  %.unpack166.unpack168 = load i64, i64* %29, align 8
  %52 = bitcast i8* %50 to i64*
  store i64 %.unpack.unpack245, i64* %52, align 8
  %.repack.repack234 = getelementptr inbounds i8, i8* %50, i64 8
  %53 = bitcast i8* %.repack.repack234 to i64*
  store i64 %.unpack.unpack195, i64* %53, align 8
  %.repack197.repack = getelementptr inbounds i8, i8* %50, i64 16
  %54 = bitcast i8* %.repack197.repack to i64*
  store i64 %.unpack150.unpack244, i64* %54, align 8
  %.repack197.repack231 = getelementptr inbounds i8, i8* %50, i64 24
  %55 = bitcast i8* %.repack197.repack231 to i64*
  store i64 %.unpack150.unpack192, i64* %55, align 8
  %.repack199.repack = getelementptr inbounds i8, i8* %50, i64 32
  %56 = bitcast i8* %.repack199.repack to i64*
  store i64 %.unpack152.unpack243, i64* %56, align 8
  %.repack199.repack229 = getelementptr inbounds i8, i8* %50, i64 40
  %57 = bitcast i8* %.repack199.repack229 to i64*
  store i64 %.unpack152.unpack189, i64* %57, align 8
  %.repack201.repack = getelementptr inbounds i8, i8* %50, i64 48
  %58 = bitcast i8* %.repack201.repack to i64*
  store i64 %.unpack154.unpack242, i64* %58, align 8
  %.repack201.repack227 = getelementptr inbounds i8, i8* %50, i64 56
  %59 = bitcast i8* %.repack201.repack227 to i64*
  store i64 %.unpack154.unpack186, i64* %59, align 8
  %.repack203.repack = getelementptr inbounds i8, i8* %50, i64 64
  %60 = bitcast i8* %.repack203.repack to i64*
  store i64 %.unpack156.unpack241, i64* %60, align 8
  %.repack203.repack225 = getelementptr inbounds i8, i8* %50, i64 72
  %61 = bitcast i8* %.repack203.repack225 to i64*
  store i64 %.unpack156.unpack183, i64* %61, align 8
  %.repack205.repack = getelementptr inbounds i8, i8* %50, i64 80
  %62 = bitcast i8* %.repack205.repack to i64*
  store i64 %.unpack158.unpack240, i64* %62, align 8
  %.repack205.repack223 = getelementptr inbounds i8, i8* %50, i64 88
  %63 = bitcast i8* %.repack205.repack223 to i64*
  store i64 %.unpack158.unpack180, i64* %63, align 8
  %.repack207.repack = getelementptr inbounds i8, i8* %50, i64 96
  %64 = bitcast i8* %.repack207.repack to i64*
  store i64 %.unpack160.unpack239, i64* %64, align 8
  %.repack207.repack221 = getelementptr inbounds i8, i8* %50, i64 104
  %65 = bitcast i8* %.repack207.repack221 to i64*
  store i64 %.unpack160.unpack177, i64* %65, align 8
  %.repack209.repack = getelementptr inbounds i8, i8* %50, i64 112
  %66 = bitcast i8* %.repack209.repack to i64*
  store i64 %.unpack162.unpack238, i64* %66, align 8
  %.repack209.repack219 = getelementptr inbounds i8, i8* %50, i64 120
  %67 = bitcast i8* %.repack209.repack219 to i64*
  store i64 %.unpack162.unpack174, i64* %67, align 8
  %.repack211.repack = getelementptr inbounds i8, i8* %50, i64 128
  %68 = bitcast i8* %.repack211.repack to i64*
  store i64 %.unpack164.unpack237, i64* %68, align 8
  %.repack211.repack217 = getelementptr inbounds i8, i8* %50, i64 136
  %69 = bitcast i8* %.repack211.repack217 to i64*
  store i64 %.unpack164.unpack171, i64* %69, align 8
  %.repack213.repack = getelementptr inbounds i8, i8* %50, i64 144
  %70 = bitcast i8* %.repack213.repack to i64*
  store i64 %.unpack166.unpack236, i64* %70, align 8
  %.repack213.repack215 = getelementptr inbounds i8, i8* %50, i64 152
  %71 = bitcast i8* %.repack213.repack215 to i64*
  store i64 %.unpack166.unpack168, i64* %71, align 8
  %72 = bitcast %61** %local12589_ to i64*
  %73 = ptrtoint i8* %50 to i64
  %74 = bitcast { { i8*, i64 }*, i64 }* %table to i64*
  store i64 %73, i64* %74, align 8
  %table.repack246 = getelementptr inbounds { { i8*, i64 }*, i64 }, { { i8*, i64 }*, i64 }* %table, i64 0, i32 1
  store i64 10, i64* %table.repack246, align 8
  %b.repack = getelementptr inbounds { i8*, i64 }, { i8*, i64 }* %b, i64 0, i32 0
  store i8* null, i8** %b.repack, align 8
  %b.repack248 = getelementptr inbounds { i8*, i64 }, { i8*, i64 }* %b, i64 0, i32 1
  store i64 0, i64* %b.repack248, align 8
  %75 = call i8* @malloc(i64 20)
  %.cast = ptrtoint i8* %75 to i64
  %76 = bitcast { i8*, i64 }* %b to i64*
  store i64 %.cast, i64* %76, align 8
  %77 = inttoptr i64 %73 to { i8*, i64 }*
  br label %cond

cond:                                             ; preds = %then, %entry
  %78 = phi i64 [ %82, %then ], [ %0, %entry ]
  %79 = phi i32 [ %85, %then ], [ 0, %entry ]
  %80 = icmp sgt i64 %78, 10
  br i1 %80, label %then, label %if_cont

then:                                             ; preds = %cond
  %81 = call i64 @mod.28(i64 %78, i64 10)
  %82 = sdiv i64 %78, 10
  %.elt266 = getelementptr inbounds { { i8*, i64 }*, i64 }, { { i8*, i64 }*, i64 }* %table, i64 0, i32 0
  %.elt270 = getelementptr inbounds { i8*, i64 }, { i8*, i64 }* %77, i64 %81, i32 0
  %.unpack271274276 = load i8*, i8** %.elt270, align 8
  %83 = load i8, i8* %.unpack271274276, align 1
  %84 = sext i32 %79 to i64
  %parith4 = getelementptr i8, i8* %75, i64 %84
  store i8 %83, i8* %parith4, align 1
  %85 = add i32 %79, 1
  br label %cond

if_cont:                                          ; preds = %cond
  %86 = call i64 @mod.29(i64 %78, i64 10)
  %87 = sdiv i64 %78, 10
  %.elt = getelementptr inbounds { { i8*, i64 }*, i64 }, { { i8*, i64 }*, i64 }* %table, i64 0, i32 0
  %.elt251 = getelementptr inbounds { i8*, i64 }, { i8*, i64 }* %77, i64 %86, i32 0
  %.unpack252255257 = load i8*, i8** %.elt251, align 8
  %88 = load i8, i8* %.unpack252255257, align 1
  %89 = sext i32 %79 to i64
  %parith9 = getelementptr i8, i8* %75, i64 %89
  store i8 %88, i8* %parith9, align 1
  br label %cond10

cond10:                                           ; preds = %then11, %if_cont
  %90 = phi i32 [ %95, %then11 ], [ %79, %if_cont ]
  %91 = icmp sgt i32 %90, -1
  br i1 %91, label %then11, label %if_cont12

then11:                                           ; preds = %cond10
  %92 = sext i32 %90 to i64
  %parith13 = getelementptr i8, i8* %75, i64 %92
  %93 = load i8, i8* %parith13, align 1
  %94 = call {} @putchar(i8 %93)
  %95 = add i32 %90, -1
  br label %cond10

if_cont12:                                        ; preds = %cond10
  ret {} zeroinitializer
}

define i64 @mod.28(i64, i64) {
entry:
  %2 = srem i64 %0, %1
  ret i64 %2
}

define i64 @mod.29(i64, i64) {
entry:
  %2 = srem i64 %0, %1
  ret i64 %2
}

define {} @print.32({ i8*, i64 }) {
entry:
  %1 = extractvalue { i8*, i64 } %0, 0
  %2 = extractvalue { i8*, i64 } %0, 1
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @printfFmtStr.33, i64 0, i64 0), i64 %2, i8* %1)
  ret {} zeroinitializer
}

define {} @print.34(%55) {
entry:
  %local13989_ = alloca i16, align 8
  %tmpcast = bitcast i16* %local13989_ to %62*
  %local14854_ = alloca i16, align 8
  %tmpcast59 = bitcast i16* %local14854_ to %63*
  %local15330_ = alloca i16, align 8
  %tmpcast49 = bitcast i16* %local15330_ to %64*
  %local16480_ = alloca i16, align 8
  %tmpcast56 = bitcast i16* %local16480_ to %65*
  %1 = call i8* @malloc(i64 1)
  store i8 123, i8* %1, align 1
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @printfFmtStr.35, i64 0, i64 0), i64 1, i8* %1)
  %3 = call i8* @malloc(i64 1)
  store i8 120, i8* %3, align 1
  %4 = insertvalue { i8*, i64 } undef, i8* %3, 0
  %5 = insertvalue { i8*, i64 } %4, i64 1, 1
  %6 = call {} @print.36({ i8*, i64 } %5)
  %.repack = bitcast i16* %local13989_ to i8*
  store i8 58, i8* %.repack, align 8
  %7 = getelementptr inbounds %62, %62* %tmpcast, i64 0, i32 1
  store i8 32, i8* %7, align 1
  %8 = call i8* @malloc(i64 2)
  %9 = load i16, i16* %local13989_, align 8
  %10 = bitcast i8* %8 to i16*
  store i16 %9, i16* %10, align 2
  %11 = insertvalue { i8*, i64 } undef, i8* %8, 0
  %12 = insertvalue { i8*, i64 } %11, i64 2, 1
  %13 = call {} @print.38({ i8*, i64 } %12)
  %14 = extractvalue %55 %0, 0
  %15 = call {} @print.40(i64 %14)
  %.repack57 = bitcast i16* %local14854_ to i8*
  store i8 44, i8* %.repack57, align 8
  %16 = getelementptr inbounds %63, %63* %tmpcast59, i64 0, i32 1
  store i8 32, i8* %16, align 1
  %17 = call i8* @malloc(i64 2)
  %18 = load i16, i16* %local14854_, align 8
  %19 = bitcast i8* %17 to i16*
  store i16 %18, i16* %19, align 2
  %20 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @printfFmtStr.44, i64 0, i64 0), i64 2, i8* %17)
  %21 = call i8* @malloc(i64 1)
  store i8 121, i8* %21, align 1
  %22 = insertvalue { i8*, i64 } undef, i8* %21, 0
  %23 = insertvalue { i8*, i64 } %22, i64 1, 1
  %24 = call {} @print.45({ i8*, i64 } %23)
  %.repack47 = bitcast i16* %local15330_ to i8*
  store i8 58, i8* %.repack47, align 8
  %25 = getelementptr inbounds %64, %64* %tmpcast49, i64 0, i32 1
  store i8 32, i8* %25, align 1
  %26 = call i8* @malloc(i64 2)
  %27 = load i16, i16* %local15330_, align 8
  %28 = bitcast i8* %26 to i16*
  store i16 %27, i16* %28, align 2
  %29 = insertvalue { i8*, i64 } undef, i8* %26, 0
  %30 = insertvalue { i8*, i64 } %29, i64 2, 1
  %31 = call {} @print.47({ i8*, i64 } %30)
  %32 = extractvalue %55 %0, 1
  %33 = call {} @print.49(i64* %32)
  %34 = call i8* @malloc(i64 1)
  store i8 125, i8* %34, align 1
  %35 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @printfFmtStr.56, i64 0, i64 0), i64 1, i8* %34)
  ret {} zeroinitializer
}

define {} @print.36({ i8*, i64 }) {
entry:
  %1 = extractvalue { i8*, i64 } %0, 0
  %2 = extractvalue { i8*, i64 } %0, 1
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @printfFmtStr.37, i64 0, i64 0), i64 %2, i8* %1)
  ret {} zeroinitializer
}

define {} @print.38({ i8*, i64 }) {
entry:
  %1 = extractvalue { i8*, i64 } %0, 0
  %2 = extractvalue { i8*, i64 } %0, 1
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @printfFmtStr.39, i64 0, i64 0), i64 %2, i8* %1)
  ret {} zeroinitializer
}

define {} @print.40(i64) {
entry:
  %1 = call {} @printInt.41(i64 %0)
  ret {} zeroinitializer
}

define {} @printInt.41(i64) {
entry:
  %table = alloca { { i8*, i64 }*, i64 }, align 8
  %b = alloca { i8*, i64 }, align 8
  %local14656_ = alloca %66, align 8
  %local14667_ = alloca %66*, align 8
  %1 = call i8* @malloc(i64 1)
  store i8 48, i8* %1, align 1
  %2 = call i8* @malloc(i64 1)
  store i8 49, i8* %2, align 1
  %3 = call i8* @malloc(i64 1)
  store i8 50, i8* %3, align 1
  %4 = call i8* @malloc(i64 1)
  store i8 51, i8* %4, align 1
  %5 = call i8* @malloc(i64 1)
  store i8 52, i8* %5, align 1
  %6 = call i8* @malloc(i64 1)
  store i8 53, i8* %6, align 1
  %7 = call i8* @malloc(i64 1)
  store i8 54, i8* %7, align 1
  %8 = call i8* @malloc(i64 1)
  store i8 55, i8* %8, align 1
  %9 = call i8* @malloc(i64 1)
  store i8 56, i8* %9, align 1
  %10 = call i8* @malloc(i64 1)
  store i8 57, i8* %10, align 1
  %.repack.repack = getelementptr inbounds %66, %66* %local14656_, i64 0, i32 0, i32 0
  store i8* %1, i8** %.repack.repack, align 8
  %11 = getelementptr inbounds %66, %66* %local14656_, i64 0, i32 0, i32 1
  store i64 1, i64* %11, align 8
  %12 = getelementptr inbounds %66, %66* %local14656_, i64 0, i32 1, i32 0
  store i8* %2, i8** %12, align 8
  %13 = getelementptr inbounds %66, %66* %local14656_, i64 0, i32 1, i32 1
  store i64 1, i64* %13, align 8
  %14 = getelementptr inbounds %66, %66* %local14656_, i64 0, i32 2, i32 0
  store i8* %3, i8** %14, align 8
  %15 = getelementptr inbounds %66, %66* %local14656_, i64 0, i32 2, i32 1
  store i64 1, i64* %15, align 8
  %16 = getelementptr inbounds %66, %66* %local14656_, i64 0, i32 3, i32 0
  store i8* %4, i8** %16, align 8
  %17 = getelementptr inbounds %66, %66* %local14656_, i64 0, i32 3, i32 1
  store i64 1, i64* %17, align 8
  %18 = getelementptr inbounds %66, %66* %local14656_, i64 0, i32 4, i32 0
  store i8* %5, i8** %18, align 8
  %19 = getelementptr inbounds %66, %66* %local14656_, i64 0, i32 4, i32 1
  store i64 1, i64* %19, align 8
  %20 = getelementptr inbounds %66, %66* %local14656_, i64 0, i32 5, i32 0
  store i8* %6, i8** %20, align 8
  %21 = getelementptr inbounds %66, %66* %local14656_, i64 0, i32 5, i32 1
  store i64 1, i64* %21, align 8
  %22 = getelementptr inbounds %66, %66* %local14656_, i64 0, i32 6, i32 0
  store i8* %7, i8** %22, align 8
  %23 = getelementptr inbounds %66, %66* %local14656_, i64 0, i32 6, i32 1
  store i64 1, i64* %23, align 8
  %24 = getelementptr inbounds %66, %66* %local14656_, i64 0, i32 7, i32 0
  store i8* %8, i8** %24, align 8
  %25 = getelementptr inbounds %66, %66* %local14656_, i64 0, i32 7, i32 1
  store i64 1, i64* %25, align 8
  %26 = getelementptr inbounds %66, %66* %local14656_, i64 0, i32 8, i32 0
  store i8* %9, i8** %26, align 8
  %27 = getelementptr inbounds %66, %66* %local14656_, i64 0, i32 8, i32 1
  store i64 1, i64* %27, align 8
  %28 = getelementptr inbounds %66, %66* %local14656_, i64 0, i32 9, i32 0
  store i8* %10, i8** %28, align 8
  %29 = getelementptr inbounds %66, %66* %local14656_, i64 0, i32 9, i32 1
  store i64 1, i64* %29, align 8
  %30 = bitcast %66* %local14656_ to i64*
  %31 = ptrtoint i8* %1 to i64
  %32 = bitcast i8** %12 to i64*
  %33 = ptrtoint i8* %2 to i64
  %34 = bitcast i8** %14 to i64*
  %35 = ptrtoint i8* %3 to i64
  %36 = bitcast i8** %16 to i64*
  %37 = ptrtoint i8* %4 to i64
  %38 = bitcast i8** %18 to i64*
  %39 = ptrtoint i8* %5 to i64
  %40 = bitcast i8** %20 to i64*
  %41 = ptrtoint i8* %6 to i64
  %42 = bitcast i8** %22 to i64*
  %43 = ptrtoint i8* %7 to i64
  %44 = bitcast i8** %24 to i64*
  %45 = ptrtoint i8* %8 to i64
  %46 = bitcast i8** %26 to i64*
  %47 = ptrtoint i8* %9 to i64
  %48 = bitcast i8** %28 to i64*
  %49 = ptrtoint i8* %10 to i64
  store i64 %31, i64* %30, align 8
  store i64 1, i64* %11, align 8
  store i64 %33, i64* %32, align 8
  store i64 1, i64* %13, align 8
  store i64 %35, i64* %34, align 8
  store i64 1, i64* %15, align 8
  store i64 %37, i64* %36, align 8
  store i64 1, i64* %17, align 8
  store i64 %39, i64* %38, align 8
  store i64 1, i64* %19, align 8
  store i64 %41, i64* %40, align 8
  store i64 1, i64* %21, align 8
  store i64 %43, i64* %42, align 8
  store i64 1, i64* %23, align 8
  store i64 %45, i64* %44, align 8
  store i64 1, i64* %25, align 8
  store i64 %47, i64* %46, align 8
  store i64 1, i64* %27, align 8
  store i64 %49, i64* %48, align 8
  store i64 1, i64* %29, align 8
  %50 = call i8* @malloc(i64 160)
  %51 = bitcast %66** %local14667_ to i8**
  store i8* %50, i8** %51, align 8
  %.unpack.unpack245 = load i64, i64* %30, align 8
  %.unpack.unpack195 = load i64, i64* %11, align 8
  %.unpack150.unpack244 = load i64, i64* %32, align 8
  %.unpack150.unpack192 = load i64, i64* %13, align 8
  %.unpack152.unpack243 = load i64, i64* %34, align 8
  %.unpack152.unpack189 = load i64, i64* %15, align 8
  %.unpack154.unpack242 = load i64, i64* %36, align 8
  %.unpack154.unpack186 = load i64, i64* %17, align 8
  %.unpack156.unpack241 = load i64, i64* %38, align 8
  %.unpack156.unpack183 = load i64, i64* %19, align 8
  %.unpack158.unpack240 = load i64, i64* %40, align 8
  %.unpack158.unpack180 = load i64, i64* %21, align 8
  %.unpack160.unpack239 = load i64, i64* %42, align 8
  %.unpack160.unpack177 = load i64, i64* %23, align 8
  %.unpack162.unpack238 = load i64, i64* %44, align 8
  %.unpack162.unpack174 = load i64, i64* %25, align 8
  %.unpack164.unpack237 = load i64, i64* %46, align 8
  %.unpack164.unpack171 = load i64, i64* %27, align 8
  %.unpack166.unpack236 = load i64, i64* %48, align 8
  %.unpack166.unpack168 = load i64, i64* %29, align 8
  %52 = bitcast i8* %50 to i64*
  store i64 %.unpack.unpack245, i64* %52, align 8
  %.repack.repack234 = getelementptr inbounds i8, i8* %50, i64 8
  %53 = bitcast i8* %.repack.repack234 to i64*
  store i64 %.unpack.unpack195, i64* %53, align 8
  %.repack197.repack = getelementptr inbounds i8, i8* %50, i64 16
  %54 = bitcast i8* %.repack197.repack to i64*
  store i64 %.unpack150.unpack244, i64* %54, align 8
  %.repack197.repack231 = getelementptr inbounds i8, i8* %50, i64 24
  %55 = bitcast i8* %.repack197.repack231 to i64*
  store i64 %.unpack150.unpack192, i64* %55, align 8
  %.repack199.repack = getelementptr inbounds i8, i8* %50, i64 32
  %56 = bitcast i8* %.repack199.repack to i64*
  store i64 %.unpack152.unpack243, i64* %56, align 8
  %.repack199.repack229 = getelementptr inbounds i8, i8* %50, i64 40
  %57 = bitcast i8* %.repack199.repack229 to i64*
  store i64 %.unpack152.unpack189, i64* %57, align 8
  %.repack201.repack = getelementptr inbounds i8, i8* %50, i64 48
  %58 = bitcast i8* %.repack201.repack to i64*
  store i64 %.unpack154.unpack242, i64* %58, align 8
  %.repack201.repack227 = getelementptr inbounds i8, i8* %50, i64 56
  %59 = bitcast i8* %.repack201.repack227 to i64*
  store i64 %.unpack154.unpack186, i64* %59, align 8
  %.repack203.repack = getelementptr inbounds i8, i8* %50, i64 64
  %60 = bitcast i8* %.repack203.repack to i64*
  store i64 %.unpack156.unpack241, i64* %60, align 8
  %.repack203.repack225 = getelementptr inbounds i8, i8* %50, i64 72
  %61 = bitcast i8* %.repack203.repack225 to i64*
  store i64 %.unpack156.unpack183, i64* %61, align 8
  %.repack205.repack = getelementptr inbounds i8, i8* %50, i64 80
  %62 = bitcast i8* %.repack205.repack to i64*
  store i64 %.unpack158.unpack240, i64* %62, align 8
  %.repack205.repack223 = getelementptr inbounds i8, i8* %50, i64 88
  %63 = bitcast i8* %.repack205.repack223 to i64*
  store i64 %.unpack158.unpack180, i64* %63, align 8
  %.repack207.repack = getelementptr inbounds i8, i8* %50, i64 96
  %64 = bitcast i8* %.repack207.repack to i64*
  store i64 %.unpack160.unpack239, i64* %64, align 8
  %.repack207.repack221 = getelementptr inbounds i8, i8* %50, i64 104
  %65 = bitcast i8* %.repack207.repack221 to i64*
  store i64 %.unpack160.unpack177, i64* %65, align 8
  %.repack209.repack = getelementptr inbounds i8, i8* %50, i64 112
  %66 = bitcast i8* %.repack209.repack to i64*
  store i64 %.unpack162.unpack238, i64* %66, align 8
  %.repack209.repack219 = getelementptr inbounds i8, i8* %50, i64 120
  %67 = bitcast i8* %.repack209.repack219 to i64*
  store i64 %.unpack162.unpack174, i64* %67, align 8
  %.repack211.repack = getelementptr inbounds i8, i8* %50, i64 128
  %68 = bitcast i8* %.repack211.repack to i64*
  store i64 %.unpack164.unpack237, i64* %68, align 8
  %.repack211.repack217 = getelementptr inbounds i8, i8* %50, i64 136
  %69 = bitcast i8* %.repack211.repack217 to i64*
  store i64 %.unpack164.unpack171, i64* %69, align 8
  %.repack213.repack = getelementptr inbounds i8, i8* %50, i64 144
  %70 = bitcast i8* %.repack213.repack to i64*
  store i64 %.unpack166.unpack236, i64* %70, align 8
  %.repack213.repack215 = getelementptr inbounds i8, i8* %50, i64 152
  %71 = bitcast i8* %.repack213.repack215 to i64*
  store i64 %.unpack166.unpack168, i64* %71, align 8
  %72 = bitcast %66** %local14667_ to i64*
  %73 = ptrtoint i8* %50 to i64
  %74 = bitcast { { i8*, i64 }*, i64 }* %table to i64*
  store i64 %73, i64* %74, align 8
  %table.repack246 = getelementptr inbounds { { i8*, i64 }*, i64 }, { { i8*, i64 }*, i64 }* %table, i64 0, i32 1
  store i64 10, i64* %table.repack246, align 8
  %b.repack = getelementptr inbounds { i8*, i64 }, { i8*, i64 }* %b, i64 0, i32 0
  store i8* null, i8** %b.repack, align 8
  %b.repack248 = getelementptr inbounds { i8*, i64 }, { i8*, i64 }* %b, i64 0, i32 1
  store i64 0, i64* %b.repack248, align 8
  %75 = call i8* @malloc(i64 20)
  %.cast = ptrtoint i8* %75 to i64
  %76 = bitcast { i8*, i64 }* %b to i64*
  store i64 %.cast, i64* %76, align 8
  %77 = inttoptr i64 %73 to { i8*, i64 }*
  br label %cond

cond:                                             ; preds = %then, %entry
  %78 = phi i64 [ %82, %then ], [ %0, %entry ]
  %79 = phi i32 [ %85, %then ], [ 0, %entry ]
  %80 = icmp sgt i64 %78, 10
  br i1 %80, label %then, label %if_cont

then:                                             ; preds = %cond
  %81 = call i64 @mod.42(i64 %78, i64 10)
  %82 = sdiv i64 %78, 10
  %.elt266 = getelementptr inbounds { { i8*, i64 }*, i64 }, { { i8*, i64 }*, i64 }* %table, i64 0, i32 0
  %.elt270 = getelementptr inbounds { i8*, i64 }, { i8*, i64 }* %77, i64 %81, i32 0
  %.unpack271274276 = load i8*, i8** %.elt270, align 8
  %83 = load i8, i8* %.unpack271274276, align 1
  %84 = sext i32 %79 to i64
  %parith4 = getelementptr i8, i8* %75, i64 %84
  store i8 %83, i8* %parith4, align 1
  %85 = add i32 %79, 1
  br label %cond

if_cont:                                          ; preds = %cond
  %86 = call i64 @mod.43(i64 %78, i64 10)
  %87 = sdiv i64 %78, 10
  %.elt = getelementptr inbounds { { i8*, i64 }*, i64 }, { { i8*, i64 }*, i64 }* %table, i64 0, i32 0
  %.elt251 = getelementptr inbounds { i8*, i64 }, { i8*, i64 }* %77, i64 %86, i32 0
  %.unpack252255257 = load i8*, i8** %.elt251, align 8
  %88 = load i8, i8* %.unpack252255257, align 1
  %89 = sext i32 %79 to i64
  %parith9 = getelementptr i8, i8* %75, i64 %89
  store i8 %88, i8* %parith9, align 1
  br label %cond10

cond10:                                           ; preds = %then11, %if_cont
  %90 = phi i32 [ %95, %then11 ], [ %79, %if_cont ]
  %91 = icmp sgt i32 %90, -1
  br i1 %91, label %then11, label %if_cont12

then11:                                           ; preds = %cond10
  %92 = sext i32 %90 to i64
  %parith13 = getelementptr i8, i8* %75, i64 %92
  %93 = load i8, i8* %parith13, align 1
  %94 = call {} @putchar(i8 %93)
  %95 = add i32 %90, -1
  br label %cond10

if_cont12:                                        ; preds = %cond10
  ret {} zeroinitializer
}

define i64 @mod.42(i64, i64) {
entry:
  %2 = srem i64 %0, %1
  ret i64 %2
}

define i64 @mod.43(i64, i64) {
entry:
  %2 = srem i64 %0, %1
  ret i64 %2
}

define {} @print.45({ i8*, i64 }) {
entry:
  %1 = extractvalue { i8*, i64 } %0, 0
  %2 = extractvalue { i8*, i64 } %0, 1
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @printfFmtStr.46, i64 0, i64 0), i64 %2, i8* %1)
  ret {} zeroinitializer
}

define {} @print.47({ i8*, i64 }) {
entry:
  %1 = extractvalue { i8*, i64 } %0, 0
  %2 = extractvalue { i8*, i64 } %0, 1
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @printfFmtStr.48, i64 0, i64 0), i64 %2, i8* %1)
  ret {} zeroinitializer
}

define {} @print.49(i64*) {
entry:
  %local15621_ = alloca i16, align 8
  %tmpcast = bitcast i16* %local15621_ to %67*
  %.repack = bitcast i16* %local15621_ to i8*
  store i8 42, i8* %.repack, align 8
  %1 = getelementptr inbounds %67, %67* %tmpcast, i64 0, i32 1
  store i8 40, i8* %1, align 1
  %2 = call i8* @malloc(i64 2)
  %3 = load i16, i16* %local15621_, align 8
  %4 = bitcast i8* %2 to i16*
  store i16 %3, i16* %4, align 2
  %5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @printfFmtStr.50, i64 0, i64 0), i64 2, i8* %2)
  %6 = ptrtoint i64* %0 to i64
  %7 = call {} @printHex(i64 %6)
  %8 = call i8* @malloc(i64 1)
  store i8 41, i8* %8, align 1
  %9 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @printfFmtStr.54, i64 0, i64 0), i64 1, i8* %8)
  ret {} zeroinitializer
}

define {} @printHex(i64) {
entry:
  %table = alloca { { i8*, i64 }*, i64 }, align 8
  %b = alloca { i8*, i64 }, align 8
  %local16212_ = alloca %68, align 8
  %local16229_ = alloca %68*, align 8
  %local16395_ = alloca i16, align 8
  %tmpcast = bitcast i16* %local16395_ to %69*
  %1 = call i8* @malloc(i64 1)
  store i8 48, i8* %1, align 1
  %2 = call i8* @malloc(i64 1)
  store i8 49, i8* %2, align 1
  %3 = call i8* @malloc(i64 1)
  store i8 50, i8* %3, align 1
  %4 = call i8* @malloc(i64 1)
  store i8 51, i8* %4, align 1
  %5 = call i8* @malloc(i64 1)
  store i8 52, i8* %5, align 1
  %6 = call i8* @malloc(i64 1)
  store i8 53, i8* %6, align 1
  %7 = call i8* @malloc(i64 1)
  store i8 54, i8* %7, align 1
  %8 = call i8* @malloc(i64 1)
  store i8 55, i8* %8, align 1
  %9 = call i8* @malloc(i64 1)
  store i8 56, i8* %9, align 1
  %10 = call i8* @malloc(i64 1)
  store i8 57, i8* %10, align 1
  %11 = call i8* @malloc(i64 1)
  store i8 97, i8* %11, align 1
  %12 = call i8* @malloc(i64 1)
  store i8 98, i8* %12, align 1
  %13 = call i8* @malloc(i64 1)
  store i8 99, i8* %13, align 1
  %14 = call i8* @malloc(i64 1)
  store i8 100, i8* %14, align 1
  %15 = call i8* @malloc(i64 1)
  store i8 101, i8* %15, align 1
  %16 = call i8* @malloc(i64 1)
  store i8 102, i8* %16, align 1
  %.repack.repack = getelementptr inbounds %68, %68* %local16212_, i64 0, i32 0, i32 0
  store i8* %1, i8** %.repack.repack, align 8
  %17 = getelementptr inbounds %68, %68* %local16212_, i64 0, i32 0, i32 1
  store i64 1, i64* %17, align 8
  %18 = getelementptr inbounds %68, %68* %local16212_, i64 0, i32 1, i32 0
  store i8* %2, i8** %18, align 8
  %19 = getelementptr inbounds %68, %68* %local16212_, i64 0, i32 1, i32 1
  store i64 1, i64* %19, align 8
  %20 = getelementptr inbounds %68, %68* %local16212_, i64 0, i32 2, i32 0
  store i8* %3, i8** %20, align 8
  %21 = getelementptr inbounds %68, %68* %local16212_, i64 0, i32 2, i32 1
  store i64 1, i64* %21, align 8
  %22 = getelementptr inbounds %68, %68* %local16212_, i64 0, i32 3, i32 0
  store i8* %4, i8** %22, align 8
  %23 = getelementptr inbounds %68, %68* %local16212_, i64 0, i32 3, i32 1
  store i64 1, i64* %23, align 8
  %24 = getelementptr inbounds %68, %68* %local16212_, i64 0, i32 4, i32 0
  store i8* %5, i8** %24, align 8
  %25 = getelementptr inbounds %68, %68* %local16212_, i64 0, i32 4, i32 1
  store i64 1, i64* %25, align 8
  %26 = getelementptr inbounds %68, %68* %local16212_, i64 0, i32 5, i32 0
  store i8* %6, i8** %26, align 8
  %27 = getelementptr inbounds %68, %68* %local16212_, i64 0, i32 5, i32 1
  store i64 1, i64* %27, align 8
  %28 = getelementptr inbounds %68, %68* %local16212_, i64 0, i32 6, i32 0
  store i8* %7, i8** %28, align 8
  %29 = getelementptr inbounds %68, %68* %local16212_, i64 0, i32 6, i32 1
  store i64 1, i64* %29, align 8
  %30 = getelementptr inbounds %68, %68* %local16212_, i64 0, i32 7, i32 0
  store i8* %8, i8** %30, align 8
  %31 = getelementptr inbounds %68, %68* %local16212_, i64 0, i32 7, i32 1
  store i64 1, i64* %31, align 8
  %32 = getelementptr inbounds %68, %68* %local16212_, i64 0, i32 8, i32 0
  store i8* %9, i8** %32, align 8
  %33 = getelementptr inbounds %68, %68* %local16212_, i64 0, i32 8, i32 1
  store i64 1, i64* %33, align 8
  %34 = getelementptr inbounds %68, %68* %local16212_, i64 0, i32 9, i32 0
  store i8* %10, i8** %34, align 8
  %35 = getelementptr inbounds %68, %68* %local16212_, i64 0, i32 9, i32 1
  store i64 1, i64* %35, align 8
  %36 = getelementptr inbounds %68, %68* %local16212_, i64 0, i32 10, i32 0
  store i8* %11, i8** %36, align 8
  %37 = getelementptr inbounds %68, %68* %local16212_, i64 0, i32 10, i32 1
  store i64 1, i64* %37, align 8
  %38 = getelementptr inbounds %68, %68* %local16212_, i64 0, i32 11, i32 0
  store i8* %12, i8** %38, align 8
  %39 = getelementptr inbounds %68, %68* %local16212_, i64 0, i32 11, i32 1
  store i64 1, i64* %39, align 8
  %40 = getelementptr inbounds %68, %68* %local16212_, i64 0, i32 12, i32 0
  store i8* %13, i8** %40, align 8
  %41 = getelementptr inbounds %68, %68* %local16212_, i64 0, i32 12, i32 1
  store i64 1, i64* %41, align 8
  %42 = getelementptr inbounds %68, %68* %local16212_, i64 0, i32 13, i32 0
  store i8* %14, i8** %42, align 8
  %43 = getelementptr inbounds %68, %68* %local16212_, i64 0, i32 13, i32 1
  store i64 1, i64* %43, align 8
  %44 = getelementptr inbounds %68, %68* %local16212_, i64 0, i32 14, i32 0
  store i8* %15, i8** %44, align 8
  %45 = getelementptr inbounds %68, %68* %local16212_, i64 0, i32 14, i32 1
  store i64 1, i64* %45, align 8
  %46 = getelementptr inbounds %68, %68* %local16212_, i64 0, i32 15, i32 0
  store i8* %16, i8** %46, align 8
  %47 = getelementptr inbounds %68, %68* %local16212_, i64 0, i32 15, i32 1
  store i64 1, i64* %47, align 8
  %48 = bitcast %68* %local16212_ to i64*
  %49 = ptrtoint i8* %1 to i64
  %50 = bitcast i8** %18 to i64*
  %51 = ptrtoint i8* %2 to i64
  %52 = bitcast i8** %20 to i64*
  %53 = ptrtoint i8* %3 to i64
  %54 = bitcast i8** %22 to i64*
  %55 = ptrtoint i8* %4 to i64
  %56 = bitcast i8** %24 to i64*
  %57 = ptrtoint i8* %5 to i64
  %58 = bitcast i8** %26 to i64*
  %59 = ptrtoint i8* %6 to i64
  %60 = bitcast i8** %28 to i64*
  %61 = ptrtoint i8* %7 to i64
  %62 = bitcast i8** %30 to i64*
  %63 = ptrtoint i8* %8 to i64
  %64 = bitcast i8** %32 to i64*
  %65 = ptrtoint i8* %9 to i64
  %66 = bitcast i8** %34 to i64*
  %67 = ptrtoint i8* %10 to i64
  %68 = bitcast i8** %36 to i64*
  %69 = ptrtoint i8* %11 to i64
  %70 = bitcast i8** %38 to i64*
  %71 = ptrtoint i8* %12 to i64
  %72 = bitcast i8** %40 to i64*
  %73 = ptrtoint i8* %13 to i64
  %74 = bitcast i8** %42 to i64*
  %75 = ptrtoint i8* %14 to i64
  %76 = bitcast i8** %44 to i64*
  %77 = ptrtoint i8* %15 to i64
  %78 = bitcast i8** %46 to i64*
  %79 = ptrtoint i8* %16 to i64
  store i64 %49, i64* %48, align 8
  store i64 1, i64* %17, align 8
  store i64 %51, i64* %50, align 8
  store i64 1, i64* %19, align 8
  store i64 %53, i64* %52, align 8
  store i64 1, i64* %21, align 8
  store i64 %55, i64* %54, align 8
  store i64 1, i64* %23, align 8
  store i64 %57, i64* %56, align 8
  store i64 1, i64* %25, align 8
  store i64 %59, i64* %58, align 8
  store i64 1, i64* %27, align 8
  store i64 %61, i64* %60, align 8
  store i64 1, i64* %29, align 8
  store i64 %63, i64* %62, align 8
  store i64 1, i64* %31, align 8
  store i64 %65, i64* %64, align 8
  store i64 1, i64* %33, align 8
  store i64 %67, i64* %66, align 8
  store i64 1, i64* %35, align 8
  store i64 %69, i64* %68, align 8
  store i64 1, i64* %37, align 8
  store i64 %71, i64* %70, align 8
  store i64 1, i64* %39, align 8
  store i64 %73, i64* %72, align 8
  store i64 1, i64* %41, align 8
  store i64 %75, i64* %74, align 8
  store i64 1, i64* %43, align 8
  store i64 %77, i64* %76, align 8
  store i64 1, i64* %45, align 8
  store i64 %79, i64* %78, align 8
  store i64 1, i64* %47, align 8
  %80 = call i8* @malloc(i64 256)
  %81 = bitcast %68** %local16229_ to i8**
  store i8* %80, i8** %81, align 8
  %.unpack.unpack389 = load i64, i64* %48, align 8
  %.unpack.unpack309 = load i64, i64* %17, align 8
  %.unpack234.unpack388 = load i64, i64* %50, align 8
  %.unpack234.unpack306 = load i64, i64* %19, align 8
  %.unpack236.unpack387 = load i64, i64* %52, align 8
  %.unpack236.unpack303 = load i64, i64* %21, align 8
  %.unpack238.unpack386 = load i64, i64* %54, align 8
  %.unpack238.unpack300 = load i64, i64* %23, align 8
  %.unpack240.unpack385 = load i64, i64* %56, align 8
  %.unpack240.unpack297 = load i64, i64* %25, align 8
  %.unpack242.unpack384 = load i64, i64* %58, align 8
  %.unpack242.unpack294 = load i64, i64* %27, align 8
  %.unpack244.unpack383 = load i64, i64* %60, align 8
  %.unpack244.unpack291 = load i64, i64* %29, align 8
  %.unpack246.unpack382 = load i64, i64* %62, align 8
  %.unpack246.unpack288 = load i64, i64* %31, align 8
  %.unpack248.unpack381 = load i64, i64* %64, align 8
  %.unpack248.unpack285 = load i64, i64* %33, align 8
  %.unpack250.unpack380 = load i64, i64* %66, align 8
  %.unpack250.unpack282 = load i64, i64* %35, align 8
  %.unpack252.unpack379 = load i64, i64* %68, align 8
  %.unpack252.unpack279 = load i64, i64* %37, align 8
  %.unpack254.unpack378 = load i64, i64* %70, align 8
  %.unpack254.unpack276 = load i64, i64* %39, align 8
  %.unpack256.unpack377 = load i64, i64* %72, align 8
  %.unpack256.unpack273 = load i64, i64* %41, align 8
  %.unpack258.unpack376 = load i64, i64* %74, align 8
  %.unpack258.unpack270 = load i64, i64* %43, align 8
  %.unpack260.unpack375 = load i64, i64* %76, align 8
  %.unpack260.unpack267 = load i64, i64* %45, align 8
  %.unpack262.unpack374 = load i64, i64* %78, align 8
  %.unpack262.unpack264 = load i64, i64* %47, align 8
  %82 = bitcast i8* %80 to i64*
  store i64 %.unpack.unpack389, i64* %82, align 8
  %.repack.repack372 = getelementptr inbounds i8, i8* %80, i64 8
  %83 = bitcast i8* %.repack.repack372 to i64*
  store i64 %.unpack.unpack309, i64* %83, align 8
  %.repack311.repack = getelementptr inbounds i8, i8* %80, i64 16
  %84 = bitcast i8* %.repack311.repack to i64*
  store i64 %.unpack234.unpack388, i64* %84, align 8
  %.repack311.repack369 = getelementptr inbounds i8, i8* %80, i64 24
  %85 = bitcast i8* %.repack311.repack369 to i64*
  store i64 %.unpack234.unpack306, i64* %85, align 8
  %.repack313.repack = getelementptr inbounds i8, i8* %80, i64 32
  %86 = bitcast i8* %.repack313.repack to i64*
  store i64 %.unpack236.unpack387, i64* %86, align 8
  %.repack313.repack367 = getelementptr inbounds i8, i8* %80, i64 40
  %87 = bitcast i8* %.repack313.repack367 to i64*
  store i64 %.unpack236.unpack303, i64* %87, align 8
  %.repack315.repack = getelementptr inbounds i8, i8* %80, i64 48
  %88 = bitcast i8* %.repack315.repack to i64*
  store i64 %.unpack238.unpack386, i64* %88, align 8
  %.repack315.repack365 = getelementptr inbounds i8, i8* %80, i64 56
  %89 = bitcast i8* %.repack315.repack365 to i64*
  store i64 %.unpack238.unpack300, i64* %89, align 8
  %.repack317.repack = getelementptr inbounds i8, i8* %80, i64 64
  %90 = bitcast i8* %.repack317.repack to i64*
  store i64 %.unpack240.unpack385, i64* %90, align 8
  %.repack317.repack363 = getelementptr inbounds i8, i8* %80, i64 72
  %91 = bitcast i8* %.repack317.repack363 to i64*
  store i64 %.unpack240.unpack297, i64* %91, align 8
  %.repack319.repack = getelementptr inbounds i8, i8* %80, i64 80
  %92 = bitcast i8* %.repack319.repack to i64*
  store i64 %.unpack242.unpack384, i64* %92, align 8
  %.repack319.repack361 = getelementptr inbounds i8, i8* %80, i64 88
  %93 = bitcast i8* %.repack319.repack361 to i64*
  store i64 %.unpack242.unpack294, i64* %93, align 8
  %.repack321.repack = getelementptr inbounds i8, i8* %80, i64 96
  %94 = bitcast i8* %.repack321.repack to i64*
  store i64 %.unpack244.unpack383, i64* %94, align 8
  %.repack321.repack359 = getelementptr inbounds i8, i8* %80, i64 104
  %95 = bitcast i8* %.repack321.repack359 to i64*
  store i64 %.unpack244.unpack291, i64* %95, align 8
  %.repack323.repack = getelementptr inbounds i8, i8* %80, i64 112
  %96 = bitcast i8* %.repack323.repack to i64*
  store i64 %.unpack246.unpack382, i64* %96, align 8
  %.repack323.repack357 = getelementptr inbounds i8, i8* %80, i64 120
  %97 = bitcast i8* %.repack323.repack357 to i64*
  store i64 %.unpack246.unpack288, i64* %97, align 8
  %.repack325.repack = getelementptr inbounds i8, i8* %80, i64 128
  %98 = bitcast i8* %.repack325.repack to i64*
  store i64 %.unpack248.unpack381, i64* %98, align 8
  %.repack325.repack355 = getelementptr inbounds i8, i8* %80, i64 136
  %99 = bitcast i8* %.repack325.repack355 to i64*
  store i64 %.unpack248.unpack285, i64* %99, align 8
  %.repack327.repack = getelementptr inbounds i8, i8* %80, i64 144
  %100 = bitcast i8* %.repack327.repack to i64*
  store i64 %.unpack250.unpack380, i64* %100, align 8
  %.repack327.repack353 = getelementptr inbounds i8, i8* %80, i64 152
  %101 = bitcast i8* %.repack327.repack353 to i64*
  store i64 %.unpack250.unpack282, i64* %101, align 8
  %.repack329.repack = getelementptr inbounds i8, i8* %80, i64 160
  %102 = bitcast i8* %.repack329.repack to i64*
  store i64 %.unpack252.unpack379, i64* %102, align 8
  %.repack329.repack351 = getelementptr inbounds i8, i8* %80, i64 168
  %103 = bitcast i8* %.repack329.repack351 to i64*
  store i64 %.unpack252.unpack279, i64* %103, align 8
  %.repack331.repack = getelementptr inbounds i8, i8* %80, i64 176
  %104 = bitcast i8* %.repack331.repack to i64*
  store i64 %.unpack254.unpack378, i64* %104, align 8
  %.repack331.repack349 = getelementptr inbounds i8, i8* %80, i64 184
  %105 = bitcast i8* %.repack331.repack349 to i64*
  store i64 %.unpack254.unpack276, i64* %105, align 8
  %.repack333.repack = getelementptr inbounds i8, i8* %80, i64 192
  %106 = bitcast i8* %.repack333.repack to i64*
  store i64 %.unpack256.unpack377, i64* %106, align 8
  %.repack333.repack347 = getelementptr inbounds i8, i8* %80, i64 200
  %107 = bitcast i8* %.repack333.repack347 to i64*
  store i64 %.unpack256.unpack273, i64* %107, align 8
  %.repack335.repack = getelementptr inbounds i8, i8* %80, i64 208
  %108 = bitcast i8* %.repack335.repack to i64*
  store i64 %.unpack258.unpack376, i64* %108, align 8
  %.repack335.repack345 = getelementptr inbounds i8, i8* %80, i64 216
  %109 = bitcast i8* %.repack335.repack345 to i64*
  store i64 %.unpack258.unpack270, i64* %109, align 8
  %.repack337.repack = getelementptr inbounds i8, i8* %80, i64 224
  %110 = bitcast i8* %.repack337.repack to i64*
  store i64 %.unpack260.unpack375, i64* %110, align 8
  %.repack337.repack343 = getelementptr inbounds i8, i8* %80, i64 232
  %111 = bitcast i8* %.repack337.repack343 to i64*
  store i64 %.unpack260.unpack267, i64* %111, align 8
  %.repack339.repack = getelementptr inbounds i8, i8* %80, i64 240
  %112 = bitcast i8* %.repack339.repack to i64*
  store i64 %.unpack262.unpack374, i64* %112, align 8
  %.repack339.repack341 = getelementptr inbounds i8, i8* %80, i64 248
  %113 = bitcast i8* %.repack339.repack341 to i64*
  store i64 %.unpack262.unpack264, i64* %113, align 8
  %114 = bitcast %68** %local16229_ to i64*
  %115 = load i64, i64* %114, align 8
  %116 = bitcast { { i8*, i64 }*, i64 }* %table to i64*
  store i64 %115, i64* %116, align 8
  %table.repack390 = getelementptr inbounds { { i8*, i64 }*, i64 }, { { i8*, i64 }*, i64 }* %table, i64 0, i32 1
  store i64 16, i64* %table.repack390, align 8
  %b.repack = getelementptr inbounds { i8*, i64 }, { i8*, i64 }* %b, i64 0, i32 0
  store i8* null, i8** %b.repack, align 8
  %b.repack392 = getelementptr inbounds { i8*, i64 }, { i8*, i64 }* %b, i64 0, i32 1
  store i64 0, i64* %b.repack392, align 8
  %117 = call i8* @malloc(i64 20)
  %.cast = ptrtoint i8* %117 to i64
  %118 = bitcast { i8*, i64 }* %b to i64*
  store i64 %.cast, i64* %118, align 8
  %119 = inttoptr i64 %115 to { i8*, i64 }*
  br label %cond

cond:                                             ; preds = %then, %entry
  %120 = phi i64 [ %124, %then ], [ %0, %entry ]
  %121 = phi i32 [ %127, %then ], [ 0, %entry ]
  %122 = icmp sgt i64 %120, 16
  br i1 %122, label %then, label %if_cont

then:                                             ; preds = %cond
  %123 = call i64 @mod.51(i64 %120, i64 16)
  %124 = sdiv i64 %120, 16
  %.elt411 = getelementptr inbounds { { i8*, i64 }*, i64 }, { { i8*, i64 }*, i64 }* %table, i64 0, i32 0
  %.elt415 = getelementptr inbounds { i8*, i64 }, { i8*, i64 }* %119, i64 %123, i32 0
  %.unpack416419421 = load i8*, i8** %.elt415, align 8
  %125 = load i8, i8* %.unpack416419421, align 1
  %126 = sext i32 %121 to i64
  %parith4 = getelementptr i8, i8* %117, i64 %126
  store i8 %125, i8* %parith4, align 1
  %127 = add i32 %121, 1
  br label %cond

if_cont:                                          ; preds = %cond
  %128 = call i64 @mod.52(i64 %120, i64 16)
  %129 = sdiv i64 %120, 16
  %.elt = getelementptr inbounds { { i8*, i64 }*, i64 }, { { i8*, i64 }*, i64 }* %table, i64 0, i32 0
  %.elt395 = getelementptr inbounds { i8*, i64 }, { i8*, i64 }* %119, i64 %128, i32 0
  %.unpack396399401 = load i8*, i8** %.elt395, align 8
  %130 = load i8, i8* %.unpack396399401, align 1
  %131 = sext i32 %121 to i64
  %parith9 = getelementptr i8, i8* %117, i64 %131
  store i8 %130, i8* %parith9, align 1
  %.repack = bitcast i16* %local16395_ to i8*
  store i8 48, i8* %.repack, align 8
  %132 = getelementptr inbounds %69, %69* %tmpcast, i64 0, i32 1
  store i8 120, i8* %132, align 1
  %133 = call i8* @malloc(i64 2)
  %134 = load i16, i16* %local16395_, align 8
  %135 = bitcast i8* %133 to i16*
  store i16 %134, i16* %135, align 2
  %136 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @printfFmtStr.53, i64 0, i64 0), i64 2, i8* %133)
  br label %cond10

cond10:                                           ; preds = %then11, %if_cont
  %137 = phi i32 [ %142, %then11 ], [ %121, %if_cont ]
  %138 = icmp sgt i32 %137, -1
  br i1 %138, label %then11, label %if_cont12

then11:                                           ; preds = %cond10
  %139 = sext i32 %137 to i64
  %parith13 = getelementptr i8, i8* %117, i64 %139
  %140 = load i8, i8* %parith13, align 1
  %141 = call {} @putchar(i8 %140)
  %142 = add i32 %137, -1
  br label %cond10

if_cont12:                                        ; preds = %cond10
  ret {} zeroinitializer
}

define i64 @mod.51(i64, i64) {
entry:
  %2 = srem i64 %0, %1
  ret i64 %2
}

define i64 @mod.52(i64, i64) {
entry:
  %2 = srem i64 %0, %1
  ret i64 %2
}

define {} @print.57({ i8*, i64 }) {
entry:
  %1 = extractvalue { i8*, i64 } %0, 0
  %2 = extractvalue { i8*, i64 } %0, 1
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @printfFmtStr.58, i64 0, i64 0), i64 %2, i8* %1)
  ret {} zeroinitializer
}

define {} @print.59({ i64, [8 x i8] }) {
entry:
  %local17701_ = alloca i16, align 8
  %tmpcast82 = bitcast i16* %local17701_ to %70*
  %local19028_ = alloca i16, align 8
  %tmpcast = bitcast i16* %local19028_ to %71*
  %1 = call i8* @malloc(i64 1)
  store i8 123, i8* %1, align 1
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @printfFmtStr.60, i64 0, i64 0), i64 1, i8* %1)
  %3 = extractvalue { i64, [8 x i8] } %0, 0
  switch i64 %3, label %if_cont42 [
    i64 1, label %then32
    i64 2, label %then40
  ]

then32:                                           ; preds = %entry
  %4 = call i8* @malloc(i64 1)
  store i8 120, i8* %4, align 1
  %5 = insertvalue { i8*, i64 } undef, i8* %4, 0
  %6 = insertvalue { i8*, i64 } %5, i64 1, 1
  %7 = call {} @print.61({ i8*, i64 } %6)
  %.repack80 = bitcast i16* %local17701_ to i8*
  store i8 58, i8* %.repack80, align 8
  %8 = getelementptr inbounds %70, %70* %tmpcast82, i64 0, i32 1
  store i8 32, i8* %8, align 1
  %9 = call i8* @malloc(i64 2)
  %10 = load i16, i16* %local17701_, align 8
  %11 = bitcast i8* %9 to i16*
  store i16 %10, i16* %11, align 2
  %12 = insertvalue { i8*, i64 } undef, i8* %9, 0
  %13 = insertvalue { i8*, i64 } %12, i64 2, 1
  %14 = call {} @print.63({ i8*, i64 } %13)
  %15 = extractvalue { i64, [8 x i8] } %0, 1
  %dumbcast = alloca i32, align 4
  %16 = bitcast i32* %dumbcast to [8 x i8]*
  %.repack89 = bitcast i32* %dumbcast to i8*
  %.elt90 = extractvalue [8 x i8] %15, 0
  store i8 %.elt90, i8* %.repack89, align 4
  %.repack91 = getelementptr inbounds [8 x i8], [8 x i8]* %16, i64 0, i64 1
  %.elt92 = extractvalue [8 x i8] %15, 1
  store i8 %.elt92, i8* %.repack91, align 1
  %.repack93 = getelementptr inbounds [8 x i8], [8 x i8]* %16, i64 0, i64 2
  %.elt94 = extractvalue [8 x i8] %15, 2
  store i8 %.elt94, i8* %.repack93, align 2
  %.repack95 = getelementptr inbounds [8 x i8], [8 x i8]* %16, i64 0, i64 3
  %.elt96 = extractvalue [8 x i8] %15, 3
  store i8 %.elt96, i8* %.repack95, align 1
  %.repack97 = getelementptr inbounds i32, i32* %dumbcast, i64 1
  %17 = bitcast i32* %.repack97 to i8*
  %.elt98 = extractvalue [8 x i8] %15, 4
  store i8 %.elt98, i8* %17, align 4
  %.repack99 = getelementptr inbounds [8 x i8], [8 x i8]* %16, i64 0, i64 5
  %.elt100 = extractvalue [8 x i8] %15, 5
  store i8 %.elt100, i8* %.repack99, align 1
  %.repack101 = getelementptr inbounds [8 x i8], [8 x i8]* %16, i64 0, i64 6
  %.elt102 = extractvalue [8 x i8] %15, 6
  store i8 %.elt102, i8* %.repack101, align 2
  %.repack103 = getelementptr inbounds [8 x i8], [8 x i8]* %16, i64 0, i64 7
  %.elt104 = extractvalue [8 x i8] %15, 7
  store i8 %.elt104, i8* %.repack103, align 1
  %18 = load i32, i32* %dumbcast, align 4
  %19 = call {} @print.65(i32 %18)
  br label %if_cont42

then40:                                           ; preds = %entry
  %20 = call i8* @malloc(i64 1)
  store i8 121, i8* %20, align 1
  %21 = insertvalue { i8*, i64 } undef, i8* %20, 0
  %22 = insertvalue { i8*, i64 } %21, i64 1, 1
  %23 = call {} @print.69({ i8*, i64 } %22)
  %.repack = bitcast i16* %local19028_ to i8*
  store i8 58, i8* %.repack, align 8
  %24 = getelementptr inbounds %71, %71* %tmpcast, i64 0, i32 1
  store i8 32, i8* %24, align 1
  %25 = call i8* @malloc(i64 2)
  %26 = load i16, i16* %local19028_, align 8
  %27 = bitcast i8* %25 to i16*
  store i16 %26, i16* %27, align 2
  %28 = insertvalue { i8*, i64 } undef, i8* %25, 0
  %29 = insertvalue { i8*, i64 } %28, i64 2, 1
  %30 = call {} @print.71({ i8*, i64 } %29)
  %31 = extractvalue { i64, [8 x i8] } %0, 1
  %dumbcast48 = alloca i1, align 1
  %.repack59 = bitcast i1* %dumbcast48 to i8*
  %.elt = extractvalue [8 x i8] %31, 0
  store i8 %.elt, i8* %.repack59, align 1
  %.repack60 = getelementptr inbounds i1, i1* %dumbcast48, i64 1
  %32 = bitcast i1* %.repack60 to i8*
  %.elt61 = extractvalue [8 x i8] %31, 1
  store i8 %.elt61, i8* %32, align 1
  %.repack62 = getelementptr inbounds i1, i1* %dumbcast48, i64 2
  %33 = bitcast i1* %.repack62 to i8*
  %.elt63 = extractvalue [8 x i8] %31, 2
  store i8 %.elt63, i8* %33, align 1
  %.repack64 = getelementptr inbounds i1, i1* %dumbcast48, i64 3
  %34 = bitcast i1* %.repack64 to i8*
  %.elt65 = extractvalue [8 x i8] %31, 3
  store i8 %.elt65, i8* %34, align 1
  %.repack66 = getelementptr inbounds i1, i1* %dumbcast48, i64 4
  %35 = bitcast i1* %.repack66 to i8*
  %.elt67 = extractvalue [8 x i8] %31, 4
  store i8 %.elt67, i8* %35, align 1
  %.repack68 = getelementptr inbounds i1, i1* %dumbcast48, i64 5
  %36 = bitcast i1* %.repack68 to i8*
  %.elt69 = extractvalue [8 x i8] %31, 5
  store i8 %.elt69, i8* %36, align 1
  %.repack70 = getelementptr inbounds i1, i1* %dumbcast48, i64 6
  %37 = bitcast i1* %.repack70 to i8*
  %.elt71 = extractvalue [8 x i8] %31, 6
  store i8 %.elt71, i8* %37, align 1
  %.repack72 = getelementptr inbounds i1, i1* %dumbcast48, i64 7
  %38 = bitcast i1* %.repack72 to i8*
  %.elt73 = extractvalue [8 x i8] %31, 7
  store i8 %.elt73, i8* %38, align 1
  %39 = trunc i8 %.elt to i1
  %40 = call {} @print.73(i1 %39)
  br label %if_cont42

if_cont42:                                        ; preds = %then32, %entry, %then40
  %41 = call i8* @malloc(i64 1)
  store i8 125, i8* %41, align 1
  %42 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @printfFmtStr.75, i64 0, i64 0), i64 1, i8* %41)
  ret {} zeroinitializer
}

define {} @print.61({ i8*, i64 }) {
entry:
  %1 = extractvalue { i8*, i64 } %0, 0
  %2 = extractvalue { i8*, i64 } %0, 1
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @printfFmtStr.62, i64 0, i64 0), i64 %2, i8* %1)
  ret {} zeroinitializer
}

define {} @print.63({ i8*, i64 }) {
entry:
  %1 = extractvalue { i8*, i64 } %0, 0
  %2 = extractvalue { i8*, i64 } %0, 1
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @printfFmtStr.64, i64 0, i64 0), i64 %2, i8* %1)
  ret {} zeroinitializer
}

define {} @print.65(i32) {
entry:
  %1 = call {} @printInt.66(i32 %0)
  ret {} zeroinitializer
}

define {} @printInt.66(i32) {
entry:
  %table = alloca { { i8*, i64 }*, i64 }, align 8
  %b = alloca { i8*, i64 }, align 8
  %local18378_ = alloca %72, align 8
  %local18389_ = alloca %72*, align 8
  %1 = call i8* @malloc(i64 1)
  store i8 48, i8* %1, align 1
  %2 = call i8* @malloc(i64 1)
  store i8 49, i8* %2, align 1
  %3 = call i8* @malloc(i64 1)
  store i8 50, i8* %3, align 1
  %4 = call i8* @malloc(i64 1)
  store i8 51, i8* %4, align 1
  %5 = call i8* @malloc(i64 1)
  store i8 52, i8* %5, align 1
  %6 = call i8* @malloc(i64 1)
  store i8 53, i8* %6, align 1
  %7 = call i8* @malloc(i64 1)
  store i8 54, i8* %7, align 1
  %8 = call i8* @malloc(i64 1)
  store i8 55, i8* %8, align 1
  %9 = call i8* @malloc(i64 1)
  store i8 56, i8* %9, align 1
  %10 = call i8* @malloc(i64 1)
  store i8 57, i8* %10, align 1
  %.repack.repack = getelementptr inbounds %72, %72* %local18378_, i64 0, i32 0, i32 0
  store i8* %1, i8** %.repack.repack, align 8
  %11 = getelementptr inbounds %72, %72* %local18378_, i64 0, i32 0, i32 1
  store i64 1, i64* %11, align 8
  %12 = getelementptr inbounds %72, %72* %local18378_, i64 0, i32 1, i32 0
  store i8* %2, i8** %12, align 8
  %13 = getelementptr inbounds %72, %72* %local18378_, i64 0, i32 1, i32 1
  store i64 1, i64* %13, align 8
  %14 = getelementptr inbounds %72, %72* %local18378_, i64 0, i32 2, i32 0
  store i8* %3, i8** %14, align 8
  %15 = getelementptr inbounds %72, %72* %local18378_, i64 0, i32 2, i32 1
  store i64 1, i64* %15, align 8
  %16 = getelementptr inbounds %72, %72* %local18378_, i64 0, i32 3, i32 0
  store i8* %4, i8** %16, align 8
  %17 = getelementptr inbounds %72, %72* %local18378_, i64 0, i32 3, i32 1
  store i64 1, i64* %17, align 8
  %18 = getelementptr inbounds %72, %72* %local18378_, i64 0, i32 4, i32 0
  store i8* %5, i8** %18, align 8
  %19 = getelementptr inbounds %72, %72* %local18378_, i64 0, i32 4, i32 1
  store i64 1, i64* %19, align 8
  %20 = getelementptr inbounds %72, %72* %local18378_, i64 0, i32 5, i32 0
  store i8* %6, i8** %20, align 8
  %21 = getelementptr inbounds %72, %72* %local18378_, i64 0, i32 5, i32 1
  store i64 1, i64* %21, align 8
  %22 = getelementptr inbounds %72, %72* %local18378_, i64 0, i32 6, i32 0
  store i8* %7, i8** %22, align 8
  %23 = getelementptr inbounds %72, %72* %local18378_, i64 0, i32 6, i32 1
  store i64 1, i64* %23, align 8
  %24 = getelementptr inbounds %72, %72* %local18378_, i64 0, i32 7, i32 0
  store i8* %8, i8** %24, align 8
  %25 = getelementptr inbounds %72, %72* %local18378_, i64 0, i32 7, i32 1
  store i64 1, i64* %25, align 8
  %26 = getelementptr inbounds %72, %72* %local18378_, i64 0, i32 8, i32 0
  store i8* %9, i8** %26, align 8
  %27 = getelementptr inbounds %72, %72* %local18378_, i64 0, i32 8, i32 1
  store i64 1, i64* %27, align 8
  %28 = getelementptr inbounds %72, %72* %local18378_, i64 0, i32 9, i32 0
  store i8* %10, i8** %28, align 8
  %29 = getelementptr inbounds %72, %72* %local18378_, i64 0, i32 9, i32 1
  store i64 1, i64* %29, align 8
  %30 = bitcast %72* %local18378_ to i64*
  %31 = ptrtoint i8* %1 to i64
  %32 = bitcast i8** %12 to i64*
  %33 = ptrtoint i8* %2 to i64
  %34 = bitcast i8** %14 to i64*
  %35 = ptrtoint i8* %3 to i64
  %36 = bitcast i8** %16 to i64*
  %37 = ptrtoint i8* %4 to i64
  %38 = bitcast i8** %18 to i64*
  %39 = ptrtoint i8* %5 to i64
  %40 = bitcast i8** %20 to i64*
  %41 = ptrtoint i8* %6 to i64
  %42 = bitcast i8** %22 to i64*
  %43 = ptrtoint i8* %7 to i64
  %44 = bitcast i8** %24 to i64*
  %45 = ptrtoint i8* %8 to i64
  %46 = bitcast i8** %26 to i64*
  %47 = ptrtoint i8* %9 to i64
  %48 = bitcast i8** %28 to i64*
  %49 = ptrtoint i8* %10 to i64
  store i64 %31, i64* %30, align 8
  store i64 1, i64* %11, align 8
  store i64 %33, i64* %32, align 8
  store i64 1, i64* %13, align 8
  store i64 %35, i64* %34, align 8
  store i64 1, i64* %15, align 8
  store i64 %37, i64* %36, align 8
  store i64 1, i64* %17, align 8
  store i64 %39, i64* %38, align 8
  store i64 1, i64* %19, align 8
  store i64 %41, i64* %40, align 8
  store i64 1, i64* %21, align 8
  store i64 %43, i64* %42, align 8
  store i64 1, i64* %23, align 8
  store i64 %45, i64* %44, align 8
  store i64 1, i64* %25, align 8
  store i64 %47, i64* %46, align 8
  store i64 1, i64* %27, align 8
  store i64 %49, i64* %48, align 8
  store i64 1, i64* %29, align 8
  %50 = call i8* @malloc(i64 160)
  %51 = bitcast %72** %local18389_ to i8**
  store i8* %50, i8** %51, align 8
  %.unpack.unpack245 = load i64, i64* %30, align 8
  %.unpack.unpack195 = load i64, i64* %11, align 8
  %.unpack150.unpack244 = load i64, i64* %32, align 8
  %.unpack150.unpack192 = load i64, i64* %13, align 8
  %.unpack152.unpack243 = load i64, i64* %34, align 8
  %.unpack152.unpack189 = load i64, i64* %15, align 8
  %.unpack154.unpack242 = load i64, i64* %36, align 8
  %.unpack154.unpack186 = load i64, i64* %17, align 8
  %.unpack156.unpack241 = load i64, i64* %38, align 8
  %.unpack156.unpack183 = load i64, i64* %19, align 8
  %.unpack158.unpack240 = load i64, i64* %40, align 8
  %.unpack158.unpack180 = load i64, i64* %21, align 8
  %.unpack160.unpack239 = load i64, i64* %42, align 8
  %.unpack160.unpack177 = load i64, i64* %23, align 8
  %.unpack162.unpack238 = load i64, i64* %44, align 8
  %.unpack162.unpack174 = load i64, i64* %25, align 8
  %.unpack164.unpack237 = load i64, i64* %46, align 8
  %.unpack164.unpack171 = load i64, i64* %27, align 8
  %.unpack166.unpack236 = load i64, i64* %48, align 8
  %.unpack166.unpack168 = load i64, i64* %29, align 8
  %52 = bitcast i8* %50 to i64*
  store i64 %.unpack.unpack245, i64* %52, align 8
  %.repack.repack234 = getelementptr inbounds i8, i8* %50, i64 8
  %53 = bitcast i8* %.repack.repack234 to i64*
  store i64 %.unpack.unpack195, i64* %53, align 8
  %.repack197.repack = getelementptr inbounds i8, i8* %50, i64 16
  %54 = bitcast i8* %.repack197.repack to i64*
  store i64 %.unpack150.unpack244, i64* %54, align 8
  %.repack197.repack231 = getelementptr inbounds i8, i8* %50, i64 24
  %55 = bitcast i8* %.repack197.repack231 to i64*
  store i64 %.unpack150.unpack192, i64* %55, align 8
  %.repack199.repack = getelementptr inbounds i8, i8* %50, i64 32
  %56 = bitcast i8* %.repack199.repack to i64*
  store i64 %.unpack152.unpack243, i64* %56, align 8
  %.repack199.repack229 = getelementptr inbounds i8, i8* %50, i64 40
  %57 = bitcast i8* %.repack199.repack229 to i64*
  store i64 %.unpack152.unpack189, i64* %57, align 8
  %.repack201.repack = getelementptr inbounds i8, i8* %50, i64 48
  %58 = bitcast i8* %.repack201.repack to i64*
  store i64 %.unpack154.unpack242, i64* %58, align 8
  %.repack201.repack227 = getelementptr inbounds i8, i8* %50, i64 56
  %59 = bitcast i8* %.repack201.repack227 to i64*
  store i64 %.unpack154.unpack186, i64* %59, align 8
  %.repack203.repack = getelementptr inbounds i8, i8* %50, i64 64
  %60 = bitcast i8* %.repack203.repack to i64*
  store i64 %.unpack156.unpack241, i64* %60, align 8
  %.repack203.repack225 = getelementptr inbounds i8, i8* %50, i64 72
  %61 = bitcast i8* %.repack203.repack225 to i64*
  store i64 %.unpack156.unpack183, i64* %61, align 8
  %.repack205.repack = getelementptr inbounds i8, i8* %50, i64 80
  %62 = bitcast i8* %.repack205.repack to i64*
  store i64 %.unpack158.unpack240, i64* %62, align 8
  %.repack205.repack223 = getelementptr inbounds i8, i8* %50, i64 88
  %63 = bitcast i8* %.repack205.repack223 to i64*
  store i64 %.unpack158.unpack180, i64* %63, align 8
  %.repack207.repack = getelementptr inbounds i8, i8* %50, i64 96
  %64 = bitcast i8* %.repack207.repack to i64*
  store i64 %.unpack160.unpack239, i64* %64, align 8
  %.repack207.repack221 = getelementptr inbounds i8, i8* %50, i64 104
  %65 = bitcast i8* %.repack207.repack221 to i64*
  store i64 %.unpack160.unpack177, i64* %65, align 8
  %.repack209.repack = getelementptr inbounds i8, i8* %50, i64 112
  %66 = bitcast i8* %.repack209.repack to i64*
  store i64 %.unpack162.unpack238, i64* %66, align 8
  %.repack209.repack219 = getelementptr inbounds i8, i8* %50, i64 120
  %67 = bitcast i8* %.repack209.repack219 to i64*
  store i64 %.unpack162.unpack174, i64* %67, align 8
  %.repack211.repack = getelementptr inbounds i8, i8* %50, i64 128
  %68 = bitcast i8* %.repack211.repack to i64*
  store i64 %.unpack164.unpack237, i64* %68, align 8
  %.repack211.repack217 = getelementptr inbounds i8, i8* %50, i64 136
  %69 = bitcast i8* %.repack211.repack217 to i64*
  store i64 %.unpack164.unpack171, i64* %69, align 8
  %.repack213.repack = getelementptr inbounds i8, i8* %50, i64 144
  %70 = bitcast i8* %.repack213.repack to i64*
  store i64 %.unpack166.unpack236, i64* %70, align 8
  %.repack213.repack215 = getelementptr inbounds i8, i8* %50, i64 152
  %71 = bitcast i8* %.repack213.repack215 to i64*
  store i64 %.unpack166.unpack168, i64* %71, align 8
  %72 = bitcast %72** %local18389_ to i64*
  %73 = ptrtoint i8* %50 to i64
  %74 = bitcast { { i8*, i64 }*, i64 }* %table to i64*
  store i64 %73, i64* %74, align 8
  %table.repack246 = getelementptr inbounds { { i8*, i64 }*, i64 }, { { i8*, i64 }*, i64 }* %table, i64 0, i32 1
  store i64 10, i64* %table.repack246, align 8
  %b.repack = getelementptr inbounds { i8*, i64 }, { i8*, i64 }* %b, i64 0, i32 0
  store i8* null, i8** %b.repack, align 8
  %b.repack248 = getelementptr inbounds { i8*, i64 }, { i8*, i64 }* %b, i64 0, i32 1
  store i64 0, i64* %b.repack248, align 8
  %75 = call i8* @malloc(i64 20)
  %.cast = ptrtoint i8* %75 to i64
  %76 = bitcast { i8*, i64 }* %b to i64*
  store i64 %.cast, i64* %76, align 8
  %77 = inttoptr i64 %73 to { i8*, i64 }*
  br label %cond

cond:                                             ; preds = %then, %entry
  %78 = phi i32 [ %82, %then ], [ %0, %entry ]
  %79 = phi i32 [ %86, %then ], [ 0, %entry ]
  %80 = icmp sgt i32 %78, 10
  br i1 %80, label %then, label %if_cont

then:                                             ; preds = %cond
  %81 = call i32 @mod.67(i32 %78, i32 10)
  %82 = sdiv i32 %78, 10
  %.elt266 = getelementptr inbounds { { i8*, i64 }*, i64 }, { { i8*, i64 }*, i64 }* %table, i64 0, i32 0
  %83 = sext i32 %81 to i64
  %.elt270 = getelementptr inbounds { i8*, i64 }, { i8*, i64 }* %77, i64 %83, i32 0
  %.unpack271274276 = load i8*, i8** %.elt270, align 8
  %84 = load i8, i8* %.unpack271274276, align 1
  %85 = sext i32 %79 to i64
  %parith4 = getelementptr i8, i8* %75, i64 %85
  store i8 %84, i8* %parith4, align 1
  %86 = add i32 %79, 1
  br label %cond

if_cont:                                          ; preds = %cond
  %87 = call i32 @mod.68(i32 %78, i32 10)
  %88 = sdiv i32 %78, 10
  %.elt = getelementptr inbounds { { i8*, i64 }*, i64 }, { { i8*, i64 }*, i64 }* %table, i64 0, i32 0
  %89 = sext i32 %87 to i64
  %.elt251 = getelementptr inbounds { i8*, i64 }, { i8*, i64 }* %77, i64 %89, i32 0
  %.unpack252255257 = load i8*, i8** %.elt251, align 8
  %90 = load i8, i8* %.unpack252255257, align 1
  %91 = sext i32 %79 to i64
  %parith9 = getelementptr i8, i8* %75, i64 %91
  store i8 %90, i8* %parith9, align 1
  br label %cond10

cond10:                                           ; preds = %then11, %if_cont
  %92 = phi i32 [ %97, %then11 ], [ %79, %if_cont ]
  %93 = icmp sgt i32 %92, -1
  br i1 %93, label %then11, label %if_cont12

then11:                                           ; preds = %cond10
  %94 = sext i32 %92 to i64
  %parith13 = getelementptr i8, i8* %75, i64 %94
  %95 = load i8, i8* %parith13, align 1
  %96 = call {} @putchar(i8 %95)
  %97 = add i32 %92, -1
  br label %cond10

if_cont12:                                        ; preds = %cond10
  ret {} zeroinitializer
}

define i32 @mod.67(i32, i32) {
entry:
  %2 = srem i32 %0, %1
  ret i32 %2
}

define i32 @mod.68(i32, i32) {
entry:
  %2 = srem i32 %0, %1
  ret i32 %2
}

define {} @print.69({ i8*, i64 }) {
entry:
  %1 = extractvalue { i8*, i64 } %0, 0
  %2 = extractvalue { i8*, i64 } %0, 1
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @printfFmtStr.70, i64 0, i64 0), i64 %2, i8* %1)
  ret {} zeroinitializer
}

define {} @print.71({ i8*, i64 }) {
entry:
  %1 = extractvalue { i8*, i64 } %0, 0
  %2 = extractvalue { i8*, i64 } %0, 1
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @printfFmtStr.72, i64 0, i64 0), i64 %2, i8* %1)
  ret {} zeroinitializer
}

define {} @print.73(i1) {
entry:
  %local19323_ = alloca %73, align 8
  %local19339_ = alloca %73*, align 8
  br i1 %0, label %if_cont18, label %else17

else17:                                           ; preds = %entry
  %.repack = getelementptr inbounds %73, %73* %local19323_, i64 0, i32 0
  store i8 102, i8* %.repack, align 8
  %1 = getelementptr inbounds %73, %73* %local19323_, i64 0, i32 1
  store i8 97, i8* %1, align 1
  %2 = getelementptr inbounds %73, %73* %local19323_, i64 0, i32 2
  store i8 108, i8* %2, align 2
  %3 = getelementptr inbounds %73, %73* %local19323_, i64 0, i32 3
  store i8 115, i8* %3, align 1
  %4 = getelementptr inbounds %73, %73* %local19323_, i64 0, i32 4
  store i8 101, i8* %4, align 4
  store i8 102, i8* %.repack, align 8
  store i8 97, i8* %1, align 1
  store i8 108, i8* %2, align 2
  store i8 115, i8* %3, align 1
  store i8 101, i8* %4, align 4
  %5 = call i8* @malloc(i64 5)
  %6 = bitcast %73** %local19339_ to i8**
  store i8* %5, i8** %6, align 8
  store i8 102, i8* %5, align 1
  %7 = getelementptr inbounds i8, i8* %5, i64 1
  store i8 97, i8* %7, align 1
  %8 = getelementptr inbounds i8, i8* %5, i64 2
  store i8 108, i8* %8, align 1
  %9 = getelementptr inbounds i8, i8* %5, i64 3
  store i8 115, i8* %9, align 1
  %10 = getelementptr inbounds i8, i8* %5, i64 4
  store i8 101, i8* %10, align 1
  %11 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @printfFmtStr.74, i64 0, i64 0), i64 5, i8* %5)
  br label %if_cont18

if_cont18:                                        ; preds = %entry, %else17
  ret {} zeroinitializer
}

define {} @print.76({ i8*, i64 }) {
entry:
  %1 = extractvalue { i8*, i64 } %0, 0
  %2 = extractvalue { i8*, i64 } %0, 1
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @printfFmtStr.77, i64 0, i64 0), i64 %2, i8* %1)
  ret {} zeroinitializer
}

!llvm.module.flags = !{!0, !1}

!0 = !{i32 2, !"Dwarf Version", i32 2}
!1 = !{i32 2, !"Debug Info Version", i32 3}
