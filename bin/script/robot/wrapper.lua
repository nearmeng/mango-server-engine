function startup(user_name, role_name, server_url, class, body)

	if not class then class = clsClass1 end
	if not body then body = 1 end

	local user = create_user(user_name)
    server_url = server_url or SERVER_URL
	print("startup.create_user," .. server_url .. "," .. user_name)
	
	local ret, msg

    ret = user:connect(server_url)
    check(ret, "startup.connect failed")

	ret = recv_many(user, msgid.connected, user, msgid.g2c_allow_login)
    check(ret[1], "startup.recv.connected failed")
	check(ret[2], "startup.recv.g2c_allow_login failed")

	ret = send(user, msgid.c2g_login, g_SharedConfig.dwLoginTag, 1, 1, 0, 0, true)
    check(ret, "startup.send.c2g_login failed")

	msg = recv(user, msgid.g2c_sync_role_list)
    check(msg, "startup.recv.g2c_sync_role_list failed")
	
	local role_id = 0;
	if get_table_size(msg.basedata) == 0 then
		print("begin to create role")
		send(user, msgid.c2g_create_role, role_name, class, body)

		msg = recv(user, msgid.g2c_create_role_ack)
		check(msg, "recv failed")

		role_id = msg["basedata"]["roleid"]
	else
		for i,v in pairs(msg.basedata) do
			print(v.rolename);
			if v.rolename == role_name then
				role_id = v.roleid;
				break;
			end
		end
	end
	
	print("begin to select role, role_id: " .. role_id)

	ret = send(user, msgid.c2g_select_role, role_id)
	check(ret, "send select role failed")
	    
    msg = recv_many(user, msgid.g2c_sync_role_data_begin, user, msgid.g2c_enter_scene)
	check(msg[1], "recv failed")
	check(msg[2], "recv enter scene failed")

--	msg = recv(user, msgid.g2c_sync_scene_info)
--	check(msg, "recv sync scene info failed")

	local role = CGameWorld:instance():find_role(role_id);
	role:set_server_group(1);

    return user,role
end

function endup(user)

    print("begin to user disconnect")

	local ret = send(user, msgid.c2g_logout)
    check(ret, "endup.c2g_logout failed")

    local msg = recv(user, msgid.closed)
    check(msg, "endup.recv.closed failed")

    ret = destroy_user(user);
    check(ret, "endup.destroy_user failed")

    return ret 
end

function create_user(user_name)
    return do_create_user(user_name, coro_data.current_coname)
end

function destroy_user(user)
    return do_destroy_user(user)
end

function create_user_name(user_name)
	-- local name_prefix = string.format("%#x",os.time() % 0xFFFFFF)
	local len = string.len(user_name)
	local random_name = randomStr(12-len)
	print("user name: " .. user_name .. " len: " .. len .. " ran: " .. random_name)
	return user_name .. random_name
	-- return 't' .. randomStr(10)
end

function randomStr(len)
	local rankStr = ""
	local randNum = 0
	for i=1,len do
	  if math.random(1,3)==1 then
		randNum=string.char(math.random(26)+64)
	  elseif math.random(1,3)==2 then
		randNum=string.char(math.random(26)+96)
	  else
		randNum=math.random(0,9)
	  end
	  rankStr=rankStr..randNum
	end
	return rankStr
end


function print_t(root)
	if root == nil then
		print("nil")
		return
	end
    local cache = {  [root] = "." }
    local function _dump(t,space,name)
        local temp = {}
        for k,v in pairs(t) do
            local key = tostring(k)
            if cache[v] then
                table.insert(temp,"+" .. key .. " {" .. cache[v].."}")
            elseif type(v) == "table" then
                local new_key = name .. "." .. key
                cache[v] = new_key
                table.insert(temp,"+" .. key .. _dump(v,space .. (next(t,k) and "|" or " " ).. 
                    string.rep(" ",#key),new_key))
            else
                table.insert(temp,"+" .. key .. " [" .. tostring(v).."]")
            end
        end
        return table.concat(temp,"\n"..space)
    end
    print(_dump(root, "",""))
end

function float_equal(x,v)
	local EPSILON = 0.000001
	return ((v - EPSILON) < x) and (x <( v + EPSILON))
end