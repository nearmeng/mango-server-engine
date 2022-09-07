LoadPackage('test_role.pkg')
Include('bt_node.lua')

function test_lua_func(o, p0, p1, p2, p3, p4)
	INF_LOG("param is " .. p0 .. " " ..p1 .. " " .. p2 .. " " .. p3 .. " " .. p4)
	INF_LOG("test lua, get level " .. o:get_level())

	o:set_level(100)
	INF_LOG("test lua, get level " .. o:get_level())

	return brvSuccess
end

test_value = 321

tree = 
{btParallel, param = {10, 10},
	{batSequencer,
		{batCheckRoleLevel, param = {boLT, 15} },
		{batRoleRun, param = {1, 2, 3, 4} },
	},
	{batSequencer,
		{batCheckRoleLevel, param = {boGT, 5}},
		{batRoleRun, param = {7, 6, 5, 4} },
	},
	{batSequencer,
		{test_lua_func, param = {123, 'E0', 'T1', 'R50', 'test_value'}},
	}
}

register_global_event('evtRoleKillNpc', 10, 0, tree, 1);