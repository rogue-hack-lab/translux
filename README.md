translux
========

Code related to our TransLux Datawall LED display

Active development is in the arduinolux directory; currently there is a
Leonardo board installed in the sign as it's permanent brain, and connected 
to lab.roguehacklab.com via USB.

See more project details at: https://roguehacklab.com/mediawiki/index.php/TransLux

Arduino based development is using the 'ino tools' suite to do command line 
builds rather than the arduino IDE. Unfortunately, ino tools and the arduino 
IDE require different naming conventions / directory structures for source 
files. So if you want to try it out with the IDE, you have to copy the 
arduinolux/src/arduinolux.ino file into a sketch first.

REQUIREMENTS:
-------------

- ino tools as of 2013-03-14 via http://inotools.org
- arduino 1.0.4 (1.0.3 doesn't seem to support the leonardo board)

BUILD:
------

1.  make yourself an account on lab.roguehacklab.com and add yourself to the 
    'dialout' group to have r/w access to the Leonardo board on /dev/ttyACM0
    
        rhl@lab> sudo adduser YOURUSER
        rhl@lab> sudo adduser YOURUSER dialout

2.  Log into your new account, and check out this project

    If you want to commit changes, get set up as part of the rogue-hack-lab
    github organization, then:
    
        YOURNAME@lab> git clone git@github.com:rogue-hack-lab/translux.git
    
    To get a read only git clone:
    
        YOURNAME@lab> git clone https://github.com/rogue-hack-lab/translux.git

3.  Build your copy of the tree
    
        YOURNAME@lab> cd translux/arduinolux
        YOURNAME@lab> ino build

4.  Upload to the sign:
    
        YOURNAME@lab> ino upload
    
    Make fabulous improvements, and commit :)

