width = 100
height = 99
test = 111

f = function(x,y)
	return x + y
end

t = {}

t.x  = 11

---require "userdata"

---a = array.new(100)
---print(array.size(a))

function printMsg()
	print("hello world")
end

x = 100

local win = require "WinFeature"
print(WinFeature.Add(1,1))
---WinFeature.ShowMsgBox("Hello, this is a msgBox", "Tip")
for k , v in pairs(win) do 
	print(k,v)
end

me = { name = "zilongshanren", age = 27}

xmlHead  = '<?xml version="1.0" encoding="utf-8" ?>\n'
--- open note file to write
function openNodeFile(fileName)
	return io.open(fileName,"w")
end
---Close Writed note file
function closeNoteFile(noteFile)
	noteFile:close()
end

print("-------------------------------- userdata test begin")

a = WinFeature.new(100)
print(WinFeature.size(a))

local t = WinFeature.newtable(a)
print(t[1],t[2])
print("---------------")

for i = 1, 100 do 
	WinFeature.set(a,i,1)
end

print(WinFeature.get(a,10))

print("-------------------------------- userdata test end")
