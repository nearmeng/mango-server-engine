CaseConfig.login = {}

function CaseConfig.login.normal()
	local user_name = 'normal'..os.date("%H%M%S");
	local user = create_user(user_name)

    ret = user:connect(SERVER_URL)
    check(ret, "connect failed")

	ret = recv_many(user, msgid.connected, user, msgid.g2c_allow_login)
	check(ret[1], "connect failed")
    check(ret[2], "startup.recv.allow.login failed")

	ret = send(user, msgid.c2g_login, g_SharedConfig.dwLoginTag, 1, 1, 0, 0, true)
	check(ret, "send failed")

	msg = recv(user, msgid.g2c_sync_role_list)
	check(msg, "recv failed")
	
	local role_id = 0;
	if get_table_size(msg.basedata) == 0 then
		print("begin to create role")
		send(user, msgid.c2g_create_role, user_name, clsClass1, 1);

		msg = recv(user, msgid.g2c_create_role_ack)
		check(msg, "recv failed")

		role_id = msg["basedata"]["roleid"]
	else
		role_id = msg["basedata"][0]["roleid"]
	end
	
	print("begin to select role, role_id: " .. role_id)

	ret = send(user, msgid.c2g_select_role, role_id)
	check(ret, "send select role failed")

	msg = recv_many(user, msgid.g2c_sync_role_data_begin, user, msgid.g2c_enter_scene, user, msgid.g2c_sync_obj_list, user, msgid.g2c_sync_new_char)
	check(msg[1], "recv failed")
	check(msg[2], "recv enter scene failed")
	check(msg[3], "recv g2c_sync_obj_list failed")
	check(msg[4])

    print("begin to user disconnect")
    ret = user:disconnect()
    check(ret, "disconnnect failed")

    ret = destroy_user(user);
    check(ret, "destory user failed")
end

function CaseConfig.login.repeated_2()
	local user_name = create_user_name("lgrpt2");
	local user = create_user(user_name)

    ret = user:connect(SERVER_URL)
    check(ret, "connect failed")

	msg = recv_many(user, msgid.connected, user, msgid.g2c_allow_login)
	check(msg[1], "connect failed")
    check(msg[2], "startup.recv.allow.login failed")

	ret = send(user, msgid.c2g_login, g_SharedConfig.dwLoginTag, 1, 1, 0, 0, true)
	check(ret, "send failed")

	msg = recv(user, msgid.g2c_sync_role_list)
	check(msg, "recv failed")

	local role_id = 0;
	if get_table_size(msg.basedata) == 0 then
		send(user, msgid.c2g_create_role, user_name, clsClass1, 1)

		msg = recv(user, msgid.g2c_create_role_ack)
		check(msg, "recv failed")

		role_id = msg["basedata"]["roleid"]
	else
		role_id = msg["basedata"][0]["roleid"]
	end
	
	ret = send(user, msgid.c2g_select_role, role_id)
	check(ret, "send select role failed")

	msg = recv_many(user, msgid.g2c_sync_role_data_begin, user, msgid.g2c_enter_scene, user, msgid.g2c_sync_obj_list)
	check(msg[1], "recv failed")
	check(msg[2], "recv enter scene failed")
	check(msg[3], "recv g2c_sync_obj_list failed")
    
    local user2 = create_user(user_name)
    check(user2, "create user failed")

    ret = user2:connect(SERVER_URL)
    check(ret, "connected failed")

	msg = recv_many(user2, msgid.connected, user2, msgid.g2c_allow_login)
	check(msg[1], "connect failed")
    check(msg[2], "startup.recv.allow.login failed")

	ret = send(user2, msgid.c2g_login, g_SharedConfig.dwLoginTag, 1, 1, 0, 0, true)
	check(ret, "send failed")
    
	msg = recv_many(user, msgid.closed, user2, msgid.closed)
	check(msg[1], "closed failed")
	check(msg[2], "closed failed")
	
	ret = destroy_user(user)
	check(ret, "destory user failed")
	
    ret = destroy_user(user2)
	check(ret, "destory user2 failed")
