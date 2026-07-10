-- =======================================================
-- GROW A GARDEN REMOTE SPY & SNIFFER
-- Displays RemoteEvents/RemoteFunctions on the screen
-- =======================================================

local Players = game:GetService("Players")
local LocalPlayer = Players.LocalPlayer

-- Create UI
local ScreenGui = Instance.new("ScreenGui")
ScreenGui.Name = "GardenRemoteSpyGui"
ScreenGui.ResetOnSpawn = false

-- Use CoreGui if possible to avoid deletion on reset, fallback to PlayerGui
pcall(function() ScreenGui.Parent = game:GetService("CoreGui") end)
if not ScreenGui.Parent then
    ScreenGui.Parent = LocalPlayer:WaitForChild("PlayerGui")
end

local MainFrame = Instance.new("Frame", ScreenGui)
MainFrame.Size = UDim2.new(0, 420, 0, 280)
MainFrame.Position = UDim2.new(0.05, 0, 0.5, 0)
MainFrame.BackgroundColor3 = Color3.fromRGB(15, 17, 26)
MainFrame.BorderSizePixel = 0
Instance.new("UICorner", MainFrame).CornerRadius = UDim.new(0, 8)

-- Make UI Draggable
local dragging, dragInput, dragStart, startPos
local function update(input)
    local delta = input.Position - dragStart
    MainFrame.Position = UDim2.new(startPos.X.Scale, startPos.X.Offset + delta.X, startPos.Y.Scale, startPos.Y.Offset + delta.Y)
end
MainFrame.InputBegan:Connect(function(input)
    if input.UserInputType == Enum.UserInputType.MouseButton1 or input.UserInputType == Enum.UserInputType.Touch then
        dragging = true
        dragStart = input.Position
        startPos = MainFrame.Position
        input.Changed:Connect(function()
            if input.UserInputState == Enum.UserInputState.End then dragging = false end
        end)
    end
end)
MainFrame.InputChanged:Connect(function(input)
    if input.UserInputType == Enum.UserInputType.MouseMovement or input.UserInputType == Enum.UserInputType.Touch then
        dragInput = input
    end
end)
game:GetService("UserInputService").InputChanged:Connect(function(input)
    if input == dragInput and dragging then update(input) end
end)

local Stroke = Instance.new("UIStroke", MainFrame)
Stroke.Thickness = 2
Stroke.Color = Color3.fromRGB(0, 170, 255)

local Title = Instance.new("TextLabel", MainFrame)
Title.Size = UDim2.new(1, -10, 0, 30)
Title.Position = UDim2.new(0, 10, 0, 0)
Title.BackgroundTransparency = 1
Title.Text = "📡 Grow a Garden - Network Hook Spy (Draggable)"
Title.TextColor3 = Color3.fromRGB(255, 255, 255)
Title.TextSize = 12
Title.Font = Enum.Font.GothamBold
Title.TextXAlignment = Enum.TextXAlignment.Left

local ClearBtn = Instance.new("TextButton", MainFrame)
ClearBtn.Size = UDim2.new(0, 60, 0, 20)
ClearBtn.Position = UDim2.new(1, -70, 0, 5)
ClearBtn.Text = "CLEAR"
ClearBtn.TextSize = 9
ClearBtn.Font = Enum.Font.GothamBold
ClearBtn.TextColor3 = Color3.fromRGB(255, 255, 255)
ClearBtn.BackgroundColor3 = Color3.fromRGB(180, 50, 50)
Instance.new("UICorner", ClearBtn).CornerRadius = UDim.new(0, 4)

local Scroll = Instance.new("ScrollingFrame", MainFrame)
Scroll.Size = UDim2.new(1, -16, 1, -45)
Scroll.Position = UDim2.new(0, 8, 0, 35)
Scroll.BackgroundTransparency = 1
Scroll.BorderSizePixel = 0
Scroll.CanvasSize = UDim2.new(0, 0, 0, 0)
Scroll.ScrollBarThickness = 4

local ListLayout = Instance.new("UIListLayout", Scroll)
ListLayout.SortOrder = Enum.SortOrder.LayoutOrder
ListLayout.Padding = UDim.new(0, 4)

