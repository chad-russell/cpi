; ModuleID = 'module'
source_filename = "module"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-darwin17.5.0"

%0 = type { i64 }

declare void @panic(i8*)

declare i8* @malloc(i64)

declare i8* @memset(i8*, i64, i64)

declare void @free(i8*)

define void @main() {
entry:
  %0 = call %0 @buffer()
  ret void
}

define %0 @buffer() {
entry:
  ret %0 { i64 3 }
}

!llvm.module.flags = !{!0, !1}

!0 = !{i32 2, !"Dwarf Version", i32 2}
!1 = !{i32 2, !"Debug Info Version", i32 3}
