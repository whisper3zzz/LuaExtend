-- LuaTask 模块，提供任务调度和异步操作的支持
local LuaTask = {}
local Scheduler = require("Scheduler")

-- CancellationTokenSource 类
local CancellationTokenSource = {}
CancellationTokenSource.__index = CancellationTokenSource

function CancellationTokenSource.new()
    local self = setmetatable({}, CancellationTokenSource)
    self._cancelled = false
    self._callbacks = {}
    self.token = {
        cancelled = false,
        onCancel = function(_, fn)
            if self._cancelled then
                fn()
            else
                table.insert(self._callbacks, fn)
            end
        end
    }
    return self
end

function CancellationTokenSource:cancel()
    if self._cancelled then return end
    self._cancelled = true
    self.token.cancelled = true
    for _, cb in ipairs(self._callbacks) do
        pcall(cb)
    end
    self._callbacks = {}
end

function CancellationTokenSource:isCancelled()
    return self._cancelled
end

LuaTask.CancellationTokenSource = CancellationTokenSource

-- Task 类
LuaTask.Task = {}
LuaTask.Task.__index = LuaTask.Task

function LuaTask.Task.new(func, cts)
    local self = setmetatable({}, LuaTask.Task)
    self.completed = false
    self.result = nil
    self.error = nil
    self.cts = cts or CancellationTokenSource.new()
    self.token = self.cts.token
    self.continuations = {}

    local function resolve(value)
        if self.completed then return end
        self.completed = true
        self.result = value
        for _, cont in ipairs(self.continuations) do
            cont(value, nil)
        end
    end

    local function reject(err)
        if self.completed then return end
        self.completed = true
        self.error = err
        for _, cont in ipairs(self.continuations) do
            cont(nil, err)
        end
    end

    local ok, err = pcall(function()
        func(resolve, reject, self.token)
    end)
    if not ok then
        reject(err)
    end

    return self
end

function LuaTask.Task:await()
    return function(callback)
        if self.completed then
            callback(self.result, self.error)
        else
            table.insert(self.continuations, callback)
        end
    end
end

function LuaTask.Task:continueWith(fn)
    if self.completed then
        fn(self)
    else
        table.insert(self.continuations, function()
            fn(self)
        end)
    end
    return self
end

function LuaTask.Task:getResult()
    if not self.completed then
        error("Task not completed")
    end
    if self.error then
        error(self.error)
    end
    return self.result
end

function LuaTask.Task:isCompleted()
    return self.completed
end

function LuaTask.Task:isCanceled()
    return self.token.cancelled
end

function LuaTask.Task:isFaulted()
    return self.error ~= nil
end

function LuaTask.Task:cancel()
    if self.cts then
        self.cts:cancel()
    end
end

function LuaTask.Task.run(asyncFn)
    return LuaTask.Task.new(function(resolve, reject, token)
        local co = coroutine.create(asyncFn)
        local function step(...)
            if token.cancelled then
                reject("cancelled")
                return
            end
            local ok, ret = coroutine.resume(co, ...)
            if not ok then
                reject(ret)
                return
            end
            if coroutine.status(co) == "dead" then
                resolve(ret)
            elseif type(ret) == "function" then
                ret(function(r, err)
                    if err then
                        reject(err)
                    else
                        step(r)
                    end
                end)
            else
                reject("Invalid yield")
            end
        end
        step()
    end)
end

function LuaTask.Task:forget()
    self:continueWith(function(t)
        if t:isFaulted() then
            if LuaTask.onForgetError then
                LuaTask.onForgetError(t.error)
            else
                print(" Task error: ", t.error)
            end
        end
    end)
end

LuaTask.onForgetError = function(err)
    print("任务被忘记，但抛出了异常：", err)
end

-- 等待 yieldable
function LuaTask.await(yieldable)
    local co = coroutine.running()
    assert(co, "await must be used inside coroutine")
    yieldable(function(result, err)
        if err then
            error(err)
        else
            coroutine.resume(co, result)
        end
    end)
    return coroutine.yield()
end

-- whenAll
function LuaTask.Task.whenAll(tasks)
    return LuaTask.Task.new(function(resolve, reject, _)
        local results = {}
        local remaining = #tasks
        if remaining == 0 then
            resolve(results)
            return
        end
        for i, task in ipairs(tasks) do
            task:continueWith(function(t)
                if t:isFaulted() then
                    reject(t.error)
                    return
                elseif t:isCanceled() then
                    reject("One task was cancelled")
                    return
                end
                results[i] = t:getResult()
                remaining = remaining - 1
                if remaining == 0 then
                    resolve(results)
                end
            end)
        end
    end)
end

-- whenAny
function LuaTask.Task.whenAny(tasks)
    return LuaTask.Task.new(function(resolve, reject, _)
        local resolved = false
        for _, task in ipairs(tasks) do
            task:continueWith(function(t)
                if resolved then return end
                if t:isFaulted() then
                    reject(t.error)
                elseif t:isCanceled() then
                    reject("One task was cancelled")
                else
                    resolved = true
                    resolve(t:getResult())
                end
            end)
        end
    end)
end

-- delay
function LuaTask.delay(ms, cts)
    return LuaTask.Task.new(function(resolve, _, token)
        Scheduler.delay(ms, function()
            if (cts and cts:isCancelled()) or (token and token.cancelled) then return end
            resolve()
        end)
    end, cts)
end

-- nextFrame
function LuaTask.nextFrame(cts)
    return LuaTask.Task.new(function(resolve, _, token)
        Scheduler.delay(0, function()
            if (cts and cts:isCancelled()) or (token and token.cancelled) then return end
            resolve()
        end)
    end, cts)
end

return LuaTask
