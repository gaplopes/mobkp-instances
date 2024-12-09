#!/usr/bin/env bash

trap "exit" INT

type=1
m=3
weight_factor=0.5

ns=(20)
seeds=(1 2 3 4 5 6 7 8 9 10)

# Note that the maximum correlation value must be superior to -1/(m-1)
# For example:
# - If m=2, the maximum correlation value must be superior to -1
# - If m=3, the maximum correlation value must be superior to -0.5 (-0.48)
# - If m=4, the maximum correlation value must be superior to -0.33 (-0.31)
# - If m=5, the maximum correlation value must be superior to -0.25 (-0.23)
correlations=(-0.48 -0.25 -0.1)

for n in ${ns[@]}; do
  for s in ${seeds[@]}; do
    for c in ${correlations[@]}; do
      ./../build/mobkp-instances --type=$type --n=$n --m=$m --seed=$s --weight-factor=$weight_factor --correlation=$c
    done
  done
done
