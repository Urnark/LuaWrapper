-- keyboard.lua --

local freeIndex = 0
local enummt = {
  __index = function(table, key) 
	return table.enums[key]
  end,
  __newindex = function(table, key, value) 
	if tonumber(value) then
		freeIndex = value
	end
	table.enums[key] = freeIndex
	freeIndex = freeIndex + 1
    return table.enums[key]
  end
}

local function Enum(t)
  local e = { enums = t }
  return setmetatable(e, enummt)
end

local keyboard = Enum {}
keyboard.Unknown = -1		--< Unhandled key
keyboard.A = 0				--< The A key
keyboard.B = nil            --< The B key
keyboard.C = nil            --< The C key
keyboard.D = nil            --< The D key
keyboard.E = nil            --< The E key
keyboard.F = nil            --< The F key
keyboard.G = nil            --< The G key
keyboard.H = nil            --< The H key
keyboard.I = nil            --< The I key
keyboard.J = nil            --< The J key
keyboard.K = nil            --< The K key
keyboard.L = nil            --< The L key
keyboard.M = nil            --< The M key
keyboard.N = nil            --< The N key
keyboard.O = nil            --< The O key
keyboard.P = nil            --< The P key
keyboard.Q = nil            --< The Q key
keyboard.R = nil            --< The R key
keyboard.S = nil            --< The S key
keyboard.T = nil            --< The T key
keyboard.U = nil            --< The U key
keyboard.V = nil            --< The V key
keyboard.W = nil            --< The W key
keyboard.X = nil            --< The X key
keyboard.Y = nil            --< The Y key
keyboard.Z = nil            --< The Z key
keyboard.Num0 = nil         --< The 0 key
keyboard.Num1 = nil         --< The 1 key
keyboard.Num2 = nil         --< The 2 key
keyboard.Num3 = nil         --< The 3 key
keyboard.Num4 = nil         --< The 4 key
keyboard.Num5 = nil         --< The 5 key
keyboard.Num6 = nil         --< The 6 key
keyboard.Num7 = nil         --< The 7 key
keyboard.Num8 = nil         --< The 8 key
keyboard.Num9 = nil         --< The 9 key
keyboard.Escape = nil       --< The Escape key
keyboard.LControl = nil     --< The left Control key
keyboard.LShift = nil       --< The left Shift key
keyboard.LAlt = nil         --< The left Alt key
keyboard.LSystem = nil      --< The left OS specific key: window (Windows and Linux), apple (MacOS X), ...
keyboard.RControl = nil     --< The right Control key
keyboard.RShift = nil       --< The right Shift key
keyboard.RAlt = nil         --< The right Alt key
keyboard.RSystem = nil      --< The right OS specific key: window (Windows and Linux), apple (MacOS X), ...
keyboard.Menu = nil         --< The Menu key
keyboard.LBracket = nil     --< The [ key
keyboard.RBracket = nil     --< The ] key
keyboard.Semicolon = nil    --< The ; key
keyboard.Comma = nil        --< The , key
keyboard.Period = nil       --< The . key
keyboard.Quote = nil        --< The ' key
keyboard.Slash = nil        --< The / key
keyboard.Backslash = nil    --< The \ key
keyboard.Tilde = nil        --< The ~ key
keyboard.Equal = nil        --< The = key
keyboard.Hyphen = nil       --< The - key (hyphen)
keyboard.Space = nil        --< The Space key
keyboard.Enter = nil        --< The Enter/Return keys
keyboard.Backspace = nil    --< The Backspace key
keyboard.Tab = nil          --< The Tabulation key
keyboard.PageUp = nil       --< The Page up key
keyboard.PageDown = nil     --< The Page down key
keyboard.End = nil          --< The End key
keyboard.Home = nil         --< The Home key
keyboard.Insert = nil       --< The Insert key
keyboard.Delete = nil       --< The Delete key
keyboard.Add = nil          --< The + key
keyboard.Subtract = nil     --< The - key (minus, usually from numpad)
keyboard.Multiply = nil     --< The * key
keyboard.Divide = nil       --< The / key
keyboard.Left = nil         --< Left arrow
keyboard.Right = nil        --< Right arrow
keyboard.Up = nil           --< Up arrow
keyboard.Down = nil         --< Down arrow
keyboard.Numpad0 = nil      --< The numpad 0 key
keyboard.Numpad1 = nil      --< The numpad 1 key
keyboard.Numpad2 = nil      --< The numpad 2 key
keyboard.Numpad3 = nil      --< The numpad 3 key
keyboard.Numpad4 = nil      --< The numpad 4 key
keyboard.Numpad5 = nil      --< The numpad 5 key
keyboard.Numpad6 = nil      --< The numpad 6 key
keyboard.Numpad7 = nil      --< The numpad 7 key
keyboard.Numpad8 = nil      --< The numpad 8 key
keyboard.Numpad9 = nil      --< The numpad 9 key
keyboard.F1 = nil           --< The F1 key
keyboard.F2 = nil           --< The F2 key
keyboard.F3 = nil           --< The F3 key
keyboard.F4 = nil           --< The F4 key
keyboard.F5 = nil           --< The F5 key
keyboard.F6 = nil           --< The F6 key
keyboard.F7 = nil           --< The F7 key
keyboard.F8 = nil           --< The F8 key
keyboard.F9 = nil           --< The F9 key
keyboard.F10 = nil          --< The F10 key
keyboard.F11 = nil          --< The F11 key
keyboard.F12 = nil          --< The F12 key
keyboard.F13 = nil          --< The F13 key
keyboard.F14 = nil          --< The F14 key
keyboard.F15 = nil          --< The F15 key
keyboard.Pause = nil        --< The Pause key

keyboard.KeyCount = nil     --< Keep last -- the total number of keyboard keys

return keyboard