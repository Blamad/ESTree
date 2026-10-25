# treegen_cli: headless tree generator. Links ONLY treegen (proof that the module is engine-free).
# Included from the root CMakeLists (option ESTREE_BUILD_TOOLS), outside the ESTREE_BUILD_ENGINE gate.

add_executable(treegen_cli main.cpp)
target_link_libraries(treegen_cli PRIVATE treegen)
# The root include_directories()/add_definitions() are global: keep the tool as clean as the library.
set_property(TARGET treegen_cli PROPERTY INCLUDE_DIRECTORIES "")
if(MSVC)
	target_compile_options(treegen_cli PRIVATE /W4)
else()
	target_compile_options(treegen_cli PRIVATE -Wall -Wextra -Wpedantic)
endif()
if(COMMAND estree_asan_runtime)
	estree_asan_runtime(treegen_cli)
endif()

if(ESTREE_BUILD_TESTS)
	add_executable(obj_check tests/obj_check.cpp)
	if(MSVC)
		target_compile_options(obj_check PRIVATE /W4)
	else()
		target_compile_options(obj_check PRIVATE -Wall -Wextra -Wpedantic)
	endif()
	if(COMMAND estree_asan_runtime)
		estree_asan_runtime(obj_check)
	endif()

	set(CLI_OUT ${CMAKE_BINARY_DIR}/treegen_cli_tests)
	set(CLI_RULES ${PROJECT_SOURCE_DIR}/Resources/LindenmayerRules)
	set(CLI $<TARGET_FILE:treegen_cli>)
	set(CHECK $<TARGET_FILE:obj_check>)
	file(MAKE_DIRECTORY ${CLI_OUT})

	# name, expected exit code, expected output substring, remaining command line
	function(treegen_cli_run name exit needle)
		add_test(NAME treegen_cli_${name}
			COMMAND ${CHECK} run ${exit} ${needle} ${CLI_OUT}/${name}.log -- ${CLI} ${ARGN})
		set_tests_properties(treegen_cli_${name} PROPERTIES WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
	endfunction()

	treegen_cli_run(help 0 "usage: treegen_cli" --help)
	treegen_cli_run(no_args 2 "missing rule file")
	treegen_cli_run(missing_out 2 "missing --out" ${CLI_RULES}/randomTree.json)
	treegen_cli_run(unknown_option 2 "unknown option" ${CLI_RULES}/randomTree.json --out ${CLI_OUT}/x.obj --bogus 1)
	treegen_cli_run(bad_seed 2 "invalid value" ${CLI_RULES}/randomTree.json --out ${CLI_OUT}/x.obj --seed abc)
	treegen_cli_run(bad_ring 2 "invalid value" ${CLI_RULES}/randomTree.json --out ${CLI_OUT}/x.obj --ring 2)
	treegen_cli_run(missing_file 3 "no_such_rules.json" ${CLI_OUT}/no_such_rules.json --out ${CLI_OUT}/x.obj)
	treegen_cli_run(bad_json 3 "truncated.json" ${PROJECT_SOURCE_DIR}/tests/treegen/data/truncated.json --out ${CLI_OUT}/x.obj)
	treegen_cli_run(unwritable_out 4 "failed" ${CLI_RULES}/randomTree.json --seed 1 --depth 1 --out ${CLI_OUT})

	treegen_cli_run(gen_monopodial 0 "segments=" ${CLI_RULES}/parametricMonopodialTreeA.json --seed 1 --out ${CLI_OUT}/monopodial_s1.obj)
	treegen_cli_run(gen_monopodial_again 0 "segments=" ${CLI_RULES}/parametricMonopodialTreeA.json --seed 1 --out ${CLI_OUT}/monopodial_s1_again.obj)
	treegen_cli_run(gen_random_s1 0 "seed=1 " ${CLI_RULES}/randomTree.json --seed 1 --out ${CLI_OUT}/random_s1.obj)
	treegen_cli_run(gen_random_s1_again 0 "seed=1 " ${CLI_RULES}/randomTree.json --seed 1 --out ${CLI_OUT}/random_s1_again.obj)
	treegen_cli_run(gen_random_s2 0 "seed=2 " ${CLI_RULES}/randomTree.json --seed 2 --out ${CLI_OUT}/random_s2.obj)
	treegen_cli_run(gen_threads 0 "segments=" ${CLI_RULES}/randomTree.json --seed 1 --threads 3 --out ${CLI_OUT}/random_s1_t3.obj)
	set_tests_properties(treegen_cli_gen_monopodial treegen_cli_gen_monopodial_again
		treegen_cli_gen_random_s1 treegen_cli_gen_random_s1_again treegen_cli_gen_random_s2 treegen_cli_gen_threads
		PROPERTIES FIXTURES_SETUP treegen_cli_objs)

	function(treegen_cli_check name)
		add_test(NAME treegen_cli_${name} COMMAND ${CHECK} ${ARGN})
		set_tests_properties(treegen_cli_${name} PROPERTIES FIXTURES_REQUIRED treegen_cli_objs)
	endfunction()
	treegen_cli_check(validate_monopodial validate ${CLI_OUT}/monopodial_s1.obj)
	treegen_cli_check(validate_random validate ${CLI_OUT}/random_s1.obj)
	treegen_cli_check(same_seed_monopodial same ${CLI_OUT}/monopodial_s1.obj ${CLI_OUT}/monopodial_s1_again.obj)
	treegen_cli_check(same_seed_random same ${CLI_OUT}/random_s1.obj ${CLI_OUT}/random_s1_again.obj)
	treegen_cli_check(same_for_any_thread_count same ${CLI_OUT}/random_s1.obj ${CLI_OUT}/random_s1_t3.obj)
	treegen_cli_check(different_seed_differs differ ${CLI_OUT}/random_s1.obj ${CLI_OUT}/random_s2.obj)
endif()
