GREEN=\033[0;1;92m
NC=\033[0m

# Default to release build
build: build-release

build-release: FORCE
	rm -fr logs # Old version log files
	mkdir -p build
	
	cmake -DCMAKE_BUILD_TYPE=Release -G "CodeBlocks - Unix Makefiles" . -B cmake-build-release
	cmake --build cmake-build-release --target all
	
	cp cmake-build-release/nr-gnb build/
	cp cmake-build-release/nr-ue build/
	cp cmake-build-release/nr-cli build/
	cp cmake-build-release/src/nflambda/runtime/runtime build/
	cp cmake-build-release/libdevbnd.so build/
	cp tools/nr-binder build/
	
	# Copy test executables if they exist
	@if [ -f cmake-build-release/src/nflambda/memory/memory_test ]; then cp cmake-build-release/src/nflambda/memory/memory_test build/; fi
	@if [ -f cmake-build-release/src/nflambda/mailbox/mailbox_test ]; then cp cmake-build-release/src/nflambda/mailbox/mailbox_test build/; fi
	@if [ -f cmake-build-release/src/nflambda/event_system/event_system_test ]; then cp cmake-build-release/src/nflambda/event_system/event_system_test build/; fi
	@if [ -f cmake-build-release/src/nflambda/event_system/event_pool_test ]; then cp cmake-build-release/src/nflambda/event_system/event_pool_test build/; fi
	@if [ -f cmake-build-release/src/nflambda/app/ping_pong/ping_pong_demo ]; then cp cmake-build-release/src/nflambda/app/ping_pong/ping_pong_demo build/; fi
	@if [ -f cmake-build-release/src/nflambda/app/simulated_5g_core_with_runtime/simulated_5g_core_with_runtime_demo ]; then cp cmake-build-release/src/nflambda/app/simulated_5g_core_with_runtime/simulated_5g_core_with_runtime_demo build/; fi
	@if [ -f cmake-build-release/src/nflambda/app/ipc_echo/ipc_echo_demo ]; then cp cmake-build-release/src/nflambda/app/ipc_echo/ipc_echo_demo build/; fi
	@if [ -f cmake-build-release/src/nflambda/app/ipc_echo/ipc_client_demo ]; then cp cmake-build-release/src/nflambda/app/ipc_echo/ipc_client_demo build/; fi
	@if [ -f cmake-build-release/src/nflambda/app/nflambda_5gcore/nas_ipc_protocol_test ]; then cp cmake-build-release/src/nflambda/app/nflambda_5gcore/nas_ipc_protocol_test build/; fi
	@if [ -f cmake-build-release/nflambda_5gcore ]; then cp cmake-build-release/nflambda_5gcore build/; fi
	@if [ -f cmake-build-release/nflambda_5gcore_test_client ]; then cp cmake-build-release/nflambda_5gcore_test_client build/; fi

	@printf "${GREEN}UERANSIM successfully built (Release).${NC}\n"

build-debug: FORCE
	rm -fr logs # Old version log files
	mkdir -p build
	
	cmake -DCMAKE_BUILD_TYPE=Debug -G "CodeBlocks - Unix Makefiles" . -B cmake-build-debug
	cmake --build cmake-build-debug --target all
	
	cp cmake-build-debug/nr-gnb build/
	cp cmake-build-debug/nr-ue build/
	cp cmake-build-debug/nr-cli build/
	cp cmake-build-debug/src/nflambda/runtime/runtime build/
	cp cmake-build-debug/libdevbnd.so build/
	cp tools/nr-binder build/
	
	# Copy test executables if they exist
	@if [ -f cmake-build-debug/src/nflambda/memory/memory_test ]; then cp cmake-build-debug/src/nflambda/memory/memory_test build/; fi
	@if [ -f cmake-build-debug/src/nflambda/mailbox/mailbox_test ]; then cp cmake-build-debug/src/nflambda/mailbox/mailbox_test build/; fi
	@if [ -f cmake-build-debug/src/nflambda/event_system/event_system_test ]; then cp cmake-build-debug/src/nflambda/event_system/event_system_test build/; fi
	@if [ -f cmake-build-debug/src/nflambda/event_system/event_pool_test ]; then cp cmake-build-debug/src/nflambda/event_system/event_pool_test build/; fi
	@if [ -f cmake-build-debug/src/nflambda/app/ping_pong/ping_pong_demo ]; then cp cmake-build-debug/src/nflambda/app/ping_pong/ping_pong_demo build/; fi
	@if [ -f cmake-build-debug/src/nflambda/app/simulated_5g_core_with_runtime/simulated_5g_core_with_runtime_demo ]; then cp cmake-build-debug/src/nflambda/app/simulated_5g_core_with_runtime/simulated_5g_core_with_runtime_demo build/; fi
	@if [ -f cmake-build-debug/src/nflambda/app/ipc_echo/ipc_echo_demo ]; then cp cmake-build-debug/src/nflambda/app/ipc_echo/ipc_echo_demo build/; fi
	@if [ -f cmake-build-debug/src/nflambda/app/ipc_echo/ipc_client_demo ]; then cp cmake-build-debug/src/nflambda/app/ipc_echo/ipc_client_demo build/; fi
	@if [ -f cmake-build-debug/src/nflambda/app/nflambda_5gcore/nas_ipc_protocol_test ]; then cp cmake-build-debug/src/nflambda/app/nflambda_5gcore/nas_ipc_protocol_test build/; fi
	@if [ -f cmake-build-debug/nflambda_5gcore ]; then cp cmake-build-debug/nflambda_5gcore build/; fi
	@if [ -f cmake-build-debug/nflambda_5gcore_test_client ]; then cp cmake-build-debug/nflambda_5gcore_test_client build/; fi

	@printf "${GREEN}UERANSIM successfully built (Debug).${NC}\n"

FORCE:

clean:
	rm -fr build
	rm -fr cmake-build-release/
	rm -fr cmake-build-debug/