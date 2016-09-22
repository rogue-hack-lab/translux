translux
========

Code related to our TransLux Datawall LED display

Active development is in the arduinolux/src directory; currently there is a
Leonardo board installed in the sign as it's permanent brain.

Arduino based development is using the 'ino tools' suite to do command line 
builds rather than the arduino IDE. 

Ino tools and the arduino IDE require different naming conventions and directory
structures for source files. If you want to try it out with the Arduino IDE, 
open the arduinolux/src/arduinolux.ino file from the IDE.

REQUIREMENTS:
-------------

- ino tools as of 2013-03-14 via http://inotool.org
- arduino 1.0.4 (1.0.3 doesn't seem to support the leonardo board)

BUILD:
------

2.  Check out this project

    If you want to commit changes, get set up as part of the rogue-hack-lab
    github organization, then:
    
        git clone git@github.com:rogue-hack-lab/translux.git
    
    To get a read only git clone:
    
        git clone https://github.com/rogue-hack-lab/translux.git

3.  Build your copy of the tree
    
        cd translux/arduinolux
        ino build

4.  Upload to the sign:
    
        ino upload
    
    Make fabulous improvements, and commit :)

READING & WRITING TO THE SIGN
-----------------------------

The sign should be powered on and the arduino connected to lab.roguehacklab.com
via USB normally. If you connect to the serial port, you will see diagnostic messages
being written periodically.

    cd translux/arduinolux
    ino serial

This connects to the default serial-port configured in translux/arduinolux/ino.ini 
configuration file using picocom (a minicom clone terminal.) The commands that the 
arduino responds to on the serial interface are in translux/arduinolux/src/arduinolux.ino

    ?: help, prints a little usage message
    r: read, responds with the current message being displayed by the sign
    s: set line, use like 's2This is my new line two' will set line 2 (of 1 - 4) to 'This is my new line two'
    m: temporary message, not yet implemented
    p: pixel data, not yet implemented
    f: set font data, not yet implemented

UPDATING SIGN FROM MEETUP DATA
------------------------------

There is a pre-written script to set the sign to display the next 3 upcomming meetup events.
You need to set a meetup API authentication key in your shell's environment so that the 
script can talk to meetup servers.

1.  Find your meetup account's API key:
    
    Go to [meetup api authentication key page](http://www.meetup.com/meetup_api/key/)

        YOURNAME@lab> cd translux
        YOURNAME@lab> export MEETUP_API_KEY=<value from website above>

2.  Run the script:
    
        YOURNAME@lab> ./displaymeetupevents.py

