set action=%1
echo "172.129.4.2:22"
@echo off

if "%action%"=="enter" ( docker exec -it scripts-dev /bin/bash )
if "%action%"=="build" ( docker-compose build )
if "%action%"=="up" ( docker-compose up -d )
if "%action%"=="down" ( docker-compose down )
