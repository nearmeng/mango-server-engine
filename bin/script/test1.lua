LoadPackage('test_role.pkg')

INF_LOG("this is a lua log")
INF_LOG("----------------------------test---------------------------")

function create_role(o)
	INF_LOG("age is " .. o:get_age())

	o:set_age(10)
	o:set_level(10)
	INF_LOG("age is " .. o:get_age())
end