end

function CaseConfig.login.relay_login_SyncProto()
-----------------------
	local user_name = create_user_name("lgrlsp");
	local user = create_user(user_name)
    check(user, "create user failed")

    ret = user:connect(SERVER_URL)
    check(ret, "user connected failed")

	msg = recv_many(user, msgid.connected, user, msgid.g2c_allow_login)
	check(msg[1], "connected failed")
    check(msg[2], "startup.recv.allow.login failed")

	print(msgid.c2g_login)
	ret = send(user, msgid.c2g_login, g_SharedConfig.dwLoginTag, 1, 1, 0, 0, true)
	check(ret, "c2g_login failed")

	msg = recv_many(user, msgid.g2c_login, user, msgid.g2c_sync_role_list)
	check(msg[1], "g2c_login failed")
	check(msg[1].type == LOGIN_RET_TYPE.lrtNormal, "msg.type == LOGIN_RET_TYPE.lrtNormal failed")
	check(msg[2], "g2c_sync_role_list failed")
	
	local role_id = 0;
	if get_table_size(msg[2].basedata) == 0 then
		print("begin to create role")
		send(user, msgid.c2g_create_role, user_name, clsClass1, 1)

		msg = recv(user, msgid.g2c_create_role_ack)
		check(msg, "recv failed")

		role_id = msg["basedata"]["roleid"]
	else
		role_id = msg["basedata"][0]["roleid"]
	end
	
	print("begin to select role, role_id: " .. role_id)

	ret = send(user, msgid.c2g_select_role, role_id)
	check(ret, "send select role failed")

	msg = recv_many(user, msgid.g2c_sync_role_data_begin, 
					user, msgid.g2c_enter_scene, 
					user, msgid.g2c_sync_obj_list, 
					user, msgid.g2c_sync_new_char)
	check(msg[1], "recv failed")
	check(msg[2], "recv enter scene failed")
	check(msg[3], "recv g2c_sync_obj_list failed")
	check(msg[4], "recv g2c_sync_new_char failed")

    ret = user:disconnect()
    check(ret, "disconnect failed")

	ret = destroy_user(user)
	check(ret, "destory user failed")

-----------------------

	local new_user = create_user(user_name)

    ret = new_user:connect(SERVER_URL)
    check(ret, "new_user connect failed")

	ret = recv_many(new_user, msgid.connected, new_user, msgid.g2c_allow_login)
	check(ret[1], "connect failed")
    check(ret[2], "startup.recv.allow.login failed")

	ret = send(new_user, msgid.c2g_login, g_SharedConfig.dwLoginTag, 1, 1, 0, 0, true, true, 0, 0)
	check(ret, "c2g_login failed")

	msg = recv_many(new_user, msgid.g2c_login, 
					new_user, msgid.g2c_relay_sync_begin,
					new_user, msgid.g2c_sync_role_data_begin,
					new_user, msgid.g2c_sync_role_data_end, 
					new_user, msgid.g2c_sync_obj_list, 
					new_user, msgid.g2c_sync_new_char, 
					new_user, msgid.g2c_relay_sync_end
				)
	check(msg[1], "g2c_login failed")
	check(msg[1].type == LOGIN_RET_TYPE.lrtRelaySyncSnapshot, "msg.type == LOGIN_RET_TYPE.lrtRelaySyncSnapshot failed")
	check(msg[2], "g2c_relay_sync_begin failed")
	check(msg[3], "g2c_sync_role_data_begin failed")
	check(msg[4], "g2c_sync_role_data_end failed")
	check(msg[5], "recv g2c_sync_obj_list failed")
	check(msg[6], "g2c_sync_new_char failed")
	check(msg[7], "g2c_relay_sync_end failed")

	-----------------------

	send(new_user, msgid.c2g_logout)
	check(msg, "c2g_logout failed")

    msg = recv(new_user, msgid.closed)
    check(msg, "recv closed failed") --send()

	ret = destroy_user(new_user)
	check(ret, "destory user failed")
end

