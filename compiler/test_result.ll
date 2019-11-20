source_filename = "test.c"

@nom_nom = global i32 88, align 4
@arroz = common global i32 0, align 4

define i32 @aqua_baldo() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = load i32, i32* @nom_nom, align 4
  %5 = sub nsw i32 160, %4
  store i32 %5, i32* %1, align 4
  %6 = load i32, i32* %1, align 4
  %7 = srem i32 %6, 11
  %8 = sub nsw i32 %6, 2
  store i32 %8, i32* %2, align 4
  %9 = load i32, i32* %2, align 4
  %10 = add nsw i32 6, %9
  %11 = srem i32 11, %15
  store i32 %11, i32* %3, align 4
  %12 = load i32, i32* %1, align 4
  %13 = sub nsw i32 1, %12
  %14 = load i32, i32* %2, align 4
  %15 = load i32, i32* %3, align 4
  %16 = srem i32 %14, %15
  %17 = add nsw i32 %13, %14
  ret i32 %17
}

define i32 @function7(i32, i32) #0 {
  %3 = alloca i32, align 4
  %4 = load i32, i32* %0, align 4
  %5 = load i32, i32* %0, align 4
  %6 = srem i32 %4, %5
  store i32 %4, i32* %3, align 4
  %7 = load i32, i32* %0, align 4
  %8 = load i32, i32* %0, align 4
  %9 = load i32, i32* %3, align 4
  %10 = add nsw i32 %8, %9
  %11 = mul i32 %-7, %17
  %12 = load i32, i32* %3, align 4
  %13 = load i32, i32* %0, align 4
  %14 = sub nsw i32 %12, %1
  ret i32 %14
}

define void @kk(i32) #0 {
  %2 = load i32, i32* @arroz, align 4
  %3 = load i32, i32* %0, align 4
  %4 = alloca i32, align4
}

define i32 @aster(i32, i32, i32) #0 {
  %4 = alloca i32, align 4
  %5 = load i32, i32* %0, align 4
  %6 = mul i32 3, %5
  %7 = load i32, i32* %0, align 4
  %8 = mul i32 5, %7
  %9 = add nsw i32 %6, %8
  %10 = load i32, i32* %0, align 4
  %11 = mul i32 2, %10
  %12 = add nsw i32 %14, %11
  store i32 %5, i32* %4, align 4
  %13 = load i32, i32* %0, align 4
  %14 = mul i32 255, %13
  %15 = load i32, i32* %4, align 4
  %16 = load i32, i32* %0, align 4
  %17 = mul i32 255, %16
  %18 = load i32, i32* %4, align 4
  %19 = load i32, i32* %0, align 4
  %20 = sub nsw i32 %19, 127
  %21 = mul i32 255, %20
  %22 = load i32, i32* %4, align 4
  %23 = load i32, i32* %0, align 4
  %24 = load i32, i32* %0, align 4
  %25 = add nsw i32 %23, %24
  %26 = load i32, i32* %0, align 4
  %27 = add nsw i32 %47, %26
  ret i32 %27
}

define i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  %3 = call i32 @aster (i32 0, i32 0, i32 0)
  store i32 %0, i32* %2, align 4
  %4 = call i32 @aqua_baldo ()
  %5 = alloca i32, align4
  call void @kk(
  %6 = load i32, i32* %2, align 4i32 111)
  %7 = call i32 @function7 (i32 37, i32 0)
  %8 = load i32, i32* @arroz, align 4
  %9 = sub nsw i32 %0, %8
  ret i32 0
}