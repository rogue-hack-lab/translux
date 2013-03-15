#!/usr/bin/env python
import os, curl, json, time, serial, sys

# Even the read only meetup api requests require authentication. The simplest
# thing to do is to use a single account key to authenticate. However, this
# key is like your account password; anything your account can do the API will
# let someone who knows this key do. So
#
#               DON'T CHECK IT IN TO VERSION CONTROL!
#
# You can get find yours by logging in to meetup on the web, then going to
# http://www.meetup.com/meetup_api/key/
# then set it as an environment variable, however your platform of choice does
# that.
key = os.getenv('MEETUP_API_KEY')

# not sure how you are "supposed" to figure this out
# I got it by messing around w/ the console api tool, looking up orgs
# in medford and inspecting result for RHL
rhl_groupid = '4767212'

url = "https://api.meetup.com/2/events?key=%s&sign=true&group_id=%s&page=20" % (key, rhl_groupid)
c = curl.Curl()
c.get(url)

# this gives back
# resp['meta'] (info about request), and
# resp['results'] (array of dicts)
resp = json.loads(c.body())

# takes time in milliseconds since the epoch (that's what meetup gives us as time values)
def format_event_time(t):
    tf = time.localtime(t/1000)
    hourint = int(time.strftime("%H", tf))
    hourstr = str(hourint-12)
    if (hourint == 12): hourstr = "12"
    #ampm = "am" if hourint < 12 else "pm"
    ampm = ""
    
    return "%s %s:%s%s" % (time.strftime("%a %d", tf), hourstr,
                           time.strftime("%M", tf),
                           ampm)

msg = []
#           01234567890123456789012345678901 <-- 32 char positions
msg.append("{ UPCOMING EVENTS @ the Hack Lab")

for r in resp['results']:
    #msg.append("%s: %s with %d RSVPs" % (format_event_time(r['time']), r['name'], r['yes_rsvp_count']))
    msg.append("%s %s" % (format_event_time(r['time']), r['name']))

print "Got some events:"
for s in msg[:6]:
    print s

def flushserialin():
    print "Reading from serial port..."
    time.sleep(1)
    while f.inWaiting():
        sys.stdout.write(f.read())
    print
    
sdev = "/dev/ttyUSB0"
baud = 9600
sdelay = 6

print "Opening serial port", sdev, "at", baud, "baud..."
f = serial.Serial(sdev, baud)
print "Waiting", sdelay, "seconds for board to reset before we send serial data..."
time.sleep(sdelay)

print "Sending help command..."
f.write("?")
flushserialin()

for i in xrange(4):
    print "Setting line", i+1, "data"
    f.write("s%d%s\r\n" % (i+1, msg[i]))
    flushserialin()

print "Requesting current msg data..."
f.write("r")
flushserialin()

f.close()
