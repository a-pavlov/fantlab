#!/bin/sh

# http://www.pgtk.edu.ru/gooamoko/doku.php?id=java:daemon
# Setup variables
EXEC=$HOME/local/bin/jsvc
NAME=recd
#JAVA_HOME="/usr/lib/jvm/default-java"
JAVA_HOME="/usr/lib/jvm/java-8-oracle"
CLASS_HOME="$HOME/dev/marks/fantlab/mahout/target"
CLASS_PATH="${CLASS_HOME}/commons-daemon-1.0.15.jar:${CLASS_HOME}/${NAME}.jar"
CLASS="org.dkfsoft.RecommendationDaemon"
OPTIONS="--port=8080 --filename=/media/elements/data/fantlab/marks.csv"
USER=apavlov
PID="/tmp/${NAME}.pid"
LOG_OUT="$CLASS_HOME/${NAME}.log"
LOG_ERR="$CLASS_HOME/${NAME}.err"
PROCNAME=${NAME}
LOGGING="-Djava.util.logging.config.file=/home/apavlov/dev/marks/fantlab/mahout/src/main/resources/logging.properties"

do_exec()
{
    $EXEC -Xmx1g -Xms800m -nodetach -debug $LOGGING -home $JAVA_HOME -procname $PROCNAME -cp $CLASS_PATH -outfile $LOG_OUT -errfile $LOG_ERR  -pidfile $PID $1 $CLASS $OPTIONS
}

case "$1" in
    start)
        do_exec
            ;;
    stop)
        do_exec "-stop"
            ;;
    restart)
        if [ -f "$PID" ]; then
            do_exec "-stop"
            do_exec
        else
            echo "service not running."
            exit 1
        fi
            ;;
    *)
            echo "usage: daemon {start|stop|restart}" >&2
            exit 3
            ;;
esac
