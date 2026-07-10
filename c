-- ==========================================
-- VIP GIFT KHOGAMEGIARE.COM V3.19 - DYNAMIC TRACKING, MISSING CHECK & AUTO FILL
-- [MODDED] ADDED AUTO DROP & FAST AUTO COLLECT
-- ==========================================

local Players = game:GetService("Players")
local RS = game:GetService("ReplicatedStorage")
local HttpService = game:GetService("HttpService")
local LP = Players.LocalPlayer

-- Executor Check
local request = (syn and syn.request) or (http and http.request) or http_request or request
if not request then
    LP:Kick("Executor not supported")
    return
end

local hasFS = (isfile and readfile and writefile)
local HISTORY_FILE = "Khogamegiare_History.json"
local WEBHOOK_FILE = "Khogamegiare_Webhook.txt"
local DAILY_FILE = "Khogamegiare_Daily.json"
local SETTINGS_FILE = "Khogamegiare_Settings.json"

local CurrentWebhook = ""
local AppSettings = { ConfirmPrompt = true }

-- Load/Save Systems
local function LoadWebhook()
    if hasFS and isfile(WEBHOOK_FILE) then
        local ok, result = pcall(function() return readfile(WEBHOOK_FILE) end)
        if ok and type(result) == "string" then CurrentWebhook = result end
    end
end
LoadWebhook()

local function SaveWebhook(url)
    CurrentWebhook = url
    if hasFS then pcall(function() writefile(WEBHOOK_FILE, url) end) end
end

local function LoadSettings()
    if hasFS and isfile(SETTINGS_FILE) then
        local ok, res = pcall(function() return HttpService:JSONDecode(readfile(SETTINGS_FILE)) end)
        if ok and type(res) == "table" and res.ConfirmPrompt ~= nil then
            AppSettings.ConfirmPrompt = res.ConfirmPrompt
        end
    end
end
LoadSettings()

local function SaveSettings()
    if hasFS then pcall(function() writefile(SETTINGS_FILE, HttpService:JSONEncode(AppSettings)) end) end
end

-- ==========================================
-- SUPER ANTI-AFK & HUMAN-LIKE MOVEMENT
-- ==========================================
local VirtualUser = game:GetService("VirtualUser")

pcall(function()
    for i, v in pairs(getconnections(LP.Idled)) do
        v:Disable()
    end
end)

LP.Idled:Connect(function()
    pcall(function()
        VirtualUser:CaptureController()
        VirtualUser:ClickButton2(Vector2.new())
    end)
end)

task.spawn(function()
    while task.wait(900) do
        pcall(function()
            VirtualUser:CaptureController()
            VirtualUser:ClickButton2(Vector2.new())
        end)
    end
end)

task.spawn(function()
    while task.wait(300) do
        pcall(function()
            local char = LP.Character
            if char then
                local root = char:FindFirstChild("HumanoidRootPart")
                local hum = char:FindFirstChildOfClass("Humanoid")
                
                if root and hum and hum.Health > 0 then
                    local randomX = math.random(-12, 12)
                    local randomZ = math.random(-12, 12)
                    local targetPos = root.Position + Vector3.new(randomX, 0, randomZ)
                    
                    hum:MoveTo(targetPos)
                    if math.random(1, 10) > 5 then
                        task.wait(math.random(1, 2))
                        hum.Jump = true
                    end
                end
            end
        end)
    end
end)

local Net = require(RS:WaitForChild("SharedModules"):WaitForChild("Networking"))
local PS = require(RS:WaitForChild("ClientModules"):WaitForChild("PlayerStateClient"))

-- ==========================================
-- DISCORD WEBHOOK LOGIC
-- ==========================================
local function SendWebhookLog(uid, targetName, description, isShort)
    if not request or CurrentWebhook == "" then return end
    local currentTime = os.date("%H:%M:%S | %d/%m/%Y")
    task.spawn(function()
        local avatarUrl = "https://tr.rbxcdn.com/38c6edcb50633730ff4cf39ac8859840/150/150/AvatarHeadshot/Png"
        pcall(function()
            local res = request({Url = "https://thumbnails.roblox.com/v1/users/avatar-headshot?userIds=" .. tostring(uid) .. "&size=150x150&format=Png&isCircular=false", Method = "GET"})
            if res and res.Body then
                local decoded = HttpService:JSONDecode(res.Body)
                if decoded and decoded.data and decoded.data[1] and decoded.data[1].imageUrl then avatarUrl = decoded.data[1].imageUrl end
            end
        end)
        
        local embedColor = isShort and 16711680 or 5763719
        local embedTitle = isShort and "⚠️ PARTIAL GIFT SENT" or "📤 GIFT SENT SUCCESSFULLY"
        
        local data = {
            ["embeds"] = {{
                ["title"] = embedTitle,
                ["color"] = embedColor,
                ["description"] = description,
                ["thumbnail"] = { ["url"] = avatarUrl },
                ["footer"] = { ["text"] = "Vip Gift Khogamegiare.com • " .. currentTime }
            }}
        }
        pcall(function() request({Url = CurrentWebhook, Method = "POST", Headers = {["Content-Type"] = "application/json"}, Body = HttpService:JSONEncode(data)}) end)
    end)
end

local function SendReceiveWebhookLog(uid, senderName, description)
    if not request or CurrentWebhook == "" then return end
    local currentTime = os.date("%H:%M:%S | %d/%m/%Y")
    task.spawn(function()
        local avatarUrl = "https://tr.rbxcdn.com/1a7ebc4ab12e126fcb7cb41f7ee6fb80/150/150/Image/Png"
        if uid > 1 then
            pcall(function()
                local res = request({Url = "https://thumbnails.roblox.com/v1/users/avatar-headshot?userIds=" .. tostring(uid) .. "&size=150x150&format=Png&isCircular=false", Method = "GET"})
                if res and res.Body then
                    local decoded = HttpService:JSONDecode(res.Body)
                    if decoded and decoded.data and decoded.data[1] and decoded.data[1].imageUrl then avatarUrl = decoded.data[1].imageUrl end
                end
            end)
        end
        local data = {
            ["embeds"] = {{
                ["title"] = "📥 NEW GIFT ARRIVED",
                ["color"] = 16766720,
                ["description"] = description,
                ["thumbnail"] = { ["url"] = avatarUrl },
                ["footer"] = { ["text"] = "Vip Gift Khogamegiare.com • " .. currentTime }
            }}
        }
        pcall(function() request({Url = CurrentWebhook, Method = "POST", Headers = {["Content-Type"] = "application/json"}, Body = HttpService:JSONEncode(data)}) end)
    end)
end

-- ==========================================
-- UI SETUP
-- ==========================================
local ScreenGui = Instance.new("ScreenGui", game.CoreGui)
ScreenGui.Name = "VipGift_Khogamegiare"
ScreenGui.ResetOnSpawn = false

local MainFrame = Instance.new("Frame", ScreenGui)
MainFrame.Size = UDim2.new(0, 270, 0, 395) -- Tăng chiều cao để chứa nút Auto Collect
MainFrame.Position = UDim2.new(0.5, -250, 0.5, -182) 
MainFrame.BackgroundColor3 = Color3.fromRGB(15, 15, 15)
MainFrame.Active = true
MainFrame.Draggable = true 
Instance.new("UICorner", MainFrame)

local Title = Instance.new("TextLabel", MainFrame)
Title.Size = UDim2.new(1, 0, 0, 30)
Title.Text = "Vip Gift Khogamegiare.com"
Title.TextColor3 = Color3.fromRGB(0, 255, 150)
Title.Font = Enum.Font.GothamBold
Title.TextSize = 11
Title.BackgroundTransparency = 1

