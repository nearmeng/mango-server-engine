SERVER_URL = ""
		                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
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
