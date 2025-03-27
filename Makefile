.PHONY: build run test

build:
	docker build -t aegisgraph .

run:
	docker run -it aegisgraph

test:
	docker run -it aegisgraph pytest tests/