local function logMessage(text, color)
    local LogLabel = Instance.new("TextLabel", Scroll)
    LogLabel.Size = UDim2.new(1, -10, 0, 18)
    LogLabel.BackgroundTransparency = 1
    LogLabel.Text = text
    LogLabel.TextColor3 = color or Color3.fromRGB(220, 220, 220)
    LogLabel.TextSize = 10
    LogLabel.Font = Enum.Font.Code
    LogLabel.TextXAlignment = Enum.TextXAlignment.Left
    LogLabel.TextWrapped = true
    
    Scroll.CanvasSize = UDim2.new(0, 0, 0, ListLayout.AbsoluteContentSize.Y)
    Scroll.CanvasPosition = Vector2.new(0, Scroll.CanvasSize.Y.Offset)
    
    local children = Scroll:GetChildren()
    if #children > 60 then
        children[2]:Destroy() -- Index 1 is UIListLayout
    end
end

ClearBtn.MouseButton1Click:Connect(function()
    for _, child in ipairs(Scroll:GetChildren()) do
        if child:IsA("TextLabel") then child:Destroy() end
    end
    Scroll.CanvasSize = UDim2.new(0, 0, 0, 0)
end)

logMessage("Spy Active! Ready to capture drop/collect network calls...", Color3.fromRGB(0, 255, 127))

-- Helpers to format arguments
local function formatArgs(...)
    local args = {...}
    local strParts = {}
    for i, v in ipairs(args) do
        local t = type(v)
        if t == "string" then
            table.insert(strParts, string.format('"%s"', v))
        elseif t == "number" or t == "boolean" then
            table.insert(strParts, tostring(v))
        elseif t == "table" then
            local success, keys = pcall(function()
                local parts = {}
                local count = 0
                for k, val in pairs(v) do
                    count = count + 1
                    if count > 5 then
                        table.insert(parts, "...")
                        break
                    end
                    table.insert(parts, tostring(k) .. ":" .. tostring(type(val) == "table" and "{...}" or tostring(val)))
                end
                return "{" .. table.concat(parts, ", ") .. "}"
            end)
            table.insert(strParts, success and keys or "{Table}")
        elseif t == "userdata" then
            local success, name = pcall(function() return v.Name end)
            table.insert(strParts, success and ("Instance:" .. name) or "Userdata")
        else
            table.insert(strParts, tostring(v))
        end
    end
    return table.concat(strParts, ", ")
end

