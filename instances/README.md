# Instances

There are three types of instances available to generate the Pareto front of the MOKP:

- Random instances;
- Instances with positive correlation between objectives;
- Instances with negative correlation between objectives.

## Folder structure

The instances are stored in the following structure:

```
instances/
├── neg_corr/
│   ├── 2D/
│   │   ├── 100_2_-0.25.in
│   │   ├── ...
│   │── 3D/
│   │   ├── 100_3_-0.25.in
│   │   ├── ...
│   │── ...
├── pos_corr/
│   ├── 2D/
│   │   ├── 100_2_0.25.in
│   │   ├── ...
│   │── 3D/
│   │   ├── 100_3_0.25.in
│   │   ├── ...
│   │── ...
├── random/
│   ├── 2D/
│   │   ├── 100_2.in
│   │   ├── ...
│   │── 3D/
│   │   ├── 100_3.in
│   │   ├── ...
│   │── ...
└── README.md
```

From the root folder, the instances are divided into three subfolders:

- `neg_corr`: instances with negative correlation between objectives;
- `pos_corr`: instances with positive correlation between objectives;
- `random`: instances with random correlation between objectives.

Each subfolder contains a set of subfolders, one for each dimension of the problem. Each of these subfolders contains the instances for that dimension.

## Instance naming

Each instance, depending on the type of instance, is stored in a file with a specific name.

### Random instances

The instances are named according to the following pattern:

```
n_m.in
```

where:

- `n` is the number of items;
- `m` is the number of objectives.

### Correlated instances

The instances are named according to the following pattern:

```
n_m_c.in
```

where:

- `n` is the number of items;
- `m` is the number of objectives;
- `c` is the correlation between objectives.

## Instances structure

Each instance is stored in a file with the following structure:

```
n m
W
w_1 p^1_1 ... p^m_1
...
w_n p^1_1 ... p^m_n
nd
p_1 ... p_m
...
p_nd ... p_m
```

where:

- `n` is the number of items;
- `m` is the number of objectives;
- `W` is the knapsack capacity;
- `w_i` is the weight of item `i`;
- `p^i_j` is the profit of item `j` in objective `i`;
- `nd` is the number of non-dominated points;
- `p_j` is the value of the non-dominated point in objective `j`.
