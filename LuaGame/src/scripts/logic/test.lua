-- Test.lua --

function WorldUpdate(dt)
	local player = require("logic/entities/player")

	-- Update the player
	player.Update(dt)
end