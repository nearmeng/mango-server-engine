table_stack = {};

function get_table_stack()
    local s = '';
    for i, tn in ipairs(table_stack) do
        s = s..tn..'.'
    end
    return s;
end

function overwrite_config(src, dst)
    for k,v in pairs(src) do
        if dst[k] == nil then
			if type(k) == 'number' then
				dst[k] = v;
			else
	            warn('dest config '..get_table_stack()..k.." is nil");
			end
        else
            if type(v) == 'table' then
                table.insert(table_stack, k);
				overwrite_config(src[k], dst[k]);
				table.remove(table_stack);
            else
                dst[k] = v;
            end
        end
    end
end

function load_traverse_table(d, t)
    for k,v in pairs(t) do
		if d[k] == nil then
--  			warn("variable "..get_table_stack()..k.." is nil");
		else
			if type(v) == "table" then
                table.insert(table_stack, k);
				load_traverse_table(d[k], v)
				table.remove(table_stack);
			else
				d[k] = t[k];
			end
		end
    end
end

function fill_traverse_table(d, t, n)
    for k,v in pairs(t) do
		if d[k] == nil then
			n[k] = v;
		else
			if type(v) == "table" then
				n[k] = {};
				fill_traverse_table(d[k], v, n[k])
			else
				n[k] = d[k];
			end
		end
    end
end


-- 序列化table表--將表转化成string
function serialize_table(obj)
    local lua = ""
    local t = type(obj)
    if t == "number" then
        lua = lua .. obj
    elseif t == "boolean" then
        lua = lua .. tostring(obj)
    elseif t == "string" then
        lua = lua .. string.format("%q", obj)
    elseif t == "table" then
        lua = lua .. "{\n"
		for k, v in pairs(obj) do
			lua = lua .. "[" .. serialize_table(k) .. "]=" .. serialize_table(v) .. ",\n"
		end
		local metatable = getmetatable(obj)
        if metatable ~= nil and type(metatable.__index) == "table" then
			for k, v in pairs(metatable.__index) do
				lua = lua .. "[" .. serialize_table(k) .. "]=" .. serialize_table(v) .. ",\n"
			end
		end
        lua = lua .. "}"
    elseif t == "nil" then
        return nil
    else
        return "-nil-" 
    end
    return lua
end

function load_file(config, config_name)
	load_traverse_table(config, _G[config_name]);
end

function load_string(dst, src_string)
	local f, msg = load('return '..src_string);
	if f then
		local src_table = f();
		overwrite_config(src_table, dst);
	else
		error('failed to load string '..src_string);
	end
end

function save_string(src)
	local n = {};
	fill_traverse_table(src,ServerConfig.ServerGroupConfig[0],n);
	return serialize_table(n);	
end
