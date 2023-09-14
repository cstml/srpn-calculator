local tests = dofile('tests.lua')

function executeTest (input,output,file)
   outputFile = tostring(file) .. ".txt"
   cmd = "echo '" .. input .. "' | ./bin/srpn > " .. outputFile
   succes = os.execute("echo '" .. input .. "' | ./bin/srpn > " .. tostring(file) .. ".txt")
   res = io.open(outputFile)
   x = io.lines(outputFile)
   acc = ""
   for line in x do
      acc = acc .. line .." "
   end
   is = function(a,b) assert(a == b,
                             "\nIn test: " .. file .. ".\n" ..
                             "\nExpected:\n" .. tostring(a) ..
                             "\nGot:\n" .. tostring(b)) end
   is(output,acc)
   is(true,succes)
   os.remove(outputFile)
end

for k,v in pairs(tests) do
   executeTest(v.input,v.output,k)
end

print "All tests succeeded."
