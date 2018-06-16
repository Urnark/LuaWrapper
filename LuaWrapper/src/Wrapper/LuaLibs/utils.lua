utils = {}

--[[ ENUM ]]
--[[
Ex: local enum = utils.Enum ({}, { "First", "Second" })
print(enum.First)
print(enum.Second)
]]
function utils.Enum(nrT, t)
	local newT = setmetatable({}, {
		__index = function(table, key) 
			return table[key]
		end,
		__len = function(table)
			return table.__SIZE - 1
		end
	})

	local nr = 0
	for key, v in ipairs(t) do
		-- If the table nrT have elements in it then use those as the values for the enums
		if key < #nrT + 1 then
			if rawget(nrT, key) then
				nr = nrT[key]
			end
		end
		newT[v] = nr
		nr = nr + 1
	end
	-- Set the __SIZE key to be able to get the nr of elements in the Enum table 
	newT["__SIZE"] = nr
	return newT
end