local HistoryFrame = Instance.new("Frame", MainFrame)
HistoryFrame.Size = UDim2.new(0, 210, 1, 0)
HistoryFrame.Position = UDim2.new(1, 5, 0, 0)
HistoryFrame.BackgroundColor3 = Color3.fromRGB(15, 15, 15)
Instance.new("UICorner", HistoryFrame)

local HistoryTitle = Instance.new("TextLabel", HistoryFrame)
HistoryTitle.Size = UDim2.new(1, 0, 0, 30)
HistoryTitle.Text = " HISTORY"
HistoryTitle.TextColor3 = Color3.fromRGB(0, 200, 255)
HistoryTitle.Font = Enum.Font.GothamBold
HistoryTitle.TextSize = 12
HistoryTitle.TextXAlignment = Enum.TextXAlignment.Left
HistoryTitle.BackgroundTransparency = 1

local ClearBtn = Instance.new("TextButton", HistoryFrame)
ClearBtn.Size = UDim2.new(0, 50, 0, 20)
ClearBtn.Position = UDim2.new(1, -55, 0, 5)
ClearBtn.Text = "🗑️ CLEAR"
ClearBtn.BackgroundColor3 = Color3.fromRGB(150, 30, 30)
ClearBtn.TextColor3 = Color3.fromRGB(255, 255, 255)
ClearBtn.Font = Enum.Font.GothamBold
ClearBtn.TextSize = 10
Instance.new("UICorner", ClearBtn)

local HistoryScroll = Instance.new("ScrollingFrame", HistoryFrame)
HistoryScroll.Size = UDim2.new(1, -10, 1, -35)
HistoryScroll.Position = UDim2.new(0, 5, 0, 30)
HistoryScroll.BackgroundTransparency = 1
HistoryScroll.ScrollBarThickness = 2
local HistoryLayout = Instance.new("UIListLayout", HistoryScroll)
HistoryLayout.Padding = UDim.new(0, 4)
HistoryLayout:GetPropertyChangedSignal("AbsoluteContentSize"):Connect(function()
    HistoryScroll.CanvasSize = UDim2.new(0, 0, 0, HistoryLayout.AbsoluteContentSize.Y + 10)
end)

-- ==========================================
-- MULTI-ACCOUNT DAILY LIMIT MANAGER
-- ==========================================
local DailyData = { date = "", accounts = {} }
local DailyLimitLabel = nil

local function GetCurrentGameDay()
    local t = os.date("*t")
    if t.hour < 7 then return os.date("%Y-%m-%d", os.time() - 24 * 3600)
    else return os.date("%Y-%m-%d") end
end

local function SaveDailyCount()
    if hasFS then pcall(function() writefile(DAILY_FILE, HttpService:JSONEncode(DailyData)) end) end
end

local function GetMyCount()
    local myName = tostring(LP.Name)
    if type(DailyData.accounts) ~= "table" then DailyData.accounts = {} end
    return DailyData.accounts[myName] or 0
end

local function UpdateDailyUI()
    if DailyLimitLabel then
        local c = GetMyCount()
        DailyLimitLabel.Text = string.format("📮 Acc Limit [%s]: %d/50", LP.Name, c)
        if c >= 50 then DailyLimitLabel.TextColor3 = Color3.fromRGB(255, 50, 50)
        else DailyLimitLabel.TextColor3 = Color3.fromRGB(0, 200, 255) end
    end
end

local function AddMyCount(amount)
    local myName = tostring(LP.Name)
    if type(DailyData.accounts) ~= "table" then DailyData.accounts = {} end
    DailyData.accounts[myName] = GetMyCount() + amount
    SaveDailyCount()
    UpdateDailyUI()
end

local function LoadDailyCount()
    local currentDay = GetCurrentGameDay()
    if hasFS and isfile(DAILY_FILE) then
        local ok, result = pcall(function() return HttpService:JSONDecode(readfile(DAILY_FILE)) end)
        if ok and type(result) == "table" then DailyData = result end
    end
    
    if DailyData.date ~= currentDay then
        DailyData.date = currentDay
        DailyData.accounts = {}
        SaveDailyCount()
    end
    if type(DailyData.accounts) ~= "table" then DailyData.accounts = {} end
    UpdateDailyUI()
end

-- ==========================================
-- CONFIRMATION POPUP UI
-- ==========================================
local ConfirmFrame = Instance.new("Frame", MainFrame)
ConfirmFrame.Size = UDim2.new(1, 0, 1, 0)
ConfirmFrame.BackgroundColor3 = Color3.fromRGB(0, 0, 0)
ConfirmFrame.BackgroundTransparency = 0.2
ConfirmFrame.ZIndex = 50
ConfirmFrame.Visible = false
Instance.new("UICorner", ConfirmFrame)

local ConfirmModal = Instance.new("Frame", ConfirmFrame)
ConfirmModal.Size = UDim2.new(0, 230, 0, 150)
ConfirmModal.Position = UDim2.new(0.5, -115, 0.5, -75)
ConfirmModal.BackgroundColor3 = Color3.fromRGB(25, 25, 25)
ConfirmModal.ZIndex = 51
Instance.new("UICorner", ConfirmModal)

local ConfirmTitle = Instance.new("TextLabel", ConfirmModal)
ConfirmTitle.Size = UDim2.new(1, 0, 0, 25)
ConfirmTitle.Text = "⚠️ CONFIRM GIFT"
ConfirmTitle.TextColor3 = Color3.fromRGB(255, 200, 0)
ConfirmTitle.Font = Enum.Font.GothamBold
ConfirmTitle.TextSize = 12
ConfirmTitle.BackgroundTransparency = 1
ConfirmTitle.ZIndex = 52

local ConfirmMsg = Instance.new("TextLabel", ConfirmModal)
ConfirmMsg.Size = UDim2.new(1, -10, 1, -65)
ConfirmMsg.Position = UDim2.new(0, 5, 0, 25)
ConfirmMsg.Text = "Are you sure?"
ConfirmMsg.TextColor3 = Color3.fromRGB(220, 220, 220)
ConfirmMsg.Font = Enum.Font.Gotham
ConfirmMsg.TextSize = 10
ConfirmMsg.TextWrapped = true
ConfirmMsg.TextYAlignment = Enum.TextYAlignment.Top
ConfirmMsg.BackgroundTransparency = 1
ConfirmMsg.ZIndex = 52

local ConfirmYes = Instance.new("TextButton", ConfirmModal)
ConfirmYes.Size = UDim2.new(0, 100, 0, 25)
ConfirmYes.Position = UDim2.new(0, 10, 1, -35)
ConfirmYes.Text = "✔ CONFIRM"
ConfirmYes.BackgroundColor3 = Color3.fromRGB(0, 170, 80)
ConfirmYes.TextColor3 = Color3.fromRGB(255, 255, 255)
ConfirmYes.Font = Enum.Font.GothamBold
ConfirmYes.TextSize = 10
ConfirmYes.ZIndex = 52
Instance.new("UICorner", ConfirmYes)

local ConfirmNo = Instance.new("TextButton", ConfirmModal)
ConfirmNo.Size = UDim2.new(0, 100, 0, 25)
ConfirmNo.Position = UDim2.new(1, -110, 1, -35)
ConfirmNo.Text = "✖ CANCEL"
ConfirmNo.BackgroundColor3 = Color3.fromRGB(180, 40, 40)
ConfirmNo.TextColor3 = Color3.fromRGB(255, 255, 255)
ConfirmNo.Font = Enum.Font.GothamBold
ConfirmNo.TextSize = 10
ConfirmNo.ZIndex = 52
Instance.new("UICorner", ConfirmNo)

local currentConfirmConn, currentCancelConn

