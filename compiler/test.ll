; ModuleID = 'test.c'
source_filename = "test.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.15.0"

@nom_nom = global i32 88, align 4
@arroz = common global i32 0, align 4

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @aqua_baldo() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = load i32, i32* @nom_nom, align 4
  %5 = sub nsw i32 160, %4
  store i32 %5, i32* %1, align 4
  %6 = load i32, i32* %1, align 4
  %7 = srem i32 %6, 9
  store i32 %7, i32* %2, align 4
  %8 = load i32, i32* %2, align 4
  %9 = add nsw i32 6, %8
  %10 = srem i32 11, %9
  store i32 %10, i32* %3, align 4
  %11 = load i32, i32* %1, align 4
  %12 = sub nsw i32 1, %11
  %13 = load i32, i32* %2, align 4
  %14 = load i32, i32* %3, align 4
  %15 = srem i32 %13, %14
  %16 = add nsw i32 %12, %15
  ret i32 %16
}

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @main() #0 {
  %1 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  %2 = call i32 @aqua_baldo()
  store i32 %2, i32* @nom_nom, align 4
  ret i32 0
}

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "darwin-stkchk-strong-link" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "probe-stack"="___chkstk_darwin" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0, !1, !2}
!llvm.ident = !{!3}

!0 = !{i32 2, !"SDK Version", [2 x i32] [i32 10, i32 15]}
!1 = !{i32 1, !"wchar_size", i32 4}
!2 = !{i32 7, !"PIC Level", i32 2}
!3 = !{!"Apple clang version 11.0.0 (clang-1100.0.33.8)"}
