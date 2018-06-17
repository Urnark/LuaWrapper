-- init.lua --

entities = {}

function InitWorld(world_width, world_height)
	CreatePlayer(world_width / 2, world_height / 2, 150)

	-- Add entities from C++ to the table entities
	if GetNrOfEntities() > 0 then
		for i = 0, GetNrOfEntities() - 1, 1 do
			entities[i] = GetEntity(i)
		end
	end

	-- Create the walls
	local index = GetNrOfEntities()
	for x = 0, world_width / 30, 1 do
		entities[index + x] = CreateWall(30 * x, 0)
	end
end