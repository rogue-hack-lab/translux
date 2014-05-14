#!/usr/bin/env python
#Python 2.7

##Documentation
#twitter - https://dev.twitter.com/docs
#twython - http://twython.readthedocs.org/en/latest/
from twython import Twython, TwythonError

#to keep API keys out of version control create apikeys.py. 
#it onlyneeds to contain the two definitions below
import apikeys, time, sys, os, serial

#requires Authentication as of Twitter API v1.1
twitter = Twython(apikeys.TWITTER_APP_KEY, apikeys.TWITTER_APP_SECRET)

#define what strings you are searching Twitter for and how many of each result you wish to see.
tags = ['#trtt2014', 'tinkerfest', 'RogueHackLab', 'ScienceWorks', '@Soupala']
results_per_tag = 1
	
def GetTweets(String, Count):
	try:
		search_results = twitter.search(q=String, count=Count)
	except TwythonError as e:
		print e
	return search_results

def TweetDict(tags, results_per_tag):
	t = {}
	for tag in tags:
		search_results = GetTweets(tag, results_per_tag)
		for tweet in search_results['statuses']:
			#if tweet['id'] not in t:
			t[tweet['id']] = tweet #"%s \n %s \n~@%s" % (tweet['created_at'], tweet['text'].encode('utf-8'), tweet['user']['screen_name'].encode('utf-8'))
	return t

def TweetCleaner(tweetText):
	h = tweetText.find('http')
	if h != -1:
		i = tweetText[0:h-1] + " " + tweetText[h + 24:len(tweetText)]
	else:
		i = tweetText
	return i

def BreakToLines(text, lineLength):
	lines = []
	beg = 0
	end = 0
	while (beg + lineLength < len(text)):
		if beg + lineLength < len(text):
			end = text[beg:lineLength + beg].rfind(' ')
			lines.append(text[beg:end])
			beg = end
		else:
			lines.append(text[beg:len(text)])
		print "%s    %s" % (beg,end)
	return lines

	
	
tweets = TweetDict(tags, results_per_tag)

for s in tweets.values():
	text = TweetCleaner(s['text']).encode('utf-8')
	ts = time.strftime('%Y-%m-%d %H:%M:%S', time.strptime(s['created_at'],'%a %b %d %H:%M:%S +0000 %Y'))
	#lines = BreakToLines(text, 32)
	#for line in lines:
	#	print line
	line_1 = text[0:31]
	line_2 = text[32:63]
	line_3 = text[64:95]
	line_4 = text[96:125 - len(s['user']['screen_name'])] + " -@" + s['user']['screen_name'].encode('utf-8')
	print "%s \n %s \n %s \n %s \n" % (line_1, line_2, line_3, line_4)
