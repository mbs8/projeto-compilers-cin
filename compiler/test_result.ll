source_filename = "test.c"


define i32 @aqua_baldo() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 160, i32* %1, align 4
  %4 = load i32, i32* %1, align 4
  %5 = srem i32 %1, 11
  store i32 4, i32* %2, align 4
  %6 = load i32, i32* %2, align 4
  %7 = add nsw i32 6, %2
  store i32 1, i32* %3, align 4
  %8 = load i32, i32* %1, align 4
  %9 = sub nsw i32 1, %1
  %10 = load i32, i32* %2, align 4
  %11 = load i32, i32* %3, align 4
  %12 = srem i32 %2, %3
}

define i32 @function7(i32, i32) #0 {
  %3 = alloca i32, align 4
  %4 = load i32, i32* %a, align 4
  %5 = load i32, i32* %b, align 4
  %6 = srem i32 %a, %b
  store i32 0, i32* %3, align 4
  %7 = load i32, i32* %a, align 4
  %8 = load i32, i32* %b, align 4
  %9 = load i32, i32* %3, align 4
  %10 = add nsw i32 %b, %3
  %11 = load i32, i32* %3, align 4
  %12 = load i32, i32* %b, align 4
  %13 = sdiv i32 %b, 2
  %14 = sub nsw i32 %3, 0
}

define i32 @main() #0 {
}