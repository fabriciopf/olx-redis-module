#!/usr/bin/env ruby

require 'sinatra'
require 'optparse'
require 'redis'
require 'json'

# Default options
$options = {
	:redis_host => "localhost",
	:redis_port => 6379,
	:geo_key => "ads:geo"
}

OptionParser.new do |opts|
	opts.banner = "Usage: example.rb [options]"

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

get '/' do
	'Hello world!'
end

get '/ads' do
	content_type :json
	headers "Access-Control-Allow-Origin" => "http://localhost:8080"

	lat = params['lat']
	long = params['long']
	radius = params['radius']

	redis = Redis.new(:host => $options[:redis_host], :port => $options[:redis_port])

	ads = redis.georadius($options[:geo_key], long, lat, radius, 'km', 'WITHCOORD')

	ads.to_json
end
