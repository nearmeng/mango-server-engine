pre_project(STATIC)

add_project()

post_project()

add_dependencies (${TARGET_NAME} router_client game_data common)
