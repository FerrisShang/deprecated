from platform import system as system_name
from os import system as system_call
from threading import Thread, Lock
import os
import sys
import re
import time
import glob
import shutil

OVPN_PATH = "D:\\Download\\ovpn_udp\\*.ovpn"
LIST_PATH = 'D:\\server.list'
IP_MARK = 'remote '

MAX_PRIORITY = 65535
POS_MARK = 0
POS_IP = 1
POS_NAME = 2
POS_TIME = 3
POS_PRI = 4


def cus_ping(host):
    if system_name().lower() == "windows":
        return system_call("ping -n 2 " + host + " > nul") == 0
    else:
        return system_call("ping -c 2 " + host + " > /dev/null 2>&1") == 0


def get_servers():
    # check server.list
    server_list = glob.glob(LIST_PATH)
    if len(server_list) == 0 or os.stat(LIST_PATH).st_size == 0:
        fw = open(LIST_PATH, 'w')
        server_list = glob.glob(OVPN_PATH)
        assert(len(server_list)>0)
        for filename in server_list:
            fr = open(filename, 'r')
            for line in fr.readlines():
                if IP_MARK in line:
                    items = line.replace("\n", "").split(' ')
                    server_name = filename.split('\\')[-1]
                    fw.write(
                        items[POS_MARK].strip()+' ' +
                        items[POS_IP].replace("\n", "").strip()+' ' +
                        server_name.replace("\n", "").strip()+' ' +
                        str(round(time.time())).replace("\n", "").strip()+' ' +
                        '0\n')
            fr.close()
        fw.close()
    # parse server.list
    servers = []
    fr = open(LIST_PATH, 'r')
    for line in fr.readlines():
        items = line.replace("\n", "").split(' ')
        servers.append((items[POS_MARK].strip(),
                        items[POS_IP].strip(),
                        items[POS_NAME].strip(),
                        items[POS_TIME].strip(),
                        items[POS_PRI].strip(),)
                       )
    fr.close()
    assert(len(servers) > 0)
    print('Number of server = ' + str(len(servers)))
    return servers


def find_ip():
    idx = 0
    servers = get_servers()
    new_servs = []
    for items in servers:
        idx += 1
        if cus_ping(items[POS_IP]):
            print("{}: ========== {} Responsed ==========".format(idx, items[POS_NAME]))
            priority = round(int(items[POS_PRI]) / 3) + MAX_PRIORITY
        else:
            print("{}: {} no response".format(idx, items[POS_NAME]))
            priority = round(int(items[POS_PRI]) / 3)
        new_servs.append(
            (items[POS_MARK].strip(),
             items[POS_IP].strip(),
             items[POS_NAME].strip(),
             str(round(time.time())).replace("\n", "").strip(),
             str(priority),)
        )
    new_servs.sort(key=lambda item: item[POS_PRI], reverse=True)
    print(new_servs)
    fw = open(LIST_PATH+'.tmp', 'w')
    for items in new_servs:
        fw.write(
            items[POS_MARK].strip()+' ' +
            items[POS_IP].strip()+' ' +
            items[POS_NAME].strip()+' ' +
            items[POS_TIME].strip()+' ' +
            items[POS_PRI]+'\n')
    fw.close()
    shutil.move(LIST_PATH+'.tmp', LIST_PATH)


def fast_find_for_android():
    OVPN_PATH = "/storage/emulated/0/nordvpn/*.ovpn"

    def get_server_list():
        server = []
        server_list = glob.glob(OVPN_PATH)
        assert (len(server_list) > 0)
        for filename in server_list:
            fr = open(filename, 'r')
            for line in fr.readlines():
                if IP_MARK in line:
                    items = line.replace("\n", "").split(' ')
                    server_name = filename.split('/')[-1]
                    server.append((items[POS_MARK].strip(),
                                    items[POS_IP].replace("\n", "").strip(),
                                    server_name.replace("\n", "").strip()))
            fr.close()
        print('Number of server = ' + str(len(server)))
        return server
    global idx
    global servers
    global new_servs
    global data_lock
    idx = 0
    servers = get_server_list()
    new_servs = []
    data_lock = Lock()

    class PingServer(Thread):
        def __init__(self, ):
            Thread.__init__(self)
            self.start()

        def run(self):
            global idx
            global servers
            global new_servs
            global data_lock
            while True:
                data_lock.acquire()
                if idx == len(servers):
                    data_lock.release()
                    return
                next_idx = idx
                idx += 1
                data_lock.release()
                if cus_ping(servers[next_idx][POS_IP]):
                    print('----------------------')
                    print(servers[next_idx][POS_NAME])
                    print(servers[next_idx][POS_IP])
                    data_lock.acquire()
                    new_servs.append(
                        (servers[next_idx][POS_MARK].strip(),
                         servers[next_idx][POS_IP].strip(),
                         servers[next_idx][POS_NAME].strip())
                    )
                    data_lock.release()

    thread_pool = []
    for x in range(0, 150):
        th = PingServer()
        thread_pool.append(th)
    for x in range(0, 150):
        th = thread_pool[x]
        th.join()

    print('\nvvvvvvvvvvvvvvvvvvvvvvvvvv\n')
    for items in new_servs:
        print(items[POS_NAME])
        print(items[POS_IP])
        print('--------------------------')

    def fast_find():
        global idx
        global servers
        global new_servs
        global data_lock
        idx = 0
        servers = get_servers()
        new_servs = []
        data_lock = Lock()

        class PingServer(Thread):
            def __init__(self, ):
                Thread.__init__(self)
                self.start()

            def run(self):
                global idx
                global servers
                global new_servs
                global data_lock
                while True:
                    data_lock.acquire()
                    if idx == len(servers):
                        data_lock.release()
                        return
                    next_idx = idx
                    idx += 1
                    data_lock.release()
                    if cus_ping(servers[next_idx][POS_IP]):
                        print("{}: ========== {} Responsed ==========".format(next_idx, servers[next_idx][POS_NAME]))
                        pri = round(int(servers[next_idx][POS_PRI]) / 3) + MAX_PRIORITY
                    else:
                        print("{}: {} no response".format(next_idx, servers[next_idx][POS_NAME]))
                        pri = round(int(servers[next_idx][POS_PRI]) / 3)
                    data_lock.acquire()
                    new_servs.append(
                        (servers[next_idx][POS_MARK].strip(),
                         servers[next_idx][POS_IP].strip(),
                         servers[next_idx][POS_NAME].strip(),
                         str(round(time.time())).replace("\n", "").strip(),
                         str(pri),)
                    )
                    data_lock.release()

        thread_pool = []
        for x in range(0, 150):
            th = PingServer()
            thread_pool.append(th)
        for x in range(0, 150):
            th = thread_pool[x]
            th.join()
        new_servs.sort(key=lambda item: item[POS_PRI], reverse=True)
        fw = open(LIST_PATH + '.tmp', 'w')
        for items in new_servs:
            fw.write(
                items[POS_MARK].strip() + ' ' +
                items[POS_IP].strip() + ' ' +
                items[POS_NAME].strip() + ' ' +
                items[POS_TIME].strip() + ' ' +
                items[POS_PRI] + '\n')
        fw.close()
        shutil.move(LIST_PATH + '.tmp', LIST_PATH)
fast_find_for_android()
