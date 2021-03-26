import sys
sys.setdlopenflags( 0x100 | 0x2 ) # RTLD_GLOBAL | RTLD_NOW

from Sniper.libSniperPython import *
from Sniper.PyAlgBase import PyAlgBase
