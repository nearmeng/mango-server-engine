LoadPackage('bt.pkg')

bt_optimize = true;
g_param_func_counter = g_param_func_counter or 1;
PARAM_FUNC_CACHE = PARAM_FUNC_CACHE or {};

PARAM_ENV = {};

setmetatable(PARAM_ENV, {
	__index = function (t, k)
		local v = _ENV[k];
		return v;
	end,

	__newindex = function (t, k, v)
		if string.sub(k, 1, 9) == 'ParamFunc' then
			_ENV[k] = v;
		else
			error('cannot set global variable '..k..' in param function');
		end
	end,
});

PREFIX_LIST = PREFIX_LIST or {
	[string.byte('E')] = { flag = bptLocalEventVar },
	[string.byte('T')] = { flag = bptLocalTriggerVar },
	[string.byte('R')] = { flag = bptRoleVar },
	[string.byte('S')] = { flag = bptSceneVar },
};

function get_global_name(f)
    for k,v in pairs(_G) do
        if v == f then
            return k;
        end
    end
end

-- 通过一个table创建一颗行为树
function create_node_from_table(parent, parent_type, t)
    local node = nil;
    local ok = true;
    local nodeType = btInvalid;
    local name;
    local param;
	local varflag = 0;
	local retvalue = 0;
    local children = {};
	local childrencount = 0;
	local varbit = 5;
	
    if t.param then
		-- 6 param analysis
		param = {};
		for i = 1, 6 do
			local pvalue = t.param[i];
			local ptype = type(pvalue);

			if ptype == 'number' or ptype == 'integer' then
				param[i] = pvalue;
			elseif ptype == 'boolean' then
				if pvalue then
					param[i] = 1;
				else
					param[i] = 0;
				end
			elseif ptype == 'function' then
				param[i] = t.param[i]();
			elseif ptype == 'string' then
				local prefix = PREFIX_LIST[string.byte(pvalue, 1, 1)];
				local nvalue = tonumber(string.sub(pvalue, 2, -1));
				if prefix and nvalue then
					param[i] = nvalue;
					varflag = varflag + (prefix.flag << ((i - 1) * varbit));
				elseif pvalue == '' or tonumber(pvalue) then
					param[i] = tonumber(pvalue);
				else
					param[i] = PARAM_FUNC_CACHE[pvalue];
					if not param[i] then
						param[i] = g_param_func_counter;
						local s = "function ParamFunc"..param[i].."(o) return "..pvalue.." end";
						local f,msg = load(s, _, _, PARAM_ENV);
						if f then
							f();
							PARAM_FUNC_CACHE[pvalue] = param[i];
							g_param_func_counter = g_param_func_counter + 1;
						else
							warn(s);
							warn('failed to load string '..msg);
						end
					end

					varflag = varflag + (bptLuaFunc << ((i - 1) * varbit));
				end
			elseif ptype == 'table' then
				local subnode, subok = create_node_from_table(0, 0, pvalue);
				if not subok then
					warn('failed to load embedded tree');
					warn(t.param[i]);
				    print_node(subnode);
					return 0, false;
				else
					local tree_id = create_bt_tree(subnode);
					param[i] = tree_id;

					if debug then
						print( "sub bt tree "..tree_id);
						print_tree(tree_id);
					end
				end
			else
				param[i] = pvalue;
			end
		end
    else
        param = {0, 0, 0, 0, 0, 0};
    end

    for k,v in pairs(t) do
    	if type(v) == 'string' and string.sub(v, 1, 3) == 'on_' then
    		v = _G[v];
    	end
        if k == 'param' then
        
		elseif type(v) == 'table' then
			children[k] = v;
			childrencount = childrencount + 1;
		else
			if type(v) == 'function' then
				name = get_global_name(v);
				nodeType = find_bt_action_id(name);
				if not nodeType then
				    nodeType = register_bt_action(name);
				end
			elseif type(v) == 'number' then
				nodeType = v;
				name = tostring(nodeType);
	        elseif v == nil then
	        
			else
			    warn("unknown node k="..tostring(k)..", v="..tostring(v));
                ok = false;
			end
		end
    end
    
    if bt_optimize then
		if childrencount == 0 and (nodeType == batSequencer or nodeType == batParallel or nodeType == batSelector) then
	        return 0, true;
		elseif childrencount == 1 and (nodeType == batSequencer or nodeType == batSelector) then
			for k,v in pairs(children) do
	            return create_node_from_table(parent, parent_type, v);
			end
		elseif parent_type > 0 then
			if (nodeType == batSequencer and parent_type == batSequencer) or 
			   (nodeType == batSelector and parent_type == batSelector) or
			   (nodeType == batParallel and parent_type == batParallel) then
				node = parent;
			end
		end
    end

    if nodeType ~= btInvalid and not node then
        node = create_bt_node(parent, nodeType, param[1], param[2], param[3], param[4], param[5], param[6], varflag);
    end
    
    if node then
        for k,v in pairs(children) do
            local child, child_ok = create_node_from_table(node, nodeType, v);
            if not child_ok then
				if name then
					warn('failed to create child node of node '..name);
				else
					warn('failed to create child node of unknown name node');
				end
				
                ok = false;
                break;
            end

			if type(k) == 'number' then
				if k >= brvError then
					set_node_ret_value(child, k);			
				end
			end
        end
    else
		if name then
			warn('failed to create node '..name);
		else
			warn('failed to create unknown name node');
		end
        ok = false;
    end
    
	return node, ok;
end


function load_behavior_tree(tree, debug)
	local t;
	local tree_type = type(tree);

	-- 确定table
	if tree_type == 'string' then
		-- 通过字符串函数创建行为树
		t = _G[tree];
		if t then
			if type(t) == 'function' then
				t = t();
				if not t then
					return 0, 1;
				end
			end
		end
	elseif tree_type == 'table' then
		-- 通过table创建行为树
		t = tree;
		if debug then
			tree = get_global_name(t);
		else 
			tree = 'table';
		end
	elseif tree_type == 'function' then
		-- 通过函数创建行为树
		t = tree();
		if debug then
			tree = get_global_name(tree);
		else 
			tree = 'function';
		end
	end

	if not t then
		return 0, 1;
	end

	-- 根据table创建行为树
	local node, ok = create_node_from_table(0, 0, t);
	if not ok then
	    warn( "bt tree:"..tree.." error");
	    print_node(node);
	    return 0, 0;
	else
		if node > 0 then
			-- 根据nodeid重新建立一颗逻辑数
			local tree_id = create_bt_tree(node);
			if debug then
				print( "bt tree:"..tree);
	   			print_tree(tree_id);
			end
		    return tree_id, 1;
		else
			return 0, 1;
		end
	end
end

function register_global_event(event_name, template_id, event_param, tree, debug)
	local tree_id = load_behavior_tree(tree, debug);
	if tree_id > 0 then
		local event_id = create_bt_event(event_name..template_id..event_param, template_id, _G[event_name], tree_id, event_param, 0);
		return c_register_global_event(event_id);
	else
	    warn( "create tree failed");
		return false;
	end
end
