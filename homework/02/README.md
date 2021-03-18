# Homework 2 -- parallel solution using OpenMP with task parallelism

Two versions of homework 2 were implemented.

The first implementation of (`HW 2 #1`)
added tasks on top of sequential solution from homework 1,
which consists of hard copying data
and passing them to the recursive function.

Second implementation (`HW 2 #2`)
also implements using references for the sequential version of recursion,
which boosts the time even more.
The threshold is set to 35 % of `maxDepth`.

| File      | Cost | Reference \[s] | HW 1 | HW 2 #1 | HW 2 #2 |
|:----------|-----:|---------------:|-----:|:--------|:--------|
| saj1.txt  |    5 |            0.0 |  0.0 | 0.002   | 0.003   |
| saj2.txt  |   14 |            0.6 |  2.0 | 0.540   | 0.183   |
| saj3.txt  |   12 |           0.01 | 0.05 | 0.011   | 0.005   |
| saj4.txt  |   14 |            0.4 |    1 | 0.270   | 0.104   |
| saj5.txt  |   12 |             12 |   46 | 0.082   | 0.009   |
| saj6.txt  |   16 |             31 |  107 | 1.673   | 0.312   |
| saj7.txt  |   16 |             67 |  258 | 1.598   | 0.208   |
| saj8.txt  |   20 |           1170 | 4287 | 38      | 10      |
| saj9.txt  |   26 |            6.3 |    7 | 0.856   | 0.130   |
| saj10.txt |   20 |             36 |   81 | 8       | 5.455   |
| saj11.txt |   18 |            0.2 |  0.6 | 0.172   | 0.085   |
| saj12.txt |   22 |             72 |  313 | 94      | 17      |