function CaseConfig.login.relay_login_kick()
-----------------------
	local user_name = create_user_name("lgrlk");
	local user = create_user(user_name)

    ret = user:connect(SERVER_URL)
    check(ret, "user connect failed")

	msg = recv_many(user, msgid.connected, user, msgid.g2c_allow_login)
	check(msg[1], "connected failed")
    check(msg[2], "startup.recv.allow.login failed")

	ret = send(user, msgid.c2g_login, g_SharedConfig.dwLoginTag, 1, 1, 0, 0, true)
	check(ret, "c2g_login failed")

	msg = recv_many(user, msgid.g2c_login, user, msgid.g2c_sync_role_list)
	check(msg[1], "g2c_login failed")
	check(msg[1].type == LOGIN_RET_TYPE.lrtNormal, "msg.type == LOGIN_RET_TYPE.lrtNormal failed")
	check(msg[2], "g2c_sync_role_list failed")
	
	local role_id = 0;
	if get_table_size(msg[2].basedata) == 0 then
		print("begin to create role")
		send(user, msgid.c2g_create_role, user_name, clsClass1, 1)
	
        msg = recv(user, msgid.g2c_create_role_ack)
		check(msg, "recv failed")

		role_id = msg["basedata"]["roleid"]
	else
		role_id = msg["basedata"][0]["roleid"]
	end
	
	print("begin to select role, role_id: " .. role_id)
	ret = send(user, msgid.c2g_select_role, role_id)
	check(ret, "send select role failed")

	msg = recv_many(user, msgid.g2c_sync_role_data_begin, 
					user, msgid.g2c_enter_scene, 
					user, msgid.g2c_sync_obj_list, 
					user, msgid.g2c_sync_new_char)
	check(msg[1], "recv failed")
	check(msg[2], "recv enter scene failed")
	check(msg[3], "recv g2c_sync_obj_list failed")
	check(msg[4], "recv g2c_sync_new_char failed")

    ret = user:disconnect()
    check(ret, "user disconn failed")

	ret = destroy_user(user)
	check(ret, "destory user failed")

-----------------------

	local new_user = create_user(user_name)

    ret = new_user:connect(SERVER_URL)
    check(ret, "user conn failed")

	msg = recv_many(new_user, msgid.connected, new_user, msgid.g2c_allow_login)
	check(msg[1], "connected failed")
    check(msg[2], "startup.recv.allow.login failed")

	ret = send(new_user, msgid.c2g_login, g_SharedConfig.dwLoginTag, 1, 1, 0, 0, true)
	check(ret, "c2g_login failed")

	-- msg = recv(new_user, msgid.g2c_login)
	-- check(msg, "g2c_login failed")
	-- check(msg.type == LOGIN_RET_TYPE.lrtRelaySyncSnapshot, "msg.type == LOGIN_RET_TYPE.lrtRelaySyncSnapshot failed")

    msg = recv(new_user, msgid.closed)
    check(msg, "recv closed failed")

	ret = destroy_user(new_user)
	check(ret, "destory user failed")
end


