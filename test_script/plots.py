import logging
import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import sem

# data_paths = [
#     "105262_650.txt",
#     "105262_back_650.txt",
#     "105263_650.txt",
#     "146185_650.txt",
#     "146185_back_650.txt",
#     "206764_650.txt",
#     "206764_back_650.txt",
#     "zwykla_650.txt",
# ]

data_paths = [
    "pcb_650.txt",
    "swivel_650.txt",
    "pcb_1600.txt",
    "swivel_1600.txt",
    "pcb_2200.txt",
    "swivel_2200.txt",
]

data_paths = [
    "small_650.txt",
    "small_1000.txt",
    "small_1500.txt",
    # "small_2000.txt",
    "small_2_2000.txt",
    # "big_650.txt",
    # "big_1000.txt",
]

logging.getLogger().setLevel(logging.INFO)
bar_plot_rssi_dict = {}
bar_plot_snr_dict = {}
bar_plot_transmited_dict = {}
bar_plot_received_dict = {}
error_plot_rssi = {}
error_plot_received = {}
error_plot_snr = {}


plt.figure(0, figsize=(10, 6))
for path in data_paths:
    test_data = np.loadtxt("outputs/" + path, delimiter=";", dtype=str)
    rssi = np.array([int(z) for z in test_data[:,5]])
    snr = np.array([float(z) for z in test_data[:,6]])
    transmited_packets = np.array([int(z) for z in test_data[:,2]])
    received_packets = np.array([int(z) for z in test_data[:,3]])
    logging.info(f"Transmited packets: {transmited_packets[-1]}-> {transmited_packets[-1] / 500.0 * 100}%")
    logging.info(f"Transmited packets: {received_packets[-1]}-> {received_packets[-1] / 500.0 * 100}%")
    logging.info(f"RSSI:\tAverage {np.mean(rssi)}\tMAX {max(rssi)}\tMin {min(rssi)}")
    logging.info(f"SNR:\tAverage {np.mean(snr)}\tMAX {max(snr)}\tMin {min(snr)}")

    plt.plot(rssi, snr, "o", alpha = 0.3)
    # _, _, bars = plt.errorbar(rssi, snr, xerr=sem(rssi), yerr=sem(snr), fmt='.k')
    # [bar.set_alpha(0.1) for bar in bars]
    bar_plot_rssi_dict[path[:-4]] = np.mean(rssi)
    bar_plot_snr_dict[path[:-4]] = np.mean(snr)
    error_plot_rssi[path[:-4]] = sem(rssi)
    error_plot_snr[path[:-4]] = sem(snr)
    error_plot_received[path[:-4]] = (-5, 5)

    if np.size(received_packets) > 250:
        bar_plot_received_dict[path[:-4]] = np.size(received_packets) / 500.0 * 100.0
    else:
        bar_plot_received_dict[path[:-4]] = np.size(received_packets) / 250.0 * 100.0


# # plt.title("850m")
# plt.ylabel("Signal to noise [dB]")
# plt.xlabel("RSSI [dBm]")
# plt.grid(True)
# plt.legend([x[:-4] for x in data_paths])
# # plt.show()
# plt.savefig("plots/signal_to_noise")

# fig = plt.figure(2, figsize=(10, 6))
# ax = fig.add_subplot(111)
# ax.set_yticks(range(-130, -50, 1))
# ax.set_ylim([-115, -100])
# plt.title("Average RSSI")
# plt.bar(bar_plot_rssi_dict.keys(), bar_plot_rssi_dict.values(), color=["orange", "blue"], alpha=0.7,
#         yerr=error_plot_rssi.values(), ecolor='black', capsize=10)
# plt.grid(True)
# plt.xlabel("Antenna type and distance [m]")
# plt.ylabel("RSSI [dBm]")
# plt.savefig("plots/average_rssi")
# plt.show()

# fig = plt.figure(2, figsize=(10, 6))
# ax = fig.add_subplot(111)
# plt.title("Received frames")
# ax.set_yticks(range(0, 101, 10))
# plt.title("Average snr")
# plt.bar(bar_plot_snr_dict.keys(), bar_plot_snr_dict.values(), color=["orange", "blue"], alpha=0.7,
#         yerr=error_plot_snr.values(), ecolor='black', capsize=10)
# plt.grid(True)
# plt.ylabel("Signal to noise [dB]")
# plt.xlabel("Antenna type and distance [m]")
# plt.savefig("plots/average_snr")

fig = plt.figure(3, figsize=(10, 6))
ax = fig.add_subplot(111)
plt.title("Received frames")
ax.set_yticks(range(0, 101, 5))
ax.set_ylim([50, 100])
print(bar_plot_received_dict)
plt.bar(["PCB_antenna_650", "PCB_antenna_1000", "PCB_antenna_1600", "PCB_antenna_2200"], bar_plot_received_dict.values(), color=["blue"], alpha=0.7, yerr=1, ecolor='black', capsize=10)

plt.grid(True)
plt.ylabel("Received frames [%]")
plt.xlabel("Antenna type and distance [m]")
plt.savefig("plots/received")
plt.show()