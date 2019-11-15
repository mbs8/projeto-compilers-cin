
@arroz = common global i32 0, align 4

define void @kk() #0 {
  %hh = alloca i32, align 4
  store i32 10, i32* %hh, align 4
}

define i32 @main() #0 {
}