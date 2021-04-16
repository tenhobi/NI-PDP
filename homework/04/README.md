# Homework 3 -- parallel solution using OpenMP with data parallelism

Implementation of data parallelism brings neither speed-up nor slowdown.
However, it should be noted that this solution bring a stable result,
in comparison with task parallelism.

| File      | Cost | Reference \[s] | HW 1 | HW 2 #1 | HW 2 #2 | HW 3  |
|:----------|-----:|---------------:|-----:|:--------|:--------|:------|
| saj1.txt  |    5 |            0.0 |  0.0 | 0.002   | 0.003   | 0.005 |
| saj2.txt  |   14 |            0.6 |  2.0 | 0.540   | 0.183   | 0.111 |
| saj3.txt  |   12 |           0.01 | 0.05 | 0.011   | 0.005   | 0.022 |
| saj4.txt  |   14 |            0.4 |    1 | 0.270   | 0.104   | 0.069 |
| saj5.txt  |   12 |             12 |   46 | 0.082   | 0.009   | 0.029 |
| saj6.txt  |   16 |             31 |  107 | 1.673   | 0.312   | 0.288 |
| saj7.txt  |   16 |             67 |  258 | 1.598   | 0.208   | 0.040 |
| saj8.txt  |   20 |           1170 | 4287 | 38      | 10      | 8.853 |
| saj9.txt  |   26 |            6.3 |    7 | 0.856   | 0.130   | 0.142 |
| saj10.txt |   20 |             36 |   81 | 8       | 5.455   | 7.910 |
| saj11.txt |   18 |            0.2 |  0.6 | 0.172   | 0.085   | 0.078 |
| saj12.txt |   22 |             72 |  313 | 94      | 17      | 20    |