function CaseConfig.login.relay_login_complex()
-----------------------
	local user_name = create_user_name("lgrlcp");
	local user = create_user(user_name)

    ret = user:connect(SERVER_URL)
    check(ret, "user connect failed")

	msg = recv_many(user, msgid.connected, user, msgid.g2c_allow_login)
	check(msg[1], "connected failed")
    check(msg[2], "startup.recv.allow.login failed")

	ret = send(user, msgid.c2g_login, g_SharedConfig.dwLoginTag, 1, 1, 0, 0, true)
	check(ret, "c2g_login failed")

	msg = recv_many(user, msgid.g2c_login, user, msgid.g2c_sync_role_list)
	check(msg[1], "g2c_login failed")
	check(msg[1].type == LOGIN_RET_TYPE.lrtNormal, "msg.type == LOGIN_RET_TYPE.lrtNormal failed")
	check(msg[2], "g2c_sync_role_list failed")
	
	local role_id = 0;
	if get_table_size(msg[2].basedata) == 0 then
		print("begin to create role")
		send(user, msgid.c2g_create_role, user_name, clsClass1, 1)

		msg = recv(user, msgid.g2c_create_role_ack)
		check(msg, "recv failed")

		role_id = msg["basedata"]["roleid"]
	else
		role_id = msg["basedata"][0]["roleid"]
	end

	print("begin to select role, role_id: " .. role_id)
	ret = send(user, msgid.c2g_select_role, role_id)
	check(ret, "send select role failed")

	msg = recv_many(user, msgid.g2c_sync_role_data_begin, 
					user, msgid.g2c_enter_scene, 
					user, msgid.g2c_sync_obj_list, 
					user, msgid.g2c_sync_new_char)
	check(msg[1], "recv failed")
	check(msg[2], "recv enter scene failed")
	check(msg[3], "recv g2c_sync_obj_list failed")
	check(msg[4], "recv g2c_sync_new_char failed")

    ret = user:disconnect()
    check(ret, "user disconn failed")

	ret = destroy_user(user)
	check(ret, "destory user failed")

-----------------------

	local new_user1 = create_user(user_name)

    ret = new_user1:connect(SERVER_URL)
    check(ret, "user conn failed")

	msg = recv_many(new_user1, msgid.connected, new_user1, msgid.g2c_allow_login)
	check(msg[1], "connected failed")
    check(msg[2], "startup.recv.allow.login failed")


-----------------------

	local new_user2 = create_user(user_name)

    ret = new_user2:connect(SERVER_URL)
    check(ret, "user2 conn failed")

	msg = recv_many(new_user2, msgid.connected, new_user2, msgid.g2c_allow_login)
	check(msg[1], "connected failed")
    check(msg[2], "startup.recv.allow.login failed")

-----------------------

	local new_user3 = create_user(user_name)
    
    ret = new_user3:connect(SERVER_URL)
    check(ret, "user 3 conn failed")

	msg = recv_many(new_user3, msgid.connected, new_user3, msgid.g2c_allow_login)
	check(msg[1], "connected failed")
    check(msg[2], "startup.recv.allow.login failed")

	ret = send(new_user3, msgid.c2g_login, g_SharedConfig.dwLoginTag, 1, 1, 0, 0, true, true, false, 0, 0)
	check(ret, "c2g_login failed")

	msg = recv_many(new_user3, msgid.g2c_login, 
					new_user3, msgid.g2c_relay_sync_begin,
					new_user3, msgid.g2c_sync_role_data_begin,
					new_user3, msgid.g2c_sync_role_data_end, 
					new_user3, msgid.g2c_sync_obj_list, 
					new_user3, msgid.g2c_sync_new_char, 
					new_user3, msgid.g2c_relay_sync_end
				)
	check(msg[1], "g2c_login failed")
	check(msg[1].type == LOGIN_RET_TYPE.lrtRelaySyncSnapshot, "msg.type == LOGIN_RET_TYPE.lrtRelaySyncSnapshot failed")
	check(msg[2], "g2c_relay_sync_begin failed")
	check(msg[3], "g2c_sync_role_data_begin failed")
	check(msg[4], "g2c_sync_role_data_end failed")
	check(msg[5], "recv g2c_sync_obj_list failed")
	check(msg[6], "g2c_sync_new_char failed")
	check(msg[7], "g2c_relay_sync_end failed")

-----------------------

	ret = send(new_user1, msgid.c2g_login, g_SharedConfig.dwLoginTag, 1, 1, 0, 0, true)
	check(ret, "c2g_login failed")

    msg = recv_many(new_user1, msgid.closed, new_user3, msgid.closed)
    check(msg[1], "recv closed")
    check(msg[2], "recv closed")

	ret = destroy_user(new_user1)
	check(ret, "destory user failed")
	ret = destroy_user(new_user3)
	check(ret, "destory user failed")

