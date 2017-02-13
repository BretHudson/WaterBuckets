# Compiling

Note: Currently only builds/runs on Windows.

If you haven't already, make sure you have [MinGW](http://www.mingw.org/) downloaded. The site has a downloads button at the top, of which you'll want **MinGW/Base/gcc**. I am using the Version 4 branch (4.7.1 to be exact), however, you are free to use the Version 5 branch. If so, your mileage may vary. The the amount of options there is daunting, however, they do have a [easy to use installer](https://sourceforge.net/projects/mingw/files/latest/download) to help make things easier.

Once you're set up, you'll want to open up the build.bat file and change the directory

``` javascript
"C:\Program Files (x86)\CodeBlocks\MinGW\bin\"
```

to your MinGW directory. I don't know why I still have CodeBlocks on my computer ([Visual Studio](https://www.visualstudio.com/) and [4coder](http://www.4coder.net/) are my main IDEs), but it had MinGW all set up and ready to go. Can't complain there.

After that's done, ```main.c``` will be compiled to ```WaterBuckets.exe```. Run that and enjoy the simulation.

# Okay, now what?

This programming challenge is a simple logic problem where you have two unmarked buckets and an unlimited source of water. The example problem has a 5 gallon bucket and a 3 gallon bucket. With these, you can fill a bucket, dump a bucket, or transfer the contents of one bucket to the other. The goal is to have 4 gallons in a single bucket.

This program allows you to specify the size of the buckets and the goal. If the given set is solvable, the program will find the shortest sequence of steps to satisfy the goal, once using the larger bucket first, and a second time using the smaller bucket. From there, it will let you know which path was shorter.

# Why are the pauses so long?

**Tip:** If you're rolling your eyes at the 1 second pauses between each step, hold **enter** to increase the speed of the pauses to 0.1 seconds.