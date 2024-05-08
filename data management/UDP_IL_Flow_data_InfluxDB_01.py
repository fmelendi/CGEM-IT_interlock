#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Jan 15 17:58:55 2024

@author: matias
"""


import socket
import sys
import re
import numpy as np
import datetime
from datetime import date
import time
import smtplib
from influxdb import InfluxDBClient
from datetime import datetime
from datetime import timezone

N_Chip_cap      = 6
N_BME280        = 1
N_gas_fluxmeter = 2
N_flux_sensors  = 1
N_GEMROC        = 11

dim_matrix = (8 + N_Chip_cap*2 + N_BME280*3 + N_gas_fluxmeter + N_flux_sensors + N_GEMROC)
data_matrix = np.empty((0,dim_matrix),float)

UDP_IP   = "192.168.2.176"
UDP_PORT = 8888
PORTNUM  = 8888
IPADDR   = '192.168.2.177'

serverSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
serverSock.bind((UDP_IP, UDP_PORT))
serverSock.sendto("Start".encode(),(IPADDR,8888))

# --- InfluxDB connection settings ---


client = InfluxDBClient('192.168.38.133',8086)
#client.create_database('Interlock')

#client.get_list_database()
#client.switch_database('interlock_DB')

def connet_to_database():
    try:
        client.get_list_database()
        client.switch_database('interlock_DB_1')
        connection = True 
    except:
        print("Connection error")
        connection = False

def send_data(json): 
    try:
        client.write_points(json)
        print(json)
        connection = True
    except:
        print("Error in sending data! connnection lost!")        
        connection = False

def save_data(where,what):
    with open(where,'a') as csvfile:
        np.savetxt(csvfile,what,fmt='%f')
        
        
def ricevi_comandi(s,data_matrix):
    
    Num_data = 0                    #Number of recived data from the interlock
    while True:

        data, addr = s.recvfrom(1024)
        data = str(data.decode()).split("/")
        #print(data)
        
        if(data[0] == 'AE'):
                print('Enviroment Alarm')
                print(data)
                
    
        if(data[0] == 'AG'):
                print('Gas Alarm')
                print(data)
               
    
        if(data[0] == 'AC'):
                print('Cooling Alarm')
                print(data)
        
        
        
        if(data[0] == 'D'):

            t = time.time()
            data[0] = t
            sensors_values = data
            print(data)
            data_matrix = np.append(data_matrix,[sensors_values[0:len(sensors_values)]],axis=0)
            data_matrix = data_matrix.astype(float)
            Num_data += 1
            
            """
            client = InfluxDBClient('192.168.38.133',8086)
            #client.create_database('Interlock')
            client.get_list_database()
            client.switch_database('interlock_DB')
            """            

            data_json = [
                {
                "measurement": "Services",
                "time": str(datetime.now(timezone.utc)),
                "fields":{
                    "det_state"     :float(data[1]),
                    "env_state"     :float(data[2]),
                    "env_T1"        :float(data[3]),
                    "env_H1"        :float(data[4]),
                    "env_T2"        :float(data[5]),
                    "env_H2"        :float(data[6]),
                    "env_T3"        :float(data[7]),
                    "env_H3"        :float(data[8]),
                    "env_T4"        :float(data[9]),
                    "env_H4"        :float(data[10]),
                    "env_T5"        :float(data[11]),
                    "env_H5"        :float(data[12]),
                    "env_T6"        :float(data[13]),
                    "env_H6"        :float(data[14]),
                    "coo_state"     :float(data[15]),
                    "cooling_flux"  :float(data[16]),
                    "gas_state"     :float(data[17]),
                    "gas_T1"        :float(data[18]),
                    "gas_H1"        :float(data[19]),
                    "gas_P1"        :float(data[20]),
                    "Ar_flux"       :float(data[21]),
                    "ISO_flux"      :float(data[22]),
                    "LY1"           :float(data[23]),
                    "LY2"           :float(data[24]),
                    "LY3"           :float(data[25]),
                    "gemroc_01"     :float(data[26]),
                    "gemroc_23"     :float(data[27]),
                    "gemroc_45"     :float(data[28]),
                    "gemroc_67"     :float(data[29]),
                    "gemroc_89"     :float(data[30]),
                    "gemroc_1011"   :float(data[31]),
                    "gemroc_1213"   :float(data[32]),
                    "gemroc_1415"   :float(data[33]),
                    "gemroc_1617"   :float(data[34]),
                    "gemroc_1819"   :float(data[35]),
                    "gemroc_2021"   :float(data[36]),
                    }
                }
            ]

            #json_CONTROLLINO_measure.append(json_data(data))
            #json_CONTROLLINO_measure = json_data(data)
            
            #Send data to influxDB
            send_data(data_json)
            
            """
            if(connection == False):
               connet_to_database()
               print("Try to connection... ")
            """
            
            #Save data locally
            if(len(data_matrix) == 5):
                data_file_dir = 'C:/Users/pife/Documents/Interlock_saved_data/' + 'Pechino' + str(date.today()) + '.txt'
                save_data(data_file_dir,data_matrix)
                data_matrix = np.empty((0,dim_matrix),float)


connet_to_database()

while True:
    data_file_dir = 'C:/Users/pife/Documents/Interlock_saved_data/' + 'Pechino' + str(date.today()) + '.txt'
    ricevi_comandi(serverSock,data_matrix)