-----------------------

    coro_wait(2)

	ret = send(new_user2, msgid.c2g_login, g_SharedConfig.dwLoginTag, 1, 1, 0, 0, true)
	check(ret, "c2g_login failed")

	msg = recv(new_user2, msgid.g2c_login)
	check(msg, "g2c_login failed")
	check(msg.type == LOGIN_RET_TYPE.lrtNormal, "msg.type == LOGIN_RET_TYPE.lrtNormal failed")

	send(new_user2, msgid.c2g_logout)
	check(msg, "c2g_logout failed")
    msg = recv(new_user2, msgid.closed)
    check(msg, "recv closed failed")
	ret = destroy_user(new_user2)
	check(ret, "destory user failed")
end

function CaseConfig.login.interrupt_login()
-----------------------
	local user_name = create_user_name("itrlgn");
	local user = create_user(user_name)

    ret = user:connect(SERVER_URL)
    check(ret, "user connect failed")

	msg = recv_many(user, msgid.connected, user, msgid.g2c_allow_login)
	check(msg[1], "connected failed")
    check(msg[2], "startup.recv.allow.login failed")

	ret = send(user, msgid.c2g_login, g_SharedConfig.dwLoginTag, 1, 1, 0, 0, true)
	check(ret, "c2g_login failed")

	msg = recv(user, msgid.g2c_sync_role_list)
	check(msg, "g2c_sync_role_list failed")

-----------------------

	local new_user = create_user(user_name)

    ret = new_user:connect(SERVER_URL)
    check(ret, "new user conn failed")

	msg = recv_many(new_user, msgid.connected, new_user, msgid.g2c_allow_login)
	check(msg[1], "connected failed")
    check(msg[2], "startup.recv.allow.login failed")

	ret = send(new_user, msgid.c2g_login, g_SharedConfig.dwLoginTag, 1, 1, 0, 0, true)
	check(ret, "c2g_login failed")

-----------------------

    msg = recv_many(user, msgid.closed, new_user, msgid.closed)
    check(msg[1], "recv server closed failed")
    check(msg[2], "recv server closed failed")

    ret = destroy_user(user)
    check(ret, "destroy user failed")
    ret = destroy_user(new_user)
    check(ret, "destroy user failed")
end

function CaseConfig.login.login_logout()
	local user_name = create_user_name("lgnlgo");
    local user = create_user(user_name)

    ret = user:connect(SERVER_URL)
    check(ret, "user connect failed")

	msg = recv_many(user, msgid.connected, user, msgid.g2c_allow_login)
	check(msg[1], "connected failed")
    check(msg[2], "startup.recv.allow.login failed")

	ret = send(user, msgid.c2g_login, g_SharedConfig.dwLoginTag, 1, 1, 0, 0, true)
	check(ret, "send failed")

	msg = recv(user, msgid.g2c_sync_role_list)
	check(msg, "recv failed")

	local role_id = 0;
	if get_table_size(msg.basedata) == 0 then
		print("begin to create role")
		send(user, msgid.c2g_create_role, user_name, clsClass1, 1)

		msg = recv(user, msgid.g2c_create_role_ack)
		check(msg, "recv failed")

		role_id = msg["basedata"]["roleid"]
	else
		role_id = msg["basedata"][0]["roleid"]
	end
	
	print("begin to select role, role_id: " .. role_id)
    ret = send(user, msgid.c2g_select_role, role_id)
	check(ret, "send select role failed")

	msg = recv_many(user, msgid.g2c_sync_role_data_begin, 
					user, msgid.g2c_enter_scene, 
					user, msgid.g2c_sync_obj_list, 
					user, msgid.g2c_sync_new_char)
	check(msg[1], "recv failed")
	check(msg[2], "recv enter scene failed")
	check(msg[3], "recv g2c_sync_obj_list failed")
	check(msg[4], "recv g2c_sync_new_char failed")

	send(user, msgid.c2g_logout)
	check(msg, "c2g_logout failed")

    ret = recv(user, msgid.closed)
    check(ret, "recv closed failed")

    ret = destroy_user(user)
    check(ret, "destroy user failed")
end