local function ShowConfirmPrompt(message, onConfirm, onCancel)
    ConfirmFrame.Visible = true
    ConfirmMsg.Text = message
    
    if currentConfirmConn then currentConfirmConn:Disconnect() end
    if currentCancelConn then currentCancelConn:Disconnect() end
    
    currentConfirmConn = ConfirmYes.MouseButton1Click:Connect(function()
        ConfirmFrame.Visible = false
        if onConfirm then onConfirm() end
    end)
    
    currentCancelConn = ConfirmNo.MouseButton1Click:Connect(function()
        ConfirmFrame.Visible = false
        if onCancel then onCancel() end
    end)
end

-- ==========================================
-- HISTORY DATA MANAGER
-- ==========================================
local LocalHistory = {}
local THREE_DAYS = 3 * 24 * 60 * 60

local function SaveHistoryToFile()
    if hasFS then pcall(function() writefile(HISTORY_FILE, HttpService:JSONEncode(LocalHistory)) end) end
end

local function LoadHistoryFromFile()
    if hasFS and isfile(HISTORY_FILE) then
        local ok, result = pcall(function() return HttpService:JSONDecode(readfile(HISTORY_FILE)) end)
        if ok and type(result) == "table" then LocalHistory = result end
    end
end

local function RenderHistoryEntry(entry)
    local EntryFrame = Instance.new("Frame", HistoryScroll)
    Instance.new("UICorner", EntryFrame)

    local AvatarImg = Instance.new("ImageLabel", EntryFrame)
    AvatarImg.Size = UDim2.new(0, 24, 0, 24)
    AvatarImg.Position = UDim2.new(0, 4, 0.5, -12)
    AvatarImg.BackgroundTransparency = 1
    AvatarImg.Image = "rbxthumb://type=AvatarHeadShot&id=" .. tostring(entry.uid) .. "&w=48&h=48"
    local AvatarCorner = Instance.new("UICorner", AvatarImg)
    AvatarCorner.CornerRadius = UDim.new(1, 0)

    local TextLbl = Instance.new("TextLabel", EntryFrame)
    TextLbl.Size = UDim2.new(1, -34, 1, 0)
    TextLbl.Position = UDim2.new(0, 32, 0, 0)
    TextLbl.BackgroundTransparency = 1
    TextLbl.Font = Enum.Font.Gotham
    TextLbl.TextSize = 10
    TextLbl.TextXAlignment = Enum.TextXAlignment.Left
    TextLbl.TextWrapped = true
    
    local timeStr = os.date("%H:%M", entry.time or os.time())
    local fullText = ""
    
    if entry.type == "online_receive" then
        EntryFrame.BackgroundColor3 = Color3.fromRGB(10, 40, 80) 
        TextLbl.TextColor3 = Color3.fromRGB(120, 200, 255) 
        fullText = string.format("🔔 Gift received (Online) [%s]\nFrom: %s\nItem: %s", timeStr, entry.name, entry.item)
    elseif entry.type == "claim" then
        EntryFrame.BackgroundColor3 = Color3.fromRGB(80, 20, 60) 
        TextLbl.TextColor3 = Color3.fromRGB(255, 130, 200) 
        fullText = string.format("💖 Claimed successfully [%s]\nFrom: %s\nItem: %s", timeStr, entry.name, entry.item)
    elseif entry.type == "short" then
        EntryFrame.BackgroundColor3 = Color3.fromRGB(60, 15, 15) 
        TextLbl.TextColor3 = Color3.fromRGB(255, 100, 100)
        local amtStr = (entry.amount == "") and "" or (" - " .. tostring(entry.amount))
        fullText = string.format("To: %s [%s]\nItem: %s%s", entry.name, timeStr, entry.item, amtStr)
    else 
        EntryFrame.BackgroundColor3 = Color3.fromRGB(30, 30, 30) 
        TextLbl.TextColor3 = Color3.fromRGB(220, 220, 220)
        local amtStr = (entry.amount == "") and "" or (" - " .. tostring(entry.amount))
        fullText = string.format("To: %s [%s]\nItem: %s%s", entry.name, timeStr, entry.item, amtStr)
    end
    
    TextLbl.Text = fullText
    local lineBreaks = select(2, string.gsub(fullText, "\n", ""))
    local textLen = string.len(fullText)
    local frameHeight = 35 + (lineBreaks * 10)
    if textLen > 45 then frameHeight = frameHeight + math.ceil((textLen - 45) / 35) * 12 end
    EntryFrame.Size = UDim2.new(1, 0, 0, frameHeight)
end

local function SyncHistoryUI()
    for _, child in ipairs(HistoryScroll:GetChildren()) do if child:IsA("Frame") then child:Destroy() end end
    local currentTime = os.time()
    local validHistory = {}
    for _, entry in ipairs(LocalHistory) do
        if (currentTime - entry.time) <= THREE_DAYS then
            table.insert(validHistory, entry)
            RenderHistoryEntry(entry)
        end
    end
    if #validHistory ~= #LocalHistory then LocalHistory = validHistory; SaveHistoryToFile() end
end

local function AddHistoryRecord(uid, name, item, amount, hType)
    table.insert(LocalHistory, 1, {uid = uid, name = name, item = item, amount = amount, type = hType, time = os.time()})
    SaveHistoryToFile()
    SyncHistoryUI()
end

ClearBtn.MouseButton1Click:Connect(function() LocalHistory = {}; SaveHistoryToFile(); SyncHistoryUI() end)
LoadHistoryFromFile()
SyncHistoryUI()

-- ==========================================
-- MAIN APP UI
-- ==========================================
local TradeFrame = Instance.new("Frame", MainFrame)
TradeFrame.Size = UDim2.new(1, 0, 1, -30)
TradeFrame.Position = UDim2.new(0, 0, 0, 30)
TradeFrame.BackgroundTransparency = 1
TradeFrame.Visible = true 

local UserInput = Instance.new("TextBox", TradeFrame)
UserInput.Size = UDim2.new(0, 250, 0, 28)
UserInput.Position = UDim2.new(0.5, -125, 0, 5)
UserInput.PlaceholderText = "Target Username..."
UserInput.BackgroundColor3 = Color3.fromRGB(25, 25, 25)
UserInput.TextColor3 = Color3.fromRGB(255, 255, 255)
UserInput.TextSize = 11
Instance.new("UICorner", UserInput)

-- ==========================================
-- TABS SYSTEM
-- ==========================================
local CurrentTab = "All"

local TabsFrame = Instance.new("Frame", TradeFrame)
TabsFrame.Size = UDim2.new(0, 250, 0, 22)
TabsFrame.Position = UDim2.new(0.5, -125, 0, 38)
TabsFrame.BackgroundTransparency = 1

local TabLayout = Instance.new("UIListLayout", TabsFrame)
TabLayout.FillDirection = Enum.FillDirection.Horizontal
TabLayout.SortOrder = Enum.SortOrder.LayoutOrder
TabLayout.Padding = UDim.new(0, 5)

local ItemInputs = {}
local Scroll = Instance.new("ScrollingFrame", TradeFrame)
local ListLayout = Instance.new("UIListLayout", Scroll)

local tabButtons = {}
local function CreateTab(name, order)
    local btn = Instance.new("TextButton", TabsFrame)
    btn.Size = UDim2.new(0.25, -3.75, 1, 0) 
    btn.Text = name
    btn.Font = Enum.Font.GothamBold
    btn.TextSize = 10
    btn.TextColor3 = Color3.fromRGB(255, 255, 255)
    btn.BackgroundColor3 = (name == CurrentTab) and Color3.fromRGB(0, 170, 80) or Color3.fromRGB(45, 45, 45)
    btn.LayoutOrder = order
    Instance.new("UICorner", btn).CornerRadius = UDim.new(0, 4)

    btn.MouseButton1Click:Connect(function()
        if CurrentTab ~= name then
            CurrentTab = name
            for tName, tBtn in pairs(tabButtons) do
                tBtn.BackgroundColor3 = (tName == CurrentTab) and Color3.fromRGB(0, 170, 80) or Color3.fromRGB(45, 45, 45)
            end
            
            for hash, data in pairs(ItemInputs) do
                local isVis = (CurrentTab == "All" or CurrentTab == data.TabCategory)
                data.UI.Visible = isVis
            end
            Scroll.CanvasSize = UDim2.new(0, 0, 0, ListLayout.AbsoluteContentSize.Y)
        end
    end)
    tabButtons[name] = btn
