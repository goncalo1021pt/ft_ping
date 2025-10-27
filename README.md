# ft_ping
An implementation of the linux command ping


Add delay:
sudo tc qdisc add dev eth0 root netem delay 2000ms

Add packet loss:
sudo tc qdisc add dev eth0 root netem loss 50%

Add both delay and loss:
sudo tc qdisc add dev eth0 root netem delay 1000ms loss 20%

Check current rules:
tc qdisc show dev eth0

Remove all rules:
sudo tc qdisc del dev eth0 root

Note on Interface Names
Modern systems might use different interface names. Check yours with:
ip link show

Common names are:

eth0 (traditional Ethernet)
enp0s3 (new naming scheme)
wlan0 (WiFi)
wlp2s0 (new WiFi naming)