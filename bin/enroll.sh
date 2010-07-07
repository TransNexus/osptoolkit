#!/bin/sh
####################################################################
#                                                                  #
#                      enroll.sh                                   #
#                                                                  #
####################################################################
#                                                                  #
# This shell script is used to enroll the device with the Server   #
####################################################################
OPENSSL_CONF=./openssl.cnf
export OPENSSL_CONF
RANDFILE=./.rnd
export RANDFILE
if [ $# -le 0 ]; then
echo "Usage: <script name> <list of osp server ip addresses>"
exit 127
fi

openssl req -outform PEM -nodes -newkey rsa:512 -md5 -new -out certreq.pem -keyform PEM -keyout pkey.pem
ret_val=$?
export ret_val
echo ""
echo "Error Code returned from openssl command :" $ret_val
if [ "$ret_val" != "0" ]; then
echo "Request Failed"
exit 127
fi

enroll -function getcacert -caurl http://$1:5045/tep > cacert_0.pem
ret_val=$?
export ret_val
echo "[SP: $1]Error Code returned from getcacert command :" $ret_val
if [ "$ret_val" != "0" ]; then
echo "Request Failed"
exit 127
fi

enroll -function request -username trans -password nexus -customer 1000 -device 1000 -cacert cacert_0.pem -certreq certreq.pem -sslurl https://$1:1443/tep > localcert.pem
ret_val=$?
export ret_val
echo "Error Code returned from localcert command :" $ret_val
if [ "$ret_val" != "0" ]; then
echo "Request Failed"
exit 127
fi

i=1
shift
while [ $# -ge 1 ]; do
enroll -function getcacert -caurl http://$1:5045/tep > cacert_$i.pem
ret_val=$?
export ret_val
echo "[SP: $1]Error Code returned from getcacert command :" $ret_val
if [ "$ret_val" != "0" ]; then
echo "Request Failed"
exit 127
fi
i=`expr $i + 1`
shift
done
