-- Test.lua --

-- get keyboard inputs
local keyboard = require("utils/keyboard")

function Update(dt)
	player = GetPlayer()

	x, y = player:GetPosition()
	
	-- Left
	if (IsKeyPressed(keyboard.A) == true) then
		x = x - dt * player:GetSpeed()
	end
	-- Right
	if (IsKeyPressed(keyboard.D)) then
		x = x + dt * player:GetSpeed()
	end
	-- Up
	if (IsKeyPressed(keyboard.W)) then
		y = y - dt * player:GetSpeed()
	end
	-- Down
	if (IsKeyPressed(keyboard.S)) then
		y = y + dt * player:GetSpeed()
	end

	player:SetPosition(x, y)

	return player
end