#!/bin/bash

targets=(
    "AIKONF4" \
    "CLRACINGF4" \
    "REVOLT" \
    "NOX" \
    "PYRODRONEF4" \
)


for target in "${targets[@]}"
do
	make TARGET=${target} clean
	make TARGET=${target} 
done
