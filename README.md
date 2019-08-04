# CircularSoundVisualizer
[demo](https://www.reddit.com/r/unixporn/comments/ck3mrj/oc_mpd_circular_real_time_sound_visualizer/?utm_source=share&utm_medium=web2x)

Build with ```g++ -o visualizer main.cpp -std=c++14```
and execute with ``` ./visualizer```

add 
```
audio_output {
  type  "fifo"
  name  "fifo"
  path  "/tmp/mpd.fifo"
  format  "44100:16:2"
}
```
to your mpd config file

also other PCM output may works too (not tested)
if you want to try this out,
first, you need to set PCM( Pulse-Code Modulation ) output for your audio manager,
then changed the ```const char *path=``` (line 80) to the output location, recomplie. 
