# Chapter-3
用法
make

sudo insmod task_info.ko

cat /proc/task_info

You will get

Current Task:

PID: 12029

Comm: insmod

State: 0

All Tasks:

PID: 1 Comm: systemd

PID: 2 Comm: kthreadd

PID: 3 Comm: pool_workqueue_

PID: 4 Comm: kworker/R-rcu_g

....
