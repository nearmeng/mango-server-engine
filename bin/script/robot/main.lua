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

SERVER_URL="tcp://127.0.0.1:8888"

function startup(user_name, server_url, class, body)
	local ret, msg
	if not class then class = clsClass1 end
	if not body then body = 1 end

	local user = create_user(user_name)
    server_url = server_url or SERVER_URL
	print("startup.create_user," .. server_url .. "," .. user_name)
	

    ret = user:connect(server_url)
	if ret then
		print("startup.connect success")
	else 
		print("startup.connect failed")
	end

end

function endup(user)
end


