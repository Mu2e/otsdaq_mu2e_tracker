#!/usr/bin/bash

link=$1

rocUtil -l $link -a 14 -w 0x1 write_register
