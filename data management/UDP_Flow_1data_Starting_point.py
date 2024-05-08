"""
Created on Tue Sep 27 17:57:27 2022

@author: matias

Paterns:
    - first character D (data);
    - enviroment sensors Chip cap: temp + humi;
    - gas sensors BME280 and flowmeter: temp + humi + press;
    
    patern recived:
    D + N° measure + state dec + state E + (T_env + H_env)xN_chipcapr + state C + flux + state G + (T_gas + H_gas + P_gas) + gas flow
    
    patern stored:
        Time + N° measure + state dec + state E + (T_env + H_env)xN_chipcapr + state C + flux + state G + (T_gas + H_gas + P_gas) + gas flow
        
    - save the icoming data in a file, (inside Phyton Scripts);
    
    
    E-mail: interlockcgemit@outlook.it password: interlock2023
    
    
"""

import socket
import sys
import re
import numpy as np
import datetime
import time
import smtplib


N_Chip_cap      = 6
N_BME280        = 1
N_gas_fluxmeter = 2
N_flux_sensors  = 1

dim_matrix = (6 + N_Chip_cap*2 + N_BME280*3 + N_gas_fluxmeter + N_flux_sensors)
data_matrix = np.empty((0,dim_matrix),float)

alarms_sender = 1          # 1 is on, 0 is off

email = smtplib.SMTP("smtp-mail.outlook.com", 587)
email.ehlo()
email.starttls()


E_message_object  = "Subject: Environment Alarm \n\n"
E_message_content = "Check the environment"
E_message         = E_message_object + E_message_content

C_message_object  = "Subject: Cooling Alarm \n\n"
C_message_content = "Check the Cooling"
C_message         = C_message_object + C_message_content

G_message_object  = "Subject: Gas Alarm \n\n"
G_message_content = "Check the Gas"
G_message         = G_message_object + G_message_content

UDP_IP = "192.168.2.176"
UDP_PORT = 8888
PORTNUM  = 8888
IPADDR   = '192.168.2.177'

serverSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
serverSock.bind((UDP_IP, UDP_PORT))
serverSock.sendto("Start".encode(),(IPADDR,8888))

def save_data(where,what):
    with open(where,'a') as csvfile:
        np.savetxt(csvfile,what,fmt='%f')

def ricevi_comandi(s,data_matrix):
    
    M_AE = 1
    M_AC = 1
    
    M_AG = 1
    
    Num_data = 0                    #Number of recived data from the interlock
    while True:
        
        data, addr = s.recvfrom(1024)
        data = str(data.decode()).split("/")
        print(data)
        
        if(data[0] == 'AE'):
            print('Enviroment Alarm')
            print(data)
            if(M_AE == 1 and alarms_sender == 1):
                email.login("interlockcgemit@outlook.it","interlock2023")
                email.sendmail("interlockcgemit@outlook.it","matias.melendi@gmail.com",E_message)
                M_AE = 0  
            
        if(data[0] == 'AG'):
            print('Gas Alarm')
            print(data)
            if(M_AG == 1 and alarms_sender == 1):
                email.login("interlockcgemit@outlook.it","interlock2023")
                email.sendmail("interlockcgemit@outlook.it","matias.melendi@gmail.com",G_message)
                M_AG = 0 
            
        if(data[0] == 'AC'):
            print('Cooling Alarm')
            print(data)
            if(M_AC == 1 and alarms_sender == 1):
                email.login("interlockcgemit@outlook.it","interlock2023")
                email.sendmail("interlockcgemit@outlook.it","matias.melendi@gmail.com",C_message)
                M_AC = 0 
            
        if(data[0] == 'D'):
            
            t = time.time()
            data[0] = t
            sensors_values = data
            data_matrix = np.append(data_matrix,[sensors_values[0:len(sensors_values)-1]],axis=0)
            data_matrix = data_matrix.astype(float)
            Num_data += 1
            
            if(len(data_matrix) == 5):
                print(data_matrix)
                save_data(data_file_dir,data_matrix)
                data_matrix = np.empty((0,dim_matrix),float)
                print(Num_data)
               

while True:
    #data_file_dir = 'C:/Users/CGEMI/Documents/Interlock_data/' + 'data_4-27b' + '.txt'
    data_file_dir = 'C:/Users/matia/Documents/Python Scripts/saved_data/' + 'Pechino_02_12_23' + '.txt'
    ricevi_comandi(serverSock,data_matrix)     



