#!/bin/sh
echo '{"mode":2,"bpm":2000,"icicle_new_min":750,"icicle_new_max":1000}'|curl -d@- -XPOST http://10.65.3.246/lights