function CaseConfig.login.active_disconn_login()
	local user_name = create_user_name("acdilg");	
    local user = create_user(user_name)

    ret = user:connect(SERVER_URL)
    check(ret, "user connect failed")

	msg = recv_many(user, msgid.connected, user, msgid.g2c_allow_login)
	check(msg[1], "connected failed")
    check(msg[2], "startup.recv.allow.login failed")

	ret = send(user, msgid.c2g_login, g_SharedConfig.dwLoginTag, 1, 1, 0, 0, true)
	check(ret, "send failed")

	msg = recv(user, msgid.g2c_sync_role_list)
	check(msg, "recv g2c_sync_role_list failed")

	local role_id = 0;
	if get_table_size(msg.basedata) == 0 then
		print("begin to create role")
		send(user, msgid.c2g_create_role, user_name, clsClass1, 1)

		msg = recv(user, msgid.g2c_create_role_ack)
		check(msg, "recv g2c_create_role_ack failed")

		role_id = msg["basedata"]["roleid"]
	else
		role_id = msg["basedata"][0]["roleid"]
	end
	
	print("begin to select role, role_id: " .. role_id)
    ret = send(user, msgid.c2g_select_role, role_id)
	check(ret, "send select role failed")

	msg = recv_many(user, msgid.g2c_sync_role_data_begin, 
					user, msgid.g2c_enter_scene, 
					user, msgid.g2c_sync_obj_list, 
					user, msgid.g2c_sync_new_char)
	check(msg[1], "recv failed")
	check(msg[2], "recv enter scene failed")
	check(msg[3], "recv g2c_sync_obj_list failed")
	check(msg[4], "recv g2c_sync_new_char failed")

    ret = user:disconnect()
    check(ret, "user disconn failed")

    ret = destroy_user(user)
    check(ret, "destroy user failed")
	
	local new_user = create_user(user_name)

    ret = new_user:connect(SERVER_URL)
    check(ret, "user connect failed")

	ret = recv_many(new_user, msgid.connected, new_user, msgid.g2c_allow_login)
	check(ret[1], "connect failed")
    check(ret[2], "startup.recv.allow.login failed")

	ret = send(new_user, msgid.c2g_login, g_SharedConfig.dwLoginTag, 1, 1, 0, 0, true, true)
	check(ret, "send failed")

	msg = recv(new_user, msgid.g2c_sync_obj_list, msgid.g2c_sync_new_char)
	check(msg, "recv g2c_sync_obj_list failed")

	send(new_user, msgid.c2g_logout)
	check(msg, "c2g_logout failed")

    ret = recv(new_user, msgid.closed)
    check(ret, "recv closed failed")

    ret = destroy_user(new_user)
    check(ret, "destroy user failed")
end

function CaseConfig.login.passive_disconn_login()
	local user_name = create_user_name("psdslg");
	local user = create_user(user_name)
    ret = user:connect(SERVER_URL)
    check(ret, "user connnect failed")

	msg = recv_many(user, msgid.connected, user, msgid.g2c_allow_login)
	check(msg[1], "connected failed")
    check(msg[2], "startup.recv.allow.login failed")

	ret = send(user, msgid.c2g_login, g_SharedConfig.dwLoginTag, 1, 1, 0, 0, true)
	check(ret, "send failed")

	msg = recv(user, msgid.g2c_sync_role_list)
	check(msg, "recv failed")

	local role_id = 0;
	if get_table_size(msg.basedata) == 0 then
		print("begin to create role")
		send(user, msgid.c2g_create_role, user_name, clsClass1, 1)

		msg = recv(user, msgid.g2c_create_role_ack) 
        check(msg, "recv failed")

		role_id = msg["basedata"]["roleid"]
	else
		role_id = msg["basedata"][0]["roleid"]
	end
	
    ret = send(user, msgid.c2g_select_role, role_id)
	check(ret, "send select role failed")

	msg = recv(user, msgid.g2c_sync_role_data_begin)
	check(msg, "recv g2c_sync_role_data_begin failed")

    --make server disconn    
    local new_user = create_user(user_name)

    ret = new_user:connect(SERVER_URL);
    check(ret, "user conn failed")

	ret = recv_many(new_user, msgid.connected, new_user, msgid.g2c_allow_login)
	check(ret[1], "connect failed")
    check(ret[2], "startup.recv.allow.login failed")

	ret = send(new_user, msgid.c2g_login, g_SharedConfig.dwLoginTag, 1, 1, 0, 0, true)
	check(ret, "send failed")
        
    msg = recv_two(user, msgid.closed, new_user, msgid.closed)
    check(msg, "closed failed")

    coro_wait(3)

    local real_user = create_user(user_name)
    
    ret = real_user:connect(SERVER_URL)
    check(ret, "user connect failed")

    ret = recv_many(real_user, msgid.connected, real_user, msgid.g2c_allow_login)
	check(ret[1], "connect failed")
    check(ret[2], "startup.recv.allow.login failed")

	ret = send(real_user, msgid.c2g_login, g_SharedConfig.dwLoginTag, 1, 1, 0, 0, true)
	check(ret, "send failed")

	msg = recv(real_user, msgid.g2c_sync_role_list)
	check(msg, "recv failed")
    
	send(real_user, msgid.c2g_logout)
	check(msg, "c2g_logout failed")

    ret = recv(real_user, msgid.closed)
    check(ret, "recv closed failed")

    ret = destroy_user(real_user)
    check(ret, "destroy failed")