end

CreateTab("All", 1)
CreateTab("Seeds", 2)
CreateTab("Pets", 3)
CreateTab("Other", 4)

-- ==========================================
-- SCROLL & BUTTONS
-- ==========================================
Scroll.Size = UDim2.new(0, 260, 0, 110)
Scroll.Position = UDim2.new(0.5, -130, 0, 65)
Scroll.BackgroundTransparency = 1
Scroll.ScrollBarThickness = 2
ListLayout.Padding = UDim.new(0, 4)

local StatusLabel = Instance.new("TextLabel", TradeFrame)
StatusLabel.Size = UDim2.new(1, 0, 0, 20)
StatusLabel.Position = UDim2.new(0, 0, 0, 177)
StatusLabel.Text = "Ready!"
StatusLabel.TextColor3 = Color3.fromRGB(200, 200, 200)
StatusLabel.BackgroundTransparency = 1
StatusLabel.TextSize = 10

local ClaimAllBtn = Instance.new("TextButton", TradeFrame)
ClaimAllBtn.Size = UDim2.new(0, 120, 0, 30)
ClaimAllBtn.Position = UDim2.new(0.5, -125, 0, 197)
ClaimAllBtn.Text = "🎁 CLAIM ALL"
ClaimAllBtn.BackgroundColor3 = Color3.fromRGB(130, 0, 220)
ClaimAllBtn.TextColor3 = Color3.fromRGB(255, 255, 255)
ClaimAllBtn.Font = Enum.Font.GothamBold
ClaimAllBtn.TextSize = 10
Instance.new("UICorner", ClaimAllBtn)

local FillTabBtn = Instance.new("TextButton", TradeFrame)
FillTabBtn.Size = UDim2.new(0, 120, 0, 30)
FillTabBtn.Position = UDim2.new(0.5, 5, 0, 197)
FillTabBtn.Text = "💯 MAX IN TAB"
FillTabBtn.BackgroundColor3 = Color3.fromRGB(220, 100, 0)
FillTabBtn.TextColor3 = Color3.fromRGB(255, 255, 255)
FillTabBtn.Font = Enum.Font.GothamBold
FillTabBtn.TextSize = 10
Instance.new("UICorner", FillTabBtn)

FillTabBtn.MouseButton1Click:Connect(function()
    for hash, data in pairs(ItemInputs) do
        if data.UI.Visible then
            data.Input.Text = tostring(data.Total)
        else
            data.Input.Text = "0"
        end
    end
end)

local StartBtn = Instance.new("TextButton", TradeFrame)
StartBtn.Size = UDim2.new(0, 250, 0, 30)
StartBtn.Position = UDim2.new(0.5, -125, 0, 230)
StartBtn.Text = "🚀 SEND GIFT"
StartBtn.BackgroundColor3 = Color3.fromRGB(0, 170, 80)
StartBtn.TextColor3 = Color3.fromRGB(255, 255, 255)
StartBtn.Font = Enum.Font.GothamBold
StartBtn.TextSize = 11
Instance.new("UICorner", StartBtn)

local ToggleConfirmBtn = Instance.new("TextButton", TradeFrame)
ToggleConfirmBtn.Size = UDim2.new(0, 250, 0, 15)
ToggleConfirmBtn.Position = UDim2.new(0.5, -125, 0, 265)
ToggleConfirmBtn.Font = Enum.Font.GothamBold
ToggleConfirmBtn.TextSize = 10
ToggleConfirmBtn.BackgroundTransparency = 1

local function UpdateToggleUI()
    if AppSettings.ConfirmPrompt then
        ToggleConfirmBtn.Text = "☑️ Confirm Prompt: ON"
        ToggleConfirmBtn.TextColor3 = Color3.fromRGB(0, 255, 150)
    else
        ToggleConfirmBtn.Text = "🔲 Confirm Prompt: OFF"
        ToggleConfirmBtn.TextColor3 = Color3.fromRGB(150, 150, 150)
    end
end
UpdateToggleUI()

ToggleConfirmBtn.MouseButton1Click:Connect(function()
    AppSettings.ConfirmPrompt = not AppSettings.ConfirmPrompt
    SaveSettings()
    UpdateToggleUI()
end)

local WebhookInput = Instance.new("TextBox", TradeFrame)
WebhookInput.Size = UDim2.new(0, 195, 0, 25)
WebhookInput.Position = UDim2.new(0.5, -125, 0, 285)
WebhookInput.PlaceholderText = "Discord Webhook URL..."
WebhookInput.Text = CurrentWebhook
WebhookInput.BackgroundColor3 = Color3.fromRGB(25, 25, 25)
WebhookInput.TextColor3 = Color3.fromRGB(200, 200, 200)
WebhookInput.TextSize = 9
WebhookInput.ClearTextOnFocus = false
WebhookInput.ClipsDescendants = true
Instance.new("UICorner", WebhookInput)

local SaveWHBtn = Instance.new("TextButton", TradeFrame)
SaveWHBtn.Size = UDim2.new(0, 50, 0, 25)
SaveWHBtn.Position = UDim2.new(0.5, 75, 0, 285)
SaveWHBtn.Text = "💾 SAVE"
SaveWHBtn.BackgroundColor3 = Color3.fromRGB(0, 120, 200)
SaveWHBtn.TextColor3 = Color3.fromRGB(255, 255, 255)
SaveWHBtn.Font = Enum.Font.GothamBold
SaveWHBtn.TextSize = 10
Instance.new("UICorner", SaveWHBtn)

SaveWHBtn.MouseButton1Click:Connect(function()
    SaveWebhook(WebhookInput.Text)
    SaveWHBtn.Text = "✔"
    SaveWHBtn.BackgroundColor3 = Color3.fromRGB(0, 170, 80)
    task.wait(1)
    SaveWHBtn.Text = "💾 SAVE"
    SaveWHBtn.BackgroundColor3 = Color3.fromRGB(0, 120, 200)
end)

DailyLimitLabel = Instance.new("TextLabel", TradeFrame)
DailyLimitLabel.Size = UDim2.new(1, 0, 0, 15)
DailyLimitLabel.Position = UDim2.new(0, 0, 0, 315)
DailyLimitLabel.Text = "📮 Acc Limit: Loading..."
DailyLimitLabel.TextColor3 = Color3.fromRGB(0, 200, 255)
DailyLimitLabel.BackgroundTransparency = 1
DailyLimitLabel.TextSize = 10
DailyLimitLabel.Font = Enum.Font.GothamBold

-- ==========================================
-- [NEW] AUTO COLLECT (LỤM ĐỒ SIÊU TỐC)
-- ==========================================
local AutoCollectBtn = Instance.new("TextButton", TradeFrame)
AutoCollectBtn.Size = UDim2.new(0, 250, 0, 25)
AutoCollectBtn.Position = UDim2.new(0.5, -125, 0, 335)
AutoCollectBtn.Text = "⚡ AUTO COLLECT: OFF"
AutoCollectBtn.BackgroundColor3 = Color3.fromRGB(45, 45, 45)
AutoCollectBtn.TextColor3 = Color3.fromRGB(255, 255, 255)
AutoCollectBtn.Font = Enum.Font.GothamBold
AutoCollectBtn.TextSize = 10
Instance.new("UICorner", AutoCollectBtn)

