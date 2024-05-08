# -*- coding: utf-8 -*-
"""
Created on Fri Nov 10 11:06:34 2023

@author: matias

Real time plots of recived data of the interlock!
To use with UDP_IL_Flow_data.py code.


updated up to 07/12/2023

"""

import time
import datetime as dt
from datetime import date
import matplotlib.pyplot as plt
import numpy as np
import matplotlib.animation as animation
import psutil


#To use always the same window 
from IPython import get_ipython
get_ipython().run_line_magic('matplotlib', 'qt')

N_measures = 1000


fig, ax =   plt.subplots(3, 2,figsize = (15,15),sharex=(True), sharey=(False),gridspec_kw={'height_ratios': [2, 2, 2]})
plt.subplots_adjust(bottom=0.2)


TIME     = []
env_humi = []
env_temp = []


# This function is called periodically from FuncAnimation
def animate(i, TIME, env_humi,env_temp):
    
    file_name   = 'C:/Users/pife/Documents/' + 'Pechino' + str(date.today()) + '.txt'
    S = np.loadtxt(file_name)
    print(S)
    
    N_final_row = S.shape[0]
    if(N_final_row <= N_measures):
        N_initial_row = 0
    else: N_initial_row = N_final_row - N_measures
    
    
    TIME        = [dt.datetime.fromtimestamp(ts) for ts in S[N_initial_row:N_final_row,0]]
    states      = np.array([S[N_initial_row:N_final_row,0],S[N_initial_row:N_final_row,1],S[N_initial_row:N_final_row,2],S[N_initial_row:N_final_row,15],S[N_initial_row:N_final_row,17]]).transpose()                # det, env, coo
    env_temp    = np.array([S[N_initial_row:N_final_row,0],S[N_initial_row:N_final_row,3],S[N_initial_row:N_final_row,5],S[N_initial_row:N_final_row,7],S[N_initial_row:N_final_row,9],S[N_initial_row:N_final_row,11],S[N_initial_row:N_final_row,13]]).transpose()   
    env_humi    = np.array([S[N_initial_row:N_final_row,0],S[N_initial_row:N_final_row,4],S[N_initial_row:N_final_row,6],S[N_initial_row:N_final_row,8],S[N_initial_row:N_final_row,10],S[N_initial_row:N_final_row,12],S[N_initial_row:N_final_row,14]]).transpose()
    gas_flux    = np.array([S[N_initial_row:N_final_row,0],S[N_initial_row:N_final_row,21],S[N_initial_row:N_final_row,22]]).transpose()
    coo_flux    = np.array([S[N_initial_row:N_final_row,0],S[N_initial_row:N_final_row,16]]).transpose()
    
    #env_humi           = np.array([S[N_initial_row:N_final_row,0],S[N_initial_row:N_final_row,5],S[N_initial_row:N_final_row,7]]).transpose()   
    #env_temp           = np.array([S[N_initial_row:N_final_row,0],S[N_initial_row:N_final_row,4],S[N_initial_row:N_final_row,6]]).transpose() 
    #gas_temp           = np.array([S[N_initial_row:N_final_row,0],S[N_initial_row:N_final_row,11],S[N_initial_row:N_final_row,14]]).transpose()
    #gas_humi           = np.array([S[N_initial_row:N_final_row,0],S[N_initial_row:N_final_row,12],S[N_initial_row:N_final_row,15]]).transpose()
    
    # Draw x and y lists
    ax[0,0].clear()
    ax[1,0].clear()
    ax[2,0].clear()
    ax[0,1].clear()
    ax[1,1].clear()
    ax[2,1].clear()
    
    ax[0,0].plot_date(TIME, env_humi[:,1], xdate=True, linestyle='dotted', markersize=3)
    ax[0,0].plot_date(TIME, env_humi[:,2], xdate=True, linestyle='dotted', markersize=3)
    ax[0,0].plot_date(TIME, env_humi[:,3], xdate=True, linestyle='dotted', markersize=3)
    ax[0,0].plot_date(TIME, env_humi[:,4], xdate=True, linestyle='dotted', markersize=3)
    ax[0,0].plot_date(TIME, env_humi[:,5], xdate=True, linestyle='dotted', markersize=3)
    ax[0,0].plot_date(TIME, env_humi[:,6], xdate=True, linestyle='dotted', markersize=3)
    
    ax[0,1].plot_date(TIME, env_temp[:,1], xdate=True, linestyle='dotted', markersize=3)
    ax[0,1].plot_date(TIME, env_temp[:,2], xdate=True, linestyle='dotted', markersize=3)
    ax[0,1].plot_date(TIME, env_temp[:,3], xdate=True, linestyle='dotted', markersize=3)
    ax[0,1].plot_date(TIME, env_temp[:,4], xdate=True, linestyle='dotted', markersize=3)
    ax[0,1].plot_date(TIME, env_temp[:,5], xdate=True, linestyle='dotted', markersize=3)
    ax[0,1].plot_date(TIME, env_temp[:,6], xdate=True, linestyle='dotted', markersize=3)
    
    ax[1,0].plot_date(TIME, gas_flux[:,1], xdate=True, linestyle='dotted', markersize=3)
    ax[1,0].plot_date(TIME, gas_flux[:,2], xdate=True, linestyle='dotted', markersize=3)
    ax[1,1].plot_date(TIME, coo_flux[:,1], xdate=True, linestyle='dotted', markersize=3)
    
    ax[2,1].plot_date(TIME,states[:,1], xdate=True, linestyle='dotted', markersize=3)
    ax[2,1].plot_date(TIME,states[:,2], xdate=True, linestyle='dotted', markersize=3)    
    ax[2,1].plot_date(TIME,states[:,3], xdate=True, linestyle='dotted', markersize=3)
    ax[2,1].plot_date(TIME,states[:,4], xdate=True, linestyle='dotted', markersize=3)
    
    
    ax[0,0].set_title('ENVIRONMENT HUMIDITY')
    ax[0,1].set_title('ENVIRONMENT TEMPERATURE')
    ax[1,0].set_title('GAS FLUX')
    ax[1,1].set_title('COOLING FLUX')
    
    #ax[2,0].set_title('GEMROCs STATE')
    ax[2,1].set_title('STATES')

    ax[0,0].set_ylim([0,30])
    ax[0,1].set_ylim([0,30])
    ax[1,0].set_ylim([0,3])
    ax[1,1].set_ylim([0,5])
    ax[2,1].set_ylim([0,3])
    
    ax[0,0].set_ylabel('RH (%)')
    ax[0,1].set_ylabel('temp (°C)')
    ax[1,0].set_ylabel('V')
    ax[1,1].set_ylabel('Flux (l/m)')
    ax[2,1].set_ylabel('State')
    
    
    ax[0,1].grid()
    ax[1,1].grid()
    ax[2,1].grid()
    ax[0,0].grid()
    ax[1,0].grid()
    #ax[2,0].grid()
     
    plt.sca(ax[2,0])
    plt.xticks( rotation=25 )
    plt.sca(ax[2,1])
    plt.xticks( rotation=25 )
    
    plt.xticks(fontsize = 10) 
    
    time.sleep(3)



# Set up plot to call animate() function periodically
#ani = animation.FuncAnimation(fig, animate, fargs=(time,cpu_utilization, ram_utilization, per_battery), interval=1000)
ani = animation.FuncAnimation(fig, animate, fargs=(TIME, env_humi,env_temp), interval=1000)
"""

while True:
    
     file_name   = 'C:/Users/matia/Documents/Python Scripts/saved_data/' + 'data_09_11_23' + '.txt'
     S = np.loadtxt(file_name)
     
     N_final_row = S.shape[0]
     
     if(N_final_row <= N_measures):
         N_initial_row = 0
     else: N_initial_row = N_final_row - N_measures
     
     print(N_initial_row)
     print(N_final_row)
     
     env_humi           = np.array([S[N_initial_row:N_final_row,0],S[N_initial_row:N_final_row,5],S[N_initial_row:N_final_row,7]]).transpose()   
     env_temp           = np.array([S[N_initial_row:N_final_row,0],S[N_initial_row:N_final_row,4],S[N_initial_row:N_final_row,6]]).transpose() 
     
     print(env_humi)
    
     
     time.sleep(5)
"""