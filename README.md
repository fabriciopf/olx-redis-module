# olx-redis-module

The **populate.sh** is a script to create lots of entries on a redis geo key so we can test the geo features with massive data.
We're randomizing entries from latitude -32.641688 to 3.488748 and from longitude -7.648387 to -8.257701, so the entries are mostly on Brazil. (or in the water =P)

Creating some entries:

```./populate.sh 1000```
