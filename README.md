# 1st_pebble_watch
My first Pebble watchface!
Forked from https://github.com/MicroByter/PebbleFaces/tree/master/pebbleeye_007

using https://www.npmjs.com/package/pebble-dash-api

##Started with:
* Bluetooth on/off with blue bar on right
* Time in middle (supports military time)
* Date at the bottom
* Watch Battery percentage to orange bar on left

##Added features
* Phone battery via dash to blue bar percentage
* Steps walked that day at the top

##Todo
* Add unread sms DataTypeUnreadSMSCount
* Add weather https://www.npmjs.com/package/pebble-generic-weather 
* add cell strength DataTypeGSMStrength
* other input? micrphone/light/accelerometer/magnetometer/compass/gyroscope
* vibe on bt disconnect: https://www.npmjs.com/package/pebble-connection-vibes
* get stuff from web (https://www.npmjs.com/package/pebble-simple-request)
* Make more battery efficient (use subscriptions instead of polling)
