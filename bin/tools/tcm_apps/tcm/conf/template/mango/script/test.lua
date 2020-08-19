LoadPackage('role.pkg')
Include('bt_node.lua')

INF_LOG("this is a lua log")
INF_LOG("----------------------------test---------------------------")

function create_role(o)
	INF_LOG("age is " .. o:get_age())

	o:set_age(10)
	o:set_level(10)
	INF_LOG("age is " .. o:get_age())
end


tree = 
{btRandom, param = {10, 10},
	{batSequencer,
		{batCheckRoleLevel, param = {boLT, 15} },
		{batRoleRun, param = {1, 2, 3, 4} },
	},
	{batSequencer,
		{batCheckRoleLevel, param = {boGT, 5}},
		{batRoleRun, param = {7, 6, 5, 4} },
	}
}

register_global_event('evtRoleKillNpc', 10, 0, tree, 1);