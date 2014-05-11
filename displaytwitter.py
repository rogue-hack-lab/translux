#!/usr/bin/env python
#Python 3.4
from twython import Twython, TwythonError

#to keep API keys out of version control create apikeys.py. 
#it onlyneeds to contain the two definitions below
import apikeys
APP_KEY = apikeys.APP_KEY
APP_SECRET = apikeys.APP_SECRET

# Requires Authentication as of Twitter API v1.1
twitter = Twython(APP_KEY, APP_SECRET)

#define what strings you are searching Twitter for and how many of each result you wish to see.
tags = ['#trtt2014','tinkerfest','RogueHackLab','#RHL','#ScienceWorks']
results_per_tag = 5

def GetTweets(String, Count):
	try:
		search_results = twitter.search(q=String, count=Count)
	except TwythonError as e:
		print (e)
	return search_results

tweets = {}
for tag in tags:
	search_results = GetTweets(tag, results_per_tag)
	for tweet in search_results['statuses']:
		if tweet['id'] not in tweets:
			tweets[tweet['id']] = tweet
			#msg.append("@%s-> %s" % (tweet['user']['screen_name'].encode('utf-8'), tweet['text'].encode('utf-8')))

for s in tweets.values():
	print (s['text'].encode('utf-8'), '\n')
