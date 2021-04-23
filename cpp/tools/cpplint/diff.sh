#!/bin/sh
git diff --no-index -- cpplint.orig.py cpplint.py > cpplint.diff