local isAutoCollect = false
AutoCollectBtn.MouseButton1Click:Connect(function()
    isAutoCollect = not isAutoCollect
    if isAutoCollect then
        AutoCollectBtn.Text = "⚡ AUTO COLLECT: ON"
        AutoCollectBtn.BackgroundColor3 = Color3.fromRGB(200, 150, 0)
    else
        AutoCollectBtn.Text = "⚡ AUTO COLLECT: OFF"
        AutoCollectBtn.BackgroundColor3 = Color3.fromRGB(45, 45, 45)
    end
end)

task.spawn(function()
    local RunService = game:GetService("RunService")
    RunService.RenderStepped:Connect(function()
        if isAutoCollect then
            pcall(function()
                local hrp = LP.Character and LP.Character:FindFirstChild("HumanoidRootPart")
                if hrp then
                    -- Common paths for drops in Pet Sim / standard games
                    local dropsFolder = workspace:FindFirstChild("__THINGS") and workspace.__THINGS:FindFirstChild("Drops") or workspace:FindFirstChild("Drops")
                    
                    if dropsFolder then
                        for _, drop in ipairs(dropsFolder:GetChildren()) do
                            if drop:IsA("BasePart") then
                                drop.CFrame = hrp.CFrame
                            elseif drop:IsA("Model") and drop.PrimaryPart then
                                drop:SetPrimaryPartCFrame(hrp.CFrame)
                            end
                        end
                    end
                end
            end)
        end
    end)
end)


-- ==========================================
-- PARSER: SMART VARIANT, TABS & TRUE NAME EXTRACTOR
-- ==========================================
local function isUUID(str)
    return type(str) == "string" and string.match(str, "%x%x%x%x%x%x%x%x%-") ~= nil
end

local function extractRealName(t, fallback)
    if type(t) ~= "table" then return fallback end
    local keys = {"id", "Id", "ItemKey", "itemKey", "Name", "name", "ItemName", "petId", "ItemId"}
    for _, k in ipairs(keys) do
        local v = t[k]
        if type(v) == "string" and string.len(v) > 1 and not isUUID(v) then return v end
    end
    if type(t._data) == "table" then
        for _, k in ipairs(keys) do
            local v = t._data[k]
            if type(v) == "string" and string.len(v) > 1 and not isUUID(v) then return v end
        end
    end
    local function deepSearch(tbl, depth)
        if depth > 4 then return nil end
        for k, v in pairs(tbl) do
            if type(v) == "table" then
                local res = deepSearch(v, depth + 1)
                if res then return res end
            elseif type(v) == "string" and string.len(v) > 1 and not isUUID(v) then
                for _, targetKey in ipairs(keys) do
                    if string.lower(tostring(k)) == string.lower(targetKey) then return v end
                end
            end
        end
        return nil
    end
    return deepSearch(t, 1) or fallback
end

local function ParseItemData(itemData, fallbackID, categoryName)
    local count = 0
    local sendKey = fallbackID
    
    local catStr = type(categoryName) == "string" and string.lower(categoryName) or ""
    local itemCatStr = type(itemData) == "table" and type(itemData.Category) == "string" and string.lower(itemData.Category) or ""
    local isSeedCategory = (string.find(catStr, "seed") or string.find(itemCatStr, "seed"))
    local isPetCategory = (string.find(catStr, "pet") or string.find(itemCatStr, "pet"))
    
    if type(itemData) ~= "table" then
        local bName = tostring(fallbackID)
        local lowerBName = string.lower(bName)
        local isKnownSeed = (lowerBName == "strawberry" or lowerBName == "mega" or lowerBName == "coin" or lowerBName == "diamond" or lowerBName == "potion" or lowerBName == "mushroom")
        
        local tCat = "Other"
        if isPetCategory and not isSeedCategory and not string.find(lowerBName, "seed") then tCat = "Pets"
        elseif isSeedCategory or isKnownSeed or string.find(lowerBName, "seed") then tCat = "Seeds" end
        
        if (isSeedCategory or isKnownSeed) and not isPetCategory and not string.find(lowerBName, "seed") then bName = bName .. " Seed" end
        return bName, bName, tonumber(itemData) or 0, fallbackID, tCat
    end

    local d = itemData._data or itemData
    local baseName = extractRealName(d, fallbackID)
    if isUUID(baseName) or baseName == "Unknown" then baseName = "Pet" end

    local lowerBase = string.lower(baseName)
    local isKnownSeed = (lowerBase == "strawberry" or lowerBase == "mega" or lowerBase == "coin" or lowerBase == "diamond" or lowerBase == "potion" or lowerBase == "mushroom")
    
    local tabCat = "Other"
    if isPetCategory and not isSeedCategory and not string.find(lowerBase, "seed") then
        tabCat = "Pets"
    elseif isSeedCategory or isKnownSeed or string.find(lowerBase, "seed") then
        tabCat = "Seeds"
    end
    
    if (isSeedCategory or isKnownSeed) and not isPetCategory and not string.find(lowerBase, "seed") then
        baseName = baseName .. " Seed"
    end

    local mods = {}
    local function addMod(m)
        local sm = string.lower(tostring(m))
        for _, v in ipairs(mods) do if string.lower(v) == sm then return end end
        table.insert(mods, tostring(m))
    end

    if d.sh or d.shiny or d.isShiny then addMod("Shiny") end
    
    local pt = d.pt or d.tier or d.Tier or d.type or d.Type or d.rarity or d.Rarity
    if pt then
        if pt == 1 or pt == "Gold" then addMod("Gold")
        elseif pt == 2 or pt == "Rainbow" then addMod("Rainbow")
        elseif pt == 3 or pt == "Mega" then addMod("Mega")
        elseif pt == 4 or pt == "Mig" then addMod("Mig")
        elseif pt == 5 or pt == "Big" then addMod("Big")
        elseif type(pt) == "string" and string.len(pt) < 15 and not isUUID(pt) then addMod(pt)
        elseif type(pt) == "number" then addMod("T" .. tostring(pt))
        end
    end
    
    local var = d.v or d.variant or d.Variant or d.sz or d.size or d.Size or d.mutation or d.Mutation
    if type(var) == "string" and string.len(var) < 15 and not isUUID(var) then addMod(var) end

    local name = baseName
    if #mods > 0 then name = table.concat(mods, " ") .. " " .. baseName end

    count = tonumber(itemData.Amount or itemData._am or itemData.Count or itemData.Quantity) or 1
    local hash = name
    sendKey = itemData.ItemKey or itemData.id or fallbackID
    
    return name, hash, count, sendKey, tabCat
end

-- ==========================================
-- DYNAMIC INVENTORY FETCHING & NEW UI ROW
-- ==========================================
local function GetInventoryData()
    local ok, replica = pcall(function() return PS:WaitForLocalReplica(1) end)
    if not (ok and replica and replica.Data) then return {} end
    
    local dictsToScan = {}
    if replica.Data.Inventory then dictsToScan.Inventory = replica.Data.Inventory end
    if replica.Data.Pets then dictsToScan.Pets = replica.Data.Pets end
    if replica.Data.Pet then dictsToScan.Pet = replica.Data.Pet end

    local AggregatedItems = {} 

    for catName, invDict in pairs(dictsToScan) do
        for category, items in pairs(invDict) do
            local iterTable = (type(items) == "table" and not items.id and not items.ItemKey) and items or { [category] = items }
            local iterCat = (type(items) == "table" and not items.id and not items.ItemKey) and category or catName

            for itemID, itemData in pairs(iterTable) do
                pcall(function()
                    local name, hash, count, sendKey, tabCat = ParseItemData(itemData, itemID, iterCat)
                    if count > 0 then
                        if not AggregatedItems[hash] then
                            AggregatedItems[hash] = { DisplayName = name, Total = 0, RawItems = {}, TabCategory = tabCat }
                        end
                        AggregatedItems[hash].Total = AggregatedItems[hash].Total + count
                        table.insert(AggregatedItems[hash].RawItems, {
                            DictKey = itemID, Category = iterCat, ItemKey = sendKey, Available = count
                        })
                    end
                end)
            end
        end
    end
    return AggregatedItems
