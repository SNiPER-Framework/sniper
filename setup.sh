#!/bin/bash

filename=`readlink -f "${BASH_SOURCE[0]}"`
basedir=`dirname "${filename}"`

export CMTPROJECTPATH="$basedir/share${CMTPROJECTPATH:+:${CMTPROJECTPATH}}"
export PYTHONPATH="$basedir/python:$basedir/lib${PYTHONPATH:+:${PYTHONPATH}}"
export LD_LIBRARY_PATH="$basedir/lib${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}"
