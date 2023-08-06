CFLAGS = -Wall -Wextra -pedantic -std=c17
MARKER="========================"
BUILD_FOLDER_NAME=bin

main: main.c
	$(CC) main.c -o $(BUILD_FOLDER_NAME)/main $(CFLAGS)
tests: tests.test.c
	$(CC) tests.test.c -o $(BUILD_FOLDER_NAME)/tests $(CFLAGS)

run: main
	@echo "cd ./$(BUILD_FOLDER_NAME) && ./main"
	@echo $(MARKER)
	@cd ./$(BUILD_FOLDER_NAME) && ./main
	@echo "\n$(MARKER)\n"

run-tests: tests
	@echo "cd ./$(BUILD_FOLDER_NAME) && ./tests"
	@echo $(MARKER)
	@cd ./$(BUILD_FOLDER_NAME) && ./tests
	@echo "\n$(MARKER)\n"