-- Inject Hooks
local hooked = false
local hookfunction = hookfunction or replaceclosure or detour_function
if hookfunction then
    pcall(function()
        local oldFireServer
        oldFireServer = hookfunction(Instance.new("RemoteEvent").FireServer, newcclosure(function(self, ...)
            local args = {...}
            pcall(function()
                local remoteName = self.Name
                local argsString = formatArgs(unpack(args))
                local lowerName = string.lower(remoteName)
                
                if string.find(lowerName, "drop") or string.find(lowerName, "discard") or string.find(lowerName, "trash") or string.find(lowerName, "delete") or string.find(lowerName, "remove") then
                    logMessage("[Fire] 📤 DROP: " .. remoteName .. " | Args: " .. argsString, Color3.fromRGB(255, 140, 0))
                elseif string.find(lowerName, "claim") or string.find(lowerName, "collect") or string.find(lowerName, "pickup") or string.find(lowerName, "grab") then
                    logMessage("[Fire] 📥 COLLECT: " .. remoteName .. " | Args: " .. argsString, Color3.fromRGB(0, 220, 255))
                else
                    logMessage("[Fire] " .. remoteName .. " (" .. argsString .. ")", Color3.fromRGB(140, 140, 140))
                end
            end)
            return oldFireServer(self, unpack(args))
        end))

        local oldInvokeServer
        oldInvokeServer = hookfunction(Instance.new("RemoteFunction").InvokeServer, newcclosure(function(self, ...)
            local args = {...}
            pcall(function()
                local remoteName = self.Name
                local argsString = formatArgs(unpack(args))
                local lowerName = string.lower(remoteName)
                
                if string.find(lowerName, "drop") or string.find(lowerName, "discard") or string.find(lowerName, "trash") or string.find(lowerName, "delete") or string.find(lowerName, "remove") then
                    logMessage("[Invoke] 📤 DROP: " .. remoteName .. " | Args: " .. argsString, Color3.fromRGB(255, 140, 0))
                elseif string.find(lowerName, "claim") or string.find(lowerName, "collect") or string.find(lowerName, "pickup") or string.find(lowerName, "grab") then
                    logMessage("[Invoke] 📥 COLLECT: " .. remoteName .. " | Args: " .. argsString, Color3.fromRGB(0, 220, 255))
                else
                    logMessage("[Invoke] " .. remoteName .. " (" .. argsString .. ")", Color3.fromRGB(140, 140, 140))
                end
            end)
            return oldInvokeServer(self, unpack(args))
        end))
        
        -- Hook UnreliableRemoteEvent if it exists in the game
        pcall(function()
            local oldUnreliableFire
            oldUnreliableFire = hookfunction(Instance.new("UnreliableRemoteEvent").FireServer, newcclosure(function(self, ...)
                local args = {...}
                pcall(function()
                    local remoteName = self.Name
                    local argsString = formatArgs(unpack(args))
                    local lowerName = string.lower(remoteName)
                    
                    if string.find(lowerName, "drop") or string.find(lowerName, "discard") or string.find(lowerName, "trash") or string.find(lowerName, "delete") or string.find(lowerName, "remove") then
                        logMessage("[Unreliable] 📤 DROP: " .. remoteName .. " | Args: " .. argsString, Color3.fromRGB(255, 140, 0))
                    elseif string.find(lowerName, "claim") or string.find(lowerName, "collect") or string.find(lowerName, "pickup") or string.find(lowerName, "grab") then
                        logMessage("[Unreliable] 📥 COLLECT: " .. remoteName .. " | Args: " .. argsString, Color3.fromRGB(0, 220, 255))
                    else
                        logMessage("[Unreliable] " .. remoteName .. " (" .. argsString .. ")", Color3.fromRGB(140, 140, 140))
                    end
                end)
                return oldUnreliableFire(self, unpack(args))
            end))
        end)
        
        hooked = true
        logMessage("API Hooks installed via hookfunction!", Color3.fromRGB(0, 255, 127))
    end)
end

if not hooked then
    pcall(function()
        local gmt = getrawmetatable(game)
        setreadonly(gmt, false)
        local oldNamecall = gmt.__namecall
        
        gmt.__namecall = newcclosure(function(self, ...)
            local method = getnamecallmethod()
            local isInstance = typeof(self) == "Instance"
            
            if isInstance then
                local className = nil
                pcall(function() className = self.ClassName end)
                
                if className == "RemoteEvent" or className == "RemoteFunction" or className == "UnreliableRemoteEvent" then
                    local args = {...}
                    pcall(function()
                        local remoteName = self.Name
                        local argsString = formatArgs(unpack(args))
                        local lowerName = string.lower(remoteName)
                        
                        if string.find(lowerName, "drop") or string.find(lowerName, "discard") or string.find(lowerName, "trash") or string.find(lowerName, "delete") or string.find(lowerName, "remove") then
                            logMessage("[Namecall] 📤 DROP: " .. remoteName .. " | Args: " .. argsString, Color3.fromRGB(255, 140, 0))
                        elseif string.find(lowerName, "claim") or string.find(lowerName, "collect") or string.find(lowerName, "pickup") or string.find(lowerName, "grab") then
                            logMessage("[Namecall] 📥 COLLECT: " .. remoteName .. " | Args: " .. argsString, Color3.fromRGB(0, 220, 255))
                        else
                            logMessage("[Namecall] " .. remoteName .. " (" .. argsString .. ")", Color3.fromRGB(140, 140, 140))
                        end
                    end)
                end
            end
            return oldNamecall(self, ...)
        end)
        logMessage("Metatable Hooks installed!", Color3.fromRGB(0, 255, 127))
    end)
end
