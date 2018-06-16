-- entity.lua --

-- init the entity
local entity = {}

function entity.CreateEntity(e)
	setmetatable(entity, e)
end

return entity