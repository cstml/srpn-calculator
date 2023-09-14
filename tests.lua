--[[ Test inputs

   As used by test.lua.
--]]
local tests =
   {
      ["test1"] =
         {
            input = [[1 = =]],
            output = "1 1 "
         },
      ["test2"] =
         {
            input = [[1 2 = =]],
            output = "2 2 "
         },
      ["test3"] =
         {
            input = [[1 2 3 =]],
            output = "3 "
         },
      ["last_value_twice"] =
         {
            input = [[1 = 2 3 =]],
            output = "1 3 "
         },
      ["last_value_thrice"] =
         {
            input = [[1 = 2 = 3 =]],
            output = "1 2 3 "
         },
      ["underflow"] =
         {
            input = "+",
            output = "Stack underflow! "
         },
      ["underflow_twice"] =
         {
            input = "+ +",
            output = "Stack underflow! Stack underflow! "
         },
      ["overflow"] =
         {
            input = "1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23",
            output = "Stack overflow! "
         },
      ["display"] =
         {
            input = "1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22d",
            output = "1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 "
         },
   }

return tests
