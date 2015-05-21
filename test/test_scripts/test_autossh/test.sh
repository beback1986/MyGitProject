#!/bin/sh

autossh()
{
if [ $# -ne 3 ];then
	echo "Usage: ./autossh.ssh 10.10.10.1 111111 'ls /home'"
	exit 1
fi        


	IP=$1
        PASSWD=$2
        COMM=$3
        rm -f /root/.ssh/known_hosts
 
        expect -c "
                set timeout 90
                spawn ssh $IP $COMM
                expect \"(yes/no)?\" {
                        send_user \"sending yes...\"
                        send \"yes\r\"
                } \"password:\" {
                        send_user \"sending passwd directly...\"
                        send \"$PASSWD\r\"
                        set timeout -1
                        expect eof {
                                exit 0
                        } timeout {
                                send_user \"timeout in execute phase\"
                                exit 2
                        } \"*password:\" {
                                send_user \"wrong passwd\"
                                exit 4
                        }
                } eof {
                        exit 0
                } timeout {
                        send_user \"timeout in yes_phase\"
                        exit 2
                } \"No route\" {
                        send_user \"No route to host...\"
                        exit 1
                }
                expect \"password:\" {
                        send_user \"sending passwd after yes phase...\"
                        send \"$PASSWD\r\"
                } eof {
                        exit 0
                } timeout { 
                        send_user \"timeout in passwd phase\"
                        exit 2
                }
                set timeout -1
                expect eof {
                        exit 0
                } timeout { 
                        send_user \"timeout in execute phase\"
                        exit 2
                } \"password:\" {
                        send_user \"wrong passwd\"
                        exit 4
                }
                expect \"No route\" {
                        send_user \"No route to host...\"
                        exit 2
                }
        "
}

#autossh admin@10.10.99.5 password 'portdisable 0'
#autossh admin@10.10.99.5 password 'portdisable 2'
#autossh admin@10.10.99.5 password 'portdisable 6'
#autossh admin@10.10.99.5 password 'portdisable 7'
#autossh admin@10.10.99.5 password 'portdisable 3'
#autossh admin@10.10.99.5 password 'portdisable 1'

autossh admin@10.10.99.5 password 'portenable 0'
autossh admin@10.10.99.5 password 'portenable 2'
autossh admin@10.10.99.5 password 'portenable 6'
autossh admin@10.10.99.5 password 'portenable 7'
autossh admin@10.10.99.5 password 'portenable 3'
autossh admin@10.10.99.5 password 'portenable 1'