end

local isRefreshing = false

local function RefreshInventoryUI()
    if isRefreshing then return end 
    isRefreshing = true
    
    task.spawn(function()
        pcall(function()
            local AggregatedItems = GetInventoryData()
            local trackedHashes = {}
            for h, _ in pairs(ItemInputs) do trackedHashes[h] = true end
            
            for hash, data in pairs(AggregatedItems) do
                trackedHashes[hash] = nil
                local isVisible = (CurrentTab == "All" or CurrentTab == data.TabCategory)
                
                if not ItemInputs[hash] then
                    local Row = Instance.new("Frame", Scroll)
                    Row.Size = UDim2.new(1, -10, 0, 26)
                    Row.BackgroundColor3 = Color3.fromRGB(30, 30, 30)
                    Row.Visible = isVisible
                    Instance.new("UICorner", Row)

                    local NameLabel = Instance.new("TextLabel", Row)
                    NameLabel.Size = UDim2.new(0.40, 0, 1, 0)
                    NameLabel.Position = UDim2.new(0.02, 0, 0, 0)
                    NameLabel.Text = data.DisplayName
                    NameLabel.TextColor3 = Color3.fromRGB(255, 255, 255)
                    NameLabel.TextXAlignment = Enum.TextXAlignment.Left
                    NameLabel.TextSize = 10
                    NameLabel.BackgroundTransparency = 1
                    NameLabel.ClipsDescendants = true

                    local CountLabel = Instance.new("TextLabel", Row)
                    CountLabel.Size = UDim2.new(0.12, 0, 1, 0)
                    CountLabel.Position = UDim2.new(0.42, 0, 0, 0)
                    CountLabel.Text = "(" .. data.Total .. ")"
                    CountLabel.TextColor3 = Color3.fromRGB(150, 150, 150)
                    CountLabel.TextSize = 9
                    CountLabel.BackgroundTransparency = 1

                    -- Resize input to fit the new button
                    local AmountInp = Instance.new("TextBox", Row)
                    AmountInp.Size = UDim2.new(0.14, 0, 0.8, 0)
                    AmountInp.Position = UDim2.new(0.55, 0, 0.1, 0)
                    AmountInp.Text = "0"
                    AmountInp.BackgroundColor3 = Color3.fromRGB(45, 45, 45)
                    AmountInp.TextColor3 = Color3.fromRGB(0, 255, 127)
                    AmountInp.TextSize = 10
                    Instance.new("UICorner", AmountInp)
                    
                    local MaxBtn = Instance.new("TextButton", Row)
                    MaxBtn.Size = UDim2.new(0.13, 0, 0.8, 0)
                    MaxBtn.Position = UDim2.new(0.71, 0, 0.1, 0)
                    MaxBtn.Text = "ALL"
                    MaxBtn.BackgroundColor3 = Color3.fromRGB(0, 120, 200)
                    MaxBtn.TextColor3 = Color3.fromRGB(255, 255, 255)
                    MaxBtn.Font = Enum.Font.GothamBold
                    MaxBtn.TextSize = 8
                    Instance.new("UICorner", MaxBtn)
                    
                    MaxBtn.MouseButton1Click:Connect(function()
                        if ItemInputs[hash] then
                            AmountInp.Text = tostring(ItemInputs[hash].Total)
                        end
                    end)

                    -- [NEW] NÚT AUTO DROP VẬT PHẨM THEO SỐ LƯỢNG
                    local DropBtn = Instance.new("TextButton", Row)
                    DropBtn.Size = UDim2.new(0.13, 0, 0.8, 0)
                    DropBtn.Position = UDim2.new(0.86, 0, 0.1, 0)
                    DropBtn.Text = "DROP"
                    DropBtn.BackgroundColor3 = Color3.fromRGB(200, 50, 50)
                    DropBtn.TextColor3 = Color3.fromRGB(255, 255, 255)
                    DropBtn.Font = Enum.Font.GothamBold
                    DropBtn.TextSize = 8
                    Instance.new("UICorner", DropBtn)
                    
                    DropBtn.MouseButton1Click:Connect(function()
                        local reqAmt = math.floor(tonumber(AmountInp.Text) or 0)
                        if reqAmt > 0 and ItemInputs[hash] then
                            local leftToDrop = reqAmt
                            for _, rawItem in ipairs(ItemInputs[hash].RawItems) do
                                if leftToDrop <= 0 then break end
                                local takeAmt = math.min(leftToDrop, rawItem.Available)
                                
                                pcall(function()
                                    -- Thay Net.Items.Drop bằng Remote đúng của Game nếu bị sai
                                    if Net.Items and Net.Items.Drop then
                                        Net.Items.Drop:Fire(rawItem.ItemKey, takeAmt)
                                    elseif Net.Inventory and Net.Inventory.Drop then
                                        Net.Inventory.Drop:Fire(rawItem.ItemKey, takeAmt)
                                    end
                                end)
                                
                                leftToDrop = leftToDrop - takeAmt
                            end
                            AmountInp.Text = "0"
                            task.delay(0.5, function() RefreshInventoryUI() end)
                        end
                    end)

                    ItemInputs[hash] = { UI = Row, Input = AmountInp, Label = CountLabel, Max = MaxBtn, Drop = DropBtn, RawItems = data.RawItems, DisplayName = data.DisplayName, TabCategory = data.TabCategory, Total = data.Total }
                else
                    ItemInputs[hash].UI.Visible = isVisible
                    ItemInputs[hash].Label.Text = "(" .. data.Total .. ")"
                    ItemInputs[hash].RawItems = data.RawItems 
                    ItemInputs[hash].TabCategory = data.TabCategory
                    ItemInputs[hash].Total = data.Total
                end
            end
            
            for missingHash, _ in pairs(trackedHashes) do
                if ItemInputs[missingHash] then
                    ItemInputs[missingHash].UI.Visible = false
                    ItemInputs[missingHash].Input.Text = "0"
                    ItemInputs[missingHash].RawItems = {}
                    ItemInputs[missingHash].Total = 0
                end
            end
            
            Scroll.CanvasSize = UDim2.new(0, 0, 0, ListLayout.AbsoluteContentSize.Y)
        end)
        isRefreshing = false
    end)
end

