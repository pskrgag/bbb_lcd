# bbb_lcd
LCD platform driver for beaglebone black

Driver exports sysfs interface to `/sys/class/lcd/lcd16x2`

For example:

1)
This will turn lcd into digital clock

```sh
/sys/class/lcd/lcd16x2 # echo 'time' > lcdcmd
```
2)
This will print "hello :)" into lcd
```sh
/sys/class/lcd/lcd16x2 # echo 'clear' > lcdcmd
```

```sh
/sys/class/lcd/lcd16x2 # echo 'hello :)' > lcdtext
```

![Photo](img/1.jpg)
