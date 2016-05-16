# olx-redis-module

How to generate libolxredis.so:

gcc -shared -o libolxredis.so -fPIC olx-module.cpp -I../RedisModulesSDK -lc++


The **populate.sh** is a script to create lots of entries on a redis geo key so we can test the geo features with massive data.
We're randomizing entries from latitude -32.641688 to 3.488748 and from longitude -7.648387 to -8.257701, so the entries are mostly on Brazil. (or in the water =P)

Before running the script:

```bundle install```

And then, run the following to insert 1000 entries in a local redis database:

```./populate.rb -e 1000```

To run the API server:

``` ./geoapp.rb  ```

Then, to call the API and get the ads 1km near a point:

```curl http://localhost:4567/ads?lat=-22.360462\&long=-7.810437\&radius=1```

Now, to open the web page, run the following on project root directory:

```php -S localhost:8080```