-- ==========================================
-- MAILBOX FUNCTIONS
-- ==========================================
local function ClaimAllMail()
    ClaimAllBtn.Active = false
    StatusLabel.Text = "Opening mailbox..."
    
    local ok, inbox = pcall(function() 
        local res = Net.Mailbox.OpenInbox:Fire()
        if type(res) == "table" and res.Wait then return res:Wait() end
        return res
    end)
    
    if not ok or type(inbox) ~= "table" or next(inbox) == nil then
        StatusLabel.Text = "Mailbox is empty!"
        ClaimAllBtn.Active = true
        return
    end
    
    local count = 0
    for mailId, mailData in pairs(inbox) do
        count = count + 1
        StatusLabel.Text = "Claiming mail #" .. count .. "..."
        
        local senderUid = tonumber(mailData.Sender or mailData.SenderId or mailData.From or mailData.Player) or 1
        local senderName = "Unknown"
        if senderUid > 1 then
            pcall(function() senderName = Players:GetNameFromUserIdAsync(senderUid) end)
            if senderName == "Unknown" then senderName = tostring(senderUid) end
        end
        
        local mailCat = type(mailData) == "table" and (mailData.Category or (mailData.Item and mailData.Item.Category)) or ""
        local itemsToProcess = {}
        if type(mailData) == "table" then
            if type(mailData.Items) == "table" and #mailData.Items > 0 then
                for _, v in ipairs(mailData.Items) do table.insert(itemsToProcess, v) end
            elseif type(mailData.items) == "table" and #mailData.items > 0 then
                for _, v in ipairs(mailData.items) do table.insert(itemsToProcess, v) end
            elseif type(mailData.Item) == "table" then
                table.insert(itemsToProcess, mailData.Item)
            elseif type(mailData.item) == "table" then
                table.insert(itemsToProcess, mailData.item)
            else
                table.insert(itemsToProcess, mailData) 
            end
        end
        
        local aggregated = {}
        local itemOrder = {}
        for _, itemObj in ipairs(itemsToProcess) do
            local fallback = "Unknown"
            if type(itemObj) == "table" then fallback = itemObj.ItemKey or itemObj.Id or itemObj.id or itemObj.Name or "Unknown" end
            if fallback == "Unknown" and type(mailData) == "table" then fallback = mailData.ItemKey or mailData.Id or "Unknown" end
            
            local itemName, _, itemCount = ParseItemData(itemObj, fallback, mailCat)
            
            if type(itemObj) == "table" and (itemObj.Amount or itemObj.Count) then itemCount = tonumber(itemObj.Amount or itemObj.Count) or itemCount
            elseif type(mailData) == "table" and (mailData.Amount or mailData.Count) and #itemsToProcess == 1 then itemCount = tonumber(mailData.Amount or mailData.Count) or itemCount end
            
            if not aggregated[itemName] then
                aggregated[itemName] = 0
                table.insert(itemOrder, itemName)
            end
            aggregated[itemName] = aggregated[itemName] + itemCount
        end

        local displayStrings = {}
        for _, name in ipairs(itemOrder) do
            table.insert(displayStrings, name .. " x" .. aggregated[name])
        end
        local combinedItemStr = table.concat(displayStrings, ", ")
        
        pcall(function() Net.Mailbox.Claim:Fire(mailId) end)
        
        AddHistoryRecord(senderUid, senderName, combinedItemStr, "", "claim")
        task.wait(0.2)
    end
    
    StatusLabel.Text = "✅ Claimed " .. count .. " mails!"
    task.spawn(function() task.wait(0.5); RefreshInventoryUI() end)
    ClaimAllBtn.Active = true
end

-- ==========================================
-- SMART DYNAMIC BATCH SENDER (V3.19 KERNEL)
-- ==========================================
local function ExecuteSendMailBatch(uid, targetName, itemRequests)
    local anyItemSent = false
    
    while true do
        if GetMyCount() >= 50 then
            StatusLabel.TextColor3 = Color3.fromRGB(255, 50, 50)
            StatusLabel.Text = "Daily limit reached. Stopping."
            break
        end

        local currentInv = GetInventoryData()
        local batch = {}
        local isAllDone = true
        local currentHashName = ""
        local currentSent = 0
        local currentReq = 0
        
        for hash, req in pairs(itemRequests) do
            local currTotal = currentInv[hash] and currentInv[hash].Total or 0
            
            local actuallySent = req.InitialTotal - currTotal
            req.Sent = actuallySent
            
            local leftToSend = req.Requested - actuallySent
            
            if leftToSend > 0 then
                isAllDone = false
                if currentHashName == "" then 
                    currentHashName = req.DisplayName
                    currentSent = actuallySent
                    currentReq = req.Requested
                end
                
                if currentInv[hash] then
                    for _, rawItem in ipairs(currentInv[hash].RawItems) do
                        if leftToSend <= 0 then break end
                        if #batch >= 20 then break end
                        
                        local takeAmt = math.min(leftToSend, rawItem.Available)
                        table.insert(batch, {Category = rawItem.Category, ItemKey = rawItem.ItemKey, Count = takeAmt})
                        leftToSend = leftToSend - takeAmt
                    end
                end
            end
        end
        
        if isAllDone then break end
        
        if #batch == 0 then
            StatusLabel.TextColor3 = Color3.fromRGB(255, 100, 100)
            StatusLabel.Text = "Out of items in inventory!"
            break
        end
        
        StatusLabel.TextColor3 = Color3.fromRGB(200, 200, 200)
        StatusLabel.Text = string.format("Sending [%s] %d/%d...", currentHashName, currentSent, currentReq)
        
        local batchOk = pcall(function() 
            local res = Net.Mailbox.SendBatch:Fire(uid, batch, "Made by Khogamegiare.com") 
            if type(res) == "table" and res.Wait then res:Wait() end
        end)
        
        if batchOk then
            anyItemSent = true
            AddMyCount(1)
            
            StatusLabel.TextColor3 = Color3.fromRGB(0, 255, 127)
            for countdown = 9, 1, -1 do
                local dots = string.rep(".", 1 + (9 - countdown) % 5)
                StatusLabel.Text = string.format("Checking & Waiting %ds%s", countdown, dots)
                task.wait(1)
            end
        else
            StatusLabel.TextColor3 = Color3.fromRGB(255, 50, 50)
            StatusLabel.Text = "ERROR: Network lag. Retrying in 5s..."
            task.wait(5)
        end
    end
    
    if anyItemSent then
        StatusLabel.TextColor3 = Color3.fromRGB(0, 255, 127)
        if not string.match(StatusLabel.Text, "ERROR") and not string.match(StatusLabel.Text, "limit") and not string.match(StatusLabel.Text, "Out of") then
            StatusLabel.Text = "✅ ALL ITEMS SENT SUCCESSFULLY!"
        end
        
        local webhookLines = {}
        local displayStrings = {}
        local isAnyShort = false

        local finalInv = GetInventoryData()
        for hash, req in pairs(itemRequests) do
            local currTotal = finalInv[hash] and finalInv[hash].Total or 0
            req.Sent = req.InitialTotal - currTotal
            
            if req.Sent > 0 then
                local finalAfter = math.max(0, req.InitialTotal - req.Sent)
                
                if req.Sent < req.Requested then
                    isAnyShort = true
                    table.insert(displayStrings, string.format("%s %d/%d", req.DisplayName, req.Sent, req.Requested))
                    table.insert(webhookLines, string.format("📦 **Item:** %s | 🔢 **Amount:** %d/%d | 📉 **Stock:** %d ➔ %d", req.DisplayName, req.Sent, req.Requested, req.InitialTotal, finalAfter))
                else
                    table.insert(displayStrings, string.format("%s x%d", req.DisplayName, req.Sent))
                    table.insert(webhookLines, string.format("📦 **Item:** %s | 🔢 **Amount:** x%d | 📉 **Stock:** %d ➔ %d", req.DisplayName, req.Sent, req.InitialTotal, finalAfter))
                end
            end
        end
        
        local combinedDisplay = table.concat(displayStrings, ", ")
        local webhookDesc = string.format("👤 **To:** `%s`\n\n%s", targetName, table.concat(webhookLines, "\n"))
        
        AddHistoryRecord(uid, targetName, combinedDisplay, "", isAnyShort and "short" or "send")
        SendWebhookLog(uid, targetName, webhookDesc, isAnyShort)
        
        for _, data in pairs(ItemInputs) do data.Input.Text = "0" end
        UserInput.Text = ""
        
        task.spawn(function() task.wait(0.5); RefreshInventoryUI() end)
    end
    StartBtn.Active = true
end

