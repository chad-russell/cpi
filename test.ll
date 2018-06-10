; ModuleID = 'module'
source_filename = "module"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-darwin17.5.0"

%0 = type { i64, i64 }
%1 = type { i64, i64, i64 }
%2 = type { i64, i64 }
%3 = type { %4, i64 }
%4 = type { i64, i64 }
%5 = type { i64, i64 }
%6 = type { i64, %6* }
%7 = type { i64, %7******* }
%8 = type { i32, %9* }
%9 = type { i32, %10 }
%10 = type { i32, %11** }
%11 = type { i32**, i32 ()** }
%12 = type { i32, %13** }
%13 = type { i32, %14 }
%14 = type { i32, %15***** }
%15 = type { i32, %12* }
%16 = type { i32, %17* }
%17 = type { i32, %18* }
%18 = type { i32, %19* }
%19 = type { i64** }
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
%35 = type { { i64*, i32 }, { i64*, i32 }, { i64*, i32 } }
%36 = type { i64, i64, i64, i64 }
%37 = type { i32, i32, i32, i32, i32 }
%38 = type { i32, i32, i32, i32, i32 }
%39 = type { i64, i64, i64 }
%40 = type { i64, i64, i64 }
%41 = type { i64, i64, i64, i64, i64 }
%42 = type { %43 }
%43 = type { { i8*, i32 }, { i32, [20 x i8] }* }
%44 = type { { i32, [20 x i8] }, i8* }
%45 = type { i8, i8, i8, i8 }
%46 = type { i64, { %47*, i32 } }
%47 = type { i32, i32 }

@0 = private unnamed_addr constant [20 x i8] c"assertion failed!!!\00"
@1 = private unnamed_addr constant [20 x i8] c"assertion failed!!!\00"
@2 = private unnamed_addr constant [20 x i8] c"assertion failed!!!\00"

declare void @panic(i8*)

declare i8* @malloc(i64)

declare i8* @memset(i8*, i64, i64)

declare void @free(i8*)

