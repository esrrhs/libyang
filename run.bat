@echo off
if not exist libyang.exe (
  go build -ldflags="-s -w" -o libyang.exe .
)
libyang.exe -xml sample.xml -out sample.h
