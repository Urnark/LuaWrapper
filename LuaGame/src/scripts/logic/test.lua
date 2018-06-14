-- Test.lua --

print("Hello from Test.lua!!")

function Update(player)
	player:Print()
	x, y = player:GetPosition()
	print("Player is on the position: (" .. x .. ": " .. y .. ")")
end