local function SendMailBatch()
    if GetMyCount() >= 50 then
        StatusLabel.TextColor3 = Color3.fromRGB(255, 50, 50)
        StatusLabel.Text = "ERROR: Reached 50/50 daily limit!"
        return
    end

    local targetName = UserInput.Text
    if targetName == "" then StatusLabel.Text = "Enter Username!" return end
    
    StartBtn.Active = false
    StatusLabel.Text = "Fetching UID..."
    
    local ok, uid = pcall(function() 
        local res = Net.Mailbox.LookupPlayer:Fire(targetName) 
        if type(res) == "table" and res.Get then return res:Get() end
        return res
    end)
    
    if not ok or type(uid) ~= "number" or uid <= 0 then
        StatusLabel.Text = "ERROR: User not found!"
        StartBtn.Active = true
        return
    end
    
    local itemRequests = {} 
    local summaryLines = {}
    local hasRequests = false
    
    local currentInv = GetInventoryData()
    for hash, data in pairs(ItemInputs) do
        local requestedVal = math.floor(tonumber(data.Input.Text) or 0)
        
        if requestedVal > 0 then
            local currTotal = currentInv[hash] and currentInv[hash].Total or 0
            local actuallySentTarget = math.min(requestedVal, currTotal)
            
            if actuallySentTarget > 0 then
                itemRequests[hash] = {
                    DisplayName = data.DisplayName,
                    Requested = actuallySentTarget,
                    InitialTotal = currTotal,
                    Sent = 0
                }
                table.insert(summaryLines, data.DisplayName .. " x" .. actuallySentTarget)
                hasRequests = true
            end
        end
    end
    
    if not hasRequests then StatusLabel.Text = "Enter valid amount!" StartBtn.Active = true return end
    
    if AppSettings.ConfirmPrompt then
        local summaryStr = table.concat(summaryLines, ", ")
        local promptMsg = string.format("Are you sure you want to send:\n%s\n\nTo user: %s?", summaryStr, targetName)
        
        ShowConfirmPrompt(promptMsg, function()
            ExecuteSendMailBatch(uid, targetName, itemRequests)
        end, function()
            StatusLabel.Text = "Gift cancelled."
            StartBtn.Active = true
        end)
    else
        ExecuteSendMailBatch(uid, targetName, itemRequests)
    end
end

-- ==========================================
-- WINDOW CONTROLS
-- ==========================================
ClaimAllBtn.MouseButton1Click:Connect(ClaimAllMail)
StartBtn.MouseButton1Click:Connect(SendMailBatch)

local CloseBtn = Instance.new("TextButton", MainFrame)
CloseBtn.Size = UDim2.new(0, 20, 0, 20)
CloseBtn.Position = UDim2.new(1, -24, 0, 5)
CloseBtn.Text = "X"
CloseBtn.BackgroundColor3 = Color3.fromRGB(180, 0, 0)
CloseBtn.TextColor3 = Color3.fromRGB(255, 255, 255)
CloseBtn.Font = Enum.Font.GothamBold
Instance.new("UICorner", CloseBtn)
CloseBtn.MouseButton1Click:Connect(function() ScreenGui:Destroy() end)

local MinBtn = Instance.new("TextButton", MainFrame)
MinBtn.Size = UDim2.new(0, 20, 0, 20)
MinBtn.Position = UDim2.new(1, -48, 0, 5)
MinBtn.Text = "-"
MinBtn.BackgroundColor3 = Color3.fromRGB(220, 150, 0)
MinBtn.TextColor3 = Color3.fromRGB(255, 255, 255)
MinBtn.Font = Enum.Font.GothamBold
Instance.new("UICorner", MinBtn)

local isMinimized = false
MinBtn.MouseButton1Click:Connect(function()
    isMinimized = not isMinimized
    if isMinimized then
        TradeFrame.Visible = false
        HistoryFrame.Visible = false
        MainFrame.Size = UDim2.new(0, 270, 0, 30)
        MinBtn.Text = "+"
    else
        TradeFrame.Visible = true
        HistoryFrame.Visible = true
        MainFrame.Size = UDim2.new(0, 270, 0, 395)
        MinBtn.Text = "-"
    end
end)

-- ==========================================
-- INBOX RADAR LOGIC
-- ==========================================
local TrackedMails = {}
local isFirstInboxCheck = true

task.spawn(function()
    while task.wait(10) do
        pcall(function()
            local ok, inbox = pcall(function() 
                local res = Net.Mailbox.OpenInbox:Fire()
                if type(res) == "table" and res.Wait then return res:Wait() end
                return res
            end)
            
            if ok and type(inbox) == "table" then
                for mailId, mailData in pairs(inbox) do
                    if not TrackedMails[mailId] then
                        TrackedMails[mailId] = true
                        
                        if not isFirstInboxCheck then
                            local senderUid = tonumber(mailData.Sender or mailData.SenderId or mailData.From or mailData.Player) or 1
                            local senderName = "Unknown"
                            
                            if senderUid > 1 then
                                local success, name = pcall(function() return Players:GetNameFromUserIdAsync(senderUid) end)
                                if success and name then senderName = name
                                else
                                    task.wait(1)
                                    success, name = pcall(function() return Players:GetNameFromUserIdAsync(senderUid) end)
                                    if success and name then senderName = name else senderName = tostring(senderUid) end
                                end
                            end
                            
                            local mailCat = type(mailData) == "table" and (mailData.Category or (mailData.Item and mailData.Item.Category)) or ""
                            local itemsToProcess = {}
                            if type(mailData) == "table" then
                                if type(mailData.Items) == "table" and #mailData.Items > 0 then
                                    for _, v in ipairs(mailData.Items) do table.insert(itemsToProcess, v) end
                                elseif type(mailData.items) == "table" and #mailData.items > 0 then
                                    for _, v in ipairs(mailData.items) do table.insert(itemsToProcess, v) end
                                elseif type(mailData.Item) == "table" then
                                    table.insert(itemsToProcess, mailData.Item)
                                elseif type(mailData.item) == "table" then
                                    table.insert(itemsToProcess, mailData.item)
                                else
                                    table.insert(itemsToProcess, mailData) 
                                end
                            end
                            
                            local aggregated = {}
                            local itemOrder = {}
                            
                            for _, itemObj in ipairs(itemsToProcess) do
                                local fallback = "Unknown"
                                if type(itemObj) == "table" then fallback = itemObj.ItemKey or itemObj.Id or itemObj.id or itemObj.Name or "Unknown" end
                                if fallback == "Unknown" and type(mailData) == "table" then fallback = mailData.ItemKey or mailData.Id or "Unknown" end
                                
                                local itemName, _, count = ParseItemData(itemObj, fallback, mailCat)
                                
                                if type(itemObj) == "table" and (itemObj.Amount or itemObj.Count) then count = tonumber(itemObj.Amount or itemObj.Count) or count
                                elseif type(mailData) == "table" and (mailData.Amount or mailData.Count) and #itemsToProcess == 1 then count = tonumber(mailData.Amount or mailData.Count) or count end
                                
                                if not aggregated[itemName] then
                                    aggregated[itemName] = 0
                                    table.insert(itemOrder, itemName)
                                end
                                aggregated[itemName] = aggregated[itemName] + count
                            end

                            local displayStrings = {}
                            local webhookLines = {}
                            for _, name in ipairs(itemOrder) do
                                local c = aggregated[name]
                                table.insert(displayStrings, name .. " x" .. c)
                                table.insert(webhookLines, "📦 **Item:** " .. name .. " | 🔢 **Amount:** x" .. c)
                            end
                            
                            local combinedItemStr = table.concat(displayStrings, ", ")
                            local webhookDesc = string.format("👤 **From:** `%s`\n\n%s", senderName, table.concat(webhookLines, "\n"))
                            
                            AddHistoryRecord(senderUid, senderName, combinedItemStr, "", "online_receive")
                            SendReceiveWebhookLog(senderUid, senderName, webhookDesc)
                        end
                    end
                end
                isFirstInboxCheck = false
            end
        end)
    end
end)

-- ==========================================
-- BACKGROUND SYNC & INITIALIZATION
-- ==========================================
task.spawn(function()
    LoadDailyCount()
end)

task.spawn(function() RefreshInventoryUI() end)
task.spawn(function()
    while task.wait(1) do
        if TradeFrame.Visible and not isMinimized then RefreshInventoryUI() end
    end
end)
