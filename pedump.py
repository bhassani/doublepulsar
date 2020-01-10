#!/usr/bin/env python2.7
# -*- coding:utf-8 -*-
import pefile
import sys

pe = pefile.PE(sys.argv[1])
dat = pe.sections[0].get_data()
f = open(sys.argv[2], "wb+")
f.write(dat)
f.close()
