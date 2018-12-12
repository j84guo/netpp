#!/bin/bash

for i in {1..1000}; do
	nc localhost 8000 < README.md
done;
