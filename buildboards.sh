#!/bin/bash

targets=(
    "AIKONF4" \
    "CLRACINGF4" \
    "REVOLT" \
)


for target in "${targets[@]}"
do
	make TARGET=${target} clean
	make TARGET=${target} 
done
