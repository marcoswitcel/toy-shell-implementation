CFLAGS = -Wall -Wextra -pedantic -std=c17 
MARKER="========================"

# Pastas de código fonte
SHELL_SOURCE_FOLDER=src
TESTS_SOURCE_FOLDER=tests

# parâmetros condicionais do comando "run-debug"
# ativa Address Sanitizers e emissão de informação de debug
run-debug: CFLAGS += -fsanitize=address -g
run-gdb: CFLAGS += -fsanitize=address -g

# Pasta aonde o artefacto fica após o build
BUILD_FOLDER_NAME=bin

main: $(SHELL_SOURCE_FOLDER)/main.c
	$(CC) $(SHELL_SOURCE_FOLDER)/main.c -o $(BUILD_FOLDER_NAME)/main $(CFLAGS)

tests: $(TESTS_SOURCE_FOLDER)/tests.test.c
	$(CC) $(TESTS_SOURCE_FOLDER)/tests.test.c -o $(BUILD_FOLDER_NAME)/tests $(CFLAGS)

run: main
	@echo "cd ./$(BUILD_FOLDER_NAME) && ./main"
	@echo $(MARKER)
	@cd ./$(BUILD_FOLDER_NAME) && ./main
	@echo "\n$(MARKER)\n"

run-debug: main
	@echo "cd ./$(BUILD_FOLDER_NAME) && ./main"
	@echo $(MARKER)
	@cd ./$(BUILD_FOLDER_NAME) && ./main
	@echo "\n$(MARKER)\n"

run-gdb: main
	@echo "cd ./$(BUILD_FOLDER_NAME) && gdb ./main"
	@echo $(MARKER)
	@cd ./$(BUILD_FOLDER_NAME) && gdb ./main
	@echo "\n$(MARKER)\n"

run-tests: tests
	@echo "cd ./$(BUILD_FOLDER_NAME) && ./tests"
	@echo $(MARKER)
	@cd ./$(BUILD_FOLDER_NAME) && ./tests
	@echo "\n$(MARKER)\n"
