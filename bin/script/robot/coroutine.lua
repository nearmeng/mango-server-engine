coro_data = {}
coro_data.success = {}
coro_data.failed = {}
coro_data.coro_list = {}

function coro_resume(coname, ...)
    coro_data.current_coname = coname
    local ret,msg = coroutine.resume(coro_data.coro_list[coname].co, ...)

    if ret == false then
        table.insert(coro_data.failed, msg)
        check_print(coname, "coroutine execute failed, err msg: " .. msg, tcstFailed)
    elseif ret == true and coroutine.status(coro_data.coro_list[coname].co) == 'dead' then
        table.insert(coro_data.success, coname)
        check_print(coname, "corotine finished success", tcstPassed)
    end

end

function coro_create(coname, fun, endfun, userdata, ...)
    coro_data.coro_list[coname] = {}
    coro_data.coro_list[coname].fun = fun
    coro_data.coro_list[coname].endfun = endfun
    coro_data.coro_list[coname].userdata = userdata
    coro_data.coro_list[coname].wait_end_time = 0
    coro_data.coro_list[coname].last_expect_time = 0
    coro_data.coro_list[coname].co = coroutine.create(fun)
    coro_resume(coname, ...)
end

function coro_mainloop()

    curr_time = os.time()
    for k,v in pairs(coro_data.coro_list) do
        -- check coro status
        local status = coroutine.status(v.co)
        if (status == 'dead') then
            if (endfun ~= nil) then
                v.endfun()
            end
            coro_data.coro_list[k] = nil
        -- check wait timeout 
        elseif (v.wait_end_time ~=0 and v.wait_end_time < curr_time) then
                coro_resume(k)
        -- chekc msg wait timeout
        elseif (v.last_expect_time ~= 0 and curr_time - v.last_expect_time > RECV_TIMEOUT_THRES) then
                print("recv msg timeout")
                coro_resume(k)
        end
    end
end

function coro_get_curr()
    local current_coname = coro_data.current_coname
    return coro_data.coro_list[current_coname]
end

function coro_check(coname)
    
    if(coro_data.coro_list[coname] == nil) then
        return false
    elseif coroutine.status(coro_data.coro_list[coname].co) == 'dead' then
        return false
    end

    return true;
end

--curr coro op
function coro_wait(secs)
    if (coro_data.current_coname == nil) then
        print("error: current coname is nil")
        return
    end

    coro_data.coro_list[coro_data.current_coname].wait_end_time = secs + os.time()
    coroutine.yield()

    coro_data.coro_list[coro_data.current_coname].wait_end_time = 0
end

function coro_reset_curr_expect_time()
    if (coro_data.current_coname == nil) then
        print("error: current coname is nil")
        return
    end
    
    coro_data.coro_list[coro_data.current_coname].last_expect_time = os.time()
end

function coro_clear_curr_expect_time()
    if (coro_data.current_coname == nil) then
        print("error: current coname is nil")
        return
    end
    
    coro_data.coro_list[coro_data.current_coname].last_expect_time = 0
end

function recv(user, expected_msg1, expected_msg2)
	
	expected_msg2 = expected_msg2 or 0
	user:set_lua_wait_msgid(expected_msg1, expected_msg2)
    --print("wait for msg:" .. expected_msg1)

    coro_reset_curr_expect_time()
	recv_table = coroutine.yield()
    coro_clear_curr_expect_time()

    --user:clear_lua_wait_msgid()
    user:set_lua_wait_msgid(0, 0)

	--if recv_table ~= nil then
	--	print(recv_table)
	--end
	return recv_table
end

-- 支持任意用户收取任意msg

function recv_many(...)
    local n = select('#', ...)
    local user, expected_msg
    local users, recv_tables, res_pos, msgs = {}, {}, {}, {}
    if n % 2 ~= 0 then
        print("recv_many has odd parameters")
        return
    end
    for i=1, n / 2, 1 do
        user, expected_msg = select(2 * i - 1, ...)
        local userid = user:get_user_id()
        
        users[userid] = user

        if res_pos[userid] == nil then
            res_pos[userid] = {}
        end

        if res_pos[userid][expected_msg] == nil then
            res_pos[userid][expected_msg] = {}
        end

        table.insert(res_pos[userid][expected_msg], 1, i)

        if msgs[userid] == nil then
            msgs[userid] = {}
        end
        table.insert(msgs[userid], expected_msg)
    end

    for k,v in pairs(users) do
        user_recv(v:get_connid(), table.unpack(msgs[k]))
    end

    coro_reset_curr_expect_time()
    for i=1, n / 2, 1 do
        local recv_table = coroutine.yield()
        if recv_table ~= nil then
            local pos = table.remove(res_pos[recv_table["userid"]][recv_table["msgid"]])
            recv_tables[pos] = recv_table
        end
    end
    coro_clear_curr_expect_time()
    return recv_tables
end

-- 带条件的msg接收，支持传入自定义的函数或闭包
-- 传入的函数参数为接收到的msg

function recv_with_condition(user, expected_msg, condition)
    local v = coro_get_curr()
    local res
    user:set_lua_wait_msgid(expected_msg, 0)
    coro_reset_curr_expect_time()
    while(true) do
        local recv_table = coroutine.yield()
        if condition(recv_table) then
            coro_clear_curr_expect_time()
            return recv_table
        end
        if (v.last_expect_time ~= 0 and curr_time - v.last_expect_time > RECV_TIMEOUT_THRES) then
            coro_clear_curr_expect_time()
            return nil
        end
        user:set_lua_wait_msgid(expected_msg, 0)
    end
end

function recv_until_nil(user, expected_msg)
    local v = coro_get_curr()
    local res
    user:set_lua_wait_msgid(expected_msg, 0)
    
    while(true) do
        coro_reset_curr_expect_time()
        local recv_table = coroutine.yield()
        if (v.last_expect_time ~= 0 and curr_time - v.last_expect_time > RECV_TIMEOUT_THRES) then
            coro_clear_curr_expect_time()
            return true
        end
        user:set_lua_wait_msgid(expected_msg, 0)
    end
end

function recv_error_code(user_name, error_code)
    local msg
	while (true)
	do
		msg = recv(user_name, msgid.g2c_notify_error_code);
		if msg == nil or msg.code == error_code then
			return msg
		end
    end
    return nil
end

function recv_two(user1, expected_msg1, user2, expected_msg2)
    user1:set_lua_wait_msgid(expected_msg1, 0)
    user2:set_lua_wait_msgid(expected_msg2, 0)

    coro_reset_curr_expect_time()
    recv_table = coroutine.yield()

    if recv_table ~= nil then
        --print(recv_table)
        if recv_table["userid"] == user1:get_user_id() and recv_table["msgid"] == expected_msg1 then
            recv_table = coroutine.yield()
            
            --print(recv_table)
            check(recv_table["userid"] == user2:get_user_id(), "recv two robot failed");
            coro_clear_curr_expect_time()

        elseif recv_table["userid"] == user2:get_user_id() and recv_table["msgid"] == expected_msg2 then
            recv_table = coroutine.yield()

            --print(recv_table)
            check(recv_table["userid"] == user1:get_user_id(), "recv two robot failed");
            coro_clear_curr_expect_time()
        else
            return nil
        end
    end
    return recv_table
end