#!/bin/sh

echo "#include <log/log_event.h>" > client/log_events.c
echo "" >> client/log_events.c
echo "/** Auto Generated list of events */" >> client/log_events.c
echo "struct event my_events[] = {" >> client/log_events.c
egrep -R --exclude-dir="gatoss-uc" "[^_]log_(error|info|warning|debug|count)\(\"[A-Z_]*\"" * | cut -d "\"" -f 2 | sort | uniq | awk '{print("    { .name = \"" $1 "\" },")}' >> client/log_events.c
echo "};" >> client/log_events.c
echo "int my_events_count = sizeof(my_events)/sizeof(my_events[0]);" >> client/log_events.c

