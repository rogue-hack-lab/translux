#!/usr/bin/env python
#Python 2.7

##Documentation
#twitter - https://dev.twitter.com/docs
#twython - http://twython.readthedocs.org/en/latest/

from twython import Twython, TwythonError
from random import randint
from collections import defaultdict
import apikeys, time, sys, os, serial, threading

#to keep API keys out of version control create apikeys.py. 
#it onlyneeds to contain the two definitions below
#requires Authentication as of Twitter API v1.1
twitter = Twython(apikeys.TWITTER_APP_KEY, apikeys.TWITTER_APP_SECRET)




#what strings to search Twitter for
tags = [
	'@KevinRConner','@RogueHackLab','@Soupala',		#MENTIONS
	'#RHLTweetLux','#trtt2014',  	#HASHTAGS
	'tinkerfest','Rogue Hack Lab'	#STRINGS
	] 	
print 'Search Terms:\n', tags, '\n'

#how many tweets from each tag
results_per_tag = 1 #default == 5

#length of time each string is displayed in seconds
displayInterval = 7 #default == 7

#number of tweets displayed between each API call to Twitter
#API calls are limited to 5 seconds per call or more accurately 180 calls per 15 minutes
	#https://dev.twitter.com/docs/rate-limiting/1.1
cacheInterval = 20 #default == 20
cacheMaxCount = 50

#init objects
tweets = {}
tweets_d = defaultdict(int)

def updateCache(cacheInterval):
	'''A background process that checks for new tweets every (cacheInterval) seconds'''
	#Initialize Tweets Dictionary and playcount
	tweets.update(TweetDict(tags, results_per_tag))
		#Good info about built in counters http://stackoverflow.com/questions/1692388/python-list-of-dict-if-exists-increment-a-dict-value-if-not-append-a-new-dic
	
	for tweet in tweets:
		tweets_d[tweet]
	
	while True:
		'''if len(tweets) > cacheMaxCount:
			print "### Cache reset to zero  ###"
			tweets.clear()'''
		print "\n################################"
		print "##    Updating Tweet Cache    ##"
		tweets.update(TweetDict(tags, results_per_tag))
		for tweet in tweets:
			tweets_d[tweet]
		print "##     %03d tweets cached      ##" % (len(tweets))
		print "################################"
		time.sleep(cacheInterval)

def TweetDict(tags, results_per_tag):
	'''creates a dictionary object containing {key==tweet[ID] : value==tweet[all values]}'''
	t = {}
	for tag in tags:
		search_results = GetTweets(tag, results_per_tag)
		for tweet in search_results['statuses']:
			if tweet['retweeted'] == False:
				t[tweet['id']] = tweet #"%s \n %s \n~@%s" % (tweet['created_at'], tweet['text'].encode('utf-8'), tweet['user']['screen_name'].encode('utf-8'))
	return t	

def GetTweets(String, Count):
	while True:
		try:
			search_results = twitter.search(q=String, count=Count)
		except TwythonError as e:
			ts = time.strftime("%d %b %Y %H:%M:%S", time.localtime())
			print ts, "\nUnable to Update Feed\nERROR:", e, "\n"
			wait = 1 #default == 5
			while wait != 0:
				if wait == 1:
					print "waiting", wait, "minute before attempting next cache\n"
				else:
					print "waiting", wait, "minutes before attempting next cache\n"
				time.sleep(60)
				wait -= 1
			cacheInterval += 5
			print "cache interval increased to", cacheInterval
			continue
		break
	return search_results

def TweetCleaner(tweetText):
	h = tweetText.find('http')
	if h != -1:
		tweetText = tweetText[0:h-1] + " " + tweetText[h + 24:len(tweetText)]
	if tweetText[0:2] == "RT":
		tweetText = tweetText[3:]
	return tweetText

def BreakToLines(text, lineLength):
	lines = []
	beg = 0
	end = 0
	while beg < len(text):
		if lineLength + beg < len(text):
			end = beg + text[beg:lineLength + beg].rfind(' ')
		else:
			end = len(text)
		lines.append(text[beg:end])
		beg = end + 1
	return lines
	
def PushToLux(list):
	for i in [0,1,2,len(list) - 1]: #add [:5] to this in order to fit on TransLux
		f.write("s%d%s\r\n" % (i+1, msg[i]))
		flushserialin()

	
def flushserialin():
    print "Reading from serial port..."
    time.sleep(1)
    while f.inWaiting():
        sys.stdout.write(f.read())
    print

	
	
	
#initialize Serial Port
try:
	#sdev = "/dev/ttyUSB0"
	sdev = "/dev/ttyACM0"
	baud = 9600
	sdelay = 6
	f = serial.Serial(sdev, baud)
	print "Initializing connection to Translux\r\n"
	time.sleep(sdelay)
	serialConnected = True	
except:
	print "Translux not connected. Program will run in Python window only\r\n"
	serialConnected = False

	
#start tweet caching thread
thread = threading.Thread(target=updateCache, args=(cacheInterval,))
e = threading.Event()
thread.start()
time.sleep(5)

#display tweets in a loop
while True:	
	t = min(tweets_d, key=tweets_d.get)
	if tweets_d[t] == 0:
		tweet = tweets[t]
	else:
		tweet = tweets.values()[randint(0,len(tweets) - 1)]
	text = tweet['text'].encode('utf-8')
	lines = BreakToLines(TweetCleaner(text), 32)
	lines.append("-- @%s --" % (tweet['user']['screen_name'].encode('utf-8')))
	if len(lines) == 2:
		lines.append("")
	if len(lines) == 3:
		lines.append("             { Tinkerfest 2014 {")
	print "--------------------------------"
	if tweets_d[t] == 0:
		tweets_d[t] = 1
		print "----------NEW TWEET-------------"
		print "--------------------------------"
	for i in range(len(lines)):
		print lines[i]
	if serialConnected:
		PushToLux(lines)
	print "--------------------------------"
	time.sleep(displayInterval) 
	print "\n"

	
	
#close serial
if serialConnected:
	print "Requesting current msg data..."
	f.write("r")
	flushserialin()
	f.close()