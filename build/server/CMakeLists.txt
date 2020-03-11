pre_project(RUNTIME)

add_project()

post_project()

add_dependencies (${TARGET_NAME} server_base game_data common)
