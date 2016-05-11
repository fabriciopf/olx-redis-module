#!/usr/bin/env ruby

require 'optparse'
require 'redis'

#REDIS_BIN=~/Projects/OLX/redis/redis_with_modules/src/redis-cli
$options = {
	:verbose => false,
	:entries => 10,
	:redis_host => "localhost",
	:redis_port => 6379,
	:geo_key => "ads:geo",
	:lat_min => -32.641688,
	:lat_max => 3.488748,
	:long_min => -7.648387,
	:long_max => -8.257701
}
OptionParser.new do |opts|
	opts.banner = "Usage: example.rb [options]"

	opts.on("-v", "--verbose", "Run verbosely") do |verbose|
		$options[:verbose] = verbose
	end
	opts.on("-e", "--entries [ENTRIES]", "Number of entries to insert") do |entries|
		if entries then
			$options[:entries] = entries.to_i
		end
	end
	opts.on("-h", "--host [HOST]", "Redis host") do |host|
		if host then
			$options[:redis_host] = host
		end
	end
	opts.on("-p", "--port [PORT]", "Redis port") do |port|
		if port then
			$options[:redis_port] = port
		end
	end
end.parse!

def echo(text)
	if $options[:verbose] then
		puts text
	end
end

redis = Redis.new(:host => $options[:redis_host], :port => $options[:redis_port])

1.upto($options[:entries]).each do |i|
	lat = ($options[:lat_max]-$options[:lat_min]) * rand() + $options[:lat_min]
	long = ($options[:long_max]-$options[:long_min]) * rand() + $options[:long_min]
	echo("member#{i}: {lat:#{lat}, long: #{long}}")

	redis.geoadd($options[:geo_key], long, lat, "member#{i}")
end
