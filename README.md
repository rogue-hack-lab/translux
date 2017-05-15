translux
========

Code related to our TransLux Datawall LED display

Active development is in the arduinolux directory; currently there is a
Particle Photon board installed in the sign as it's permanent brain.

Development is to be done using [Particle's](particle.io) provided [Desktop IDE](https://www.particle.io/products/development-tools/particle-desktop-ide).

REQUIREMENTS:
-------------

- [Particle Desktop IDE](https://www.particle.io/products/development-tools/particle-desktop-ide).

If you are going to develop on Rogue Hack Lab's specific installation you will also need:
- Authorization to access Rogue Hack Lab's Particle.io account
- Authorization to view Rogue Hack Lab's Translux Dropcam Feed

BUILD:
------

1.  Check out this project

    If you want to commit changes, get set up as part of the rogue-hack-lab
    github organization, then:
    
        git clone git@github.com:rogue-hack-lab/translux.git
    
    To get a read only git clone:
    
        git clone https://github.com/rogue-hack-lab/translux.git

2.  Open the Dropcam feed to the Translux

        Get the Link and Password to this from the Rogue Hack Lab community

3.  Prepare IDE
        
        Launch Particle Dev

        Log into Rogue Hack Lab's account    
            MENU:Particle > LogIn

        Select the device
            MENU:Particle > Select Device > Translux

        Open Folder 
            Ctrl + Shift + O > "translux/arduinolux/src"

4.  Open [Particle Console](https://console.particle.io/logs)

        Login with Rogue Hack Lab credentials
        "/logs" serves as a serial monitor
        "/devices" is where you can manipulate exposed functions and variables

5.  Upload to the sign:
    
        compile in the cloud
            Ctrl + R
        
        Flash to Translux Particle
            Ctrl + U

6.  Review your changes through the Dropcam Feed

    Make fabulous improvements, and commit :)



READING & WRITING TO THE SIGN
-----------------------------
*Read/Write is currently under development*

The sign should be powered on and the Photon connected to Particle.io.
If you launch the [console](https://console.particle.io/devices) you can 
see if the Translux Photon is connected via a blue dot.

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
*Requires Rebuild to utilize ParticleJS api*

There is a pre-written script to set the sign to display the next 3 upcomming meetup events.
You need to set a meetup API authentication key in your shell's environment so that the 
script can talk to meetup servers.

1.  Find your meetup account's API key:
    
    Go to [meetup api authentication key page](http://www.meetup.com/meetup_api/key/)

        YOURNAME@lab> cd translux
        YOURNAME@lab> export MEETUP_API_KEY=<value from website above>

2.  Run the script:
    
        YOURNAME@lab> ./displaymeetupevents.py