end

function CaseConfig.login.repeated_3()
	local user_name = create_user_name("lgrpt3");
	local user = create_user(user_name);

    ret = user:connect(SERVER_URL)
    check(ret, "user connect failed")

	msg = recv_many(user, msgid.connected, user, msgid.g2c_allow_login)
	check(msg[1], "connected failed")
    check(msg[2], "startup.recv.allow.login failed")

	ret = send(user, msgid.c2g_login, g_SharedConfig.dwLoginTag, 1, 1, 0, 0, true)
	check(ret, "send failed")

	msg = recv(user, msgid.g2c_sync_role_list)
	check(msg, "recv failed")

	local role_id = 0;
	if get_table_size(msg.basedata) == 0 then
		print("begin to create role")
		send(user, msgid.c2g_create_role, user_name, clsClass1, 1)

		msg = recv(user, msgid.g2c_create_role_ack)
		check(msg, "recv failed")

		role_id = msg["basedata"]["roleid"]
	else
		role_id = msg["basedata"][0]["roleid"]
	end
	
    print("begin to select role, role_id: " .. role_id)
	ret = send(user, msgid.c2g_select_role, role_id)
	check(ret, "send select role failed")

	msg = recv_many(user, msgid.g2c_sync_role_data_begin, 
					user, msgid.g2c_enter_scene, 
					user, msgid.g2c_sync_obj_list, 
					user, msgid.g2c_sync_new_char)
	check(msg[1], "recv failed")
	check(msg[2], "recv enter scene failed")
	check(msg[3], "recv g2c_sync_obj_list failed")
	check(msg[4], "recv g2c_sync_new_char failed")
    
    local new_user1 = create_user(user_name)
    local new_user2 = create_user(user_name)

    ret = new_user1:connect(SERVER_URL)
    check(ret, "user1 connect failed")

	msg = recv_many(new_user1, msgid.connected, new_user1, msgid.g2c_allow_login)
	check(msg[1], "connected failed")
    check(msg[2], "startup.recv.allow.login failed")

    ret = new_user2:connect(SERVER_URL)
    check(ret, "user2 connect failed")

	msg = recv_many(new_user2, msgid.connected, new_user2, msgid.g2c_allow_login)
	check(msg[1], "connected failed")
    check(msg[2], "startup.recv.allow.login failed")

	ret = send(new_user1, msgid.c2g_login, g_SharedConfig.dwLoginTag, 1, 1, 0, 0, true)
	check(ret, "send failed")
	
    ret = send(new_user2, msgid.c2g_login, g_SharedConfig.dwLoginTag, 1, 1, 0, 0, true)
	check(ret, "send failed")
	
	msg = recv_many(user, msgid.closed, new_user1, msgid.closed, new_user2, msgid.closed)
	check(msg[1])
	check(msg[2])
	check(msg[3])

	ret = destroy_user(user)
	check(ret, "destory user failed")
	
    ret = destroy_user(new_user1)
	check(ret, "destory user failed")

    ret = destroy_user(new_user2)
	check(ret, "destory user failed")
