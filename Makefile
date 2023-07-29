MARKER="========================"

main: main.c
	$(CC) main.c -o bin/main -Wall -Wextra -pedantic -std=c17
tests: tests.test.c
	$(CC) tests.test.c -o bin/tests -Wall -Wextra -pedantic -std=c17

run: main
	@echo ./bin/main
	@echo $(MARKER)
	@./bin/main
	@echo "\n$(MARKER)\n"

run-tests: tests
	@echo ./bin/tests
	@echo $(MARKER)
	@./bin/tests
	@echo "\n$(MARKER)\n"
