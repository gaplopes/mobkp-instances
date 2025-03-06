#!/usr/bin/env bash

trap "exit" INT

type=1
m=3
weight_factor=0.5

ns=(20 25 30 35 40 45 50 60 70 80 90 100 150 200)
seeds=(1 2 3 4 5 6 7 8 9 10)

# Note that the maximum correlation value must be superior to -1/(m-1)
# For example:
# - If m=2, the maximum correlation value must be superior to -1
# - If m=3, the maximum correlation value must be superior to -0.5 (-0.48)
# - If m=4, the maximum correlation value must be superior to -0.33 (-0.31)
# - If m=5, the maximum correlation value must be superior to -0.25 (-0.23)
if [ $m -eq 2 ]; then
  correlations=(-0.8 -0.4 -0.1)
elif [ $m -eq 3 ]; then
  correlations=(-0.45 -0.25 -0.1)
elif [ $m -eq 4 ]; then
  correlations=(-0.30 -0.15 -0.1)
elif [ $m -eq 5 ]; then
  correlations=(-0.20 -0.10 -0.05)
fi

for n in ${ns[@]}; do
  for c in ${correlations[@]}; do
    for s in ${seeds[@]}; do
      ./../build/mobkp-instances --type=$type --n=$n --m=$m --seed=$s --weight-factor=$weight_factor --correlation=$c &
    done
    wait
  done
done
