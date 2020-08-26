enum_cache = {}

function read_default(col, value, data)
	if data[col] then
		data[col] = value;
	else
		warn('data do not has col name '.. col .. ' in data struct, so can not assign value ' .. value);
	end
end

function read_array_default(col, index, value, data)
	if (type(index) == "string") then
        try_number = tonumber(index)
        if try_number then
            index = try_number
        else
		    index = _G[index]
        end
	end

	if data[col] then
		data[col][index] = value;
	else
		data[col..index] = value;
	end
end

function read_lua(col, value, data)
	local cache_value = enum_cache[value];
	if not cache_value then
		local f, s = load('return '..value);
		if f then
			local ret, v = pcall(f);
			if ret and v then
				cache_value = v;
				enum_cache[value] = cache_value;
			else
				if type(data[col]) == 'string' then
					cache_value = value;
					enum_cache[value] = cache_value;
				else
					warn(v);
				end
			end
		else
			if type(data[col]) == 'string' then
				cache_value = value;
			    enum_cache[value] = cache_value;
		    else
			    warn(s);
		    end
	    end
	end
	if cache_value then
		data[col] = cache_value;
	else
		warn('failed to set col '..col..' to value '..value);
	end
end

function read_lua_array(col, index, value, data)
	if (type(index) == "string") then
        try_number = tonumber(index)
        if try_number then
            index = try_number
        else
		    index = _G[index]
        end
	end

    local cache_value = enum_cache[value];
    if not cache_value then
	    local f, s = load('return '..value);
	    if f then
			local ret, v = pcall(f);
			if ret and v then
		        cache_value = f();
		        enum_cache[value] = cache_value;
	        else
			    if type(data[col][index]) == 'string' then
			        cache_value = value;
				    enum_cache[value] = cache_value;
			    else
				    warn(v);
			    end
			end
		else
			if type(data[col][index]) == 'string' then
				cache_value = value;
			else
		        warn(s);
	        end
        end
    end
    if cache_value then
	    data[col][index] = cache_value;
    else
	    warn('failed to set col '..col..index..' to value '..value)
    end
end