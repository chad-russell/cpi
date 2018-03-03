; ModuleID = 'module'
source_filename = "module"

%0 = type <{ i64*, i64* }>
%1 = type <{ %0, %0, %0 }>
%2 = type <{ %0*, i32 }>
%3 = type <{ i64*, i64* }>
%4 = type <{ i64*, i64* }>

declare void @panic(i8*)

define i64 @main() {
entry:
  %count = alloca i32
  %local = alloca i64
  %local1 = alloca i64
  %p = alloca %0
  %py = alloca i64*
  %local2 = alloca i64
  %local3 = alloca i64
  %e1 = alloca %0
  %local4 = alloca %1
  %ab = alloca %2
  %a = alloca <{ %0*, i32 }>
  %local5 = alloca i64
  %h1 = alloca %0
  %h2 = alloca i64*
  %local6 = alloca i64
  %local7 = alloca i64
  %local8 = alloca %0*
  %local9 = alloca %0
  %local10 = alloca i64
  %local11 = alloca i64
  %local12 = alloca %0*
  %local13 = alloca i64
  store i32 3, i32* %count
  store i64 12, i64* %local
  %0 = insertvalue %3 zeroinitializer, i64* %local, 0
  store i64 8, i64* %local1
  %1 = insertvalue %3 %0, i64* %local1, 1
  %2 = bitcast %0* %p to %3*
  store %3 %1, %3* %2
  %3 = getelementptr %0, %0* %p, i32 0, i32 1
  %4 = load i64*, i64** %3
  store i64* %4, i64** %py
  store i64 0, i64* %local2
  %5 = insertvalue %4 zeroinitializer, i64* %local2, 0
  store i64 0, i64* %local3
  %6 = insertvalue %4 %5, i64* %local3, 1
  %7 = bitcast %0* %e1 to %4*
  store %4 %6, %4* %7
  %8 = load %0, %0* %e1
  %9 = insertvalue %1 zeroinitializer, %0 %8, 0
  %10 = load %0, %0* %p
  %11 = insertvalue %1 %9, %0 %10, 1
  %12 = load %0, %0* %e1
  %13 = insertvalue %1 %11, %0 %12, 2
  store %1 %13, %1* %local4
  store %1 %13, %1* %local4
  %14 = bitcast %1* %local4 to %0*
  %15 = insertvalue %2 zeroinitializer, %0* %14, 0
  %16 = load i32, i32* %count
  %17 = insertvalue %2 %15, i32 %16, 1
  store %2 %17, %2* %ab
  %18 = load %2, %2* %ab
  %19 = bitcast <{ %0*, i32 }>* %a to %2*
  store %2 %18, %2* %19
  %20 = load <{ %0*, i32 }>, <{ %0*, i32 }>* %a
  %21 = extractvalue <{ %0*, i32 }> %20, 0
  store i64 1, i64* %local5
  %22 = load i64, i64* %local5
  %23 = mul i64 %22, 8
  store i64 %23, i64* %local7
  %24 = load i64, i64* %local7
  %25 = ptrtoint %0* %21 to i64
  %26 = add i64 %25, %24
  %27 = inttoptr i64 %26 to %0*
  store %0* %27, %0** %local8
  %28 = load %0*, %0** %local8
  %29 = load %0, %0* %28
  store %0 %29, %0* %local9
  %30 = load %0, %0* %local9
  store %0 %30, %0* %h1
  %31 = getelementptr %0, %0* %h1, i32 0, i32 0
  %32 = load i64*, i64** %31
  store i64* %32, i64** %h2
  %33 = load i64*, i64** %h2
  %34 = load i64, i64* %33
  store i64 %34, i64* %local6
  %35 = load i64, i64* %local6
  ret i64 %24
}
