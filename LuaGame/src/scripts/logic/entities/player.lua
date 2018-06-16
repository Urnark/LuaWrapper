-- player.lua --

-- get keyboard keys for the Game
local keyboard = require("utils/keyboard")

-- init the player
local player = require("logic/entities/entity")
player.CreateEntity(GetPlayer())

function player.Update(dt)
	x, y = player:GetPosition()
	
	-- Left
	if (IsKeyPressed(keyboard.A)) then
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
end

return player