define i64 @main() !dbg !5 {
entry:
  %p = alloca %0, align 8, !dbg !9
  %local375_ = alloca i64, align 8, !dbg !9
  %local463_ = alloca %1, align 8, !dbg !9
  %testArr1 = alloca { i64*, i32 }, align 8, !dbg !9
  call void @llvm.dbg.value(metadata i64 ()* ()* ()* ()* @anon, i64 0, metadata !10, metadata !17), !dbg !18
  call void @llvm.dbg.value(metadata i64 ()* ()* ()* ()* @anon, i64 0, metadata !10, metadata !17), !dbg !18
  %0 = call i64 ()* ()* ()* @anon(), !dbg !19
  %1 = call i64 ()* ()* %0(), !dbg !19
  %2 = call i64 ()* %1(), !dbg !19
  %3 = call i64 %2(), !dbg !19
  %4 = icmp eq i64 %3, 3, !dbg !19
  call void @assert(i1 %4), !dbg !20
  %5 = call i64 @apply(i64 (i64)* nonnull @inc, i64 1), !dbg !21
  %6 = call i64 @apply_overboard(i64 (i64)* nonnull @inc, i64 0), !dbg !22
  %7 = add i64 %6, %5, !dbg !22
  %8 = icmp eq i64 %7, 3, !dbg !23
  call void @assert(i1 %8), !dbg !24
  call void @llvm.dbg.value(metadata i64 3, i64 0, metadata !25, metadata !17), !dbg !26
  call void @llvm.dbg.value(metadata i64 3, i64 0, metadata !25, metadata !17), !dbg !26
  call void @assert(i1 true), !dbg !27
  call void @llvm.dbg.value(metadata i64 3, i64 0, metadata !28, metadata !17), !dbg !29
  call void @llvm.dbg.value(metadata i64 3, i64 0, metadata !28, metadata !17), !dbg !29
  call void @assert(i1 true), !dbg !30
  %9 = call i64 @a5_fn(), !dbg !31
  call void @llvm.dbg.value(metadata i64 %9, i64 0, metadata !32, metadata !17), !dbg !33
  call void @llvm.dbg.value(metadata i64 %9, i64 0, metadata !32, metadata !17), !dbg !33
  %10 = icmp eq i64 %9, 3, !dbg !34
  call void @assert(i1 %10), !dbg !35
  %11 = call i64 @a5_fn(), !dbg !36
  call void @llvm.dbg.value(metadata i64 %11, i64 0, metadata !37, metadata !17), !dbg !38
  call void @llvm.dbg.value(metadata i64 %11, i64 0, metadata !37, metadata !17), !dbg !38
  %12 = icmp eq i64 %11, 3, !dbg !39
  call void @assert(i1 %12), !dbg !40
  %13 = call i64 @anon.4(), !dbg !41
  call void @llvm.dbg.value(metadata i64 %13, i64 0, metadata !42, metadata !17), !dbg !43
  call void @llvm.dbg.value(metadata i64 %13, i64 0, metadata !42, metadata !17), !dbg !43
  %14 = icmp eq i64 %13, 3, !dbg !44
  call void @assert(i1 %14), !dbg !45
  %15 = call i64 @bar(), !dbg !46
  call void @llvm.dbg.value(metadata i64 %15, i64 0, metadata !47, metadata !17), !dbg !48
  call void @llvm.dbg.value(metadata i64 %15, i64 0, metadata !47, metadata !17), !dbg !48
  %16 = icmp eq i64 %15, 3, !dbg !49
  call void @assert(i1 %16), !dbg !50
  %17 = call i64 @testA9(), !dbg !51
  call void @llvm.dbg.value(metadata i64 %17, i64 0, metadata !52, metadata !17), !dbg !53
  call void @llvm.dbg.value(metadata i64 %17, i64 0, metadata !52, metadata !17), !dbg !53
  %18 = icmp eq i64 %17, 3, !dbg !54
  call void @assert(i1 %18), !dbg !55
  %19 = call i64 @testStruct1(), !dbg !56
  call void @llvm.dbg.value(metadata i64 %19, i64 0, metadata !57, metadata !17), !dbg !58
  call void @llvm.dbg.value(metadata i64 %19, i64 0, metadata !57, metadata !17), !dbg !58
  %20 = icmp eq i64 %19, 3, !dbg !59
  call void @assert(i1 %20), !dbg !60
  %21 = call i64 @testStruct2(), !dbg !61
  call void @llvm.dbg.value(metadata i64 %21, i64 0, metadata !62, metadata !17), !dbg !63
  call void @llvm.dbg.value(metadata i64 %21, i64 0, metadata !62, metadata !17), !dbg !63
  %22 = icmp eq i64 %21, 3, !dbg !64
  call void @assert(i1 %22), !dbg !65
  call void @llvm.dbg.value(metadata i32 13, i64 0, metadata !66, metadata !17), !dbg !68
  %23 = call i64 @testNestedCall(), !dbg !69
  call void @llvm.dbg.value(metadata i64 %23, i64 0, metadata !70, metadata !17), !dbg !71
  call void @llvm.dbg.value(metadata i64 %23, i64 0, metadata !70, metadata !17), !dbg !71
  %24 = icmp eq i64 %23, 3, !dbg !72
  call void @assert(i1 %24), !dbg !73
  %25 = call i64 @testDerefParam(), !dbg !74
  call void @llvm.dbg.value(metadata i64 %25, i64 0, metadata !75, metadata !17), !dbg !76
  call void @llvm.dbg.value(metadata i64 %25, i64 0, metadata !75, metadata !17), !dbg !76
  %26 = icmp eq i64 %25, 3, !dbg !77
  call void @assert(i1 %26), !dbg !78
  call void @testBug1(), !dbg !79
  %27 = call i64 @testDoubleDeref(), !dbg !80
  call void @llvm.dbg.value(metadata i64 %27, i64 0, metadata !81, metadata !17), !dbg !82
  call void @llvm.dbg.value(metadata i64 %27, i64 0, metadata !81, metadata !17), !dbg !82
  %28 = icmp eq i64 %27, 3, !dbg !83
  call void @assert(i1 %28), !dbg !84
  %29 = call i64 @more_struct(), !dbg !85
  call void @llvm.dbg.value(metadata i64 %29, i64 0, metadata !86, metadata !17), !dbg !87
  call void @llvm.dbg.value(metadata i64 %29, i64 0, metadata !86, metadata !17), !dbg !87
  %30 = icmp eq i64 %29, 3, !dbg !88
  call void @assert(i1 %30), !dbg !89
  %31 = call i64 @test_S2_stuff(), !dbg !90
  call void @llvm.dbg.value(metadata i64 %31, i64 0, metadata !91, metadata !17), !dbg !92
  call void @llvm.dbg.value(metadata i64 %31, i64 0, metadata !91, metadata !17), !dbg !92
  %32 = icmp eq i64 %31, 3, !dbg !93
  call void @assert(i1 %32), !dbg !94
  %33 = call i64 @testSetFromParam(), !dbg !95
  call void @llvm.dbg.value(metadata i64 %33, i64 0, metadata !96, metadata !17), !dbg !97
  call void @llvm.dbg.value(metadata i64 %33, i64 0, metadata !96, metadata !17), !dbg !97
  %34 = icmp eq i64 %33, 3, !dbg !98
  call void @assert(i1 %34), !dbg !99
  %35 = call i64 @testSetter(), !dbg !100
  call void @llvm.dbg.value(metadata i64 %35, i64 0, metadata !101, metadata !17), !dbg !102
  call void @llvm.dbg.value(metadata i64 %35, i64 0, metadata !101, metadata !17), !dbg !102
  %36 = icmp eq i64 %35, 3, !dbg !103
  call void @assert(i1 %36), !dbg !104
  %37 = call i64 @testLinkedList(), !dbg !105
  call void @llvm.dbg.value(metadata i64 %37, i64 0, metadata !106, metadata !17), !dbg !107
  call void @llvm.dbg.value(metadata i64 %37, i64 0, metadata !106, metadata !17), !dbg !107
  %38 = icmp eq i64 %37, 3, !dbg !108
  call void @assert(i1 %38), !dbg !109
  %39 = call i64 @testWeirdLinkedList(), !dbg !110
  call void @llvm.dbg.value(metadata i64 %39, i64 0, metadata !111, metadata !17), !dbg !112
  call void @llvm.dbg.value(metadata i64 %39, i64 0, metadata !111, metadata !17), !dbg !112
  %40 = icmp eq i64 %39, 9, !dbg !113
  call void @assert(i1 %40), !dbg !114
  %41 = call i64 @testMoreDotsAndDerefs(), !dbg !115
  call void @llvm.dbg.value(metadata i64 %41, i64 0, metadata !116, metadata !17), !dbg !117
  call void @llvm.dbg.value(metadata i64 %41, i64 0, metadata !116, metadata !17), !dbg !117
  %42 = icmp eq i64 %41, 3, !dbg !118
  call void @assert(i1 %42), !dbg !119
  call void @test23(), !dbg !120
  call void @test24(), !dbg !121
  call void @test25(), !dbg !122
  %43 = call i64 @fib(i32 10), !dbg !123
  %44 = icmp eq i64 %43, 89, !dbg !124
  call void @assert(i1 %44), !dbg !125
  %45 = call i64 @stupid_sqrt(i64 5), !dbg !126
  %46 = icmp eq i64 %45, 0, !dbg !127
  call void @assert(i1 %46), !dbg !128
  %47 = call i64 @stupid_sqrt(i64 9), !dbg !129
  %48 = icmp eq i64 %47, 3, !dbg !130
  call void @assert(i1 %48), !dbg !131
  %49 = call i32 @id(i32 3), !dbg !132
  %50 = icmp eq i32 %49, 3, !dbg !133
  call void @assert(i1 %50), !dbg !134
  %51 = bitcast %0* %p to i8*, !dbg !135
  call void @llvm.memset.p0i8.i64(i8* %51, i8 0, i64 16, i32 8, i1 false), !dbg !135
  %52 = getelementptr inbounds %0, %0* %p, i64 0, i32 0, !dbg !136
  store i64 8, i64* %52, align 8, !dbg !137
  %.elt26 = getelementptr inbounds %0, %0* %p, i64 0, i32 1, !dbg !138
  call void @llvm.dbg.value(metadata %0 { i64 8, i64 0 }, i64 0, metadata !139, metadata !17), !dbg !135
  %53 = call %0 @id.6(%0 { i64 8, i64 0 }), !dbg !138
  %.elt = extractvalue %0 %53, 0, !dbg !138
  %54 = icmp eq i64 %.elt, 8, !dbg !141
  call void @assert(i1 %54), !dbg !142
  store i64 3, i64* %local375_, align 8, !dbg !143
  %55 = call i64 @deref_ct(i64* nonnull %local375_), !dbg !144
  %56 = icmp eq i64 %55, 3, !dbg !145
  call void @assert(i1 %56), !dbg !146
  %57 = call i32 @add_static(i32 5), !dbg !147
  %58 = icmp eq i32 %57, 12, !dbg !148
  call void @assert(i1 %58), !dbg !149
  %59 = call i32 @apply_static(i32 3), !dbg !150
  %60 = icmp eq i32 %59, 4, !dbg !151
  call void @assert(i1 %60), !dbg !152
  %61 = call i32 @apply_static.7(i32 3), !dbg !153
  %62 = icmp eq i32 %61, 6, !dbg !154
  call void @assert(i1 %62), !dbg !155
  call void @llvm.dbg.value(metadata %0 (%0)* @id.9, i64 0, metadata !156, metadata !17), !dbg !159
  call void @llvm.dbg.value(metadata %0 { i64 8, i64 0 }, i64 0, metadata !139, metadata !17), !dbg !135
  call void @llvm.dbg.value(metadata %0 (%0)* @id.9, i64 0, metadata !156, metadata !17), !dbg !159
  %63 = call %0 @id.9(%0 { i64 8, i64 0 }), !dbg !160
  %.elt33 = extractvalue %0 %63, 0, !dbg !160
  %64 = icmp eq i64 %.elt33, 8, !dbg !161
  call void @assert(i1 %64), !dbg !162
  call void @testArray1(), !dbg !163
  call void @testArray2(), !dbg !164
  call void @testArray3(), !dbg !165
  call void @testArray4(), !dbg !166
  call void @testArray5(), !dbg !167
  %.repack = getelementptr inbounds %1, %1* %local463_, i64 0, i32 0, !dbg !168
  store i64 1, i64* %.repack, align 8, !dbg !168
  %65 = getelementptr inbounds %1, %1* %local463_, i64 0, i32 1, !dbg !168
  store i64 2, i64* %65, align 8, !dbg !168
  %66 = getelementptr inbounds %1, %1* %local463_, i64 0, i32 2, !dbg !168
  store i64 3, i64* %66, align 8, !dbg !168
  store i64 1, i64* %.repack, align 8, !dbg !168
  store i64 2, i64* %65, align 8, !dbg !168
  store i64 3, i64* %66, align 8, !dbg !168
  %67 = insertvalue { %1*, i32 } zeroinitializer, %1* %local463_, 0, !dbg !169
  %68 = insertvalue { %1*, i32 } %67, i32 3, 1, !dbg !169
  %69 = bitcast { i64*, i32 }* %testArr1 to { %1*, i32 }*, !dbg !169
  store { %1*, i32 } %68, { %1*, i32 }* %69, align 8, !dbg !169
  %70 = getelementptr inbounds { i64*, i32 }, { i64*, i32 }* %testArr1, i64 0, i32 0, !dbg !9
  %71 = load i64*, i64** %70, align 8, !dbg !9
  call void @llvm.dbg.value(metadata { i64*, i32 }* %testArr1, i64 0, metadata !170, metadata !176), !dbg !177
  %parith = getelementptr i64, i64* %71, i64 1, !dbg !178
  %72 = load i64, i64* %parith, align 8, !dbg !178
  %73 = call i64 @inc(i64 %72), !dbg !179
  %74 = icmp eq i64 %73, 3, !dbg !180
  call void @assert(i1 %74), !dbg !181
  call void @testWhile1(), !dbg !182
  call void @assert(i1 true), !dbg !183
  call void @assert(i1 true), !dbg !184
  call void @assert(i1 true), !dbg !185
  call void @testMalloc(), !dbg !186
  call void @testFor1(), !dbg !187
  call void @testFor2(), !dbg !188
  call void @testFor3(), !dbg !189
  call void @testFor4(), !dbg !190
  call void @testUnion1(), !dbg !191
  call void @testAnySimple1(), !dbg !192
  call void @testBuffer(), !dbg !193
  ret i64 0, !dbg !194
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #0

define i64 ()* ()* ()* @anon() !dbg !195 {
entry:
  ret i64 ()* ()* ()* @anon.1, !dbg !196
}

define i64 ()* ()* @anon.1() !dbg !197 {
entry:
  ret i64 ()* ()* @foo, !dbg !198
}

define i64 ()* @foo() !dbg !199 {
entry:
  ret i64 ()* @bar, !dbg !200
}

define i64 @bar() !dbg !201 {
entry:
  ret i64 3, !dbg !202
}

define void @assert(i1) !dbg !203 {
entry:
  br i1 %0, label %if_cont, label %then, !dbg !208

then:                                             ; preds = %entry
  call void @panic(i8* getelementptr inbounds ([20 x i8], [20 x i8]* @0, i64 0, i64 0)), !dbg !209
  br label %if_cont, !dbg !209

if_cont:                                          ; preds = %entry, %then
  ret void, !dbg !209
}

define i64 @inc(i64) !dbg !210 {
entry:
  %1 = add i64 %0, 1, !dbg !213
  ret i64 %1, !dbg !214
}

define i64 @apply(i64 (i64)*, i64) !dbg !215 {
entry:
  %2 = call i64 %0(i64 %1), !dbg !218
  ret i64 %2, !dbg !219
}

define i64 @apply_overboard(i64 (i64)*, i64) !dbg !220 {
entry:
  %2 = call i64 @anon.2(i64 (i64)* %0, i64 %1), !dbg !221
  ret i64 %2, !dbg !222
}

define i64 @anon.2(i64 (i64)*, i64) !dbg !223 {
entry:
  %2 = call i64 %0(i64 %1), !dbg !224
  ret i64 %2, !dbg !225
}

define i64 @a5_fn() !dbg !226 {
entry:
  call void @llvm.dbg.value(metadata i64 ()* @anon.3, i64 0, metadata !227, metadata !17), !dbg !228
  %0 = call i64 @anon.3(), !dbg !229
  ret i64 %0, !dbg !230
}

define i64 @anon.3() !dbg !231 {
entry:
  ret i64 3, !dbg !232
}

define i64 @anon.4() !dbg !233 {
entry:
  ret i64 3, !dbg !234
}

define i64 @bad() !dbg !235 {
entry:
  ret i64 999, !dbg !236
}

define i64 @testA9() !dbg !237 {
entry:
  %0 = call i64 @testA9Helper(), !dbg !238
  ret i64 %0, !dbg !239
}

define i64 @testA9Helper() !dbg !240 {
entry:
  %0 = call i64 @bad(), !dbg !241
  call void @llvm.dbg.value(metadata i64 %0, i64 0, metadata !242, metadata !17), !dbg !243
  ret i64 3, !dbg !244
}

define i64 @testStruct1() !dbg !245 {
entry:
  ret i64 3, !dbg !246
}

define i64 @testStruct2() !dbg !247 {
entry:
  ret i64 3, !dbg !248
}

define i64 @testNestedCall() !dbg !249 {
entry:
  %0 = call i64 @bad(), !dbg !250
  %1 = insertvalue %2 { i64 3, i64 undef }, i64 %0, 1, !dbg !251
  call void @llvm.dbg.value(metadata %2 %1, i64 0, metadata !252, metadata !17), !dbg !253
  %2 = call %2 @point_id(%2 %1), !dbg !254
  %.elt = extractvalue %2 %2, 0, !dbg !254
  %.elt4 = extractvalue %2 %2, 1, !dbg !254
  %3 = call %2 @point_id(%2 %2), !dbg !255
  %.elt8 = extractvalue %2 %3, 0, !dbg !255
  %.elt10 = extractvalue %2 %3, 1, !dbg !255
  %4 = call %2 @point_id(%2 %3), !dbg !256
  %.elt14 = extractvalue %2 %4, 0, !dbg !256
  %.elt16 = extractvalue %2 %4, 1, !dbg !256
  %5 = call %2 @point_id(%2 %4), !dbg !257
  %.elt20 = extractvalue %2 %5, 0, !dbg !257
  %.elt22 = extractvalue %2 %5, 1, !dbg !257
  %6 = call %2 @point_id(%2 %5), !dbg !258
  %.elt26 = extractvalue %2 %6, 0, !dbg !258
  %.elt28 = extractvalue %2 %6, 1, !dbg !258
  %7 = call %2 @point_id(%2 %6), !dbg !259
  %.elt32 = extractvalue %2 %7, 0, !dbg !259
  %.elt34 = extractvalue %2 %7, 1, !dbg !259
  %8 = call i64 @point_get_x(%2 %7), !dbg !260
  ret i64 %8, !dbg !261
}

define %2 @point_id(%2) !dbg !262 {
entry:
  ret %2 %0, !dbg !263
}

define i64 @point_get_x(%2) !dbg !264 {
entry:
  %1 = extractvalue %2 %0, 0, !dbg !267
  ret i64 %1, !dbg !268
}

define i64 @testDerefParam() !dbg !269 {
entry:
  %local2588_ = alloca i64, align 8, !dbg !270
  store i64 3, i64* %local2588_, align 8, !dbg !271
  %0 = call i64 @deref(i64* nonnull %local2588_), !dbg !272
  ret i64 %0, !dbg !273
}

define i64 @deref(i64*) !dbg !274 {
entry:
  %1 = load i64, i64* %0, align 8, !dbg !277
  ret i64 %1, !dbg !278
}

define void @testBug1() !dbg !279 {
entry:
  %p = alloca %2, align 8, !dbg !282
  %qq1 = alloca %2*, align 8, !dbg !282
  %0 = bitcast %2* %p to i8*, !dbg !283
  call void @llvm.memset.p0i8.i64(i8* %0, i8 0, i64 16, i32 8, i1 false), !dbg !283
  %1 = getelementptr %2, %2* %p, i64 0, i32 1, !dbg !284
  store i64 123, i64* %1, align 8, !dbg !285
  call void @llvm.dbg.value(metadata !4, i64 0, metadata !286, metadata !17), !dbg !289
  call void @llvm.dbg.value(metadata !4, i64 0, metadata !286, metadata !17), !dbg !289
  %.cast = ptrtoint %2* %p to i64, !dbg !290
  call void @llvm.dbg.value(metadata !4, i64 0, metadata !291, metadata !176), !dbg !290
  %2 = bitcast %2** %qq1 to i64*, !dbg !290
  store i64 %.cast, i64* %2, align 8, !dbg !290
  call void @llvm.dbg.value(metadata %2* %p, i64 0, metadata !291, metadata !17), !dbg !290
  store i64 3, i64* %1, align 8, !dbg !292
  call void @assert(i1 true), !dbg !293
  call void @llvm.dbg.value(metadata !4, i64 0, metadata !286, metadata !17), !dbg !289
  %3 = getelementptr %2, %2* %p, i64 0, i32 1, !dbg !294
  %4 = load i64, i64* %3, align 8, !dbg !295
  %5 = icmp eq i64 %4, 3, !dbg !295
  call void @assert(i1 %5), !dbg !296
  call void @llvm.dbg.value(metadata %2* %p, i64 0, metadata !291, metadata !17), !dbg !290
  %6 = load i64, i64* %1, align 8, !dbg !297
  %7 = icmp eq i64 %6, 3, !dbg !297
  call void @assert(i1 %7), !dbg !298
  ret void, !dbg !298
}

define i64 @testDoubleDeref() !dbg !299 {
entry:
  %p = alloca %2, align 8, !dbg !300
  %q1 = alloca %2, align 8, !dbg !300
  %q2 = alloca %2, align 8, !dbg !300
  %0 = bitcast %2* %p to i8*, !dbg !301
  call void @llvm.memset.p0i8.i64(i8* %0, i8 0, i64 16, i32 8, i1 false), !dbg !301
  %1 = getelementptr inbounds %2, %2* %p, i64 0, i32 1, !dbg !302
  store i64 3, i64* %1, align 8, !dbg !303
  call void @llvm.dbg.value(metadata %2* %p, i64 0, metadata !304, metadata !17), !dbg !305
  call void @llvm.dbg.value(metadata %2* %p, i64 0, metadata !304, metadata !17), !dbg !305
  %.elt = getelementptr inbounds %2, %2* %p, i64 0, i32 0, !dbg !306
  %q1.repack = getelementptr inbounds %2, %2* %q1, i64 0, i32 0, !dbg !306
  store i64 0, i64* %q1.repack, align 8, !dbg !306
  %q1.repack4 = getelementptr inbounds %2, %2* %q1, i64 0, i32 1, !dbg !306
  store i64 3, i64* %q1.repack4, align 8, !dbg !306
  call void @llvm.dbg.value(metadata %2* %p, i64 0, metadata !304, metadata !17), !dbg !305
  %q2.repack = getelementptr inbounds %2, %2* %q2, i64 0, i32 0, !dbg !307
  store i64 0, i64* %q2.repack, align 8, !dbg !307
  %q2.repack11 = getelementptr inbounds %2, %2* %q2, i64 0, i32 1, !dbg !307
  store i64 3, i64* %q2.repack11, align 8, !dbg !307
  ret i64 3, !dbg !308
}

define i64 @more_struct() !dbg !309 {
entry:
  %s = alloca %3, align 8, !dbg !310
  %local2358_ = alloca %4*, align 8, !dbg !310
  %local2357_ = alloca %4**, align 8, !dbg !310
  %psx = alloca %4***, align 8, !dbg !310
  %new_sx = alloca %5, align 8, !dbg !310
  %0 = bitcast %3* %s to i8*, !dbg !311
  call void @llvm.memset.p0i8.i64(i8* %0, i8 0, i64 24, i32 8, i1 false), !dbg !311
  %1 = getelementptr inbounds %3, %3* %s, i64 0, i32 0, !dbg !312
  store %4* %1, %4** %local2358_, align 8, !dbg !312
  store %4** %local2358_, %4*** %local2357_, align 8, !dbg !312
  call void @llvm.dbg.value(metadata %4*** %local2357_, i64 0, metadata !313, metadata !17), !dbg !312
  store %4*** %local2357_, %4**** %psx, align 8, !dbg !312
  %2 = call i64 @bad(), !dbg !321
  call void @llvm.dbg.value(metadata %4*** %local2357_, i64 0, metadata !313, metadata !17), !dbg !312
  %3 = load %4**, %4*** %local2357_, align 8, !dbg !321
  %4 = load %4*, %4** %3, align 8, !dbg !321
  %5 = getelementptr %4, %4* %4, i64 0, i32 1, !dbg !321
  store i64 %2, i64* %5, align 8, !dbg !322
  %6 = bitcast %5* %new_sx to i8*, !dbg !323
  call void @llvm.memset.p0i8.i64(i8* %6, i8 0, i64 16, i32 8, i1 false), !dbg !323
  %7 = getelementptr inbounds %5, %5* %new_sx, i64 0, i32 1, !dbg !324
  store i64 3, i64* %7, align 8, !dbg !325
  %8 = bitcast %4**** %psx to %5****, !dbg !325
  %9 = bitcast %4*** %local2357_ to %5***, !dbg !325
  call void @llvm.dbg.value(metadata %4**** %psx, i64 0, metadata !313, metadata !176), !dbg !312
  %10 = load %5**, %5*** %9, align 8, !dbg !325
  %11 = load %5*, %5** %10, align 8, !dbg !325
  %.elt = getelementptr inbounds %5, %5* %new_sx, i64 0, i32 0, !dbg !326
  %.repack = getelementptr inbounds %5, %5* %11, i64 0, i32 0, !dbg !326
  store i64 0, i64* %.repack, align 8, !dbg !326
  %.repack4 = getelementptr inbounds %5, %5* %11, i64 0, i32 1, !dbg !326
  store i64 3, i64* %.repack4, align 8, !dbg !326
  %12 = getelementptr inbounds %3, %3* %s, i64 0, i32 0, i32 1, !dbg !326
  %13 = load i64, i64* %12, align 8, !dbg !327
  ret i64 %13, !dbg !327
}

define i64 @test_S2_stuff() !dbg !328 {
entry:
  %0 = call i64 @bad(), !dbg !329
  ret i64 3, !dbg !330
}

define i64 @testSetFromParam() !dbg !331 {
entry:
  %a = alloca i64, align 8, !dbg !332
  call void @llvm.dbg.value(metadata i64 999, i64 0, metadata !333, metadata !17), !dbg !334
  store i64 999, i64* %a, align 8, !dbg !335
  call void @llvm.dbg.value(metadata i64* %a, i64 0, metadata !333, metadata !17), !dbg !334
  %0 = call i64 @set(i64* nonnull %a), !dbg !336
  %1 = load i64, i64* %a, align 8, !dbg !337
  call void @llvm.dbg.value(metadata i64 %1, i64 0, metadata !333, metadata !17), !dbg !334
  ret i64 %1, !dbg !337
}

define i64 @set(i64*) !dbg !338 {
entry:
  store i64 3, i64* %0, align 8, !dbg !339
  ret i64 999, !dbg !340
}

define i64 @testSetter() !dbg !341 {
entry:
  %p = alloca %2, align 8, !dbg !342
  %0 = bitcast %2* %p to i8*, !dbg !343
  call void @llvm.memset.p0i8.i64(i8* %0, i8 0, i64 16, i32 8, i1 false), !dbg !343
  %1 = call i64 @bad(), !dbg !344
  %2 = getelementptr inbounds %2, %2* %p, i64 0, i32 0, !dbg !344
  store i64 %1, i64* %2, align 8, !dbg !345
  %3 = call i64 @bad(), !dbg !346
  %4 = getelementptr inbounds %2, %2* %p, i64 0, i32 1, !dbg !346
  store i64 %3, i64* %4, align 8, !dbg !347
  %5 = call i64 @set_y(%2* nonnull %p, i64 3), !dbg !348
  %6 = load i64, i64* %4, align 8, !dbg !349
  ret i64 %6, !dbg !349
}

define i64 @set_y(%2*, i64) !dbg !350 {
entry:
  %2 = getelementptr %2, %2* %0, i64 0, i32 1, !dbg !353
  store i64 %1, i64* %2, align 8, !dbg !354
  ret i64 %1, !dbg !355
}

define i64 @testLinkedList() !dbg !356 {
entry:
  %l = alloca %6, align 8, !dbg !357
  %0 = bitcast %6* %l to i8*, !dbg !358
  call void @llvm.memset.p0i8.i64(i8* %0, i8 0, i64 16, i32 8, i1 false), !dbg !358
  %1 = getelementptr inbounds %6, %6* %l, i64 0, i32 0, !dbg !359
  store i64 3, i64* %1, align 8, !dbg !360
  %2 = getelementptr inbounds %6, %6* %l, i64 0, i32 1, !dbg !360
  %.cast = ptrtoint %6* %l to i64, !dbg !361
  %3 = bitcast %6** %2 to i64*, !dbg !361
  store i64 %.cast, i64* %3, align 8, !dbg !361
  ret i64 3, !dbg !362
}

define i64 @testWeirdLinkedList() !dbg !363 {
entry:
  %l = alloca %7, align 8, !dbg !364
  %local2103_ = alloca %7*, align 8, !dbg !364
  %local2102_ = alloca %7**, align 8, !dbg !364
  %local2101_ = alloca %7***, align 8, !dbg !364
  %local2100_ = alloca %7****, align 8, !dbg !364
  %local2099_ = alloca %7*****, align 8, !dbg !364
  %local2098_ = alloca %7******, align 8, !dbg !364
  %0 = bitcast %7* %l to i8*, !dbg !365
  call void @llvm.memset.p0i8.i64(i8* %0, i8 0, i64 16, i32 8, i1 false), !dbg !365
  %1 = getelementptr inbounds %7, %7* %l, i64 0, i32 0, !dbg !366
  store i64 999, i64* %1, align 8, !dbg !367
  store %7* %l, %7** %local2103_, align 8, !dbg !367
  store %7** %local2103_, %7*** %local2102_, align 8, !dbg !367
  store %7*** %local2102_, %7**** %local2101_, align 8, !dbg !367
  store %7**** %local2101_, %7***** %local2100_, align 8, !dbg !367
  store %7***** %local2100_, %7****** %local2099_, align 8, !dbg !367
  store %7****** %local2099_, %7******* %local2098_, align 8, !dbg !367
  %2 = getelementptr %7, %7* %l, i64 0, i32 1, !dbg !367
  %.cast = ptrtoint %7******* %local2098_ to i64, !dbg !368
  %3 = bitcast %7******** %2 to i64*, !dbg !368
  store i64 %.cast, i64* %3, align 8, !dbg !368
  %4 = load %7*****, %7****** %local2099_, align 8, !dbg !369
  %5 = load %7****, %7***** %4, align 8, !dbg !369
  %6 = load %7***, %7**** %5, align 8, !dbg !369
  %7 = load %7**, %7*** %6, align 8, !dbg !369
  %8 = load %7*, %7** %7, align 8, !dbg !369
  %9 = getelementptr %7, %7* %8, i64 0, i32 1, !dbg !369
  %10 = load %7*******, %7******** %9, align 8, !dbg !369
  %11 = load %7******, %7******* %10, align 8, !dbg !369
  %12 = load %7*****, %7****** %11, align 8, !dbg !369
  %13 = load %7****, %7***** %12, align 8, !dbg !369
  %14 = load %7***, %7**** %13, align 8, !dbg !369
  %15 = load %7**, %7*** %14, align 8, !dbg !369
  %16 = load %7*, %7** %15, align 8, !dbg !369
  %17 = getelementptr %7, %7* %16, i64 0, i32 1, !dbg !369
  %18 = load %7*******, %7******** %17, align 8, !dbg !369
  %19 = load %7******, %7******* %18, align 8, !dbg !369
  %20 = load %7*****, %7****** %19, align 8, !dbg !369
  %21 = load %7****, %7***** %20, align 8, !dbg !369
  %22 = load %7***, %7**** %21, align 8, !dbg !369
  %23 = load %7**, %7*** %22, align 8, !dbg !369
  %24 = load %7*, %7** %23, align 8, !dbg !369
  %25 = getelementptr %7, %7* %24, i64 0, i32 1, !dbg !369
  %26 = load %7*******, %7******** %25, align 8, !dbg !369
  %27 = load %7******, %7******* %26, align 8, !dbg !369
  %28 = load %7*****, %7****** %27, align 8, !dbg !369
  %29 = load %7****, %7***** %28, align 8, !dbg !369
  %30 = load %7***, %7**** %29, align 8, !dbg !369
  %31 = load %7**, %7*** %30, align 8, !dbg !369
  %32 = load %7*, %7** %31, align 8, !dbg !369
  %33 = getelementptr %7, %7* %32, i64 0, i32 1, !dbg !369
  %34 = load %7*******, %7******** %33, align 8, !dbg !369
  %35 = load %7******, %7******* %34, align 8, !dbg !369
  %36 = load %7*****, %7****** %35, align 8, !dbg !369
  %37 = load %7****, %7***** %36, align 8, !dbg !369
  %38 = load %7***, %7**** %37, align 8, !dbg !369
  %39 = load %7**, %7*** %38, align 8, !dbg !369
  %40 = load %7*, %7** %39, align 8, !dbg !369
  %41 = getelementptr %7, %7* %40, i64 0, i32 1, !dbg !369
  %42 = load %7*******, %7******** %41, align 8, !dbg !369
  %43 = load %7******, %7******* %42, align 8, !dbg !369
  %44 = load %7*****, %7****** %43, align 8, !dbg !369
  %45 = load %7****, %7***** %44, align 8, !dbg !369
  %46 = load %7***, %7**** %45, align 8, !dbg !369
  %47 = load %7**, %7*** %46, align 8, !dbg !369
  %48 = load %7*, %7** %47, align 8, !dbg !369
  %49 = getelementptr %7, %7* %48, i64 0, i32 1, !dbg !369
  %50 = load %7*******, %7******** %49, align 8, !dbg !369
  %51 = load %7******, %7******* %50, align 8, !dbg !369
  %52 = load %7*****, %7****** %51, align 8, !dbg !369
  %53 = load %7****, %7***** %52, align 8, !dbg !369
  %54 = load %7***, %7**** %53, align 8, !dbg !369
  %55 = load %7**, %7*** %54, align 8, !dbg !369
  %56 = load %7*, %7** %55, align 8, !dbg !369
  %57 = getelementptr %7, %7* %56, i64 0, i32 0, !dbg !369
  store i64 3, i64* %57, align 8, !dbg !370
  %58 = load %7*******, %7******** %2, align 8, !dbg !371
  %59 = load %7******, %7******* %58, align 8, !dbg !371
  %60 = load %7*****, %7****** %59, align 8, !dbg !371
  %61 = load %7****, %7***** %60, align 8, !dbg !371
  %62 = load %7***, %7**** %61, align 8, !dbg !371
  %63 = load %7**, %7*** %62, align 8, !dbg !371
  %64 = load %7*, %7** %63, align 8, !dbg !371
  %65 = getelementptr %7, %7* %64, i64 0, i32 0, !dbg !371
  %66 = load i64, i64* %1, align 8, !dbg !371
  %67 = load i64, i64* %65, align 8, !dbg !371
  %68 = add i64 %67, %66, !dbg !371
  %69 = getelementptr %7, %7* %64, i64 0, i32 1, !dbg !371
  %70 = load %7*******, %7******** %69, align 8, !dbg !371
  %71 = load %7******, %7******* %70, align 8, !dbg !371
  %72 = load %7*****, %7****** %71, align 8, !dbg !371
  %73 = load %7****, %7***** %72, align 8, !dbg !371
  %74 = load %7***, %7**** %73, align 8, !dbg !371
  %75 = load %7**, %7*** %74, align 8, !dbg !371
  %76 = load %7*, %7** %75, align 8, !dbg !371
  %77 = getelementptr %7, %7* %76, i64 0, i32 1, !dbg !371
  %78 = load %7*******, %7******** %77, align 8, !dbg !371
  %79 = load %7******, %7******* %78, align 8, !dbg !371
  %80 = load %7*****, %7****** %79, align 8, !dbg !371
  %81 = load %7****, %7***** %80, align 8, !dbg !371
  %82 = load %7***, %7**** %81, align 8, !dbg !371
  %83 = load %7**, %7*** %82, align 8, !dbg !371
  %84 = load %7*, %7** %83, align 8, !dbg !371
  %85 = getelementptr %7, %7* %84, i64 0, i32 1, !dbg !371
  %86 = load %7*******, %7******** %85, align 8, !dbg !371
  %87 = load %7******, %7******* %86, align 8, !dbg !371
  %88 = load %7*****, %7****** %87, align 8, !dbg !371
  %89 = load %7****, %7***** %88, align 8, !dbg !371
  %90 = load %7***, %7**** %89, align 8, !dbg !371
  %91 = load %7**, %7*** %90, align 8, !dbg !371
  %92 = load %7*, %7** %91, align 8, !dbg !371
  %93 = getelementptr %7, %7* %92, i64 0, i32 0, !dbg !371
  %94 = load i64, i64* %93, align 8, !dbg !371
  %95 = add i64 %94, %68, !dbg !371
  ret i64 %95, !dbg !372
}

define i64 @testMoreDotsAndDerefs() !dbg !373 {
entry:
  ret i64 3, !dbg !374
}

define void @test23() !dbg !375 {
entry:
  %foo = alloca %8, align 8, !dbg !376
  %bar = alloca %9, align 8, !dbg !376
  %baz = alloca %10, align 8, !dbg !376
  %qux = alloca %11, align 8, !dbg !376
  %local3073_ = alloca i32, align 4, !dbg !376
  %local3072_ = alloca i32*, align 8, !dbg !376
  %local3079_ = alloca i64 ()*, align 8, !dbg !376
  %local3087_ = alloca %11*, align 8, !dbg !376
  %0 = bitcast %8* %foo to i8*, !dbg !377
  call void @llvm.memset.p0i8.i64(i8* %0, i8 0, i64 16, i32 8, i1 false), !dbg !377
  %1 = bitcast %9* %bar to i8*, !dbg !378
  call void @llvm.memset.p0i8.i64(i8* %1, i8 0, i64 24, i32 8, i1 false), !dbg !378
  %2 = bitcast %10* %baz to i8*, !dbg !379
  call void @llvm.memset.p0i8.i64(i8* %2, i8 0, i64 16, i32 8, i1 false), !dbg !379
  %3 = bitcast %11* %qux to i8*, !dbg !380
  call void @llvm.memset.p0i8.i64(i8* %3, i8 0, i64 16, i32 8, i1 false), !dbg !380
  %4 = getelementptr inbounds %8, %8* %foo, i64 0, i32 0, !dbg !381
  store i32 111, i32* %4, align 8, !dbg !382
  %5 = getelementptr inbounds %9, %9* %bar, i64 0, i32 0, !dbg !383
  store i32 222, i32* %5, align 8, !dbg !384
  %6 = getelementptr inbounds %10, %10* %baz, i64 0, i32 0, !dbg !385
  store i32 333, i32* %6, align 8, !dbg !386
  store i32 444, i32* %local3073_, align 4, !dbg !387
  store i32* %local3073_, i32** %local3072_, align 8, !dbg !387
  %.cast = ptrtoint i32** %local3072_ to i64, !dbg !388
  %7 = bitcast %11* %qux to i64*, !dbg !388
  store i64 %.cast, i64* %7, align 8, !dbg !388
  store i64 ()* @anon.5, i64 ()** %local3079_, align 8, !dbg !389
  %8 = getelementptr inbounds %11, %11* %qux, i64 0, i32 1, !dbg !389
  %.cast1 = ptrtoint i64 ()** %local3079_ to i64, !dbg !390
  %9 = bitcast i32 ()*** %8 to i64*, !dbg !390
  store i64 %.cast1, i64* %9, align 8, !dbg !390
  store %11* %qux, %11** %local3087_, align 8, !dbg !390
  %10 = getelementptr inbounds %10, %10* %baz, i64 0, i32 1, !dbg !390
  %.cast2 = ptrtoint %11** %local3087_ to i64, !dbg !391
  %11 = bitcast %11*** %10 to i64*, !dbg !391
  store i64 %.cast2, i64* %11, align 8, !dbg !391
  %12 = getelementptr inbounds %9, %9* %bar, i64 0, i32 1, !dbg !391
  %13 = load %10, %10* %baz, align 8, !dbg !392
  call void @llvm.dbg.value(metadata %10 %13, i64 0, metadata !393, metadata !17), !dbg !379
  store %10 %13, %10* %12, align 8, !dbg !392
  %14 = getelementptr inbounds %8, %8* %foo, i64 0, i32 1, !dbg !392
  %.cast3 = ptrtoint %9* %bar to i64, !dbg !394
  %15 = bitcast %9** %14 to i64*, !dbg !394
  store i64 %.cast3, i64* %15, align 8, !dbg !394
  %16 = load %9, %9* %bar, align 8, !dbg !395
  call void @llvm.dbg.value(metadata %9 %16, i64 0, metadata !396, metadata !17), !dbg !378
  call void @setComplexBarFromParam(%9 %16), !dbg !398
  %17 = getelementptr %9, %9* %bar, i64 0, i32 1, i32 1, !dbg !399
  %18 = load %11**, %11*** %17, align 8, !dbg !399
  %19 = load %11*, %11** %18, align 8, !dbg !399
  %20 = getelementptr %11, %11* %19, i64 0, i32 1, !dbg !399
  %21 = load i32 ()**, i32 ()*** %20, align 8, !dbg !399
  %22 = load i32 ()*, i32 ()** %21, align 8, !dbg !399
  %23 = call i32 %22(), !dbg !399
  %24 = icmp eq i32 %23, 123, !dbg !400
  call void @assert(i1 %24), !dbg !401
  %25 = bitcast %11*** %17 to i32*****, !dbg !402
  %26 = load i32****, i32***** %25, align 8, !dbg !402
  %27 = load i32***, i32**** %26, align 8, !dbg !402
  %28 = load i32**, i32*** %27, align 8, !dbg !402
  %29 = load i32*, i32** %28, align 8, !dbg !402
  %30 = load i32, i32* %29, align 4, !dbg !402
  %31 = icmp eq i32 %30, 3, !dbg !402
  call void @assert(i1 %31), !dbg !403
  %32 = load i32****, i32***** %25, align 8, !dbg !404
  %33 = load i32***, i32**** %32, align 8, !dbg !404
  %34 = load i32**, i32*** %33, align 8, !dbg !404
  %35 = load i32*, i32** %34, align 8, !dbg !404
  %36 = load i32, i32* %35, align 4, !dbg !404
  %37 = icmp eq i32 %36, 3, !dbg !404
  call void @assert(i1 %37), !dbg !405
  %38 = bitcast %11*** %10 to i32*****, !dbg !406
  %39 = inttoptr i64 %.cast2 to i32****, !dbg !406
  %40 = load i32***, i32**** %39, align 8, !dbg !406
  %41 = load i32**, i32*** %40, align 8, !dbg !406
  %42 = load i32*, i32** %41, align 8, !dbg !406
  %43 = load i32, i32* %42, align 4, !dbg !406
  %44 = icmp eq i32 %43, 3, !dbg !406
  call void @assert(i1 %44), !dbg !407
  %45 = getelementptr inbounds %11, %11* %qux, i64 0, i32 0, !dbg !408
  %46 = load i32**, i32*** %45, align 8, !dbg !408
  %47 = load i32*, i32** %46, align 8, !dbg !408
  %48 = load i32, i32* %47, align 4, !dbg !408
  %49 = icmp eq i32 %48, 3, !dbg !408
  call void @assert(i1 %49), !dbg !409
  ret void, !dbg !409
}

define i64 @anon.5() !dbg !410 {
entry:
  ret i64 123, !dbg !411
}

define void @setComplexBarFromParam(%9) !dbg !412 {
entry:
  %local3115_ = alloca %10, align 8, !dbg !415
  %1 = extractvalue %9 %0, 1, !dbg !416
  store %10 %1, %10* %local3115_, align 8, !dbg !416
  %2 = getelementptr inbounds %10, %10* %local3115_, i64 0, i32 1, !dbg !416
  %3 = bitcast %11*** %2 to i32*****, !dbg !416
  %4 = load i32****, i32***** %3, align 8, !dbg !416
  %5 = load i32***, i32**** %4, align 8, !dbg !416
  %6 = load i32**, i32*** %5, align 8, !dbg !416
  %7 = load i32*, i32** %6, align 8, !dbg !416
  store i32 3, i32* %7, align 4, !dbg !417
  ret void, !dbg !417
}

define void @test24() !dbg !418 {
entry:
  %foo = alloca %12, align 8, !dbg !419
  %bar = alloca %13, align 8, !dbg !419
  %baz = alloca %14, align 8, !dbg !419
  %qux = alloca %15, align 8, !dbg !419
  %local1746_ = alloca %15*, align 8, !dbg !419
  %local1745_ = alloca %15**, align 8, !dbg !419
  %local1744_ = alloca %15***, align 8, !dbg !419
  %local1743_ = alloca %15****, align 8, !dbg !419
  %local1758_ = alloca %13*, align 8, !dbg !419
  %0 = bitcast %12* %foo to i8*, !dbg !420
  call void @llvm.memset.p0i8.i64(i8* %0, i8 0, i64 16, i32 8, i1 false), !dbg !420
  %1 = getelementptr inbounds %12, %12* %foo, i64 0, i32 0, !dbg !421
  store i32 555, i32* %1, align 8, !dbg !422
  %2 = bitcast %13* %bar to i8*, !dbg !423
  call void @llvm.memset.p0i8.i64(i8* %2, i8 0, i64 24, i32 8, i1 false), !dbg !423
  %3 = getelementptr inbounds %13, %13* %bar, i64 0, i32 0, !dbg !424
  store i32 666, i32* %3, align 8, !dbg !425
  %4 = bitcast %14* %baz to i8*, !dbg !426
  call void @llvm.memset.p0i8.i64(i8* %4, i8 0, i64 16, i32 8, i1 false), !dbg !426
  %5 = getelementptr inbounds %14, %14* %baz, i64 0, i32 0, !dbg !427
  store i32 777, i32* %5, align 8, !dbg !428
  %6 = bitcast %15* %qux to i8*, !dbg !429
  call void @llvm.memset.p0i8.i64(i8* %6, i8 0, i64 16, i32 8, i1 false), !dbg !429
  %7 = getelementptr inbounds %15, %15* %qux, i64 0, i32 0, !dbg !430
  store i32 888, i32* %7, align 8, !dbg !431
  %8 = getelementptr inbounds %15, %15* %qux, i64 0, i32 1, !dbg !431
  %.cast = ptrtoint %12* %foo to i64, !dbg !432
  %9 = bitcast %12** %8 to i64*, !dbg !432
  store i64 %.cast, i64* %9, align 8, !dbg !432
  store %15* %qux, %15** %local1746_, align 8, !dbg !432
  store %15** %local1746_, %15*** %local1745_, align 8, !dbg !432
  store %15*** %local1745_, %15**** %local1744_, align 8, !dbg !432
  store %15**** %local1744_, %15***** %local1743_, align 8, !dbg !432
  %10 = getelementptr inbounds %14, %14* %baz, i64 0, i32 1, !dbg !432
  %.cast1 = ptrtoint %15***** %local1743_ to i64, !dbg !433
  %11 = bitcast %15****** %10 to i64*, !dbg !433
  store i64 %.cast1, i64* %11, align 8, !dbg !433
  %12 = getelementptr inbounds %13, %13* %bar, i64 0, i32 1, !dbg !433
  %13 = load %14, %14* %baz, align 8, !dbg !434
  call void @llvm.dbg.value(metadata %14 %13, i64 0, metadata !435, metadata !17), !dbg !426
  store %14 %13, %14* %12, align 8, !dbg !434
  store %13* %bar, %13** %local1758_, align 8, !dbg !434
  %14 = getelementptr inbounds %12, %12* %foo, i64 0, i32 1, !dbg !434
  %.cast2 = ptrtoint %13** %local1758_ to i64, !dbg !436
  %15 = bitcast %13*** %14 to i64*, !dbg !436
  store i64 %.cast2, i64* %15, align 8, !dbg !436
  %16 = getelementptr inbounds %13, %13* %bar, i64 0, i32 1, i32 1, !dbg !437
  %17 = load %15*****, %15****** %16, align 8, !dbg !437
  %18 = load %15****, %15***** %17, align 8, !dbg !437
  %19 = load %15***, %15**** %18, align 8, !dbg !437
  %20 = load %15**, %15*** %19, align 8, !dbg !437
  %21 = load %15*, %15** %20, align 8, !dbg !437
  %22 = getelementptr %15, %15* %21, i64 0, i32 0, !dbg !437
  store i32 6, i32* %22, align 4, !dbg !438
  %23 = load %13**, %13*** %14, align 8, !dbg !439
  %24 = load %13*, %13** %23, align 8, !dbg !439
  %25 = getelementptr %13, %13* %24, i64 0, i32 1, i32 1, !dbg !439
  %26 = load %15*****, %15****** %25, align 8, !dbg !439
  %27 = load %15****, %15***** %26, align 8, !dbg !439
  %28 = load %15***, %15**** %27, align 8, !dbg !439
  %29 = load %15**, %15*** %28, align 8, !dbg !439
  %30 = load %15*, %15** %29, align 8, !dbg !439
  %31 = getelementptr %15, %15* %30, i64 0, i32 0, !dbg !439
  %32 = load i32, i32* %31, align 4, !dbg !440
  %33 = icmp eq i32 %32, 6, !dbg !440
  call void @assert(i1 %33), !dbg !441
  %34 = load %15*****, %15****** %16, align 8, !dbg !442
  %35 = load %15****, %15***** %34, align 8, !dbg !442
  %36 = load %15***, %15**** %35, align 8, !dbg !442
  %37 = load %15**, %15*** %36, align 8, !dbg !442
  %38 = load %15*, %15** %37, align 8, !dbg !442
  %39 = getelementptr %15, %15* %38, i64 0, i32 0, !dbg !442
  %40 = load i32, i32* %39, align 4, !dbg !443
  %41 = icmp eq i32 %40, 6, !dbg !443
  call void @assert(i1 %41), !dbg !444
  %42 = load %15****, %15***** %local1743_, align 8, !dbg !445
  %43 = load %15***, %15**** %42, align 8, !dbg !445
  %44 = load %15**, %15*** %43, align 8, !dbg !445
  %45 = load %15*, %15** %44, align 8, !dbg !445
  %46 = getelementptr %15, %15* %45, i64 0, i32 0, !dbg !445
  %47 = load i32, i32* %46, align 4, !dbg !446
  %48 = icmp eq i32 %47, 6, !dbg !446
  call void @assert(i1 %48), !dbg !447
  %49 = load i32, i32* %7, align 8, !dbg !448
  %50 = icmp eq i32 %49, 6, !dbg !448
  call void @assert(i1 %50), !dbg !449
  %51 = load %15*****, %15****** %16, align 8, !dbg !450
  %52 = load %15****, %15***** %51, align 8, !dbg !450
  %53 = load %15***, %15**** %52, align 8, !dbg !450
  %54 = load %15**, %15*** %53, align 8, !dbg !450
  %55 = load %15*, %15** %54, align 8, !dbg !450
  %56 = getelementptr %15, %15* %55, i64 0, i32 0, !dbg !450
  store i32 8, i32* %56, align 4, !dbg !451
  %57 = load %13**, %13*** %14, align 8, !dbg !452
  %58 = load %13*, %13** %57, align 8, !dbg !452
  %59 = getelementptr %13, %13* %58, i64 0, i32 1, i32 1, !dbg !452
  %60 = load %15*****, %15****** %59, align 8, !dbg !452
  %61 = load %15****, %15***** %60, align 8, !dbg !452
  %62 = load %15***, %15**** %61, align 8, !dbg !452
  %63 = load %15**, %15*** %62, align 8, !dbg !452
  %64 = load %15*, %15** %63, align 8, !dbg !452
  %65 = getelementptr %15, %15* %64, i64 0, i32 0, !dbg !452
  %66 = load i32, i32* %65, align 4, !dbg !453
  %67 = icmp eq i32 %66, 8, !dbg !453
  call void @assert(i1 %67), !dbg !454
  %68 = load %15*****, %15****** %16, align 8, !dbg !455
  %69 = load %15****, %15***** %68, align 8, !dbg !455
  %70 = load %15***, %15**** %69, align 8, !dbg !455
  %71 = load %15**, %15*** %70, align 8, !dbg !455
  %72 = load %15*, %15** %71, align 8, !dbg !455
  %73 = getelementptr %15, %15* %72, i64 0, i32 0, !dbg !455
  %74 = load i32, i32* %73, align 4, !dbg !456
  %75 = icmp eq i32 %74, 8, !dbg !456
  call void @assert(i1 %75), !dbg !457
  %76 = load %15****, %15***** %local1743_, align 8, !dbg !458
  %77 = load %15***, %15**** %76, align 8, !dbg !458
  %78 = load %15**, %15*** %77, align 8, !dbg !458
  %79 = load %15*, %15** %78, align 8, !dbg !458
  %80 = getelementptr %15, %15* %79, i64 0, i32 0, !dbg !458
  %81 = load i32, i32* %80, align 4, !dbg !459
  %82 = icmp eq i32 %81, 8, !dbg !459
  call void @assert(i1 %82), !dbg !460
  %83 = load i32, i32* %7, align 8, !dbg !461
  %84 = icmp eq i32 %83, 8, !dbg !461
  call void @assert(i1 %84), !dbg !462
  store i32 3, i32* %7, align 8, !dbg !463
  %85 = load %13**, %13*** %14, align 8, !dbg !464
  %86 = load %13*, %13** %85, align 8, !dbg !464
  %87 = getelementptr %13, %13* %86, i64 0, i32 1, i32 1, !dbg !464
  %88 = load %15*****, %15****** %87, align 8, !dbg !464
  %89 = load %15****, %15***** %88, align 8, !dbg !464
  %90 = load %15***, %15**** %89, align 8, !dbg !464
  %91 = load %15**, %15*** %90, align 8, !dbg !464
  %92 = load %15*, %15** %91, align 8, !dbg !464
  %93 = getelementptr %15, %15* %92, i64 0, i32 0, !dbg !464
  %94 = load i32, i32* %93, align 4, !dbg !465
  %95 = icmp eq i32 %94, 3, !dbg !465
  call void @assert(i1 %95), !dbg !466
  %96 = load %15*****, %15****** %16, align 8, !dbg !467
  %97 = load %15****, %15***** %96, align 8, !dbg !467
  %98 = load %15***, %15**** %97, align 8, !dbg !467
  %99 = load %15**, %15*** %98, align 8, !dbg !467
  %100 = load %15*, %15** %99, align 8, !dbg !467
  %101 = getelementptr %15, %15* %100, i64 0, i32 0, !dbg !467
  %102 = load i32, i32* %101, align 4, !dbg !468
  %103 = icmp eq i32 %102, 3, !dbg !468
  call void @assert(i1 %103), !dbg !469
  %104 = load %15*****, %15****** %10, align 8, !dbg !470
  %105 = load %15****, %15***** %104, align 8, !dbg !470
  %106 = load %15***, %15**** %105, align 8, !dbg !470
  %107 = load %15**, %15*** %106, align 8, !dbg !470
  %108 = load %15*, %15** %107, align 8, !dbg !470
  %109 = getelementptr %15, %15* %108, i64 0, i32 0, !dbg !470
  %110 = load i32, i32* %109, align 4, !dbg !471
  %111 = icmp eq i32 %110, 3, !dbg !471
  call void @assert(i1 %111), !dbg !472
  %112 = load i32, i32* %7, align 8, !dbg !473
  %113 = icmp eq i32 %112, 3, !dbg !473
  call void @assert(i1 %113), !dbg !474
  %114 = load %13**, %13*** %14, align 8, !dbg !475
  %115 = load %13*, %13** %114, align 8, !dbg !475
  %116 = getelementptr %13, %13* %115, i64 0, i32 1, i32 1, !dbg !475
  %117 = load %15*****, %15****** %116, align 8, !dbg !475
  %118 = load %15****, %15***** %117, align 8, !dbg !475
  %119 = load %15***, %15**** %118, align 8, !dbg !475
  %120 = load %15**, %15*** %119, align 8, !dbg !475
  %121 = load %15*, %15** %120, align 8, !dbg !475
  %122 = getelementptr %15, %15* %121, i64 0, i32 1, !dbg !475
  %123 = load %12*, %12** %122, align 8, !dbg !475
  %124 = getelementptr %12, %12* %123, i64 0, i32 1, !dbg !475
  %125 = load %13**, %13*** %124, align 8, !dbg !475
  %126 = load %13*, %13** %125, align 8, !dbg !475
  %127 = getelementptr %13, %13* %126, i64 0, i32 1, i32 1, !dbg !475
  %128 = load %15*****, %15****** %127, align 8, !dbg !475
  %129 = load %15****, %15***** %128, align 8, !dbg !475
  %130 = load %15***, %15**** %129, align 8, !dbg !475
  %131 = load %15**, %15*** %130, align 8, !dbg !475
  %132 = load %15*, %15** %131, align 8, !dbg !475
  %133 = getelementptr %15, %15* %132, i64 0, i32 1, !dbg !475
  %134 = load %12*, %12** %133, align 8, !dbg !475
  %135 = getelementptr %12, %12* %134, i64 0, i32 1, !dbg !475
  %136 = load %13**, %13*** %135, align 8, !dbg !475
  %137 = load %13*, %13** %136, align 8, !dbg !475
  %138 = getelementptr %13, %13* %137, i64 0, i32 1, i32 1, !dbg !475
  %139 = load %15*****, %15****** %138, align 8, !dbg !475
  %140 = load %15****, %15***** %139, align 8, !dbg !475
  %141 = load %15***, %15**** %140, align 8, !dbg !475
  %142 = load %15**, %15*** %141, align 8, !dbg !475
  %143 = load %15*, %15** %142, align 8, !dbg !475
  %144 = getelementptr %15, %15* %143, i64 0, i32 1, !dbg !475
  %145 = load %12*, %12** %144, align 8, !dbg !475
  %146 = getelementptr %12, %12* %145, i64 0, i32 1, !dbg !475
  %147 = load %13**, %13*** %146, align 8, !dbg !475
  %148 = load %13*, %13** %147, align 8, !dbg !475
  %149 = getelementptr %13, %13* %148, i64 0, i32 1, i32 1, !dbg !475
  %150 = load %15*****, %15****** %149, align 8, !dbg !475
  %151 = load %15****, %15***** %150, align 8, !dbg !475
  %152 = load %15***, %15**** %151, align 8, !dbg !475
  %153 = load %15**, %15*** %152, align 8, !dbg !475
  %154 = load %15*, %15** %153, align 8, !dbg !475
  %155 = getelementptr %15, %15* %154, i64 0, i32 1, !dbg !475
  %156 = load %12*, %12** %155, align 8, !dbg !475
  %157 = getelementptr %12, %12* %156, i64 0, i32 1, !dbg !475
  %158 = load %13**, %13*** %157, align 8, !dbg !475
  %159 = load %13*, %13** %158, align 8, !dbg !475
  %160 = getelementptr %13, %13* %159, i64 0, i32 1, i32 1, !dbg !475
  %161 = load %15*****, %15****** %160, align 8, !dbg !475
  %162 = load %15****, %15***** %161, align 8, !dbg !475
  %163 = load %15***, %15**** %162, align 8, !dbg !475
  %164 = load %15**, %15*** %163, align 8, !dbg !475
  %165 = load %15*, %15** %164, align 8, !dbg !475
  %166 = getelementptr %15, %15* %165, i64 0, i32 1, !dbg !475
  %167 = load %12*, %12** %166, align 8, !dbg !475
  %168 = getelementptr %12, %12* %167, i64 0, i32 1, !dbg !475
  %169 = load %13**, %13*** %168, align 8, !dbg !475
  %170 = load %13*, %13** %169, align 8, !dbg !475
  %171 = getelementptr %13, %13* %170, i64 0, i32 1, i32 1, !dbg !475
  %172 = load %15*****, %15****** %171, align 8, !dbg !475
  %173 = load %15****, %15***** %172, align 8, !dbg !475
  %174 = load %15***, %15**** %173, align 8, !dbg !475
  %175 = load %15**, %15*** %174, align 8, !dbg !475
  %176 = load %15*, %15** %175, align 8, !dbg !475
  %177 = getelementptr %15, %15* %176, i64 0, i32 1, !dbg !475
  %178 = load %12*, %12** %177, align 8, !dbg !475
  %179 = getelementptr %12, %12* %178, i64 0, i32 1, !dbg !475
  %180 = load %13**, %13*** %179, align 8, !dbg !475
  %181 = load %13*, %13** %180, align 8, !dbg !475
  %182 = getelementptr %13, %13* %181, i64 0, i32 1, i32 1, !dbg !475
  %183 = load %15*****, %15****** %182, align 8, !dbg !475
  %184 = load %15****, %15***** %183, align 8, !dbg !475
  %185 = load %15***, %15**** %184, align 8, !dbg !475
  %186 = load %15**, %15*** %185, align 8, !dbg !475
  %187 = load %15*, %15** %186, align 8, !dbg !475
  %188 = getelementptr %15, %15* %187, i64 0, i32 0, !dbg !475
  store i32 39, i32* %188, align 4, !dbg !476
  %189 = load %13**, %13*** %14, align 8, !dbg !477
  %190 = load %13*, %13** %189, align 8, !dbg !477
  %191 = getelementptr %13, %13* %190, i64 0, i32 1, i32 1, !dbg !477
  %192 = load %15*****, %15****** %191, align 8, !dbg !477
  %193 = load %15****, %15***** %192, align 8, !dbg !477
  %194 = load %15***, %15**** %193, align 8, !dbg !477
  %195 = load %15**, %15*** %194, align 8, !dbg !477
  %196 = load %15*, %15** %195, align 8, !dbg !477
  %197 = getelementptr %15, %15* %196, i64 0, i32 1, !dbg !477
  %198 = load %12*, %12** %197, align 8, !dbg !477
  %199 = getelementptr %12, %12* %198, i64 0, i32 1, !dbg !477
  %200 = load %13**, %13*** %199, align 8, !dbg !477
  %201 = load %13*, %13** %200, align 8, !dbg !477
  %202 = getelementptr %13, %13* %201, i64 0, i32 1, i32 1, !dbg !477
  %203 = load %15*****, %15****** %202, align 8, !dbg !477
  %204 = load %15****, %15***** %203, align 8, !dbg !477
  %205 = load %15***, %15**** %204, align 8, !dbg !477
  %206 = load %15**, %15*** %205, align 8, !dbg !477
  %207 = load %15*, %15** %206, align 8, !dbg !477
  %208 = getelementptr %15, %15* %207, i64 0, i32 1, !dbg !477
  %209 = load %12*, %12** %208, align 8, !dbg !477
  %210 = getelementptr %12, %12* %209, i64 0, i32 1, !dbg !477
  %211 = load %13**, %13*** %210, align 8, !dbg !477
  %212 = load %13*, %13** %211, align 8, !dbg !477
  %213 = getelementptr %13, %13* %212, i64 0, i32 1, i32 1, !dbg !477
  %214 = load %15*****, %15****** %213, align 8, !dbg !477
  %215 = load %15****, %15***** %214, align 8, !dbg !477
  %216 = load %15***, %15**** %215, align 8, !dbg !477
  %217 = load %15**, %15*** %216, align 8, !dbg !477
  %218 = load %15*, %15** %217, align 8, !dbg !477
  %219 = getelementptr %15, %15* %218, i64 0, i32 1, !dbg !477
  %220 = load %12*, %12** %219, align 8, !dbg !477
  %221 = getelementptr %12, %12* %220, i64 0, i32 1, !dbg !477
  %222 = load %13**, %13*** %221, align 8, !dbg !477
  %223 = load %13*, %13** %222, align 8, !dbg !477
  %224 = getelementptr %13, %13* %223, i64 0, i32 1, i32 1, !dbg !477
  %225 = load %15*****, %15****** %224, align 8, !dbg !477
  %226 = load %15****, %15***** %225, align 8, !dbg !477
  %227 = load %15***, %15**** %226, align 8, !dbg !477
  %228 = load %15**, %15*** %227, align 8, !dbg !477
  %229 = load %15*, %15** %228, align 8, !dbg !477
  %230 = getelementptr %15, %15* %229, i64 0, i32 1, !dbg !477
  %231 = load %12*, %12** %230, align 8, !dbg !477
  %232 = getelementptr %12, %12* %231, i64 0, i32 1, !dbg !477
  %233 = load %13**, %13*** %232, align 8, !dbg !477
  %234 = load %13*, %13** %233, align 8, !dbg !477
  %235 = getelementptr %13, %13* %234, i64 0, i32 1, i32 1, !dbg !477
  %236 = load %15*****, %15****** %235, align 8, !dbg !477
  %237 = load %15****, %15***** %236, align 8, !dbg !477
  %238 = load %15***, %15**** %237, align 8, !dbg !477
  %239 = load %15**, %15*** %238, align 8, !dbg !477
  %240 = load %15*, %15** %239, align 8, !dbg !477
  %241 = getelementptr %15, %15* %240, i64 0, i32 1, !dbg !477
  %242 = load %12*, %12** %241, align 8, !dbg !477
  %243 = getelementptr %12, %12* %242, i64 0, i32 1, !dbg !477
  %244 = load %13**, %13*** %243, align 8, !dbg !477
  %245 = load %13*, %13** %244, align 8, !dbg !477
  %246 = getelementptr %13, %13* %245, i64 0, i32 1, i32 1, !dbg !477
  %247 = load %15*****, %15****** %246, align 8, !dbg !477
  %248 = load %15****, %15***** %247, align 8, !dbg !477
  %249 = load %15***, %15**** %248, align 8, !dbg !477
  %250 = load %15**, %15*** %249, align 8, !dbg !477
  %251 = load %15*, %15** %250, align 8, !dbg !477
  %252 = getelementptr %15, %15* %251, i64 0, i32 1, !dbg !477
  %253 = load %12*, %12** %252, align 8, !dbg !477
  %254 = getelementptr %12, %12* %253, i64 0, i32 1, !dbg !477
  %255 = load %13**, %13*** %254, align 8, !dbg !477
  %256 = load %13*, %13** %255, align 8, !dbg !477
  %257 = getelementptr %13, %13* %256, i64 0, i32 1, i32 1, !dbg !477
  %258 = load %15*****, %15****** %257, align 8, !dbg !477
  %259 = load %15****, %15***** %258, align 8, !dbg !477
  %260 = load %15***, %15**** %259, align 8, !dbg !477
  %261 = load %15**, %15*** %260, align 8, !dbg !477
  %262 = load %15*, %15** %261, align 8, !dbg !477
  %263 = getelementptr %15, %15* %262, i64 0, i32 1, !dbg !477
  %264 = load %12*, %12** %263, align 8, !dbg !477
  %265 = getelementptr %12, %12* %264, i64 0, i32 1, !dbg !477
  %266 = load %13**, %13*** %265, align 8, !dbg !477
  %267 = load %13*, %13** %266, align 8, !dbg !477
  %268 = getelementptr %13, %13* %267, i64 0, i32 1, i32 1, !dbg !477
  %269 = load %15*****, %15****** %268, align 8, !dbg !477
  %270 = load %15****, %15***** %269, align 8, !dbg !477
  %271 = load %15***, %15**** %270, align 8, !dbg !477
  %272 = load %15**, %15*** %271, align 8, !dbg !477
  %273 = load %15*, %15** %272, align 8, !dbg !477
  %274 = getelementptr %15, %15* %273, i64 0, i32 1, !dbg !477
  %275 = load %12*, %12** %274, align 8, !dbg !477
  %276 = getelementptr %12, %12* %275, i64 0, i32 1, !dbg !477
  %277 = load %13**, %13*** %276, align 8, !dbg !477
  %278 = load %13*, %13** %277, align 8, !dbg !477
  %279 = getelementptr %13, %13* %278, i64 0, i32 1, i32 1, !dbg !477
  %280 = load %15*****, %15****** %279, align 8, !dbg !477
  %281 = load %15****, %15***** %280, align 8, !dbg !477
  %282 = load %15***, %15**** %281, align 8, !dbg !477
  %283 = load %15**, %15*** %282, align 8, !dbg !477
  %284 = load %15*, %15** %283, align 8, !dbg !477
  %285 = getelementptr %15, %15* %284, i64 0, i32 1, !dbg !477
  %286 = load %12*, %12** %285, align 8, !dbg !477
  %287 = getelementptr %12, %12* %286, i64 0, i32 1, !dbg !477
  %288 = load %13**, %13*** %287, align 8, !dbg !477
  %289 = load %13*, %13** %288, align 8, !dbg !477
  %290 = getelementptr %13, %13* %289, i64 0, i32 1, i32 1, !dbg !477
  %291 = load %15*****, %15****** %290, align 8, !dbg !477
  %292 = load %15****, %15***** %291, align 8, !dbg !477
  %293 = load %15***, %15**** %292, align 8, !dbg !477
  %294 = load %15**, %15*** %293, align 8, !dbg !477
  %295 = load %15*, %15** %294, align 8, !dbg !477
  %296 = getelementptr %15, %15* %295, i64 0, i32 0, !dbg !477
  %297 = load i32, i32* %296, align 4, !dbg !478
  %298 = icmp eq i32 %297, 39, !dbg !478
  call void @assert(i1 %298), !dbg !479
  ret void, !dbg !479
}

define void @test25() !dbg !480 {
entry:
  %foo = alloca %16, align 8, !dbg !481
  %bar = alloca %17, align 8, !dbg !481
  %baz = alloca %18, align 8, !dbg !481
  %qux = alloca %19, align 8, !dbg !481
  %local2830_ = alloca i64, align 8, !dbg !481
  %local2829_ = alloca i64*, align 8, !dbg !481
  %0 = bitcast %16* %foo to i8*, !dbg !482
  call void @llvm.memset.p0i8.i64(i8* %0, i8 0, i64 16, i32 8, i1 false), !dbg !482
  %1 = bitcast %17* %bar to i8*, !dbg !483
  call void @llvm.memset.p0i8.i64(i8* %1, i8 0, i64 16, i32 8, i1 false), !dbg !483
  %2 = bitcast %18* %baz to i8*, !dbg !484
  call void @llvm.memset.p0i8.i64(i8* %2, i8 0, i64 16, i32 8, i1 false), !dbg !484
  %3 = bitcast %19* %qux to i64*, !dbg !485
  store i64 0, i64* %3, align 8, !dbg !485
  %4 = getelementptr inbounds %16, %16* %foo, i64 0, i32 0, !dbg !486
  store i32 111, i32* %4, align 8, !dbg !487
  %5 = getelementptr inbounds %17, %17* %bar, i64 0, i32 0, !dbg !488
  store i32 222, i32* %5, align 8, !dbg !489
  %6 = getelementptr inbounds %18, %18* %baz, i64 0, i32 0, !dbg !490
  store i32 333, i32* %6, align 8, !dbg !491
  store i64 444, i64* %local2830_, align 8, !dbg !492
  store i64* %local2830_, i64** %local2829_, align 8, !dbg !492
  %.cast = ptrtoint i64** %local2829_ to i64, !dbg !493
  store i64 %.cast, i64* %3, align 8, !dbg !493
  %7 = getelementptr inbounds %16, %16* %foo, i64 0, i32 1, !dbg !493
  %.cast1 = ptrtoint %17* %bar to i64, !dbg !494
  %8 = bitcast %17** %7 to i64*, !dbg !494
  store i64 %.cast1, i64* %8, align 8, !dbg !494
  %9 = getelementptr %17, %17* %bar, i64 0, i32 1, !dbg !494
  %.cast2 = ptrtoint %18* %baz to i64, !dbg !495
  %10 = bitcast %18** %9 to i64*, !dbg !495
  store i64 %.cast2, i64* %10, align 8, !dbg !495
  %11 = getelementptr %18, %18* %baz, i64 0, i32 1, !dbg !495
  %.cast3 = ptrtoint %19* %qux to i64, !dbg !496
  %12 = bitcast %19** %11 to i64*, !dbg !496
  store i64 %.cast3, i64* %12, align 8, !dbg !496
  %13 = bitcast %19** %11 to i64****, !dbg !497
  %14 = inttoptr i64 %.cast3 to i64***, !dbg !497
  %15 = load i64**, i64*** %14, align 8, !dbg !497
  %16 = load i64*, i64** %15, align 8, !dbg !497
  store i64 443, i64* %16, align 8, !dbg !498
  %17 = load %18*, %18** %9, align 8, !dbg !499
  %18 = getelementptr %18, %18* %17, i64 0, i32 1, !dbg !499
  %19 = bitcast %19** %18 to i64****, !dbg !499
  %20 = load i64***, i64**** %19, align 8, !dbg !499
  %21 = load i64**, i64*** %20, align 8, !dbg !499
  %22 = load i64*, i64** %21, align 8, !dbg !499
  %23 = load i64, i64* %22, align 8, !dbg !499
  %24 = icmp eq i64 %23, 443, !dbg !500
  call void @assert(i1 %24), !dbg !501
  %25 = load %18*, %18** %9, align 8, !dbg !502
  %26 = getelementptr %18, %18* %25, i64 0, i32 1, !dbg !502
  %27 = bitcast %19** %26 to i64****, !dbg !502
  %28 = load i64***, i64**** %27, align 8, !dbg !502
  %29 = load i64**, i64*** %28, align 8, !dbg !502
  %30 = load i64*, i64** %29, align 8, !dbg !502
  %31 = load i64, i64* %30, align 8, !dbg !502
  %32 = icmp eq i64 %31, 443, !dbg !503
  call void @assert(i1 %32), !dbg !504
  %33 = load i64***, i64**** %13, align 8, !dbg !505
  %34 = load i64**, i64*** %33, align 8, !dbg !505
  %35 = load i64*, i64** %34, align 8, !dbg !505
  %36 = load i64, i64* %35, align 8, !dbg !505
  %37 = icmp eq i64 %36, 443, !dbg !506
  call void @assert(i1 %37), !dbg !507
  %38 = getelementptr inbounds %19, %19* %qux, i64 0, i32 0, !dbg !508
  %39 = load i64**, i64*** %38, align 8, !dbg !508
  %40 = load i64*, i64** %39, align 8, !dbg !508
  %41 = load i64, i64* %40, align 8, !dbg !508
  %42 = icmp eq i64 %41, 443, !dbg !509
  call void @assert(i1 %42), !dbg !510
  %43 = load %18*, %18** %9, align 8, !dbg !511
  %44 = getelementptr %18, %18* %43, i64 0, i32 1, !dbg !511
  %45 = bitcast %19** %44 to i64****, !dbg !511
  %46 = load i64***, i64**** %45, align 8, !dbg !511
  %47 = load i64**, i64*** %46, align 8, !dbg !511
  %48 = load i64*, i64** %47, align 8, !dbg !511
  store i64 442, i64* %48, align 8, !dbg !512
  %49 = load %18*, %18** %9, align 8, !dbg !513
  %50 = getelementptr %18, %18* %49, i64 0, i32 1, !dbg !513
  %51 = bitcast %19** %50 to i64****, !dbg !513
  %52 = load i64***, i64**** %51, align 8, !dbg !513
  %53 = load i64**, i64*** %52, align 8, !dbg !513
  %54 = load i64*, i64** %53, align 8, !dbg !513
  %55 = load i64, i64* %54, align 8, !dbg !513
  %56 = icmp eq i64 %55, 442, !dbg !514
  call void @assert(i1 %56), !dbg !515
  %57 = load %18*, %18** %9, align 8, !dbg !516
  %58 = getelementptr %18, %18* %57, i64 0, i32 1, !dbg !516
  %59 = bitcast %19** %58 to i64****, !dbg !516
  %60 = load i64***, i64**** %59, align 8, !dbg !516
  %61 = load i64**, i64*** %60, align 8, !dbg !516
  %62 = load i64*, i64** %61, align 8, !dbg !516
  %63 = load i64, i64* %62, align 8, !dbg !516
  %64 = icmp eq i64 %63, 442, !dbg !517
  call void @assert(i1 %64), !dbg !518
  %65 = load i64***, i64**** %13, align 8, !dbg !519
  %66 = load i64**, i64*** %65, align 8, !dbg !519
  %67 = load i64*, i64** %66, align 8, !dbg !519
  %68 = load i64, i64* %67, align 8, !dbg !519
  %69 = icmp eq i64 %68, 442, !dbg !520
  call void @assert(i1 %69), !dbg !521
  %70 = load i64**, i64*** %38, align 8, !dbg !522
  %71 = load i64*, i64** %70, align 8, !dbg !522
  %72 = load i64, i64* %71, align 8, !dbg !522
  %73 = icmp eq i64 %72, 442, !dbg !523
  call void @assert(i1 %73), !dbg !524
  ret void, !dbg !524
}

define i64 @fib(i32) !dbg !525 {
entry:
  %switch = icmp ult i32 %0, 2, !dbg !528
  br i1 %switch, label %then, label %else2, !dbg !528

then:                                             ; preds = %entry
  ret i64 1, !dbg !529

else2:                                            ; preds = %entry
  %1 = add i32 %0, -1, !dbg !530
  %2 = call i64 @fib(i32 %1), !dbg !531
  %3 = add i32 %0, -2, !dbg !532
  %4 = call i64 @fib(i32 %3), !dbg !533
  %5 = add i64 %4, %2, !dbg !533
  ret i64 %5, !dbg !534
}

define i64 @stupid_sqrt(i64) !dbg !535 {
entry:
  switch i64 %0, label %else8 [
    i64 1, label %then
    i64 4, label %then1
    i64 9, label %then4
    i64 16, label %then7
  ], !dbg !536

then:                                             ; preds = %entry, %else8, %then7, %then4, %then1
  %merge = phi i64 [ 1, %entry ], [ 2, %then1 ], [ 3, %then4 ], [ 4, %then7 ], [ 0, %else8 ]
  ret i64 %merge, !dbg !537

then1:                                            ; preds = %entry
  br label %then

then4:                                            ; preds = %entry
  br label %then

then7:                                            ; preds = %entry
  br label %then

else8:                                            ; preds = %entry
  br label %then
}

define i32 @id(i32) !dbg !538 {
entry:
  ret i32 %0, !dbg !541
}

define %0 @id.6(%0) !dbg !542 {
entry:
  ret %0 %0, !dbg !543
}

define i64 @deref_ct(i64*) !dbg !544 {
entry:
  %1 = load i64, i64* %0, align 8, !dbg !545
  ret i64 %1, !dbg !546
}

define i32 @add_static(i32) !dbg !547 {
entry:
  %1 = add i32 %0, 7, !dbg !548
  ret i32 %1, !dbg !549
}

define i32 @apply_static(i32) !dbg !550 {
entry:
  %1 = call i32 @poly_inc(i32 %0), !dbg !551
  ret i32 %1, !dbg !552
}

define i32 @poly_inc(i32) !dbg !553 {
entry:
  %1 = add i32 %0, 1, !dbg !554
  ret i32 %1, !dbg !555
}

define i32 @apply_static.7(i32) !dbg !556 {
entry:
  %1 = call i32 @anon.8(i32 %0), !dbg !557
  ret i32 %1, !dbg !558
}

define i32 @anon.8(i32) !dbg !559 {
entry:
  %1 = add i32 %0, 3, !dbg !560
  ret i32 %1, !dbg !561
}

define %0 @id.9(%0) !dbg !562 {
entry:
  ret %0 %0, !dbg !563
}

define void @testArray1() !dbg !564 {
entry:
  %local1012_ = alloca %20, align 8, !dbg !565
  %a = alloca { i64*, i32 }, align 8, !dbg !565
  call void @llvm.dbg.value(metadata i32 5, i64 0, metadata !566, metadata !17), !dbg !567
  %.repack = getelementptr inbounds %20, %20* %local1012_, i64 0, i32 0, !dbg !568
  store i64 1, i64* %.repack, align 8, !dbg !568
  %0 = getelementptr inbounds %20, %20* %local1012_, i64 0, i32 1, !dbg !568
  store i64 2, i64* %0, align 8, !dbg !568
  %1 = getelementptr inbounds %20, %20* %local1012_, i64 0, i32 2, !dbg !568
  store i64 3, i64* %1, align 8, !dbg !568
  %2 = getelementptr inbounds %20, %20* %local1012_, i64 0, i32 3, !dbg !568
  store i64 4, i64* %2, align 8, !dbg !568
  %3 = getelementptr inbounds %20, %20* %local1012_, i64 0, i32 4, !dbg !568
  store i64 5, i64* %3, align 8, !dbg !568
  store i64 1, i64* %.repack, align 8, !dbg !568
  store i64 2, i64* %0, align 8, !dbg !568
  store i64 3, i64* %1, align 8, !dbg !568
  store i64 4, i64* %2, align 8, !dbg !568
  store i64 5, i64* %3, align 8, !dbg !568
  call void @llvm.dbg.value(metadata i32 5, i64 0, metadata !566, metadata !17), !dbg !567
  %4 = insertvalue { %20*, i32 } zeroinitializer, %20* %local1012_, 0, !dbg !568
  %5 = insertvalue { %20*, i32 } %4, i32 5, 1, !dbg !568
  %6 = bitcast { i64*, i32 }* %a to { %20*, i32 }*, !dbg !568
  store { %20*, i32 } %5, { %20*, i32 }* %6, align 8, !dbg !568
  %7 = getelementptr inbounds { i64*, i32 }, { i64*, i32 }* %a, i64 0, i32 0, !dbg !569
  %8 = load i64*, i64** %7, align 8, !dbg !569
  call void @llvm.dbg.value(metadata { i64*, i32 }* %a, i64 0, metadata !570, metadata !176), !dbg !571
  %parith = getelementptr i64, i64* %8, i64 2, !dbg !572
  %9 = load i64, i64* %parith, align 8, !dbg !572
  %10 = icmp eq i64 %9, 3, !dbg !573
  call void @assert(i1 %10), !dbg !574
  call void @llvm.dbg.value(metadata { i64*, i32 }* %a, i64 0, metadata !570, metadata !176), !dbg !571
  %11 = load i64, i64* %parith, align 8, !dbg !575
  %12 = icmp eq i64 %11, 3, !dbg !576
  call void @assert(i1 %12), !dbg !577
  %13 = load i64, i64* %parith, align 8, !dbg !578
  %14 = icmp eq i64 %13, 3, !dbg !579
  call void @assert(i1 %14), !dbg !580
  store i64 19, i64* %parith, align 8, !dbg !581
  call void @llvm.dbg.value(metadata { i64*, i32 }* %a, i64 0, metadata !570, metadata !176), !dbg !571
  call void @assert(i1 true), !dbg !582
  call void @llvm.dbg.value(metadata { i64*, i32 }* %a, i64 0, metadata !570, metadata !176), !dbg !571
  call void @llvm.dbg.value(metadata i64* %parith, i64 0, metadata !583, metadata !17), !dbg !584
  store i64 23, i64* %parith, align 8, !dbg !585
  call void @llvm.dbg.value(metadata { i64*, i32 }* %a, i64 0, metadata !570, metadata !176), !dbg !571
  call void @assert(i1 true), !dbg !586
  ret void, !dbg !586
}

define void @testArray2() !dbg !587 {
entry:
  %local1112_ = alloca %21, align 8, !dbg !588
  %point_array = alloca { %2*, i32 }, align 8, !dbg !588
  %local1163_ = alloca %25, align 8, !dbg !588
  %local1170_ = alloca %25*, align 8, !dbg !588
  call void @llvm.dbg.value(metadata i32 3, i64 0, metadata !589, metadata !17), !dbg !590
  %.repack.repack = getelementptr inbounds %21, %21* %local1112_, i64 0, i32 0, i32 0, !dbg !591
  store i64 1, i64* %.repack.repack, align 8, !dbg !591
  %0 = getelementptr inbounds %21, %21* %local1112_, i64 0, i32 0, i32 1, !dbg !591
  store i64 1, i64* %0, align 8, !dbg !591
  %1 = getelementptr inbounds %21, %21* %local1112_, i64 0, i32 1, i32 0, !dbg !591
  store i64 2, i64* %1, align 8, !dbg !591
  %2 = getelementptr inbounds %21, %21* %local1112_, i64 0, i32 1, i32 1, !dbg !591
  store i64 3, i64* %2, align 8, !dbg !591
  %3 = getelementptr inbounds %21, %21* %local1112_, i64 0, i32 2, i32 0, !dbg !591
  store i64 4, i64* %3, align 8, !dbg !591
  %4 = getelementptr inbounds %21, %21* %local1112_, i64 0, i32 2, i32 1, !dbg !591
  store i64 5, i64* %4, align 8, !dbg !591
  store i64 1, i64* %.repack.repack, align 8, !dbg !591
  store i64 1, i64* %0, align 8, !dbg !591
  store i64 2, i64* %1, align 8, !dbg !591
  store i64 3, i64* %2, align 8, !dbg !591
  store i64 4, i64* %3, align 8, !dbg !591
  store i64 5, i64* %4, align 8, !dbg !591
  call void @llvm.dbg.value(metadata i32 3, i64 0, metadata !589, metadata !17), !dbg !590
  %5 = insertvalue { %21*, i32 } zeroinitializer, %21* %local1112_, 0, !dbg !591
  %6 = insertvalue { %21*, i32 } %5, i32 3, 1, !dbg !591
  %7 = bitcast { %2*, i32 }* %point_array to { %21*, i32 }*, !dbg !591
  store { %21*, i32 } %6, { %21*, i32 }* %7, align 8, !dbg !591
  %8 = getelementptr inbounds { %2*, i32 }, { %2*, i32 }* %point_array, i64 0, i32 0, !dbg !592
  %9 = load %2*, %2** %8, align 8, !dbg !592
  call void @llvm.dbg.value(metadata { %2*, i32 }* %point_array, i64 0, metadata !593, metadata !176), !dbg !597
  %.elt13 = getelementptr inbounds %2, %2* %9, i64 1, i32 1, !dbg !598
  %.unpack14 = load i64, i64* %.elt13, align 8, !dbg !598
  %10 = icmp eq i64 %.unpack14, 3, !dbg !599
  call void @assert(i1 %10), !dbg !600
  call void @llvm.dbg.value(metadata i32 3, i64 0, metadata !589, metadata !17), !dbg !590
  call void @llvm.dbg.value(metadata i32 1, i64 0, metadata !601, metadata !17), !dbg !602
  call void @llvm.dbg.value(metadata i32 2, i64 0, metadata !603, metadata !17), !dbg !604
  call void @llvm.dbg.value(metadata i32 3, i64 0, metadata !605, metadata !17), !dbg !606
  call void @llvm.dbg.value(metadata i32 1, i64 0, metadata !601, metadata !17), !dbg !602
  call void @llvm.dbg.value(metadata i32 2, i64 0, metadata !603, metadata !17), !dbg !604
  call void @llvm.dbg.value(metadata i32 3, i64 0, metadata !605, metadata !17), !dbg !606
  %.repack = getelementptr inbounds %25, %25* %local1163_, i64 0, i32 0, !dbg !592
  store i32 1, i32* %.repack, align 8, !dbg !592
  %11 = getelementptr inbounds %25, %25* %local1163_, i64 0, i32 1, !dbg !592
  store i32 2, i32* %11, align 4, !dbg !592
  %12 = getelementptr inbounds %25, %25* %local1163_, i64 0, i32 2, !dbg !592
  store i32 3, i32* %12, align 8, !dbg !592
  store i32 1, i32* %.repack, align 8, !dbg !592
  store i32 2, i32* %11, align 4, !dbg !592
  store i32 3, i32* %12, align 8, !dbg !592
  %13 = call i8* @malloc(i64 12), !dbg !592
  %14 = bitcast %25** %local1170_ to i8**, !dbg !592
  store i8* %13, i8** %14, align 8, !dbg !592
  %.repack29 = bitcast i8* %13 to i32*, !dbg !592
  store i32 1, i32* %.repack29, align 4, !dbg !592
  %.repack31 = getelementptr inbounds i8, i8* %13, i64 4, !dbg !592
  %15 = bitcast i8* %.repack31 to i32*, !dbg !592
  store i32 2, i32* %15, align 4, !dbg !592
  %.repack33 = getelementptr inbounds i8, i8* %13, i64 8, !dbg !592
  %16 = bitcast i8* %.repack33 to i32*, !dbg !592
  store i32 3, i32* %16, align 4, !dbg !592
  %17 = bitcast %25** %local1170_ to i32**, !dbg !592
  %parith1 = getelementptr i32, i32* %.repack29, i64 1, !dbg !607
  call void @assert(i1 true), !dbg !608
  ret void, !dbg !608
}

define void @testArray3() !dbg !609 {
entry:
  %local1217_ = alloca %26, align 8, !dbg !610
  %arr = alloca { i32*, i32 }, align 8, !dbg !610
  %copied = alloca { i32*, i32 }, align 8, !dbg !610
  call void @llvm.dbg.value(metadata i32 3, i64 0, metadata !611, metadata !17), !dbg !612
  call void @llvm.dbg.value(metadata i32 1, i64 0, metadata !613, metadata !17), !dbg !614
  call void @llvm.dbg.value(metadata i32 2, i64 0, metadata !615, metadata !17), !dbg !616
  call void @llvm.dbg.value(metadata i32 3, i64 0, metadata !617, metadata !17), !dbg !618
  call void @llvm.dbg.value(metadata i32 1, i64 0, metadata !613, metadata !17), !dbg !614
  call void @llvm.dbg.value(metadata i32 2, i64 0, metadata !615, metadata !17), !dbg !616
  call void @llvm.dbg.value(metadata i32 3, i64 0, metadata !617, metadata !17), !dbg !618
  %.repack = getelementptr inbounds %26, %26* %local1217_, i64 0, i32 0, !dbg !619
  store i32 1, i32* %.repack, align 8, !dbg !619
  %0 = getelementptr inbounds %26, %26* %local1217_, i64 0, i32 1, !dbg !619
  store i32 2, i32* %0, align 4, !dbg !619
  %1 = getelementptr inbounds %26, %26* %local1217_, i64 0, i32 2, !dbg !619
  store i32 3, i32* %1, align 8, !dbg !619
  store i32 1, i32* %.repack, align 8, !dbg !619
  store i32 2, i32* %0, align 4, !dbg !619
  store i32 3, i32* %1, align 8, !dbg !619
  call void @llvm.dbg.value(metadata i32 3, i64 0, metadata !611, metadata !17), !dbg !612
  %2 = insertvalue { %26*, i32 } zeroinitializer, %26* %local1217_, 0, !dbg !619
  %3 = insertvalue { %26*, i32 } %2, i32 3, 1, !dbg !619
  %4 = bitcast { i32*, i32 }* %arr to { %26*, i32 }*, !dbg !619
  store { %26*, i32 } %3, { %26*, i32 }* %4, align 8, !dbg !619
  %5 = load { i32*, i32 }, { i32*, i32 }* %arr, align 8, !dbg !620
  call void @llvm.dbg.value(metadata { i32*, i32 } %5, i64 0, metadata !621, metadata !17), !dbg !619
  call void @llvm.dbg.value(metadata { i32*, i32 } %5, i64 0, metadata !626, metadata !17), !dbg !620
  store { i32*, i32 } %5, { i32*, i32 }* %copied, align 8, !dbg !620
  call void @llvm.dbg.value(metadata { i32*, i32 } %5, i64 0, metadata !621, metadata !17), !dbg !619
  %6 = extractvalue { i32*, i32 } %5, 0, !dbg !627
  %parith = getelementptr i32, i32* %6, i64 1, !dbg !628
  %7 = load i32, i32* %parith, align 4, !dbg !628
  %8 = icmp eq i32 %7, 2, !dbg !629
  call void @assert(i1 %8), !dbg !630
  call void @llvm.dbg.value(metadata { i32*, i32 } %5, i64 0, metadata !626, metadata !17), !dbg !620
  %9 = load i32, i32* %parith, align 4, !dbg !631
  %10 = icmp eq i32 %9, 2, !dbg !632
  call void @assert(i1 %10), !dbg !633
  %11 = getelementptr inbounds { i32*, i32 }, { i32*, i32 }* %arr, i64 0, i32 0, !dbg !627
  %12 = load i32*, i32** %11, align 8, !dbg !627
  call void @llvm.dbg.value(metadata { i32*, i32 }* %arr, i64 0, metadata !621, metadata !176), !dbg !619
  %parith2 = getelementptr i32, i32* %12, i64 1, !dbg !634
  store i32 18, i32* %parith2, align 4, !dbg !635
  call void @llvm.dbg.value(metadata { i32*, i32 }* %arr, i64 0, metadata !621, metadata !176), !dbg !619
  call void @assert(i1 true), !dbg !636
  %13 = getelementptr inbounds { i32*, i32 }, { i32*, i32 }* %copied, i64 0, i32 0, !dbg !627
  %14 = load i32*, i32** %13, align 8, !dbg !627
  call void @llvm.dbg.value(metadata { i32*, i32 }* %copied, i64 0, metadata !626, metadata !176), !dbg !620
  %parith4 = getelementptr i32, i32* %14, i64 1, !dbg !637
  %15 = load i32, i32* %parith4, align 4, !dbg !637
  %16 = icmp eq i32 %15, 18, !dbg !638
  call void @assert(i1 %16), !dbg !639
  call void @llvm.dbg.value(metadata { i32*, i32 }* %copied, i64 0, metadata !626, metadata !176), !dbg !620
  store i32 99, i32* %parith4, align 4, !dbg !640
  call void @llvm.dbg.value(metadata { i32*, i32 }* %arr, i64 0, metadata !621, metadata !176), !dbg !619
  %17 = load i32, i32* %parith2, align 4, !dbg !641
  %18 = icmp eq i32 %17, 99, !dbg !642
  call void @assert(i1 %18), !dbg !643
  call void @llvm.dbg.value(metadata { i32*, i32 }* %copied, i64 0, metadata !626, metadata !176), !dbg !620
  %19 = load i32, i32* %parith4, align 4, !dbg !644
  %20 = icmp eq i32 %19, 99, !dbg !645
  call void @assert(i1 %20), !dbg !646
  ret void, !dbg !646
}

define void @testArray4() !dbg !647 {
entry:
  %local1313_ = alloca i64, align 8, !dbg !648
  %local1312_ = alloca i64*, align 8, !dbg !648
  %local1317_ = alloca i64, align 8, !dbg !648
  %local1316_ = alloca i64*, align 8, !dbg !648
  %p = alloca %27, align 8, !dbg !648
  %py = alloca i64**, align 8, !dbg !648
  %local1333_ = alloca i64, align 8, !dbg !648
  %local1332_ = alloca i64*, align 8, !dbg !648
  %local1337_ = alloca i64, align 8, !dbg !648
  %local1336_ = alloca i64*, align 8, !dbg !648
  %local1345_ = alloca i64, align 8, !dbg !648
  %local1344_ = alloca i64*, align 8, !dbg !648
  %local1349_ = alloca i64, align 8, !dbg !648
  %local1348_ = alloca i64*, align 8, !dbg !648
  %local1327_ = alloca %28, align 8, !dbg !648
  %local1352_ = alloca %28*, align 8, !dbg !648
  call void @llvm.dbg.value(metadata i32 3, i64 0, metadata !649, metadata !17), !dbg !650
  store i64 0, i64* %local1313_, align 8, !dbg !651
  store i64* %local1313_, i64** %local1312_, align 8, !dbg !651
  store i64 0, i64* %local1317_, align 8, !dbg !652
  store i64* %local1317_, i64** %local1316_, align 8, !dbg !652
  %.repack = getelementptr inbounds %27, %27* %p, i64 0, i32 0, !dbg !652
  store i64** %local1312_, i64*** %.repack, align 8, !dbg !652
  %0 = getelementptr inbounds %27, %27* %p, i64 0, i32 1, !dbg !652
  store i64** %local1316_, i64*** %0, align 8, !dbg !652
  %.cast = ptrtoint i64** %local1316_ to i64, !dbg !653
  call void @llvm.dbg.value(metadata %27* %p, i64 0, metadata !654, metadata !656), !dbg !653
  %1 = bitcast i64*** %py to i64*, !dbg !653
  store i64 %.cast, i64* %1, align 8, !dbg !653
  store i64 0, i64* %local1333_, align 8, !dbg !657
  store i64* %local1333_, i64** %local1332_, align 8, !dbg !657
  store i64 0, i64* %local1337_, align 8, !dbg !658
  store i64* %local1337_, i64** %local1336_, align 8, !dbg !658
  %2 = bitcast %27* %p to i64*, !dbg !658
  %3 = ptrtoint i64** %local1312_ to i64, !dbg !658
  %4 = bitcast i64*** %0 to i64*, !dbg !658
  store i64 0, i64* %local1345_, align 8, !dbg !659
  store i64* %local1345_, i64** %local1344_, align 8, !dbg !659
  store i64 0, i64* %local1349_, align 8, !dbg !660
  store i64* %local1349_, i64** %local1348_, align 8, !dbg !660
  %.repack5.repack = getelementptr inbounds %28, %28* %local1327_, i64 0, i32 0, i32 0, !dbg !660
  store i64** %local1332_, i64*** %.repack5.repack, align 8, !dbg !660
  %5 = getelementptr inbounds %28, %28* %local1327_, i64 0, i32 0, i32 1, !dbg !660
  store i64** %local1336_, i64*** %5, align 8, !dbg !660
  %6 = getelementptr inbounds %28, %28* %local1327_, i64 0, i32 1, i32 0, !dbg !660
  %7 = bitcast i64*** %6 to i64*, !dbg !660
  store i64 %3, i64* %7, align 8, !dbg !660
  %8 = getelementptr inbounds %28, %28* %local1327_, i64 0, i32 1, i32 1, !dbg !660
  %9 = bitcast i64*** %8 to i64*, !dbg !660
  store i64 %.cast, i64* %9, align 8, !dbg !660
  %10 = getelementptr inbounds %28, %28* %local1327_, i64 0, i32 2, i32 0, !dbg !660
  store i64** %local1344_, i64*** %10, align 8, !dbg !660
  %11 = getelementptr inbounds %28, %28* %local1327_, i64 0, i32 2, i32 1, !dbg !660
  store i64** %local1348_, i64*** %11, align 8, !dbg !660
  %12 = bitcast %28* %local1327_ to i64*, !dbg !660
  %13 = ptrtoint i64** %local1332_ to i64, !dbg !660
  %14 = bitcast i64*** %5 to i64*, !dbg !660
  %15 = ptrtoint i64** %local1336_ to i64, !dbg !660
  %16 = bitcast i64*** %10 to i64*, !dbg !660
  %17 = ptrtoint i64** %local1344_ to i64, !dbg !660
  %18 = bitcast i64*** %11 to i64*, !dbg !660
  %19 = ptrtoint i64** %local1348_ to i64, !dbg !660
  store i64 %13, i64* %12, align 8, !dbg !660
  store i64 %15, i64* %14, align 8, !dbg !660
  store i64 %3, i64* %7, align 8, !dbg !660
  store i64 %.cast, i64* %9, align 8, !dbg !660
  store i64 %17, i64* %16, align 8, !dbg !660
  store i64 %19, i64* %18, align 8, !dbg !660
  %20 = call i8* @malloc(i64 48), !dbg !660
  %21 = bitcast %28** %local1352_ to i8**, !dbg !660
  store i8* %20, i8** %21, align 8, !dbg !660
  %22 = bitcast i8* %20 to i64*, !dbg !660
  store i64 %13, i64* %22, align 8, !dbg !660
  %.repack61.repack70 = getelementptr inbounds i8, i8* %20, i64 8, !dbg !660
  %23 = bitcast i8* %.repack61.repack70 to i64*, !dbg !660
  store i64 %15, i64* %23, align 8, !dbg !660
  %.repack62.repack = getelementptr inbounds i8, i8* %20, i64 16, !dbg !660
  %24 = bitcast i8* %.repack62.repack to i64*, !dbg !660
  store i64 %3, i64* %24, align 8, !dbg !660
  %.repack62.repack68 = getelementptr inbounds i8, i8* %20, i64 24, !dbg !660
  %25 = bitcast i8* %.repack62.repack68 to i64*, !dbg !660
  store i64 %.cast, i64* %25, align 8, !dbg !660
  %.repack64.repack = getelementptr inbounds i8, i8* %20, i64 32, !dbg !660
  %26 = bitcast i8* %.repack64.repack to i64*, !dbg !660
  store i64 %17, i64* %26, align 8, !dbg !660
  %.repack64.repack66 = getelementptr inbounds i8, i8* %20, i64 40, !dbg !660
  %27 = bitcast i8* %.repack64.repack66 to i64*, !dbg !660
  store i64 %19, i64* %27, align 8, !dbg !660
  %28 = bitcast %28** %local1352_ to %27**, !dbg !660
  %29 = bitcast i8* %20 to %27*, !dbg !660
  %.elt78 = getelementptr inbounds %27, %27* %29, i64 1, i32 1, !dbg !661
  %30 = load i64*, i64** %local1316_, align 8, !dbg !661
  store i64 18, i64* %30, align 8, !dbg !662
  call void @llvm.dbg.value(metadata i64*** %py, i64 0, metadata !654, metadata !176), !dbg !653
  %31 = load i64*, i64** %local1316_, align 8, !dbg !663
  %32 = load i64, i64* %31, align 8, !dbg !663
  %33 = icmp eq i64 %32, 18, !dbg !664
  call void @assert(i1 %33), !dbg !665
  ret void, !dbg !665
}

define void @testArray5() !dbg !666 {
entry:
  %local1410_ = alloca i64, align 8, !dbg !667
  %local1407_ = alloca %29, align 8, !dbg !667
  %a1 = alloca { { i64*, i32 }*, i32 }, align 8, !dbg !667
  %local1432_ = alloca %31, align 8, !dbg !667
  %local1439_ = alloca %31*, align 8, !dbg !667
  %local1459_ = alloca %32, align 8, !dbg !667
  %local1466_ = alloca %32*, align 8, !dbg !667
  %local1479_ = alloca %33, align 8, !dbg !667
  %local1486_ = alloca %33*, align 8, !dbg !667
  %local1499_ = alloca %34, align 8, !dbg !667
  %local1506_ = alloca %34*, align 8, !dbg !667
  %local1457_ = alloca %35, align 8, !dbg !667
  %local1518_ = alloca %35*, align 8, !dbg !667
  call void @llvm.dbg.value(metadata i32 1, i64 0, metadata !668, metadata !17), !dbg !669
  store i64 3, i64* %local1410_, align 8, !dbg !670
  call void @llvm.dbg.value(metadata i32 1, i64 0, metadata !668, metadata !17), !dbg !669
  %0 = insertvalue { i64*, i32 } zeroinitializer, i64* %local1410_, 0, !dbg !670
  %1 = insertvalue { i64*, i32 } %0, i32 1, 1, !dbg !670
  %2 = bitcast %29* %local1407_ to { i64*, i32 }*, !dbg !670
  store { i64*, i32 } %1, { i64*, i32 }* %2, align 8, !dbg !670
  call void @llvm.dbg.value(metadata i32 1, i64 0, metadata !668, metadata !17), !dbg !669
  %3 = insertvalue { %29*, i32 } zeroinitializer, %29* %local1407_, 0, !dbg !670
  %4 = insertvalue { %29*, i32 } %3, i32 1, 1, !dbg !670
  %5 = bitcast { { i64*, i32 }*, i32 }* %a1 to { %29*, i32 }*, !dbg !670
  store { %29*, i32 } %4, { %29*, i32 }* %5, align 8, !dbg !670
  %6 = bitcast { { i64*, i32 }*, i32 }* %a1 to i64***, !dbg !671
  %7 = load i64**, i64*** %6, align 8, !dbg !671
  call void @llvm.dbg.value(metadata { { i64*, i32 }*, i32 }* %a1, i64 0, metadata !672, metadata !176), !dbg !677
  %8 = load i64*, i64** %7, align 8, !dbg !678
  %9 = load i64, i64* %8, align 8, !dbg !679
  %10 = icmp eq i64 %9, 3, !dbg !680
  call void @assert(i1 %10), !dbg !681
  %.repack = getelementptr inbounds %31, %31* %local1432_, i64 0, i32 0, !dbg !682
  store i64 1, i64* %.repack, align 8, !dbg !682
  %11 = getelementptr inbounds %31, %31* %local1432_, i64 0, i32 1, !dbg !682
  store i64 2, i64* %11, align 8, !dbg !682
  %12 = getelementptr inbounds %31, %31* %local1432_, i64 0, i32 2, !dbg !682
  store i64 3, i64* %12, align 8, !dbg !682
  store i64 1, i64* %.repack, align 8, !dbg !682
  store i64 2, i64* %11, align 8, !dbg !682
  store i64 3, i64* %12, align 8, !dbg !682
  %13 = call i8* @malloc(i64 24), !dbg !682
  %14 = bitcast %31** %local1439_ to i8**, !dbg !682
  store i8* %13, i8** %14, align 8, !dbg !682
  %.repack19 = bitcast i8* %13 to i64*, !dbg !682
  store i64 1, i64* %.repack19, align 8, !dbg !682
  %.repack21 = getelementptr inbounds i8, i8* %13, i64 8, !dbg !682
  %15 = bitcast i8* %.repack21 to i64*, !dbg !682
  store i64 2, i64* %15, align 8, !dbg !682
  %.repack23 = getelementptr inbounds i8, i8* %13, i64 16, !dbg !682
  %16 = bitcast i8* %.repack23 to i64*, !dbg !682
  store i64 3, i64* %16, align 8, !dbg !682
  %17 = bitcast %31** %local1439_ to i64**, !dbg !682
  %parith2 = getelementptr i64, i64* %.repack19, i64 2, !dbg !683
  call void @assert(i1 true), !dbg !684
  %.repack25 = getelementptr inbounds %32, %32* %local1459_, i64 0, i32 0, !dbg !685
  store i64 1, i64* %.repack25, align 8, !dbg !685
  %18 = getelementptr inbounds %32, %32* %local1459_, i64 0, i32 1, !dbg !685
  store i64 2, i64* %18, align 8, !dbg !685
  %19 = getelementptr inbounds %32, %32* %local1459_, i64 0, i32 2, !dbg !685
  store i64 3, i64* %19, align 8, !dbg !685
  store i64 1, i64* %.repack25, align 8, !dbg !685
  store i64 2, i64* %18, align 8, !dbg !685
  store i64 3, i64* %19, align 8, !dbg !685
  %20 = call i8* @malloc(i64 24), !dbg !685
  %21 = bitcast %32** %local1466_ to i8**, !dbg !685
  store i8* %20, i8** %21, align 8, !dbg !685
  %.repack45 = bitcast i8* %20 to i64*, !dbg !685
  store i64 1, i64* %.repack45, align 8, !dbg !685
  %.repack47 = getelementptr inbounds i8, i8* %20, i64 8, !dbg !685
  %22 = bitcast i8* %.repack47 to i64*, !dbg !685
  store i64 2, i64* %22, align 8, !dbg !685
  %.repack49 = getelementptr inbounds i8, i8* %20, i64 16, !dbg !685
  %23 = bitcast i8* %.repack49 to i64*, !dbg !685
  store i64 3, i64* %23, align 8, !dbg !685
  %24 = bitcast %32** %local1466_ to i64**, !dbg !685
  %25 = insertvalue { i64*, i32 } zeroinitializer, i64* %.repack45, 0, !dbg !671
  %26 = insertvalue { i64*, i32 } %25, i32 3, 1, !dbg !671
  %.repack51 = getelementptr inbounds %33, %33* %local1479_, i64 0, i32 0, !dbg !686
  store i64 4, i64* %.repack51, align 8, !dbg !686
  %27 = getelementptr inbounds %33, %33* %local1479_, i64 0, i32 1, !dbg !686
  store i64 5, i64* %27, align 8, !dbg !686
  %28 = getelementptr inbounds %33, %33* %local1479_, i64 0, i32 2, !dbg !686
  store i64 6, i64* %28, align 8, !dbg !686
  store i64 4, i64* %.repack51, align 8, !dbg !686
  store i64 5, i64* %27, align 8, !dbg !686
  store i64 6, i64* %28, align 8, !dbg !686
  %29 = call i8* @malloc(i64 24), !dbg !686
  %30 = bitcast %33** %local1486_ to i8**, !dbg !686
  store i8* %29, i8** %30, align 8, !dbg !686
  %.repack71 = bitcast i8* %29 to i64*, !dbg !686
  store i64 4, i64* %.repack71, align 8, !dbg !686
  %.repack73 = getelementptr inbounds i8, i8* %29, i64 8, !dbg !686
  %31 = bitcast i8* %.repack73 to i64*, !dbg !686
  store i64 5, i64* %31, align 8, !dbg !686
  %.repack75 = getelementptr inbounds i8, i8* %29, i64 16, !dbg !686
  %32 = bitcast i8* %.repack75 to i64*, !dbg !686
  store i64 6, i64* %32, align 8, !dbg !686
  %33 = bitcast %33** %local1486_ to i64**, !dbg !686
  %34 = insertvalue { i64*, i32 } zeroinitializer, i64* %.repack71, 0, !dbg !671
  %35 = insertvalue { i64*, i32 } %34, i32 3, 1, !dbg !671
  %.repack77 = getelementptr inbounds %34, %34* %local1499_, i64 0, i32 0, !dbg !687
  store i64 7, i64* %.repack77, align 8, !dbg !687
  %36 = getelementptr inbounds %34, %34* %local1499_, i64 0, i32 1, !dbg !687
  store i64 8, i64* %36, align 8, !dbg !687
  %37 = getelementptr inbounds %34, %34* %local1499_, i64 0, i32 2, !dbg !687
  store i64 9, i64* %37, align 8, !dbg !687
  store i64 7, i64* %.repack77, align 8, !dbg !687
  store i64 8, i64* %36, align 8, !dbg !687
  store i64 9, i64* %37, align 8, !dbg !687
  %38 = call i8* @malloc(i64 24), !dbg !687
  %39 = bitcast %34** %local1506_ to i8**, !dbg !687
  store i8* %38, i8** %39, align 8, !dbg !687
  %.repack97 = bitcast i8* %38 to i64*, !dbg !687
  store i64 7, i64* %.repack97, align 8, !dbg !687
  %.repack99 = getelementptr inbounds i8, i8* %38, i64 8, !dbg !687
  %40 = bitcast i8* %.repack99 to i64*, !dbg !687
  store i64 8, i64* %40, align 8, !dbg !687
  %.repack101 = getelementptr inbounds i8, i8* %38, i64 16, !dbg !687
  %41 = bitcast i8* %.repack101 to i64*, !dbg !687
  store i64 9, i64* %41, align 8, !dbg !687
  %42 = bitcast %34** %local1506_ to i64**, !dbg !687
  %43 = insertvalue { i64*, i32 } zeroinitializer, i64* %.repack97, 0, !dbg !671
  %44 = insertvalue { i64*, i32 } %43, i32 3, 1, !dbg !671
  %.repack103 = getelementptr inbounds %35, %35* %local1457_, i64 0, i32 0, !dbg !671
  store { i64*, i32 } %26, { i64*, i32 }* %.repack103, align 8, !dbg !671
  %45 = getelementptr inbounds %35, %35* %local1457_, i64 0, i32 1, !dbg !671
  store { i64*, i32 } %35, { i64*, i32 }* %45, align 8, !dbg !671
  %46 = getelementptr inbounds %35, %35* %local1457_, i64 0, i32 2, !dbg !671
  store { i64*, i32 } %44, { i64*, i32 }* %46, align 8, !dbg !671
  store { i64*, i32 } %26, { i64*, i32 }* %.repack103, align 8, !dbg !671
  store { i64*, i32 } %35, { i64*, i32 }* %45, align 8, !dbg !671
  store { i64*, i32 } %44, { i64*, i32 }* %46, align 8, !dbg !671
  %47 = call i8* @malloc(i64 48), !dbg !671
  %48 = bitcast %35** %local1518_ to i8**, !dbg !671
  store i8* %47, i8** %48, align 8, !dbg !671
  %.repack126 = bitcast i8* %47 to { i64*, i32 }*, !dbg !671
  store { i64*, i32 } %26, { i64*, i32 }* %.repack126, align 8, !dbg !671
  %.repack128 = getelementptr inbounds i8, i8* %47, i64 16, !dbg !671
  %49 = bitcast i8* %.repack128 to { i64*, i32 }*, !dbg !671
  store { i64*, i32 } %35, { i64*, i32 }* %49, align 8, !dbg !671
  %.repack130 = getelementptr inbounds i8, i8* %47, i64 32, !dbg !671
  %50 = bitcast i8* %.repack130 to { i64*, i32 }*, !dbg !671
  store { i64*, i32 } %44, { i64*, i32 }* %50, align 8, !dbg !671
  %51 = bitcast %35** %local1518_ to { i64*, i32 }**, !dbg !671
  %52 = getelementptr inbounds { i64*, i32 }, { i64*, i32 }* %.repack126, i64 1, i32 0, !dbg !688
  %53 = load i64*, i64** %52, align 8, !dbg !688
  %parith4 = getelementptr i64, i64* %53, i64 2, !dbg !689
  %54 = load i64, i64* %parith4, align 8, !dbg !689
  %55 = icmp eq i64 %54, 6, !dbg !690
  call void @assert(i1 %55), !dbg !691
  ret void, !dbg !691
}

define void @testWhile1() !dbg !692 {
entry:
  call void @llvm.dbg.value(metadata i64 3, i64 0, metadata !693, metadata !17), !dbg !694
  br label %cond, !dbg !695

cond:                                             ; preds = %then, %entry
  %0 = phi i64 [ %2, %then ], [ 3, %entry ]
  call void @llvm.dbg.value(metadata i64 %0, i64 0, metadata !693, metadata !17), !dbg !694
  %1 = icmp eq i64 %0, 10, !dbg !696
  call void @llvm.dbg.value(metadata i64 %0, i64 0, metadata !693, metadata !17), !dbg !694
  br i1 %1, label %if_cont, label %then, !dbg !696

then:                                             ; preds = %cond
  %2 = add i64 %0, 1, !dbg !697
  call void @llvm.dbg.value(metadata i64 %2, i64 0, metadata !693, metadata !17), !dbg !694
  br label %cond, !dbg !698

if_cont:                                          ; preds = %cond
  call void @assert(i1 true), !dbg !699
  ret void, !dbg !699
}

define void @testMalloc() !dbg !700 {
entry:
  %b = alloca %0*, align 8, !dbg !701
  %0 = call %0* @alloc_pair(), !dbg !702
  %.cast = ptrtoint %0* %0 to i64, !dbg !702
  %1 = bitcast %0** %b to i64*, !dbg !702
  store i64 %.cast, i64* %1, align 8, !dbg !702
  call void @llvm.dbg.value(metadata %0* %0, i64 0, metadata !703, metadata !17), !dbg !704
  %2 = getelementptr %0, %0* %0, i64 0, i32 0, !dbg !705
  store i64 18, i64* %2, align 8, !dbg !706
  call void @llvm.dbg.value(metadata %0* %0, i64 0, metadata !703, metadata !17), !dbg !704
  %3 = getelementptr %0, %0* %0, i64 0, i32 1, !dbg !707
  store i64 199, i64* %3, align 8, !dbg !708
  %4 = call %0* @alloc_pair(), !dbg !709
  call void @llvm.dbg.value(metadata %0* %4, i64 0, metadata !710, metadata !17), !dbg !711
  %5 = getelementptr %0, %0* %4, i64 0, i32 0, !dbg !712
  store i64 19, i64* %5, align 8, !dbg !713
  call void @llvm.dbg.value(metadata %0* %4, i64 0, metadata !710, metadata !17), !dbg !711
  %6 = getelementptr %0, %0* %4, i64 0, i32 1, !dbg !714
  store i64 200, i64* %6, align 8, !dbg !715
  call void @llvm.dbg.value(metadata %0* %0, i64 0, metadata !703, metadata !17), !dbg !704
  %7 = load i64, i64* %3, align 8, !dbg !716
  %8 = icmp eq i64 %7, 199, !dbg !716
  call void @assert(i1 %8), !dbg !717
  ret void, !dbg !717
}

define %0* @alloc_pair() !dbg !718 {
entry:
  %0 = call i8* @malloc(i64 16), !dbg !721
  %1 = bitcast i8* %0 to %0*, !dbg !721
  ret %0* %1, !dbg !722
}

define void @testFor1() !dbg !723 {
entry:
  %local646_ = alloca %36, align 8, !dbg !724
  %local655_ = alloca %36*, align 8, !dbg !724
  %foreach_index1 = alloca { i64*, i32 }, align 8, !dbg !724
  call void @llvm.dbg.value(metadata i64 1, i64 0, metadata !725, metadata !17), !dbg !726
  %.repack = getelementptr inbounds %36, %36* %local646_, i64 0, i32 0, !dbg !727
  store i64 2, i64* %.repack, align 8, !dbg !727
  %0 = getelementptr inbounds %36, %36* %local646_, i64 0, i32 1, !dbg !727
  store i64 3, i64* %0, align 8, !dbg !727
  %1 = getelementptr inbounds %36, %36* %local646_, i64 0, i32 2, !dbg !727
  store i64 4, i64* %1, align 8, !dbg !727
  %2 = getelementptr inbounds %36, %36* %local646_, i64 0, i32 3, !dbg !727
  store i64 5, i64* %2, align 8, !dbg !727
  store i64 2, i64* %.repack, align 8, !dbg !727
  store i64 3, i64* %0, align 8, !dbg !727
  store i64 4, i64* %1, align 8, !dbg !727
  store i64 5, i64* %2, align 8, !dbg !727
  %3 = call i8* @malloc(i64 32), !dbg !727
  %4 = bitcast %36** %local655_ to i8**, !dbg !727
  store i8* %3, i8** %4, align 8, !dbg !727
  %.repack26 = bitcast i8* %3 to i64*, !dbg !727
  store i64 2, i64* %.repack26, align 8, !dbg !727
  %.repack28 = getelementptr inbounds i8, i8* %3, i64 8, !dbg !727
  %5 = bitcast i8* %.repack28 to i64*, !dbg !727
  store i64 3, i64* %5, align 8, !dbg !727
  %.repack30 = getelementptr inbounds i8, i8* %3, i64 16, !dbg !727
  %6 = bitcast i8* %.repack30 to i64*, !dbg !727
  store i64 4, i64* %6, align 8, !dbg !727
  %.repack32 = getelementptr inbounds i8, i8* %3, i64 24, !dbg !727
  %7 = bitcast i8* %.repack32 to i64*, !dbg !727
  store i64 5, i64* %7, align 8, !dbg !727
  %8 = bitcast %36** %local655_ to i64**, !dbg !727
  %9 = insertvalue { i64*, i32 } zeroinitializer, i64* %.repack26, 0, !dbg !728
  %10 = insertvalue { i64*, i32 } %9, i32 4, 1, !dbg !728
  store { i64*, i32 } %10, { i64*, i32 }* %foreach_index1, align 8, !dbg !728
  %.phi.trans.insert = getelementptr inbounds { i64*, i32 }, { i64*, i32 }* %foreach_index1, i64 0, i32 1
  %.pre = load i32, i32* %.phi.trans.insert, align 8, !dbg !728
  br label %cond, !dbg !728

cond:                                             ; preds = %then, %entry
  %11 = phi i64 [ %18, %then ], [ 1, %entry ]
  %12 = phi i32 [ %19, %then ], [ 0, %entry ]
  %13 = icmp slt i32 %12, %.pre, !dbg !728
  br i1 %13, label %then, label %if_cont, !dbg !728

then:                                             ; preds = %cond
  %14 = getelementptr inbounds { i64*, i32 }, { i64*, i32 }* %foreach_index1, i64 0, i32 0, !dbg !728
  %15 = load i64*, i64** %14, align 8, !dbg !728
  %16 = sext i32 %12 to i64, !dbg !728
  %parith = getelementptr i64, i64* %15, i64 %16, !dbg !728
  %17 = load i64, i64* %parith, align 8, !dbg !728
  call void @llvm.dbg.value(metadata i64 %17, i64 0, metadata !729, metadata !17), !dbg !730
  call void @llvm.dbg.value(metadata i64 %11, i64 0, metadata !725, metadata !17), !dbg !726
  call void @llvm.dbg.value(metadata i64 %17, i64 0, metadata !729, metadata !17), !dbg !730
  %18 = mul i64 %11, %17, !dbg !731
  call void @llvm.dbg.value(metadata i64 %18, i64 0, metadata !725, metadata !17), !dbg !726
  %19 = add i32 %12, 1, !dbg !728
  br label %cond, !dbg !728

if_cont:                                          ; preds = %cond
  call void @llvm.dbg.value(metadata i64 %11, i64 0, metadata !725, metadata !17), !dbg !726
  %20 = icmp eq i64 %11, 120, !dbg !732
  call void @assert(i1 %20), !dbg !733
  ret void, !dbg !733
}

define void @testFor2() !dbg !734 {
entry:
  %local717_ = alloca %37, align 8, !dbg !735
  %local746_ = alloca %38, align 8, !dbg !735
  %local758_ = alloca %38*, align 8, !dbg !735
  %.repack = getelementptr inbounds %37, %37* %local717_, i64 0, i32 0, !dbg !736
  store i32 1, i32* %.repack, align 8, !dbg !736
  %0 = getelementptr inbounds %37, %37* %local717_, i64 0, i32 1, !dbg !736
  store i32 2, i32* %0, align 4, !dbg !736
  %1 = getelementptr inbounds %37, %37* %local717_, i64 0, i32 2, !dbg !736
  store i32 3, i32* %1, align 8, !dbg !736
  %2 = getelementptr inbounds %37, %37* %local717_, i64 0, i32 3, !dbg !736
  store i32 4, i32* %2, align 4, !dbg !736
  %3 = getelementptr inbounds %37, %37* %local717_, i64 0, i32 4, !dbg !736
  store i32 5, i32* %3, align 8, !dbg !736
  store i32 1, i32* %.repack, align 8, !dbg !736
  store i32 2, i32* %0, align 4, !dbg !736
  store i32 3, i32* %1, align 8, !dbg !736
  store i32 4, i32* %2, align 4, !dbg !736
  store i32 5, i32* %3, align 8, !dbg !736
  %.repack42 = getelementptr inbounds %38, %38* %local746_, i64 0, i32 0, !dbg !737
  store i32 1, i32* %.repack42, align 8, !dbg !737
  %4 = getelementptr inbounds %38, %38* %local746_, i64 0, i32 1, !dbg !737
  store i32 2, i32* %4, align 4, !dbg !737
  %5 = getelementptr inbounds %38, %38* %local746_, i64 0, i32 2, !dbg !737
  store i32 3, i32* %5, align 8, !dbg !737
  %6 = getelementptr inbounds %38, %38* %local746_, i64 0, i32 3, !dbg !737
  store i32 4, i32* %6, align 4, !dbg !737
  %7 = getelementptr inbounds %38, %38* %local746_, i64 0, i32 4, !dbg !737
  store i32 5, i32* %7, align 8, !dbg !737
  store i32 1, i32* %.repack42, align 8, !dbg !737
  store i32 2, i32* %4, align 4, !dbg !737
  store i32 3, i32* %5, align 8, !dbg !737
  store i32 4, i32* %6, align 4, !dbg !737
  store i32 5, i32* %7, align 8, !dbg !737
  %8 = call i8* @malloc(i64 20), !dbg !737
  %9 = bitcast %38** %local758_ to i8**, !dbg !737
  store i8* %8, i8** %9, align 8, !dbg !737
  %.repack76 = bitcast i8* %8 to i32*, !dbg !737
  store i32 1, i32* %.repack76, align 4, !dbg !737
  %.repack78 = getelementptr inbounds i8, i8* %8, i64 4, !dbg !737
  %10 = bitcast i8* %.repack78 to i32*, !dbg !737
  store i32 2, i32* %10, align 4, !dbg !737
  %.repack80 = getelementptr inbounds i8, i8* %8, i64 8, !dbg !737
  %11 = bitcast i8* %.repack80 to i32*, !dbg !737
  store i32 3, i32* %11, align 4, !dbg !737
  %.repack82 = getelementptr inbounds i8, i8* %8, i64 12, !dbg !737
  %12 = bitcast i8* %.repack82 to i32*, !dbg !737
  store i32 4, i32* %12, align 4, !dbg !737
  %.repack84 = getelementptr inbounds i8, i8* %8, i64 16, !dbg !737
  %13 = bitcast i8* %.repack84 to i32*, !dbg !737
  store i32 5, i32* %13, align 4, !dbg !737
  %14 = bitcast %38** %local758_ to i32**, !dbg !737
  %15 = insertvalue { i32*, i32 } zeroinitializer, i32* %.repack76, 0, !dbg !738
  %16 = insertvalue { i32*, i32 } %15, i32 5, 1, !dbg !738
  %17 = call i32 @add_array({ i32*, i32 } %16), !dbg !739
  %18 = icmp eq i32 %17, 15, !dbg !739
  call void @assert(i1 %18), !dbg !740
  ret void, !dbg !740
}

define i32 @add_array({ i32*, i32 }) !dbg !741 {
entry:
  %foreach_index1 = alloca { i32*, i32 }, align 8, !dbg !744
  call void @llvm.dbg.value(metadata i32 0, i64 0, metadata !745, metadata !17), !dbg !746
  store { i32*, i32 } %0, { i32*, i32 }* %foreach_index1, align 8, !dbg !747
  %.phi.trans.insert = getelementptr inbounds { i32*, i32 }, { i32*, i32 }* %foreach_index1, i64 0, i32 1
  %.pre = load i32, i32* %.phi.trans.insert, align 8, !dbg !747
  br label %cond, !dbg !747

cond:                                             ; preds = %then, %entry
  %1 = phi i32 [ %8, %then ], [ 0, %entry ]
  %2 = phi i32 [ %9, %then ], [ 0, %entry ]
  %3 = icmp slt i32 %2, %.pre, !dbg !747
  br i1 %3, label %then, label %if_cont, !dbg !747

then:                                             ; preds = %cond
  %4 = getelementptr inbounds { i32*, i32 }, { i32*, i32 }* %foreach_index1, i64 0, i32 0, !dbg !747
  %5 = load i32*, i32** %4, align 8, !dbg !747
  %6 = sext i32 %2 to i64, !dbg !747
  %parith = getelementptr i32, i32* %5, i64 %6, !dbg !747
  %7 = load i32, i32* %parith, align 4, !dbg !747
  call void @llvm.dbg.value(metadata i32 %7, i64 0, metadata !748, metadata !17), !dbg !749
  call void @llvm.dbg.value(metadata i32 %1, i64 0, metadata !745, metadata !17), !dbg !746
  call void @llvm.dbg.value(metadata i32 %7, i64 0, metadata !748, metadata !17), !dbg !749
  %8 = add i32 %1, %7, !dbg !750
  call void @llvm.dbg.value(metadata i32 %8, i64 0, metadata !745, metadata !17), !dbg !746
  %9 = add i32 %2, 1, !dbg !747
  br label %cond, !dbg !747

if_cont:                                          ; preds = %cond
  call void @llvm.dbg.value(metadata i32 %1, i64 0, metadata !745, metadata !17), !dbg !746
  ret i32 %1, !dbg !751
}

define void @testFor3() !dbg !752 {
entry:
  %local779_ = alloca %39, align 8, !dbg !753
  %local786_ = alloca %39*, align 8, !dbg !753
  %local800_ = alloca %40, align 8, !dbg !753
  %local807_ = alloca %40*, align 8, !dbg !753
  %foreach_index1 = alloca { i64*, i32 }, align 8, !dbg !753
  %foreach_index4 = alloca { i64*, i32 }, align 8, !dbg !753
  call void @llvm.dbg.value(metadata i64 0, i64 0, metadata !754, metadata !17), !dbg !755
  %.repack = getelementptr inbounds %39, %39* %local779_, i64 0, i32 0, !dbg !756
  store i64 1, i64* %.repack, align 8, !dbg !756
  %0 = getelementptr inbounds %39, %39* %local779_, i64 0, i32 1, !dbg !756
  store i64 2, i64* %0, align 8, !dbg !756
  %1 = getelementptr inbounds %39, %39* %local779_, i64 0, i32 2, !dbg !756
  store i64 3, i64* %1, align 8, !dbg !756
  store i64 1, i64* %.repack, align 8, !dbg !756
  store i64 2, i64* %0, align 8, !dbg !756
  store i64 3, i64* %1, align 8, !dbg !756
  %2 = call i8* @malloc(i64 24), !dbg !756
  %3 = bitcast %39** %local786_ to i8**, !dbg !756
  store i8* %2, i8** %3, align 8, !dbg !756
  %.repack23 = bitcast i8* %2 to i64*, !dbg !756
  store i64 1, i64* %.repack23, align 8, !dbg !756
  %.repack25 = getelementptr inbounds i8, i8* %2, i64 8, !dbg !756
  %4 = bitcast i8* %.repack25 to i64*, !dbg !756
  store i64 2, i64* %4, align 8, !dbg !756
  %.repack27 = getelementptr inbounds i8, i8* %2, i64 16, !dbg !756
  %5 = bitcast i8* %.repack27 to i64*, !dbg !756
  store i64 3, i64* %5, align 8, !dbg !756
  %6 = bitcast %39** %local786_ to i64**, !dbg !756
  %7 = insertvalue { i64*, i32 } zeroinitializer, i64* %.repack23, 0, !dbg !757
  %8 = insertvalue { i64*, i32 } %7, i32 3, 1, !dbg !757
  store { i64*, i32 } %8, { i64*, i32 }* %foreach_index1, align 8, !dbg !757
  %.phi.trans.insert = getelementptr inbounds { i64*, i32 }, { i64*, i32 }* %foreach_index1, i64 0, i32 1
  %.pre = load i32, i32* %.phi.trans.insert, align 8, !dbg !757
  br label %cond, !dbg !757

cond:                                             ; preds = %if_cont7, %entry
  %9 = phi i64 [ %35, %if_cont7 ], [ 0, %entry ]
  %10 = phi i32 [ %36, %if_cont7 ], [ 0, %entry ]
  %11 = icmp slt i32 %10, %.pre, !dbg !757
  br i1 %11, label %then, label %if_cont, !dbg !757

then:                                             ; preds = %cond
  %12 = getelementptr inbounds { i64*, i32 }, { i64*, i32 }* %foreach_index1, i64 0, i32 0, !dbg !757
  %13 = load i64*, i64** %12, align 8, !dbg !757
  %14 = sext i32 %10 to i64, !dbg !757
  %parith = getelementptr i64, i64* %13, i64 %14, !dbg !757
  %15 = load i64, i64* %parith, align 8, !dbg !757
  call void @llvm.dbg.value(metadata i64 %15, i64 0, metadata !758, metadata !17), !dbg !759
  %.repack29 = getelementptr inbounds %40, %40* %local800_, i64 0, i32 0, !dbg !760
  store i64 4, i64* %.repack29, align 8, !dbg !760
  %16 = getelementptr inbounds %40, %40* %local800_, i64 0, i32 1, !dbg !760
  store i64 5, i64* %16, align 8, !dbg !760
  %17 = getelementptr inbounds %40, %40* %local800_, i64 0, i32 2, !dbg !760
  store i64 6, i64* %17, align 8, !dbg !760
  store i64 4, i64* %.repack29, align 8, !dbg !760
  store i64 5, i64* %16, align 8, !dbg !760
  store i64 6, i64* %17, align 8, !dbg !760
  %18 = call i8* @malloc(i64 24), !dbg !760
  %19 = bitcast %40** %local807_ to i8**, !dbg !760
  store i8* %18, i8** %19, align 8, !dbg !760
  %.repack49 = bitcast i8* %18 to i64*, !dbg !760
  store i64 4, i64* %.repack49, align 8, !dbg !760
  %.repack51 = getelementptr inbounds i8, i8* %18, i64 8, !dbg !760
  %20 = bitcast i8* %.repack51 to i64*, !dbg !760
  store i64 5, i64* %20, align 8, !dbg !760
  %.repack53 = getelementptr inbounds i8, i8* %18, i64 16, !dbg !760
  %21 = bitcast i8* %.repack53 to i64*, !dbg !760
  store i64 6, i64* %21, align 8, !dbg !760
  %22 = bitcast %40** %local807_ to i64**, !dbg !760
  %23 = insertvalue { i64*, i32 } zeroinitializer, i64* %.repack49, 0, !dbg !757
  %24 = insertvalue { i64*, i32 } %23, i32 3, 1, !dbg !757
  store { i64*, i32 } %24, { i64*, i32 }* %foreach_index4, align 8, !dbg !757
  %.phi.trans.insert56 = getelementptr inbounds { i64*, i32 }, { i64*, i32 }* %foreach_index4, i64 0, i32 1
  %.pre57 = load i32, i32* %.phi.trans.insert56, align 8, !dbg !757
  br label %cond5, !dbg !757

if_cont:                                          ; preds = %cond
  call void @llvm.dbg.value(metadata i64 %9, i64 0, metadata !754, metadata !17), !dbg !755
  %25 = icmp eq i64 %9, 51, !dbg !761
  call void @assert(i1 %25), !dbg !762
  ret void, !dbg !762

cond5:                                            ; preds = %then6, %then
  %26 = phi i64 [ %33, %then6 ], [ %9, %then ]
  %27 = phi i32 [ %34, %then6 ], [ 0, %then ]
  %28 = icmp slt i32 %27, %.pre57, !dbg !757
  br i1 %28, label %then6, label %if_cont7, !dbg !757

then6:                                            ; preds = %cond5
  %29 = getelementptr inbounds { i64*, i32 }, { i64*, i32 }* %foreach_index4, i64 0, i32 0, !dbg !757
  %30 = load i64*, i64** %29, align 8, !dbg !757
  %31 = sext i32 %27 to i64, !dbg !757
  %parith8 = getelementptr i64, i64* %30, i64 %31, !dbg !757
  %32 = load i64, i64* %parith8, align 8, !dbg !757
  call void @llvm.dbg.value(metadata i64 %32, i64 0, metadata !763, metadata !17), !dbg !764
  call void @llvm.dbg.value(metadata i64 %26, i64 0, metadata !754, metadata !17), !dbg !755
  call void @llvm.dbg.value(metadata i64 %32, i64 0, metadata !763, metadata !17), !dbg !764
  %33 = add i64 %26, %32, !dbg !765
  call void @llvm.dbg.value(metadata i64 %33, i64 0, metadata !754, metadata !17), !dbg !755
  %34 = add i32 %27, 1, !dbg !757
  br label %cond5, !dbg !757

if_cont7:                                         ; preds = %cond5
  call void @llvm.dbg.value(metadata i64 %26, i64 0, metadata !754, metadata !17), !dbg !755
  call void @llvm.dbg.value(metadata i64 %15, i64 0, metadata !758, metadata !17), !dbg !759
  %35 = add i64 %15, %26, !dbg !766
  call void @llvm.dbg.value(metadata i64 %35, i64 0, metadata !754, metadata !17), !dbg !755
  %36 = add i32 %10, 1, !dbg !757
  br label %cond, !dbg !757
}

define void @testFor4() !dbg !767 {
entry:
  %local844_ = alloca %41, align 8, !dbg !768
  %local855_ = alloca %41*, align 8, !dbg !768
  %foreach_index = alloca { i64*, i32 }, align 8, !dbg !768
  call void @llvm.dbg.value(metadata i32 0, i64 0, metadata !769, metadata !17), !dbg !770
  %.repack = getelementptr inbounds %41, %41* %local844_, i64 0, i32 0, !dbg !771
  store i64 1, i64* %.repack, align 8, !dbg !771
  %0 = getelementptr inbounds %41, %41* %local844_, i64 0, i32 1, !dbg !771
  store i64 1, i64* %0, align 8, !dbg !771
  %1 = getelementptr inbounds %41, %41* %local844_, i64 0, i32 2, !dbg !771
  store i64 1, i64* %1, align 8, !dbg !771
  %2 = getelementptr inbounds %41, %41* %local844_, i64 0, i32 3, !dbg !771
  store i64 1999, i64* %2, align 8, !dbg !771
  %3 = getelementptr inbounds %41, %41* %local844_, i64 0, i32 4, !dbg !771
  store i64 1, i64* %3, align 8, !dbg !771
  store i64 1, i64* %.repack, align 8, !dbg !771
  store i64 1, i64* %0, align 8, !dbg !771
  store i64 1, i64* %1, align 8, !dbg !771
  store i64 1999, i64* %2, align 8, !dbg !771
  store i64 1, i64* %3, align 8, !dbg !771
  %4 = call i8* @malloc(i64 40), !dbg !771
  %5 = bitcast %41** %local855_ to i8**, !dbg !771
  store i8* %4, i8** %5, align 8, !dbg !771
  %.repack32 = bitcast i8* %4 to i64*, !dbg !771
  store i64 1, i64* %.repack32, align 8, !dbg !771
  %.repack34 = getelementptr inbounds i8, i8* %4, i64 8, !dbg !771
  %6 = bitcast i8* %.repack34 to i64*, !dbg !771
  store i64 1, i64* %6, align 8, !dbg !771
  %.repack36 = getelementptr inbounds i8, i8* %4, i64 16, !dbg !771
  %7 = bitcast i8* %.repack36 to i64*, !dbg !771
  store i64 1, i64* %7, align 8, !dbg !771
  %.repack38 = getelementptr inbounds i8, i8* %4, i64 24, !dbg !771
  %8 = bitcast i8* %.repack38 to i64*, !dbg !771
  store i64 1999, i64* %8, align 8, !dbg !771
  %.repack40 = getelementptr inbounds i8, i8* %4, i64 32, !dbg !771
  %9 = bitcast i8* %.repack40 to i64*, !dbg !771
  store i64 1, i64* %9, align 8, !dbg !771
  %10 = bitcast %41** %local855_ to i64**, !dbg !771
  %11 = insertvalue { i64*, i32 } zeroinitializer, i64* %.repack32, 0, !dbg !772
  %12 = insertvalue { i64*, i32 } %11, i32 5, 1, !dbg !772
  store { i64*, i32 } %12, { i64*, i32 }* %foreach_index, align 8, !dbg !772
  call void @llvm.dbg.value(metadata i32 0, i64 0, metadata !773, metadata !17), !dbg !774
  %.phi.trans.insert = getelementptr inbounds { i64*, i32 }, { i64*, i32 }* %foreach_index, i64 0, i32 1
  %.pre = load i32, i32* %.phi.trans.insert, align 8, !dbg !772
  br label %cond, !dbg !772

cond:                                             ; preds = %then, %entry
  %13 = phi i32 [ %16, %then ], [ 0, %entry ]
  %14 = phi i32 [ %17, %then ], [ 0, %entry ]
  call void @llvm.dbg.value(metadata i32 %14, i64 0, metadata !773, metadata !17), !dbg !774
  %15 = icmp slt i32 %14, %.pre, !dbg !772
  br i1 %15, label %then, label %if_cont, !dbg !772

then:                                             ; preds = %cond
  call void @llvm.dbg.value(metadata i32 %14, i64 0, metadata !773, metadata !17), !dbg !774
  call void @llvm.dbg.value(metadata i32 %13, i64 0, metadata !769, metadata !17), !dbg !770
  call void @llvm.dbg.value(metadata i32 %14, i64 0, metadata !773, metadata !17), !dbg !774
  %16 = add i32 %13, %14, !dbg !775
  call void @llvm.dbg.value(metadata i32 %16, i64 0, metadata !769, metadata !17), !dbg !770
  call void @llvm.dbg.value(metadata i32 %14, i64 0, metadata !773, metadata !17), !dbg !774
  %17 = add i32 %14, 1, !dbg !772
  call void @llvm.dbg.value(metadata i32 %17, i64 0, metadata !773, metadata !17), !dbg !774
  br label %cond, !dbg !772

if_cont:                                          ; preds = %cond
  call void @llvm.dbg.value(metadata i32 %13, i64 0, metadata !769, metadata !17), !dbg !770
  %18 = icmp eq i32 %13, 10, !dbg !776
  call void @assert(i1 %18), !dbg !777
  ret void, !dbg !777
}

define void @testUnion1() !dbg !778 {
entry:
  %a = alloca { i32, [8 x i8] }, align 8, !dbg !779
  %0 = bitcast { i32, [8 x i8] }* %a to i8*, !dbg !780
  call void @llvm.memset.p0i8.i64(i8* %0, i8 0, i64 12, i32 8, i1 false), !dbg !780
  %1 = getelementptr inbounds { i32, [8 x i8] }, { i32, [8 x i8] }* %a, i64 0, i32 1, !dbg !780
  %2 = bitcast [8 x i8]* %1 to i64*, !dbg !781
  store i64 18, i64* %2, align 8, !dbg !781
  %3 = getelementptr inbounds { i32, [8 x i8] }, { i32, [8 x i8] }* %a, i64 0, i32 0, !dbg !782
  store i32 1, i32* %3, align 8, !dbg !782
  call void @assert(i1 true), !dbg !783
  call void @assert(i1 true), !dbg !784
  call void @llvm.dbg.value(metadata !4, i64 0, metadata !785, metadata !176), !dbg !787
  store i64 0, i64* %2, align 8, !dbg !788
  store i32 2, i32* %3, align 8, !dbg !782
  call void @assert(i1 true), !dbg !789
  store i64 13, i64* %2, align 8, !dbg !790
  store i32 1, i32* %3, align 8, !dbg !782
  call void @assert(i1 true), !dbg !791
  %4 = getelementptr inbounds { i32, [8 x i8] }, { i32, [8 x i8] }* %a, i64 0, i32 1, i64 4, !dbg !792
  %5 = bitcast i8* %4 to i32*, !dbg !792
  store i32 18, i32* %5, align 4, !dbg !793
  store i32 2, i32* %3, align 8, !dbg !782
  call void @assert(i1 true), !dbg !794
  ret void, !dbg !794
}

define void @testAnySimple1() !dbg !795 {
entry:
  %local3598_ = alloca { i32, [20 x i8] }, align 8, !dbg !796
  %local3589_ = alloca %42, align 8, !dbg !796
  %local3604_ = alloca %42*, align 8, !dbg !796
  %local3622_ = alloca i64, align 8, !dbg !796
  %a = alloca %44, align 8, !dbg !796
  %local7487_ = alloca i32, align 8, !dbg !796
  %tmpcast = bitcast i32* %local7487_ to %45*, !dbg !796
  %.repack = bitcast i32* %local7487_ to i8*, !dbg !797
  store i8 102, i8* %.repack, align 8, !dbg !797
  %0 = getelementptr inbounds %45, %45* %tmpcast, i64 0, i32 1, !dbg !797
  store i8 117, i8* %0, align 1, !dbg !797
  %1 = getelementptr inbounds %45, %45* %tmpcast, i64 0, i32 2, !dbg !797
  store i8 99, i8* %1, align 2, !dbg !797
  %2 = getelementptr inbounds %45, %45* %tmpcast, i64 0, i32 3, !dbg !797
  store i8 107, i8* %2, align 1, !dbg !797
  %3 = call i8* @malloc(i64 4), !dbg !797
  %4 = load i32, i32* %local7487_, align 8, !dbg !797
  %5 = bitcast i8* %3 to i32*, !dbg !797
  store i32 %4, i32* %5, align 4, !dbg !797
  %6 = insertvalue { i8*, i32 } zeroinitializer, i8* %3, 0, !dbg !797
  %7 = insertvalue { i8*, i32 } %6, i32 4, 1, !dbg !797
  %.repack6 = getelementptr inbounds { i32, [20 x i8] }, { i32, [20 x i8] }* %local3598_, i64 0, i32 0, !dbg !797
  store i32 3, i32* %.repack6, align 8, !dbg !797
  %.repack8 = getelementptr inbounds %42, %42* %local3589_, i64 0, i32 0, i32 0, !dbg !797
  store { i8*, i32 } %7, { i8*, i32 }* %.repack8, align 8, !dbg !797
  %8 = getelementptr inbounds %42, %42* %local3589_, i64 0, i32 0, i32 1, !dbg !797
  store { i32, [20 x i8] }* %local3598_, { i32, [20 x i8] }** %8, align 8, !dbg !797
  store { i8*, i32 } %7, { i8*, i32 }* %.repack8, align 8, !dbg !797
  store { i32, [20 x i8] }* %local3598_, { i32, [20 x i8] }** %8, align 8, !dbg !797
  %9 = call i8* @malloc(i64 24), !dbg !797
  %10 = bitcast %42** %local3604_ to i8**, !dbg !797
  store i8* %9, i8** %10, align 8, !dbg !797
  %11 = bitcast { i32, [20 x i8] }** %8 to i64*, !dbg !797
  %12 = ptrtoint { i32, [20 x i8] }* %local3598_ to i64, !dbg !797
  %.repack20 = bitcast i8* %9 to { i8*, i32 }*, !dbg !797
  store { i8*, i32 } %7, { i8*, i32 }* %.repack20, align 8, !dbg !797
  %.repack21 = getelementptr inbounds i8, i8* %9, i64 16, !dbg !797
  %13 = bitcast i8* %.repack21 to i64*, !dbg !797
  store i64 %12, i64* %13, align 8, !dbg !797
  %14 = bitcast %42** %local3604_ to %43**, !dbg !797
  %15 = bitcast i8* %9 to %43*, !dbg !797
  %16 = insertvalue { %43*, i32 } zeroinitializer, %43* %15, 0, !dbg !797
  %17 = insertvalue { %43*, i32 } %16, i32 1, 1, !dbg !797
  %18 = insertvalue { { %43*, i32 } } zeroinitializer, { %43*, i32 } %17, 0, !dbg !797
  %19 = insertvalue <{ i32, { { %43*, i32 } } }> <{ i32 11, { { %43*, i32 } } zeroinitializer }>, { { %43*, i32 } } %18, 1, !dbg !797
  store i64 77, i64* %local3622_, align 8, !dbg !798
  %20 = bitcast i64* %local3622_ to i8*, !dbg !798
  %21 = insertvalue { <{ i32, { { %43*, i32 } } }>, i8* } zeroinitializer, <{ i32, { { %43*, i32 } } }> %19, 0, !dbg !798
  %22 = insertvalue { <{ i32, { { %43*, i32 } } }>, i8* } %21, i8* %20, 1, !dbg !798
  %23 = bitcast %44* %a to { <{ i32, { { %43*, i32 } } }>, i8* }*, !dbg !798
  store { <{ i32, { { %43*, i32 } } }>, i8* } %22, { <{ i32, { { %43*, i32 } } }>, i8* }* %23, align 8, !dbg !798
  %24 = getelementptr inbounds %44, %44* %a, i64 0, i32 0, i32 0, !dbg !797
  %25 = load i32, i32* %24, align 8, !dbg !797
  %26 = icmp eq i32 %25, 11, !dbg !797
  br i1 %26, label %if_cont, label %then, !dbg !797

then:                                             ; preds = %entry
  call void @panic(i8* getelementptr inbounds ([20 x i8], [20 x i8]* @2, i64 0, i64 0)), !dbg !797
  br label %if_cont, !dbg !797

if_cont:                                          ; preds = %entry, %then
  %27 = getelementptr inbounds %44, %44* %a, i64 0, i32 0, i32 1, !dbg !797
  %28 = bitcast [20 x i8]* %27 to %43**, !dbg !797
  %29 = load %43*, %43** %28, align 8, !dbg !797
  %.elt24 = getelementptr inbounds %43, %43* %29, i64 0, i32 1, !dbg !799
  %.unpack252627 = load { i32, [20 x i8] }*, { i32, [20 x i8] }** %.elt24, align 8, !dbg !799
  %30 = getelementptr { i32, [20 x i8] }, { i32, [20 x i8] }* %.unpack252627, i64 0, i32 0, !dbg !799
  %31 = load i32, i32* %30, align 4, !dbg !800
  %32 = icmp eq i32 %31, 3, !dbg !800
  call void @assert(i1 %32), !dbg !801
  ret void, !dbg !801
}

define void @testBuffer() !dbg !802 {
entry:
  %0 = call %46 @buffer(i64 0), !dbg !803
  ret void, !dbg !804
}

define %46 @buffer(i64) !dbg !805 {
entry:
  %buf = alloca { %47*, i32 }, align 8, !dbg !816
  %1 = bitcast { %47*, i32 }* %buf to i8*, !dbg !817
  call void @llvm.memset.p0i8.i64(i8* %1, i8 0, i64 16, i32 8, i1 false), !dbg !817
  %2 = shl i64 %0, 3, !dbg !818
  %3 = call i8* @malloc(i64 %2), !dbg !819
  %.cast = ptrtoint i8* %3 to i64, !dbg !820
  %4 = bitcast { %47*, i32 }* %buf to i64*, !dbg !820
  store i64 %.cast, i64* %4, align 8, !dbg !820
  %5 = load { %47*, i32 }, { %47*, i32 }* %buf, align 8, !dbg !820
  call void @llvm.dbg.value(metadata { %47*, i32 } %5, i64 0, metadata !821, metadata !17), !dbg !817
  %6 = insertvalue { i64, { %47*, i32 } } zeroinitializer, i64 %0, 0, !dbg !820
  %7 = insertvalue { i64, { %47*, i32 } } %6, { %47*, i32 } %5, 1, !dbg !820
  ret { i64, { %47*, i32 } } %7, !dbg !822
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.value(metadata, i64, metadata, metadata) #0

; Function Attrs: argmemonly nounwind
declare void @llvm.memset.p0i8.i64(i8* nocapture writeonly, i8, i64, i32, i1) #1

attributes #0 = { nounwind readnone speculatable }
attributes #1 = { argmemonly nounwind }

!llvm.module.flags = !{!0, !1}
!llvm.dbg.cu = !{!2}

!0 = !{i32 2, !"Dwarf Version", i32 2}
!1 = !{i32 2, !"Debug Info Version", i32 3}
!2 = distinct !DICompileUnit(language: DW_LANG_C, file: !3, producer: "cpi", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !4)
!3 = !DIFile(filename: "test.cpi", directory: "/Users/chadrussell/Projects/cpi/test")
!4 = !{}
!5 = distinct !DISubprogram(name: "main", linkageName: "main", scope: !3, file: !3, line: 1, type: !6, isLocal: false, isDefinition: true, scopeLine: 1, isOptimized: false, unit: !2, variables: !4)
!6 = !DISubroutineType(types: !7)
!7 = !{!8}
!8 = !DIBasicType(name: "i64", size: 64, encoding: DW_ATE_signed)
!9 = !DILocation(line: 1, column: 1, scope: !5)
!10 = !DILocalVariable(name: "f", scope: !5, file: !3, line: 2, type: !11)
!11 = !DISubroutineType(types: !12)
!12 = !{!13}
!13 = !DISubroutineType(types: !14)
!14 = !{!15}
!15 = !DISubroutineType(types: !16)
!16 = !{!6}
!17 = !DIExpression()
!18 = !DILocation(line: 2, column: 5, scope: !5)
!19 = !DILocation(line: 8, column: 17, scope: !5)
!20 = !DILocation(line: 8, column: 5, scope: !5)
!21 = !DILocation(line: 10, column: 12, scope: !5)
!22 = !DILocation(line: 10, column: 28, scope: !5)
!23 = !DILocation(line: 10, column: 55, scope: !5)
!24 = !DILocation(line: 10, column: 5, scope: !5)
!25 = !DILocalVariable(name: "a3", scope: !5, file: !3, line: 12, type: !8)
!26 = !DILocation(line: 12, column: 5, scope: !5)
!27 = !DILocation(line: 13, column: 5, scope: !5)
!28 = !DILocalVariable(name: "a4", scope: !5, file: !3, line: 16, type: !8)
!29 = !DILocation(line: 16, column: 5, scope: !5)
!30 = !DILocation(line: 17, column: 5, scope: !5)
!31 = !DILocation(line: 19, column: 11, scope: !5)
!32 = !DILocalVariable(name: "a5", scope: !5, file: !3, line: 19, type: !8)
!33 = !DILocation(line: 19, column: 5, scope: !5)
!34 = !DILocation(line: 20, column: 18, scope: !5)
!35 = !DILocation(line: 20, column: 5, scope: !5)
!36 = !DILocation(line: 22, column: 11, scope: !5)
!37 = !DILocalVariable(name: "a6", scope: !5, file: !3, line: 22, type: !8)
!38 = !DILocation(line: 22, column: 5, scope: !5)
!39 = !DILocation(line: 23, column: 18, scope: !5)
!40 = !DILocation(line: 23, column: 5, scope: !5)
!41 = !DILocation(line: 25, column: 11, scope: !5)
!42 = !DILocalVariable(name: "a7", scope: !5, file: !3, line: 25, type: !8)
!43 = !DILocation(line: 25, column: 5, scope: !5)
!44 = !DILocation(line: 26, column: 18, scope: !5)
!45 = !DILocation(line: 26, column: 5, scope: !5)
!46 = !DILocation(line: 31, column: 11, scope: !5)
!47 = !DILocalVariable(name: "a8", scope: !5, file: !3, line: 31, type: !8)
!48 = !DILocation(line: 31, column: 5, scope: !5)
!49 = !DILocation(line: 32, column: 18, scope: !5)
!50 = !DILocation(line: 32, column: 5, scope: !5)
!51 = !DILocation(line: 34, column: 11, scope: !5)
!52 = !DILocalVariable(name: "a9", scope: !5, file: !3, line: 34, type: !8)
!53 = !DILocation(line: 34, column: 5, scope: !5)
!54 = !DILocation(line: 35, column: 18, scope: !5)
!55 = !DILocation(line: 35, column: 5, scope: !5)
!56 = !DILocation(line: 37, column: 12, scope: !5)
!57 = !DILocalVariable(name: "a10", scope: !5, file: !3, line: 37, type: !8)
!58 = !DILocation(line: 37, column: 5, scope: !5)
!59 = !DILocation(line: 38, column: 19, scope: !5)
!60 = !DILocation(line: 38, column: 5, scope: !5)
!61 = !DILocation(line: 40, column: 12, scope: !5)
!62 = !DILocalVariable(name: "a11", scope: !5, file: !3, line: 40, type: !8)
!63 = !DILocation(line: 40, column: 5, scope: !5)
!64 = !DILocation(line: 41, column: 19, scope: !5)
!65 = !DILocation(line: 41, column: 5, scope: !5)
!66 = !DILocalVariable(name: "a", scope: !5, file: !3, line: 43, type: !67)
!67 = !DIBasicType(name: "i32", size: 32, encoding: DW_ATE_signed)
!68 = !DILocation(line: 43, column: 5, scope: !5)
!69 = !DILocation(line: 45, column: 12, scope: !5)
!70 = !DILocalVariable(name: "a12", scope: !5, file: !3, line: 45, type: !8)
!71 = !DILocation(line: 45, column: 5, scope: !5)
!72 = !DILocation(line: 46, column: 19, scope: !5)
!73 = !DILocation(line: 46, column: 5, scope: !5)
!74 = !DILocation(line: 48, column: 12, scope: !5)
!75 = !DILocalVariable(name: "a13", scope: !5, file: !3, line: 48, type: !8)
!76 = !DILocation(line: 48, column: 5, scope: !5)
!77 = !DILocation(line: 49, column: 19, scope: !5)
!78 = !DILocation(line: 49, column: 5, scope: !5)
!79 = !DILocation(line: 51, column: 5, scope: !5)
!80 = !DILocation(line: 53, column: 12, scope: !5)
!81 = !DILocalVariable(name: "a15", scope: !5, file: !3, line: 53, type: !8)
!82 = !DILocation(line: 53, column: 5, scope: !5)
!83 = !DILocation(line: 54, column: 19, scope: !5)
!84 = !DILocation(line: 54, column: 5, scope: !5)
!85 = !DILocation(line: 56, column: 12, scope: !5)
!86 = !DILocalVariable(name: "a16", scope: !5, file: !3, line: 56, type: !8)
!87 = !DILocation(line: 56, column: 5, scope: !5)
!88 = !DILocation(line: 57, column: 19, scope: !5)
!89 = !DILocation(line: 57, column: 5, scope: !5)
!90 = !DILocation(line: 59, column: 12, scope: !5)
!91 = !DILocalVariable(name: "a17", scope: !5, file: !3, line: 59, type: !8)
!92 = !DILocation(line: 59, column: 5, scope: !5)
!93 = !DILocation(line: 60, column: 19, scope: !5)
!94 = !DILocation(line: 60, column: 5, scope: !5)
!95 = !DILocation(line: 62, column: 12, scope: !5)
!96 = !DILocalVariable(name: "a18", scope: !5, file: !3, line: 62, type: !8)
!97 = !DILocation(line: 62, column: 5, scope: !5)
!98 = !DILocation(line: 63, column: 19, scope: !5)
!99 = !DILocation(line: 63, column: 5, scope: !5)
!100 = !DILocation(line: 65, column: 12, scope: !5)
!101 = !DILocalVariable(name: "a19", scope: !5, file: !3, line: 65, type: !8)
!102 = !DILocation(line: 65, column: 5, scope: !5)
!103 = !DILocation(line: 66, column: 19, scope: !5)
!104 = !DILocation(line: 66, column: 5, scope: !5)
!105 = !DILocation(line: 68, column: 12, scope: !5)
!106 = !DILocalVariable(name: "a20", scope: !5, file: !3, line: 68, type: !8)
!107 = !DILocation(line: 68, column: 5, scope: !5)
!108 = !DILocation(line: 69, column: 19, scope: !5)
!109 = !DILocation(line: 69, column: 5, scope: !5)
!110 = !DILocation(line: 71, column: 12, scope: !5)
!111 = !DILocalVariable(name: "a21", scope: !5, file: !3, line: 71, type: !8)
!112 = !DILocation(line: 71, column: 5, scope: !5)
!113 = !DILocation(line: 72, column: 19, scope: !5)
!114 = !DILocation(line: 72, column: 5, scope: !5)
!115 = !DILocation(line: 74, column: 12, scope: !5)
!116 = !DILocalVariable(name: "a22", scope: !5, file: !3, line: 74, type: !8)
!117 = !DILocation(line: 74, column: 5, scope: !5)
!118 = !DILocation(line: 75, column: 19, scope: !5)
!119 = !DILocation(line: 75, column: 5, scope: !5)
!120 = !DILocation(line: 77, column: 5, scope: !5)
!121 = !DILocation(line: 79, column: 5, scope: !5)
!122 = !DILocation(line: 81, column: 5, scope: !5)
!123 = !DILocation(line: 83, column: 12, scope: !5)
!124 = !DILocation(line: 83, column: 23, scope: !5)
!125 = !DILocation(line: 83, column: 5, scope: !5)
!126 = !DILocation(line: 85, column: 12, scope: !5)
!127 = !DILocation(line: 85, column: 30, scope: !5)
!128 = !DILocation(line: 85, column: 5, scope: !5)
!129 = !DILocation(line: 87, column: 12, scope: !5)
!130 = !DILocation(line: 87, column: 30, scope: !5)
!131 = !DILocation(line: 87, column: 5, scope: !5)
!132 = !DILocation(line: 89, column: 12, scope: !5)
!133 = !DILocation(line: 89, column: 27, scope: !5)
!134 = !DILocation(line: 89, column: 5, scope: !5)
!135 = !DILocation(line: 91, column: 5, scope: !5)
!136 = !DILocation(line: 92, column: 11, scope: !5)
!137 = !DILocation(line: 92, column: 5, scope: !5)
!138 = !DILocation(line: 93, column: 12, scope: !5)
!139 = !DILocalVariable(name: "p", scope: !5, file: !3, line: 91, type: !140)
!140 = !DICompositeType(tag: DW_TAG_structure_type, name: "composite_type", file: !3, size: 128, align: 8, elements: !4)
!141 = !DILocation(line: 93, column: 23, scope: !5)
!142 = !DILocation(line: 93, column: 5, scope: !5)
!143 = !DILocation(line: 95, column: 22, scope: !5)
!144 = !DILocation(line: 95, column: 12, scope: !5)
!145 = !DILocation(line: 95, column: 28, scope: !5)
!146 = !DILocation(line: 95, column: 5, scope: !5)
!147 = !DILocation(line: 97, column: 12, scope: !5)
!148 = !DILocation(line: 97, column: 36, scope: !5)
!149 = !DILocation(line: 97, column: 5, scope: !5)
!150 = !DILocation(line: 99, column: 12, scope: !5)
!151 = !DILocation(line: 99, column: 42, scope: !5)
!152 = !DILocation(line: 99, column: 5, scope: !5)
!153 = !DILocation(line: 101, column: 12, scope: !5)
!154 = !DILocation(line: 101, column: 59, scope: !5)
!155 = !DILocation(line: 101, column: 5, scope: !5)
!156 = !DILocalVariable(name: "baked", scope: !5, file: !3, line: 103, type: !157)
!157 = !DISubroutineType(types: !158)
!158 = !{!140, !140}
!159 = !DILocation(line: 103, column: 5, scope: !5)
!160 = !DILocation(line: 104, column: 12, scope: !5)
!161 = !DILocation(line: 104, column: 26, scope: !5)
!162 = !DILocation(line: 104, column: 5, scope: !5)
!163 = !DILocation(line: 106, column: 5, scope: !5)
!164 = !DILocation(line: 107, column: 5, scope: !5)
!165 = !DILocation(line: 108, column: 5, scope: !5)
!166 = !DILocation(line: 109, column: 5, scope: !5)
!167 = !DILocation(line: 110, column: 5, scope: !5)
!168 = !DILocation(line: 112, column: 44, scope: !5)
!169 = !DILocation(line: 112, column: 65, scope: !5)
!170 = !DILocalVariable(name: "testArr1", scope: !5, file: !3, line: 112, type: !171)
!171 = !DICompositeType(tag: DW_TAG_structure_type, name: "composite_type", file: !3, size: 128, align: 8, elements: !172)
!172 = !{!173, !175}
!173 = !DIDerivedType(tag: DW_TAG_member, name: "data", file: !3, line: 1, baseType: !174, size: 64, align: 64)
!174 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !8, size: 64, align: 64)
!175 = !DIDerivedType(tag: DW_TAG_member, name: "count", file: !3, line: 1, baseType: !67, size: 32, offset: 64)
!176 = !DIExpression(DW_OP_deref)
!177 = !DILocation(line: 112, column: 5, scope: !5)
!178 = !DILocation(line: 113, column: 25, scope: !5)
!179 = !DILocation(line: 113, column: 12, scope: !5)
!180 = !DILocation(line: 113, column: 32, scope: !5)
!181 = !DILocation(line: 113, column: 5, scope: !5)
!182 = !DILocation(line: 115, column: 5, scope: !5)
!183 = !DILocation(line: 117, column: 5, scope: !5)
!184 = !DILocation(line: 118, column: 5, scope: !5)
!185 = !DILocation(line: 119, column: 5, scope: !5)
!186 = !DILocation(line: 121, column: 5, scope: !5)
!187 = !DILocation(line: 123, column: 5, scope: !5)
!188 = !DILocation(line: 124, column: 5, scope: !5)
!189 = !DILocation(line: 125, column: 5, scope: !5)
!190 = !DILocation(line: 126, column: 5, scope: !5)
!191 = !DILocation(line: 128, column: 5, scope: !5)
!192 = !DILocation(line: 130, column: 5, scope: !5)
!193 = !DILocation(line: 132, column: 5, scope: !5)
!194 = !DILocation(line: 134, column: 5, scope: !5)
!195 = distinct !DISubprogram(name: "anon", linkageName: "anon", scope: !3, file: !3, line: 2, type: !11, isLocal: false, isDefinition: true, scopeLine: 2, isOptimized: false, unit: !2, variables: !4)
!196 = !DILocation(line: 3, column: 9, scope: !195)
!197 = distinct !DISubprogram(name: "anon", linkageName: "anon", scope: !3, file: !3, line: 3, type: !13, isLocal: false, isDefinition: true, scopeLine: 3, isOptimized: false, unit: !2, variables: !4)
!198 = !DILocation(line: 4, column: 13, scope: !197)
!199 = distinct !DISubprogram(name: "foo", linkageName: "foo", scope: !3, file: !3, line: 422, type: !15, isLocal: false, isDefinition: true, scopeLine: 422, isOptimized: false, unit: !2, variables: !4)
!200 = !DILocation(line: 423, column: 5, scope: !199)
!201 = distinct !DISubprogram(name: "bar", linkageName: "bar", scope: !3, file: !3, line: 614, type: !6, isLocal: false, isDefinition: true, scopeLine: 614, isOptimized: false, unit: !2, variables: !4)
!202 = !DILocation(line: 615, column: 5, scope: !201)
!203 = distinct !DISubprogram(name: "assert", linkageName: "assert", scope: !3, file: !3, line: 321, type: !204, isLocal: false, isDefinition: true, scopeLine: 321, isOptimized: false, unit: !2, variables: !4)
!204 = !DISubroutineType(types: !205)
!205 = !{!206, !207}
!206 = !DIBasicType(name: "none", encoding: DW_ATE_unsigned)
!207 = !DIBasicType(name: "bool", size: 1, encoding: DW_ATE_signed)
!208 = !DILocation(line: 322, column: 14, scope: !203)
!209 = !DILocation(line: 323, column: 9, scope: !203)
!210 = distinct !DISubprogram(name: "inc", linkageName: "inc", scope: !3, file: !3, line: 618, type: !211, isLocal: false, isDefinition: true, scopeLine: 618, isOptimized: false, unit: !2, variables: !4)
!211 = !DISubroutineType(types: !212)
!212 = !{!8, !8}
!213 = !DILocation(line: 619, column: 13, scope: !210)
!214 = !DILocation(line: 619, column: 5, scope: !210)
!215 = distinct !DISubprogram(name: "apply", linkageName: "apply", scope: !3, file: !3, line: 622, type: !216, isLocal: false, isDefinition: true, scopeLine: 622, isOptimized: false, unit: !2, variables: !4)
!216 = !DISubroutineType(types: !217)
!217 = !{!8, !211, !8}
!218 = !DILocation(line: 623, column: 9, scope: !215)
!219 = !DILocation(line: 623, column: 5, scope: !215)
!220 = distinct !DISubprogram(name: "apply_overboard", linkageName: "apply_overboard", scope: !3, file: !3, line: 626, type: !216, isLocal: false, isDefinition: true, scopeLine: 626, isOptimized: false, unit: !2, variables: !4)
!221 = !DILocation(line: 627, column: 9, scope: !220)
!222 = !DILocation(line: 627, column: 5, scope: !220)
!223 = distinct !DISubprogram(name: "anon", linkageName: "anon", scope: !3, file: !3, line: 627, type: !216, isLocal: false, isDefinition: true, scopeLine: 627, isOptimized: false, unit: !2, variables: !4)
!224 = !DILocation(line: 628, column: 13, scope: !223)
!225 = !DILocation(line: 628, column: 9, scope: !223)
!226 = distinct !DISubprogram(name: "a5_fn", linkageName: "a5_fn", scope: !3, file: !3, line: 608, type: !6, isLocal: false, isDefinition: true, scopeLine: 608, isOptimized: false, unit: !2, variables: !4)
!227 = !DILocalVariable(name: "f", scope: !226, file: !3, line: 609, type: !6)
!228 = !DILocation(line: 609, column: 5, scope: !226)
!229 = !DILocation(line: 611, column: 9, scope: !226)
!230 = !DILocation(line: 611, column: 5, scope: !226)
!231 = distinct !DISubprogram(name: "anon", linkageName: "anon", scope: !3, file: !3, line: 609, type: !6, isLocal: false, isDefinition: true, scopeLine: 609, isOptimized: false, unit: !2, variables: !4)
!232 = !DILocation(line: 609, column: 21, scope: !231)
!233 = distinct !DISubprogram(name: "anon", linkageName: "anon", scope: !3, file: !3, line: 25, type: !6, isLocal: false, isDefinition: true, scopeLine: 25, isOptimized: false, unit: !2, variables: !4)
!234 = !DILocation(line: 25, column: 26, scope: !233)
!235 = distinct !DISubprogram(name: "bad", linkageName: "bad", scope: !3, file: !3, line: 606, type: !6, isLocal: false, isDefinition: true, scopeLine: 606, isOptimized: false, unit: !2, variables: !4)
!236 = !DILocation(line: 606, column: 16, scope: !235)
!237 = distinct !DISubprogram(name: "testA9", linkageName: "testA9", scope: !3, file: !3, line: 539, type: !6, isLocal: false, isDefinition: true, scopeLine: 539, isOptimized: false, unit: !2, variables: !4)
!238 = !DILocation(line: 540, column: 9, scope: !237)
!239 = !DILocation(line: 540, column: 5, scope: !237)
!240 = distinct !DISubprogram(name: "testA9Helper", linkageName: "testA9Helper", scope: !3, file: !3, line: 532, type: !6, isLocal: false, isDefinition: true, scopeLine: 532, isOptimized: false, unit: !2, variables: !4)
!241 = !DILocation(line: 533, column: 11, scope: !240)
!242 = !DILocalVariable(name: "a9", scope: !240, file: !3, line: 533, type: !8)
!243 = !DILocation(line: 533, column: 5, scope: !240)
!244 = !DILocation(line: 536, column: 5, scope: !240)
!245 = distinct !DISubprogram(name: "testStruct1", linkageName: "testStruct1", scope: !3, file: !3, line: 594, type: !6, isLocal: false, isDefinition: true, scopeLine: 594, isOptimized: false, unit: !2, variables: !4)
!246 = !DILocation(line: 597, column: 2, scope: !245)
!247 = distinct !DISubprogram(name: "testStruct2", linkageName: "testStruct2", scope: !3, file: !3, line: 600, type: !6, isLocal: false, isDefinition: true, scopeLine: 600, isOptimized: false, unit: !2, variables: !4)
!248 = !DILocation(line: 603, column: 2, scope: !247)
!249 = distinct !DISubprogram(name: "testNestedCall", linkageName: "testNestedCall", scope: !3, file: !3, line: 569, type: !6, isLocal: false, isDefinition: true, scopeLine: 569, isOptimized: false, unit: !2, variables: !4)
!250 = !DILocation(line: 572, column: 11, scope: !249)
!251 = !DILocation(line: 572, column: 5, scope: !249)
!252 = !DILocalVariable(name: "p", scope: !249, file: !3, line: 570, type: !140)
!253 = !DILocation(line: 570, column: 5, scope: !249)
!254 = !DILocation(line: 573, column: 66, scope: !249)
!255 = !DILocation(line: 573, column: 57, scope: !249)
!256 = !DILocation(line: 573, column: 48, scope: !249)
!257 = !DILocation(line: 573, column: 39, scope: !249)
!258 = !DILocation(line: 573, column: 30, scope: !249)
!259 = !DILocation(line: 573, column: 21, scope: !249)
!260 = !DILocation(line: 573, column: 9, scope: !249)
!261 = !DILocation(line: 573, column: 5, scope: !249)
!262 = distinct !DISubprogram(name: "point_id", linkageName: "point_id", scope: !3, file: !3, line: 567, type: !157, isLocal: false, isDefinition: true, scopeLine: 567, isOptimized: false, unit: !2, variables: !4)
!263 = !DILocation(line: 567, column: 25, scope: !262)
!264 = distinct !DISubprogram(name: "point_get_x", linkageName: "point_get_x", scope: !3, file: !3, line: 565, type: !265, isLocal: false, isDefinition: true, scopeLine: 565, isOptimized: false, unit: !2, variables: !4)
!265 = !DISubroutineType(types: !266)
!266 = !{!8, !140}
!267 = !DILocation(line: 565, column: 1, scope: !264)
!268 = !DILocation(line: 565, column: 28, scope: !264)
!269 = distinct !DISubprogram(name: "testDerefParam", linkageName: "testDerefParam", scope: !3, file: !3, line: 576, type: !6, isLocal: false, isDefinition: true, scopeLine: 576, isOptimized: false, unit: !2, variables: !4)
!270 = !DILocation(line: 576, column: 1, scope: !269)
!271 = !DILocation(line: 577, column: 16, scope: !269)
!272 = !DILocation(line: 577, column: 9, scope: !269)
!273 = !DILocation(line: 577, column: 5, scope: !269)
!274 = distinct !DISubprogram(name: "deref", linkageName: "deref", scope: !3, file: !3, line: 556, type: !275, isLocal: false, isDefinition: true, scopeLine: 556, isOptimized: false, unit: !2, variables: !4)
!275 = !DISubroutineType(types: !276)
!276 = !{!8, !174}
!277 = !DILocation(line: 556, column: 1, scope: !274)
!278 = !DILocation(line: 557, column: 5, scope: !274)
!279 = distinct !DISubprogram(name: "testBug1", linkageName: "testBug1", scope: !3, file: !3, line: 543, type: !280, isLocal: false, isDefinition: true, scopeLine: 543, isOptimized: false, unit: !2, variables: !4)
!280 = !DISubroutineType(types: !281)
!281 = !{!206}
!282 = !DILocation(line: 543, column: 1, scope: !279)
!283 = !DILocation(line: 544, column: 5, scope: !279)
!284 = !DILocation(line: 545, column: 11, scope: !279)
!285 = !DILocation(line: 545, column: 5, scope: !279)
!286 = !DILocalVariable(name: "q", scope: !279, file: !3, line: 547, type: !287)
!287 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !288, size: 64, align: 64)
!288 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !140, size: 64, align: 64)
!289 = !DILocation(line: 547, column: 5, scope: !279)
!290 = !DILocation(line: 548, column: 5, scope: !279)
!291 = !DILocalVariable(name: "qq1", scope: !279, file: !3, line: 548, type: !288)
!292 = !DILocation(line: 549, column: 5, scope: !279)
!293 = !DILocation(line: 551, column: 5, scope: !279)
!294 = !DILocation(line: 552, column: 12, scope: !279)
!295 = !DILocation(line: 552, column: 19, scope: !279)
!296 = !DILocation(line: 552, column: 5, scope: !279)
!297 = !DILocation(line: 553, column: 21, scope: !279)
!298 = !DILocation(line: 553, column: 5, scope: !279)
!299 = distinct !DISubprogram(name: "testDoubleDeref", linkageName: "testDoubleDeref", scope: !3, file: !3, line: 521, type: !6, isLocal: false, isDefinition: true, scopeLine: 521, isOptimized: false, unit: !2, variables: !4)
!300 = !DILocation(line: 521, column: 1, scope: !299)
!301 = !DILocation(line: 522, column: 5, scope: !299)
!302 = !DILocation(line: 523, column: 11, scope: !299)
!303 = !DILocation(line: 523, column: 5, scope: !299)
!304 = !DILocalVariable(name: "q", scope: !299, file: !3, line: 525, type: !288)
!305 = !DILocation(line: 525, column: 5, scope: !299)
!306 = !DILocation(line: 526, column: 5, scope: !299)
!307 = !DILocation(line: 527, column: 5, scope: !299)
!308 = !DILocation(line: 529, column: 5, scope: !299)
!309 = distinct !DISubprogram(name: "more_struct", linkageName: "more_struct", scope: !3, file: !3, line: 508, type: !6, isLocal: false, isDefinition: true, scopeLine: 508, isOptimized: false, unit: !2, variables: !4)
!310 = !DILocation(line: 508, column: 1, scope: !309)
!311 = !DILocation(line: 509, column: 5, scope: !309)
!312 = !DILocation(line: 510, column: 5, scope: !309)
!313 = !DILocalVariable(name: "psx", scope: !309, file: !3, line: 510, type: !314)
!314 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !315, size: 64, align: 64)
!315 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !316, size: 64, align: 64)
!316 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !317, size: 64, align: 64)
!317 = !DICompositeType(tag: DW_TAG_structure_type, name: "composite_type", file: !3, size: 128, align: 8, elements: !318)
!318 = !{!319, !320}
!319 = !DIDerivedType(tag: DW_TAG_member, name: "p1", file: !3, line: 582, baseType: !8, size: 64)
!320 = !DIDerivedType(tag: DW_TAG_member, name: "p2", file: !3, line: 583, baseType: !8, size: 64, offset: 64)
!321 = !DILocation(line: 511, column: 14, scope: !309)
!322 = !DILocation(line: 511, column: 5, scope: !309)
!323 = !DILocation(line: 513, column: 5, scope: !309)
!324 = !DILocation(line: 514, column: 17, scope: !309)
!325 = !DILocation(line: 514, column: 5, scope: !309)
!326 = !DILocation(line: 516, column: 5, scope: !309)
!327 = !DILocation(line: 518, column: 5, scope: !309)
!328 = distinct !DISubprogram(name: "test_S2_stuff", linkageName: "test_S2_stuff", scope: !3, file: !3, line: 491, type: !6, isLocal: false, isDefinition: true, scopeLine: 491, isOptimized: false, unit: !2, variables: !4)
!329 = !DILocation(line: 493, column: 16, scope: !328)
!330 = !DILocation(line: 497, column: 5, scope: !328)
!331 = distinct !DISubprogram(name: "testSetFromParam", linkageName: "testSetFromParam", scope: !3, file: !3, line: 480, type: !6, isLocal: false, isDefinition: true, scopeLine: 480, isOptimized: false, unit: !2, variables: !4)
!332 = !DILocation(line: 480, column: 1, scope: !331)
!333 = !DILocalVariable(name: "a", scope: !331, file: !3, line: 481, type: !8)
!334 = !DILocation(line: 481, column: 5, scope: !331)
!335 = !DILocation(line: 481, column: 15, scope: !331)
!336 = !DILocation(line: 482, column: 5, scope: !331)
!337 = !DILocation(line: 483, column: 5, scope: !331)
!338 = distinct !DISubprogram(name: "set", linkageName: "set", scope: !3, file: !3, line: 486, type: !275, isLocal: false, isDefinition: true, scopeLine: 486, isOptimized: false, unit: !2, variables: !4)
!339 = !DILocation(line: 487, column: 5, scope: !338)
!340 = !DILocation(line: 488, column: 5, scope: !338)
!341 = distinct !DISubprogram(name: "testSetter", linkageName: "testSetter", scope: !3, file: !3, line: 472, type: !6, isLocal: false, isDefinition: true, scopeLine: 472, isOptimized: false, unit: !2, variables: !4)
!342 = !DILocation(line: 472, column: 1, scope: !341)
!343 = !DILocation(line: 473, column: 5, scope: !341)
!344 = !DILocation(line: 474, column: 11, scope: !341)
!345 = !DILocation(line: 474, column: 5, scope: !341)
!346 = !DILocation(line: 475, column: 11, scope: !341)
!347 = !DILocation(line: 475, column: 5, scope: !341)
!348 = !DILocation(line: 476, column: 5, scope: !341)
!349 = !DILocation(line: 477, column: 5, scope: !341)
!350 = distinct !DISubprogram(name: "set_y", linkageName: "set_y", scope: !3, file: !3, line: 467, type: !351, isLocal: false, isDefinition: true, scopeLine: 467, isOptimized: false, unit: !2, variables: !4)
!351 = !DISubroutineType(types: !352)
!352 = !{!8, !288, !8}
!353 = !DILocation(line: 467, column: 1, scope: !350)
!354 = !DILocation(line: 468, column: 5, scope: !350)
!355 = !DILocation(line: 469, column: 5, scope: !350)
!356 = distinct !DISubprogram(name: "testLinkedList", linkageName: "testLinkedList", scope: !3, file: !3, line: 459, type: !6, isLocal: false, isDefinition: true, scopeLine: 459, isOptimized: false, unit: !2, variables: !4)
!357 = !DILocation(line: 459, column: 1, scope: !356)
!358 = !DILocation(line: 460, column: 5, scope: !356)
!359 = !DILocation(line: 461, column: 15, scope: !356)
!360 = !DILocation(line: 461, column: 5, scope: !356)
!361 = !DILocation(line: 462, column: 5, scope: !356)
!362 = !DILocation(line: 464, column: 5, scope: !356)
!363 = distinct !DISubprogram(name: "testWeirdLinkedList", linkageName: "testWeirdLinkedList", scope: !3, file: !3, line: 431, type: !6, isLocal: false, isDefinition: true, scopeLine: 431, isOptimized: false, unit: !2, variables: !4)
!364 = !DILocation(line: 431, column: 1, scope: !363)
!365 = !DILocation(line: 432, column: 5, scope: !363)
!366 = !DILocation(line: 433, column: 15, scope: !363)
!367 = !DILocation(line: 433, column: 5, scope: !363)
!368 = !DILocation(line: 434, column: 5, scope: !363)
!369 = !DILocation(line: 436, column: 55, scope: !363)
!370 = !DILocation(line: 436, column: 5, scope: !363)
!371 = !DILocation(line: 438, column: 9, scope: !363)
!372 = !DILocation(line: 438, column: 5, scope: !363)
!373 = distinct !DISubprogram(name: "testMoreDotsAndDerefs", linkageName: "testMoreDotsAndDerefs", scope: !3, file: !3, line: 446, type: !6, isLocal: false, isDefinition: true, scopeLine: 446, isOptimized: false, unit: !2, variables: !4)
!374 = !DILocation(line: 451, column: 5, scope: !373)
!375 = distinct !DISubprogram(name: "test23", linkageName: "test23", scope: !3, file: !3, line: 684, type: !280, isLocal: false, isDefinition: true, scopeLine: 684, isOptimized: false, unit: !2, variables: !4)
!376 = !DILocation(line: 684, column: 1, scope: !375)
!377 = !DILocation(line: 705, column: 5, scope: !375)
!378 = !DILocation(line: 706, column: 5, scope: !375)
!379 = !DILocation(line: 707, column: 5, scope: !375)
!380 = !DILocation(line: 708, column: 5, scope: !375)
!381 = !DILocation(line: 710, column: 17, scope: !375)
!382 = !DILocation(line: 710, column: 5, scope: !375)
!383 = !DILocation(line: 711, column: 17, scope: !375)
!384 = !DILocation(line: 711, column: 5, scope: !375)
!385 = !DILocation(line: 712, column: 17, scope: !375)
!386 = !DILocation(line: 712, column: 5, scope: !375)
!387 = !DILocation(line: 713, column: 19, scope: !375)
!388 = !DILocation(line: 713, column: 5, scope: !375)
!389 = !DILocation(line: 714, column: 14, scope: !375)
!390 = !DILocation(line: 714, column: 5, scope: !375)
!391 = !DILocation(line: 716, column: 5, scope: !375)
!392 = !DILocation(line: 717, column: 5, scope: !375)
!393 = !DILocalVariable(name: "baz", scope: !375, file: !3, line: 707, type: !140)
!394 = !DILocation(line: 718, column: 5, scope: !375)
!395 = !DILocation(line: 720, column: 5, scope: !375)
!396 = !DILocalVariable(name: "bar", scope: !375, file: !3, line: 706, type: !397)
!397 = !DICompositeType(tag: DW_TAG_structure_type, name: "composite_type", file: !3, size: 192, align: 8, elements: !4)
!398 = !DILocation(line: 723, column: 5, scope: !375)
!399 = !DILocation(line: 725, column: 12, scope: !375)
!400 = !DILocation(line: 725, column: 34, scope: !375)
!401 = !DILocation(line: 725, column: 5, scope: !375)
!402 = !DILocation(line: 727, column: 12, scope: !375)
!403 = !DILocation(line: 727, column: 5, scope: !375)
!404 = !DILocation(line: 728, column: 12, scope: !375)
!405 = !DILocation(line: 728, column: 5, scope: !375)
!406 = !DILocation(line: 729, column: 12, scope: !375)
!407 = !DILocation(line: 729, column: 5, scope: !375)
!408 = !DILocation(line: 730, column: 12, scope: !375)
!409 = !DILocation(line: 730, column: 5, scope: !375)
!410 = distinct !DISubprogram(name: "anon", linkageName: "anon", scope: !3, file: !3, line: 714, type: !6, isLocal: false, isDefinition: true, scopeLine: 714, isOptimized: false, unit: !2, variables: !4)
!411 = !DILocation(line: 714, column: 21, scope: !410)
!412 = distinct !DISubprogram(name: "setComplexBarFromParam", linkageName: "setComplexBarFromParam", scope: !3, file: !3, line: 720, type: !413, isLocal: false, isDefinition: true, scopeLine: 720, isOptimized: false, unit: !2, variables: !4)
!413 = !DISubroutineType(types: !414)
!414 = !{!206, !397}
!415 = !DILocation(line: 720, column: 5, scope: !412)
!416 = !DILocation(line: 721, column: 31, scope: !412)
!417 = !DILocation(line: 721, column: 9, scope: !412)
!418 = distinct !DISubprogram(name: "test24", linkageName: "test24", scope: !3, file: !3, line: 362, type: !280, isLocal: false, isDefinition: true, scopeLine: 362, isOptimized: false, unit: !2, variables: !4)
!419 = !DILocation(line: 362, column: 1, scope: !418)
!420 = !DILocation(line: 383, column: 5, scope: !418)
!421 = !DILocation(line: 384, column: 17, scope: !418)
!422 = !DILocation(line: 384, column: 5, scope: !418)
!423 = !DILocation(line: 386, column: 5, scope: !418)
!424 = !DILocation(line: 387, column: 17, scope: !418)
!425 = !DILocation(line: 387, column: 5, scope: !418)
!426 = !DILocation(line: 389, column: 5, scope: !418)
!427 = !DILocation(line: 390, column: 17, scope: !418)
!428 = !DILocation(line: 390, column: 5, scope: !418)
!429 = !DILocation(line: 392, column: 5, scope: !418)
!430 = !DILocation(line: 393, column: 17, scope: !418)
!431 = !DILocation(line: 393, column: 5, scope: !418)
!432 = !DILocation(line: 395, column: 5, scope: !418)
!433 = !DILocation(line: 396, column: 5, scope: !418)
!434 = !DILocation(line: 397, column: 5, scope: !418)
!435 = !DILocalVariable(name: "baz", scope: !418, file: !3, line: 389, type: !140)
!436 = !DILocation(line: 398, column: 5, scope: !418)
!437 = !DILocation(line: 400, column: 29, scope: !418)
!438 = !DILocation(line: 400, column: 5, scope: !418)
!439 = !DILocation(line: 401, column: 12, scope: !418)
!440 = !DILocation(line: 401, column: 37, scope: !418)
!441 = !DILocation(line: 401, column: 5, scope: !418)
!442 = !DILocation(line: 402, column: 12, scope: !418)
!443 = !DILocation(line: 402, column: 33, scope: !418)
!444 = !DILocation(line: 402, column: 5, scope: !418)
!445 = !DILocation(line: 403, column: 12, scope: !418)
!446 = !DILocation(line: 403, column: 29, scope: !418)
!447 = !DILocation(line: 403, column: 5, scope: !418)
!448 = !DILocation(line: 404, column: 25, scope: !418)
!449 = !DILocation(line: 404, column: 5, scope: !418)
!450 = !DILocation(line: 406, column: 25, scope: !418)
!451 = !DILocation(line: 406, column: 5, scope: !418)
!452 = !DILocation(line: 407, column: 12, scope: !418)
!453 = !DILocation(line: 407, column: 37, scope: !418)
!454 = !DILocation(line: 407, column: 5, scope: !418)
!455 = !DILocation(line: 408, column: 12, scope: !418)
!456 = !DILocation(line: 408, column: 33, scope: !418)
!457 = !DILocation(line: 408, column: 5, scope: !418)
!458 = !DILocation(line: 409, column: 12, scope: !418)
!459 = !DILocation(line: 409, column: 29, scope: !418)
!460 = !DILocation(line: 409, column: 5, scope: !418)
!461 = !DILocation(line: 410, column: 25, scope: !418)
!462 = !DILocation(line: 410, column: 5, scope: !418)
!463 = !DILocation(line: 412, column: 5, scope: !418)
!464 = !DILocation(line: 413, column: 12, scope: !418)
!465 = !DILocation(line: 413, column: 37, scope: !418)
!466 = !DILocation(line: 413, column: 5, scope: !418)
!467 = !DILocation(line: 414, column: 12, scope: !418)
!468 = !DILocation(line: 414, column: 33, scope: !418)
!469 = !DILocation(line: 414, column: 5, scope: !418)
!470 = !DILocation(line: 415, column: 12, scope: !418)
!471 = !DILocation(line: 415, column: 29, scope: !418)
!472 = !DILocation(line: 415, column: 5, scope: !418)
!473 = !DILocation(line: 416, column: 25, scope: !418)
!474 = !DILocation(line: 416, column: 5, scope: !418)
!475 = !DILocation(line: 418, column: 125, scope: !418)
!476 = !DILocation(line: 418, column: 5, scope: !418)
!477 = !DILocation(line: 419, column: 12, scope: !418)
!478 = !DILocation(line: 419, column: 181, scope: !418)
!479 = !DILocation(line: 419, column: 5, scope: !418)
!480 = distinct !DISubprogram(name: "test25", linkageName: "test25", scope: !3, file: !3, line: 632, type: !280, isLocal: false, isDefinition: true, scopeLine: 632, isOptimized: false, unit: !2, variables: !4)
!481 = !DILocation(line: 632, column: 1, scope: !480)
!482 = !DILocation(line: 652, column: 5, scope: !480)
!483 = !DILocation(line: 653, column: 5, scope: !480)
!484 = !DILocation(line: 654, column: 5, scope: !480)
!485 = !DILocation(line: 655, column: 5, scope: !480)
!486 = !DILocation(line: 657, column: 17, scope: !480)
!487 = !DILocation(line: 657, column: 5, scope: !480)
!488 = !DILocation(line: 658, column: 17, scope: !480)
!489 = !DILocation(line: 658, column: 5, scope: !480)
!490 = !DILocation(line: 659, column: 17, scope: !480)
!491 = !DILocation(line: 659, column: 5, scope: !480)
!492 = !DILocation(line: 660, column: 19, scope: !480)
!493 = !DILocation(line: 660, column: 5, scope: !480)
!494 = !DILocation(line: 662, column: 5, scope: !480)
!495 = !DILocation(line: 663, column: 5, scope: !480)
!496 = !DILocation(line: 664, column: 5, scope: !480)
!497 = !DILocation(line: 670, column: 31, scope: !480)
!498 = !DILocation(line: 670, column: 5, scope: !480)
!499 = !DILocation(line: 672, column: 12, scope: !480)
!500 = !DILocation(line: 672, column: 39, scope: !480)
!501 = !DILocation(line: 672, column: 5, scope: !480)
!502 = !DILocation(line: 673, column: 12, scope: !480)
!503 = !DILocation(line: 673, column: 35, scope: !480)
!504 = !DILocation(line: 673, column: 5, scope: !480)
!505 = !DILocation(line: 674, column: 12, scope: !480)
!506 = !DILocation(line: 674, column: 31, scope: !480)
!507 = !DILocation(line: 674, column: 5, scope: !480)
!508 = !DILocation(line: 675, column: 12, scope: !480)
!509 = !DILocation(line: 675, column: 27, scope: !480)
!510 = !DILocation(line: 675, column: 5, scope: !480)
!511 = !DILocation(line: 677, column: 27, scope: !480)
!512 = !DILocation(line: 677, column: 5, scope: !480)
!513 = !DILocation(line: 678, column: 12, scope: !480)
!514 = !DILocation(line: 678, column: 39, scope: !480)
!515 = !DILocation(line: 678, column: 5, scope: !480)
!516 = !DILocation(line: 679, column: 12, scope: !480)
!517 = !DILocation(line: 679, column: 35, scope: !480)
!518 = !DILocation(line: 679, column: 5, scope: !480)
!519 = !DILocation(line: 680, column: 12, scope: !480)
!520 = !DILocation(line: 680, column: 31, scope: !480)
!521 = !DILocation(line: 680, column: 5, scope: !480)
!522 = !DILocation(line: 681, column: 12, scope: !480)
!523 = !DILocation(line: 681, column: 27, scope: !480)
!524 = !DILocation(line: 681, column: 5, scope: !480)
!525 = distinct !DISubprogram(name: "fib", linkageName: "fib", scope: !3, file: !3, line: 588, type: !526, isLocal: false, isDefinition: true, scopeLine: 588, isOptimized: false, unit: !2, variables: !4)
!526 = !DISubroutineType(types: !527)
!527 = !{!8, !67}
!528 = !DILocation(line: 589, column: 13, scope: !525)
!529 = !DILocation(line: 589, column: 17, scope: !525)
!530 = !DILocation(line: 591, column: 17, scope: !525)
!531 = !DILocation(line: 591, column: 9, scope: !525)
!532 = !DILocation(line: 591, column: 30, scope: !525)
!533 = !DILocation(line: 591, column: 22, scope: !525)
!534 = !DILocation(line: 591, column: 5, scope: !525)
!535 = distinct !DISubprogram(name: "stupid_sqrt", linkageName: "stupid_sqrt", scope: !3, file: !3, line: 347, type: !211, isLocal: false, isDefinition: true, scopeLine: 347, isOptimized: false, unit: !2, variables: !4)
!536 = !DILocation(line: 348, column: 13, scope: !535)
!537 = !DILocation(line: 349, column: 9, scope: !535)
!538 = distinct !DISubprogram(name: "id", linkageName: "id", scope: !3, file: !3, line: 335, type: !539, isLocal: false, isDefinition: true, scopeLine: 335, isOptimized: false, unit: !2, variables: !4)
!539 = !DISubroutineType(types: !540)
!540 = !{!67, !67}
!541 = !DILocation(line: 336, column: 5, scope: !538)
!542 = distinct !DISubprogram(name: "id", linkageName: "id", scope: !3, file: !3, line: 335, type: !157, isLocal: false, isDefinition: true, scopeLine: 335, isOptimized: false, unit: !2, variables: !4)
!543 = !DILocation(line: 336, column: 5, scope: !542)
!544 = distinct !DISubprogram(name: "deref_ct", linkageName: "deref_ct", scope: !3, file: !3, line: 339, type: !275, isLocal: false, isDefinition: true, scopeLine: 339, isOptimized: false, unit: !2, variables: !4)
!545 = !DILocation(line: 339, column: 1, scope: !544)
!546 = !DILocation(line: 340, column: 5, scope: !544)
!547 = distinct !DISubprogram(name: "add_static", linkageName: "add_static", scope: !3, file: !3, line: 343, type: !539, isLocal: false, isDefinition: true, scopeLine: 343, isOptimized: false, unit: !2, variables: !4)
!548 = !DILocation(line: 97, column: 27, scope: !547)
!549 = !DILocation(line: 344, column: 5, scope: !547)
!550 = distinct !DISubprogram(name: "apply_static", linkageName: "apply_static", scope: !3, file: !3, line: 327, type: !539, isLocal: false, isDefinition: true, scopeLine: 327, isOptimized: false, unit: !2, variables: !4)
!551 = !DILocation(line: 328, column: 9, scope: !550)
!552 = !DILocation(line: 328, column: 5, scope: !550)
!553 = distinct !DISubprogram(name: "poly_inc", linkageName: "poly_inc", scope: !3, file: !3, line: 331, type: !539, isLocal: false, isDefinition: true, scopeLine: 331, isOptimized: false, unit: !2, variables: !4)
!554 = !DILocation(line: 332, column: 13, scope: !553)
!555 = !DILocation(line: 332, column: 5, scope: !553)
!556 = distinct !DISubprogram(name: "apply_static", linkageName: "apply_static", scope: !3, file: !3, line: 327, type: !539, isLocal: false, isDefinition: true, scopeLine: 327, isOptimized: false, unit: !2, variables: !4)
!557 = !DILocation(line: 328, column: 9, scope: !556)
!558 = !DILocation(line: 328, column: 5, scope: !556)
!559 = distinct !DISubprogram(name: "anon", linkageName: "anon", scope: !3, file: !3, line: 101, type: !539, isLocal: false, isDefinition: true, scopeLine: 101, isOptimized: false, unit: !2, variables: !4)
!560 = !DILocation(line: 101, column: 47, scope: !559)
!561 = !DILocation(line: 101, column: 39, scope: !559)
!562 = distinct !DISubprogram(name: "id", linkageName: "id", scope: !3, file: !3, line: 335, type: !157, isLocal: false, isDefinition: true, scopeLine: 335, isOptimized: false, unit: !2, variables: !4)
!563 = !DILocation(line: 336, column: 5, scope: !562)
!564 = distinct !DISubprogram(name: "testArray1", linkageName: "testArray1", scope: !3, file: !3, line: 241, type: !280, isLocal: false, isDefinition: true, scopeLine: 241, isOptimized: false, unit: !2, variables: !4)
!565 = !DILocation(line: 241, column: 1, scope: !564)
!566 = !DILocalVariable(name: "count", scope: !564, file: !3, line: 242, type: !67)
!567 = !DILocation(line: 242, column: 5, scope: !564)
!568 = !DILocation(line: 243, column: 37, scope: !564)
!569 = !DILocation(line: 1, column: 1, scope: !564)
!570 = !DILocalVariable(name: "a", scope: !564, file: !3, line: 243, type: !171)
!571 = !DILocation(line: 243, column: 5, scope: !564)
!572 = !DILocation(line: 245, column: 14, scope: !564)
!573 = !DILocation(line: 245, column: 20, scope: !564)
!574 = !DILocation(line: 245, column: 5, scope: !564)
!575 = !DILocation(line: 246, column: 17, scope: !564)
!576 = !DILocation(line: 246, column: 24, scope: !564)
!577 = !DILocation(line: 246, column: 5, scope: !564)
!578 = !DILocation(line: 247, column: 18, scope: !564)
!579 = !DILocation(line: 247, column: 24, scope: !564)
!580 = !DILocation(line: 247, column: 5, scope: !564)
!581 = !DILocation(line: 249, column: 5, scope: !564)
!582 = !DILocation(line: 250, column: 5, scope: !564)
!583 = !DILocalVariable(name: "p2", scope: !564, file: !3, line: 252, type: !174)
!584 = !DILocation(line: 252, column: 5, scope: !564)
!585 = !DILocation(line: 253, column: 5, scope: !564)
!586 = !DILocation(line: 255, column: 5, scope: !564)
!587 = distinct !DISubprogram(name: "testArray2", linkageName: "testArray2", scope: !3, file: !3, line: 258, type: !280, isLocal: false, isDefinition: true, scopeLine: 258, isOptimized: false, unit: !2, variables: !4)
!588 = !DILocation(line: 258, column: 1, scope: !587)
!589 = !DILocalVariable(name: "count", scope: !587, file: !3, line: 259, type: !67)
!590 = !DILocation(line: 259, column: 5, scope: !587)
!591 = !DILocation(line: 261, column: 57, scope: !587)
!592 = !DILocation(line: 1, column: 1, scope: !587)
!593 = !DILocalVariable(name: "point_array", scope: !587, file: !3, line: 261, type: !594)
!594 = !DICompositeType(tag: DW_TAG_structure_type, name: "composite_type", file: !3, size: 128, align: 8, elements: !595)
!595 = !{!596, !175}
!596 = !DIDerivedType(tag: DW_TAG_member, name: "data", file: !3, line: 1, baseType: !288, size: 64, align: 64)
!597 = !DILocation(line: 261, column: 5, scope: !587)
!598 = !DILocation(line: 263, column: 24, scope: !587)
!599 = !DILocation(line: 263, column: 32, scope: !587)
!600 = !DILocation(line: 263, column: 5, scope: !587)
!601 = !DILocalVariable(name: "e1", scope: !587, file: !3, line: 266, type: !67)
!602 = !DILocation(line: 266, column: 5, scope: !587)
!603 = !DILocalVariable(name: "e2", scope: !587, file: !3, line: 267, type: !67)
!604 = !DILocation(line: 267, column: 5, scope: !587)
!605 = !DILocalVariable(name: "e3", scope: !587, file: !3, line: 268, type: !67)
!606 = !DILocation(line: 268, column: 5, scope: !587)
!607 = !DILocation(line: 270, column: 27, scope: !587)
!608 = !DILocation(line: 270, column: 5, scope: !587)
!609 = distinct !DISubprogram(name: "testArray3", linkageName: "testArray3", scope: !3, file: !3, line: 273, type: !280, isLocal: false, isDefinition: true, scopeLine: 273, isOptimized: false, unit: !2, variables: !4)
!610 = !DILocation(line: 273, column: 1, scope: !609)
!611 = !DILocalVariable(name: "count", scope: !609, file: !3, line: 274, type: !67)
!612 = !DILocation(line: 274, column: 5, scope: !609)
!613 = !DILocalVariable(name: "e1", scope: !609, file: !3, line: 275, type: !67)
!614 = !DILocation(line: 275, column: 5, scope: !609)
!615 = !DILocalVariable(name: "e2", scope: !609, file: !3, line: 276, type: !67)
!616 = !DILocation(line: 276, column: 5, scope: !609)
!617 = !DILocalVariable(name: "e3", scope: !609, file: !3, line: 277, type: !67)
!618 = !DILocation(line: 277, column: 5, scope: !609)
!619 = !DILocation(line: 279, column: 5, scope: !609)
!620 = !DILocation(line: 280, column: 5, scope: !609)
!621 = !DILocalVariable(name: "arr", scope: !609, file: !3, line: 279, type: !622)
!622 = !DICompositeType(tag: DW_TAG_structure_type, name: "composite_type", file: !3, size: 128, align: 8, elements: !623)
!623 = !{!624, !175}
!624 = !DIDerivedType(tag: DW_TAG_member, name: "data", file: !3, line: 1, baseType: !625, size: 64, align: 64)
!625 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !67, size: 64, align: 64)
!626 = !DILocalVariable(name: "copied", scope: !609, file: !3, line: 280, type: !622)
!627 = !DILocation(line: 1, column: 1, scope: !609)
!628 = !DILocation(line: 282, column: 16, scope: !609)
!629 = !DILocation(line: 282, column: 22, scope: !609)
!630 = !DILocation(line: 282, column: 5, scope: !609)
!631 = !DILocation(line: 283, column: 19, scope: !609)
!632 = !DILocation(line: 283, column: 25, scope: !609)
!633 = !DILocation(line: 283, column: 5, scope: !609)
!634 = !DILocation(line: 285, column: 9, scope: !609)
!635 = !DILocation(line: 285, column: 5, scope: !609)
!636 = !DILocation(line: 286, column: 5, scope: !609)
!637 = !DILocation(line: 287, column: 19, scope: !609)
!638 = !DILocation(line: 287, column: 25, scope: !609)
!639 = !DILocation(line: 287, column: 5, scope: !609)
!640 = !DILocation(line: 289, column: 5, scope: !609)
!641 = !DILocation(line: 290, column: 16, scope: !609)
!642 = !DILocation(line: 290, column: 22, scope: !609)
!643 = !DILocation(line: 290, column: 5, scope: !609)
!644 = !DILocation(line: 291, column: 19, scope: !609)
!645 = !DILocation(line: 291, column: 25, scope: !609)
!646 = !DILocation(line: 291, column: 5, scope: !609)
!647 = distinct !DISubprogram(name: "testArray4", linkageName: "testArray4", scope: !3, file: !3, line: 294, type: !280, isLocal: false, isDefinition: true, scopeLine: 294, isOptimized: false, unit: !2, variables: !4)
!648 = !DILocation(line: 294, column: 1, scope: !647)
!649 = !DILocalVariable(name: "count", scope: !647, file: !3, line: 295, type: !67)
!650 = !DILocation(line: 295, column: 5, scope: !647)
!651 = !DILocation(line: 302, column: 25, scope: !647)
!652 = !DILocation(line: 302, column: 30, scope: !647)
!653 = !DILocation(line: 303, column: 5, scope: !647)
!654 = !DILocalVariable(name: "py", scope: !647, file: !3, line: 303, type: !655)
!655 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !174, size: 64, align: 64)
!656 = !DIExpression(DW_OP_plus_uconst, 8, DW_OP_deref, DW_OP_stack_value)
!657 = !DILocation(line: 305, column: 27, scope: !647)
!658 = !DILocation(line: 305, column: 32, scope: !647)
!659 = !DILocation(line: 305, column: 42, scope: !647)
!660 = !DILocation(line: 305, column: 47, scope: !647)
!661 = !DILocation(line: 307, column: 9, scope: !647)
!662 = !DILocation(line: 307, column: 5, scope: !647)
!663 = !DILocation(line: 308, column: 12, scope: !647)
!664 = !DILocation(line: 308, column: 20, scope: !647)
!665 = !DILocation(line: 308, column: 5, scope: !647)
!666 = distinct !DISubprogram(name: "testArray5", linkageName: "testArray5", scope: !3, file: !3, line: 311, type: !280, isLocal: false, isDefinition: true, scopeLine: 311, isOptimized: false, unit: !2, variables: !4)
!667 = !DILocation(line: 311, column: 1, scope: !666)
!668 = !DILocalVariable(name: "count", scope: !666, file: !3, line: 312, type: !67)
!669 = !DILocation(line: 312, column: 5, scope: !666)
!670 = !DILocation(line: 313, column: 30, scope: !666)
!671 = !DILocation(line: 1, column: 1, scope: !666)
!672 = !DILocalVariable(name: "a1", scope: !666, file: !3, line: 313, type: !673)
!673 = !DICompositeType(tag: DW_TAG_structure_type, name: "composite_type", file: !3, size: 128, align: 8, elements: !674)
!674 = !{!675, !175}
!675 = !DIDerivedType(tag: DW_TAG_member, name: "data", file: !3, line: 1, baseType: !676, size: 64, align: 64)
!676 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !171, size: 64, align: 64)
!677 = !DILocation(line: 313, column: 5, scope: !666)
!678 = !DILocation(line: 314, column: 15, scope: !666)
!679 = !DILocation(line: 314, column: 18, scope: !666)
!680 = !DILocation(line: 314, column: 24, scope: !666)
!681 = !DILocation(line: 314, column: 5, scope: !666)
!682 = !DILocation(line: 316, column: 21, scope: !666)
!683 = !DILocation(line: 316, column: 24, scope: !666)
!684 = !DILocation(line: 316, column: 5, scope: !666)
!685 = !DILocation(line: 317, column: 23, scope: !666)
!686 = !DILocation(line: 317, column: 36, scope: !666)
!687 = !DILocation(line: 317, column: 49, scope: !666)
!688 = !DILocation(line: 318, column: 15, scope: !666)
!689 = !DILocation(line: 318, column: 18, scope: !666)
!690 = !DILocation(line: 318, column: 24, scope: !666)
!691 = !DILocation(line: 318, column: 5, scope: !666)
!692 = distinct !DISubprogram(name: "testWhile1", linkageName: "testWhile1", scope: !3, file: !3, line: 233, type: !280, isLocal: false, isDefinition: true, scopeLine: 233, isOptimized: false, unit: !2, variables: !4)
!693 = !DILocalVariable(name: "a", scope: !692, file: !3, line: 234, type: !8)
!694 = !DILocation(line: 234, column: 5, scope: !692)
!695 = !DILocation(line: 234, column: 10, scope: !692)
!696 = !DILocation(line: 235, column: 16, scope: !692)
!697 = !DILocation(line: 236, column: 17, scope: !692)
!698 = !DILocation(line: 236, column: 9, scope: !692)
!699 = !DILocation(line: 238, column: 5, scope: !692)
!700 = distinct !DISubprogram(name: "testMalloc", linkageName: "testMalloc", scope: !3, file: !3, line: 221, type: !280, isLocal: false, isDefinition: true, scopeLine: 221, isOptimized: false, unit: !2, variables: !4)
!701 = !DILocation(line: 221, column: 1, scope: !700)
!702 = !DILocation(line: 222, column: 10, scope: !700)
!703 = !DILocalVariable(name: "b", scope: !700, file: !3, line: 222, type: !288)
!704 = !DILocation(line: 222, column: 5, scope: !700)
!705 = !DILocation(line: 223, column: 11, scope: !700)
!706 = !DILocation(line: 223, column: 5, scope: !700)
!707 = !DILocation(line: 224, column: 11, scope: !700)
!708 = !DILocation(line: 224, column: 5, scope: !700)
!709 = !DILocation(line: 226, column: 10, scope: !700)
!710 = !DILocalVariable(name: "c", scope: !700, file: !3, line: 226, type: !288)
!711 = !DILocation(line: 226, column: 5, scope: !700)
!712 = !DILocation(line: 227, column: 11, scope: !700)
!713 = !DILocation(line: 227, column: 5, scope: !700)
!714 = !DILocation(line: 228, column: 11, scope: !700)
!715 = !DILocation(line: 228, column: 5, scope: !700)
!716 = !DILocation(line: 230, column: 19, scope: !700)
!717 = !DILocation(line: 230, column: 5, scope: !700)
!718 = distinct !DISubprogram(name: "alloc_pair", linkageName: "alloc_pair", scope: !3, file: !3, line: 217, type: !719, isLocal: false, isDefinition: true, scopeLine: 217, isOptimized: false, unit: !2, variables: !4)
!719 = !DISubroutineType(types: !720)
!720 = !{!288}
!721 = !DILocation(line: 218, column: 21, scope: !718)
!722 = !DILocation(line: 218, column: 5, scope: !718)
!723 = distinct !DISubprogram(name: "testFor1", linkageName: "testFor1", scope: !3, file: !3, line: 171, type: !280, isLocal: false, isDefinition: true, scopeLine: 171, isOptimized: false, unit: !2, variables: !4)
!724 = !DILocation(line: 171, column: 1, scope: !723)
!725 = !DILocalVariable(name: "product", scope: !723, file: !3, line: 172, type: !8)
!726 = !DILocation(line: 172, column: 5, scope: !723)
!727 = !DILocation(line: 173, column: 24, scope: !723)
!728 = !DILocation(line: 1, column: 1, scope: !723)
!729 = !DILocalVariable(name: "e", scope: !723, file: !3, line: 173, type: !8)
!730 = !DILocation(line: 173, column: 9, scope: !723)
!731 = !DILocation(line: 174, column: 19, scope: !723)
!732 = !DILocation(line: 176, column: 23, scope: !723)
!733 = !DILocation(line: 176, column: 5, scope: !723)
!734 = distinct !DISubprogram(name: "testFor2", linkageName: "testFor2", scope: !3, file: !3, line: 179, type: !280, isLocal: false, isDefinition: true, scopeLine: 179, isOptimized: false, unit: !2, variables: !4)
!735 = !DILocation(line: 179, column: 1, scope: !734)
!736 = !DILocation(line: 186, column: 36, scope: !734)
!737 = !DILocation(line: 187, column: 46, scope: !734)
!738 = !DILocation(line: 1, column: 1, scope: !734)
!739 = !DILocation(line: 187, column: 18, scope: !734)
!740 = !DILocation(line: 187, column: 5, scope: !734)
!741 = distinct !DISubprogram(name: "add_array", linkageName: "add_array", scope: !3, file: !3, line: 180, type: !742, isLocal: false, isDefinition: true, scopeLine: 180, isOptimized: false, unit: !2, variables: !4)
!742 = !DISubroutineType(types: !743)
!743 = !{!67, !622}
!744 = !DILocation(line: 180, column: 5, scope: !741)
!745 = !DILocalVariable(name: "sum", scope: !741, file: !3, line: 181, type: !67)
!746 = !DILocation(line: 181, column: 9, scope: !741)
!747 = !DILocation(line: 1, column: 1, scope: !741)
!748 = !DILocalVariable(name: "e", scope: !741, file: !3, line: 182, type: !67)
!749 = !DILocation(line: 182, column: 13, scope: !741)
!750 = !DILocation(line: 182, column: 28, scope: !741)
!751 = !DILocation(line: 183, column: 9, scope: !741)
!752 = distinct !DISubprogram(name: "testFor3", linkageName: "testFor3", scope: !3, file: !3, line: 190, type: !280, isLocal: false, isDefinition: true, scopeLine: 190, isOptimized: false, unit: !2, variables: !4)
!753 = !DILocation(line: 190, column: 1, scope: !752)
!754 = !DILocalVariable(name: "sum", scope: !752, file: !3, line: 191, type: !8)
!755 = !DILocation(line: 191, column: 5, scope: !752)
!756 = !DILocation(line: 192, column: 21, scope: !752)
!757 = !DILocation(line: 1, column: 1, scope: !752)
!758 = !DILocalVariable(name: "e", scope: !752, file: !3, line: 192, type: !8)
!759 = !DILocation(line: 192, column: 9, scope: !752)
!760 = !DILocation(line: 193, column: 25, scope: !752)
!761 = !DILocation(line: 198, column: 19, scope: !752)
!762 = !DILocation(line: 198, column: 5, scope: !752)
!763 = !DILocalVariable(name: "e", scope: !752, file: !3, line: 193, type: !8)
!764 = !DILocation(line: 193, column: 13, scope: !752)
!765 = !DILocation(line: 194, column: 19, scope: !752)
!766 = !DILocation(line: 196, column: 15, scope: !752)
!767 = distinct !DISubprogram(name: "testFor4", linkageName: "testFor4", scope: !3, file: !3, line: 201, type: !280, isLocal: false, isDefinition: true, scopeLine: 201, isOptimized: false, unit: !2, variables: !4)
!768 = !DILocation(line: 201, column: 1, scope: !767)
!769 = !DILocalVariable(name: "isum", scope: !767, file: !3, line: 202, type: !67)
!770 = !DILocation(line: 202, column: 5, scope: !767)
!771 = !DILocation(line: 203, column: 33, scope: !767)
!772 = !DILocation(line: 1, column: 1, scope: !767)
!773 = !DILocalVariable(name: "i", scope: !767, file: !3, line: 203, type: !67)
!774 = !DILocation(line: 203, column: 12, scope: !767)
!775 = !DILocation(line: 204, column: 16, scope: !767)
!776 = !DILocation(line: 206, column: 20, scope: !767)
!777 = !DILocation(line: 206, column: 5, scope: !767)
!778 = distinct !DISubprogram(name: "testUnion1", linkageName: "testUnion1", scope: !3, file: !3, line: 141, type: !280, isLocal: false, isDefinition: true, scopeLine: 141, isOptimized: false, unit: !2, variables: !4)
!779 = !DILocation(line: 141, column: 1, scope: !778)
!780 = !DILocation(line: 154, column: 5, scope: !778)
!781 = !DILocation(line: 156, column: 5, scope: !778)
!782 = !DILocation(line: 1, column: 1, scope: !778)
!783 = !DILocation(line: 158, column: 5, scope: !778)
!784 = !DILocation(line: 159, column: 5, scope: !778)
!785 = !DILocalVariable(name: "y", scope: !778, file: !3, line: 152, type: !786)
!786 = !DICompositeType(tag: DW_TAG_structure_type, name: "composite_type", file: !3, size: 64, align: 4, elements: !4)
!787 = !DILocation(line: 152, column: 5, scope: !778)
!788 = !DILocation(line: 161, column: 5, scope: !778)
!789 = !DILocation(line: 162, column: 5, scope: !778)
!790 = !DILocation(line: 164, column: 5, scope: !778)
!791 = !DILocation(line: 165, column: 5, scope: !778)
!792 = !DILocation(line: 167, column: 14, scope: !778)
!793 = !DILocation(line: 167, column: 5, scope: !778)
!794 = !DILocation(line: 168, column: 5, scope: !778)
!795 = distinct !DISubprogram(name: "testAnySimple1", linkageName: "testAnySimple1", scope: !3, file: !3, line: 804, type: !280, isLocal: false, isDefinition: true, scopeLine: 804, isOptimized: false, unit: !2, variables: !4)
!796 = !DILocation(line: 804, column: 1, scope: !795)
!797 = !DILocation(line: 1, column: 1, scope: !795)
!798 = !DILocation(line: 864, column: 29, scope: !795)
!799 = !DILocation(line: 867, column: 34, scope: !795)
!800 = !DILocation(line: 867, column: 50, scope: !795)
!801 = !DILocation(line: 867, column: 5, scope: !795)
!802 = distinct !DISubprogram(name: "testBuffer", linkageName: "testBuffer", scope: !3, file: !3, line: 786, type: !280, isLocal: false, isDefinition: true, scopeLine: 786, isOptimized: false, unit: !2, variables: !4)
!803 = !DILocation(line: 792, column: 10, scope: !802)
!804 = !DILocation(line: 796, column: 5, scope: !802)
!805 = distinct !DISubprogram(name: "buffer", linkageName: "buffer", scope: !3, file: !3, line: 733, type: !806, isLocal: false, isDefinition: true, scopeLine: 733, isOptimized: false, unit: !2, variables: !4)
!806 = !DISubroutineType(types: !807)
!807 = !{!808, !8}
!808 = !DICompositeType(tag: DW_TAG_structure_type, name: "composite_type", file: !3, size: 192, align: 8, elements: !809)
!809 = !{!810, !811}
!810 = !DIDerivedType(tag: DW_TAG_member, name: "capacity", file: !3, line: 1, baseType: !8, size: 64)
!811 = !DIDerivedType(tag: DW_TAG_member, name: "buf", file: !3, line: 1, baseType: !812, size: 128, align: 8, offset: 64)
!812 = !DICompositeType(tag: DW_TAG_structure_type, name: "composite_type", file: !3, size: 128, align: 8, elements: !813)
!813 = !{!814, !175}
!814 = !DIDerivedType(tag: DW_TAG_member, name: "data", file: !3, line: 1, baseType: !815, size: 64, align: 64)
!815 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !786, size: 64, align: 64)
!816 = !DILocation(line: 733, column: 1, scope: !805)
!817 = !DILocation(line: 734, column: 5, scope: !805)
!818 = !DILocation(line: 735, column: 32, scope: !805)
!819 = !DILocation(line: 735, column: 25, scope: !805)
!820 = !DILocation(line: 735, column: 5, scope: !805)
!821 = !DILocalVariable(name: "buf", scope: !805, file: !3, line: 734, type: !812)
!822 = !DILocation(line: 739, column: 5, scope: !805)
