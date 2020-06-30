function get_table_size(tb)
	local count = 0;
	for k,v in pairs(tb) do
		count = count + 1;
	end
	return count;
end

function check(condition, error_msg)
    local robot_name;
    local ar = debug.getinfo(2, "lS");
    local case_name = coro_data.current_coname

	if not error_msg then
		error_msg = '';
	end

    if (condition == nil or not condition) then
        local content = "["..case_name.."] error : "..error_msg..", at line "..ar.currentline.." in "..ar.source;
        if(coro_data.coro_list[case_name] and coro_data.coro_list[case_name].userdata == "func_test") then
            check_print(case_name, content, tcstFailed);
        else
            print(content)
        end
        error(content);
    else

        local content = "["..case_name.."] pass : at line "..ar.currentline.." in "..ar.source;

        if(coro_data.coro_list[case_name] and coro_data.coro_list[case_name].userdata == "func_test") then
            check_print(case_name, content, tcstRunning);
        else
            print(content)
        end
    end
end

function config(config_name, config_value)
    GLOBAL_CONFIG[config_name] = config_value
    print(config_name .. " is set to " .. config_value)
end

function print_config(config_name)
    print(config_name .. " value is " .. GLOBAL_CONFIG[config_name])
end

GLOBAL_CONFIG = {}
register_msgid()