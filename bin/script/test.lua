LoadPackage('role.pkg')

INF_LOG("this is a lua log")

function create_role(o)
	INF_LOG("age is " .. o:get_age())

	o:set_age(10)
	INF_LOG("age is " .. o:get_age())
end
