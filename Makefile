MARKER="========================"

main: main.c
	$(CC) main.c -o bin/main -Wall -Wextra -pedantic -std=c17
run: main
	@echo ./bin/main
	@echo $(MARKER)
	@./bin/main
	@echo "\n$(MARKER)\n"