end

function CaseConfig.login.repeated_4()
	local user_name = create_user_name("lgrpt4");
    local user = create_user(user_name)

    local ret = user:connect(SERVER_URL)
    check(ret, "user conn failed")

	msg = recv_many(user, msgid.connected, user, msgid.g2c_allow_login)
	check(msg[1], "connected failed")
    check(msg[2], "startup.recv.allow.login failed")

	ret = send(user, msgid.c2g_login, g_SharedConfig.dwLoginTag, 1, 1, 0, 0, true)
	check(ret, "send failed")

	msg = recv(user, msgid.g2c_sync_role_list)
	check(msg, "recv failed")

	local role_id = 0;
	if get_table_size(msg.basedata) == 0 then
		print("begin to create role")
		send(user, msgid.c2g_create_role, user_name, clsClass1, 1)

		msg = recv(user, msgid.g2c_create_role_ack)
		check(msg, "recv failed")

		role_id = msg["basedata"]["roleid"]
	else
		role_id = msg["basedata"][0]["roleid"]
	end
	
	print("begin to select role, role_id: " .. role_id)
    ret = send(user, msgid.c2g_select_role, role_id)
	check(ret, "send select role failed")

	msg = recv_many(user, msgid.g2c_sync_role_data_begin, 
					user, msgid.g2c_enter_scene, 
					user, msgid.g2c_sync_obj_list, 
					user, msgid.g2c_sync_new_char)
	check(msg[1], "recv failed")
	check(msg[2], "recv enter scene failed")
	check(msg[3], "recv g2c_sync_obj_list failed")
	check(msg[4], "recv g2c_sync_new_char failed")
    
    local new_user1 = create_user(user_name)
    local new_user2 = create_user(user_name)
    local new_user3 = create_user(user_name)

    ret = new_user1:connect(SERVER_URL)
    check(ret, "user 1 connect failed")
	
    msg = recv_many(new_user1, msgid.connected, new_user1, msgid.g2c_allow_login)
	check(msg[1], "connected failed")
    check(msg[2], "startup.recv.allow.login failed")

    ret = new_user2:connect(SERVER_URL)
    check(ret, "user 2 connect failed")
	
    msg = recv_many(new_user2, msgid.connected, new_user2, msgid.g2c_allow_login)
	check(msg[1], "connected failed")
    check(msg[2], "startup.recv.allow.login failed")

    ret = new_user3:connect(SERVER_URL)
    check(ret, "user 3 connect failed")

	msg = recv_many(new_user3, msgid.connected, new_user3, msgid.g2c_allow_login)
	check(msg[1], "connected failed")
    check(msg[2], "startup.recv.allow.login failed")

	ret = send(new_user1, msgid.c2g_login, g_SharedConfig.dwLoginTag, 1, 1, 0, 0, true)
	check(ret, "send failed")

    ret = send(new_user2, msgid.c2g_login, g_SharedConfig.dwLoginTag, 1, 1, 0, 0, true)
	check(ret, "send failed")

    ret = send(new_user3, msgid.c2g_login, g_SharedConfig.dwLoginTag, 1, 1, 0, 0, true)
	check(ret, "send failed")
	
	msg = recv_many(user, msgid.closed, new_user1, msgid.closed, new_user2, msgid.closed, new_user3, msgid.closed)
	check(msg[1])
	check(msg[2])
	check(msg[3])
	check(msg[4])

	ret = destroy_user(user)
	check(ret, "destory user failed")
	
    ret = destroy_user(new_user1)
	check(ret, "destory user failed")

    ret = destroy_user(new_user2)
	check(ret, "destory user failed")

    ret = destroy_user(new_user3)
	check(ret, "destory user failed")
end