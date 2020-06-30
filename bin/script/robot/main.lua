Include("robot/common.lua")

LoadPackage("robot.pkg");

command_cache = {};

function gmcommand(cmd)
	local f, msg = load(cmd);
	if f then
		command_cache[cmd] = f;
	elseif msg then
		print("failed to execute gm cmd: " .. cmd)
		return false;
	end

	command_cache[cmd]();

	return true;
end

function create_user(user_name)
    return do_create_user(user_name, user_name, 1)
end

function destroy_user(user)
    return do_destroy_user(user)
end

function send(user, msg_id, ...)
    return user_send(user:get_connid(), msg_id, ...)
end

function startup(user_name, server_url)
	local ret, msg

	local user = create_user(user_name)
    server_url = server_url or GLOBAL_CONFIG['server_url']
	print("startup.create_user," .. server_url .. "," .. user_name)
	

    ret = user:connect(server_url)
	if ret then
		print("startup.connect success")
	else 
		print("startup.connect failed")
	end

end

function endup(user)
    return destroy_user(user)
end


