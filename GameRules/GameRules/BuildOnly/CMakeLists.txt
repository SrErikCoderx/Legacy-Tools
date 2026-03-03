project(GameRules)
# VS Makefile project
file(GLOB_RECURSE GAME_RULES_FILES "Tutorial/*")
add_custom_target(GameRules SOURCES ${GAME_RULES_FILES})
