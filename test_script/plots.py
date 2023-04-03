import logging
import numpy as np
import matplotlib.pyplot as plt

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
    "zwykla_850.txt",
    "105263_850.txt",
]

logging.getLogger().setLevel(logging.INFO)
bar_plot_rssi_dict = {}
bar_plot_snr_dict = {}
bar_plot_transmited_dict = {}
bar_plot_receive_dict = {}
bar_plot_received_dict = {}


plt.figure(0)
for path in data_paths:
    test_data = np.loadtxt("outputs/" + path, delimiter=";", dtype=str)
    rssi = np.array([int(z) for z in test_data[:,5]])
    snr = np.array([float(z) for z in test_data[:,6]])
    transmited_packets = np.array([int(z) for z in test_data[:,2]])
    received_packets = np.array([int(z) for z in test_data[:,3]])
    logging.info(f"Transmited packets: {transmited_packets[-1]}-> {transmited_packets[-1] / 500.0 * 100}%")
    logging.info(f"Transmited packets: {received_packets[-1]}-> {received_packets[-1] / 500.0 * 100}%")
    logging.info(f"RSSI:\tAverage {np.average(rssi)}\tMAX {max(rssi)}\tMin {min(rssi)}")
    logging.info(f"SNR:\tAverage {np.average(snr)}\tMAX {max(snr)}\tMin {min(snr)}")

    plt.plot(rssi, snr, "o", alpha = 0.3)
    bar_plot_rssi_dict[path[:-8]] = np.average(rssi)
    bar_plot_snr_dict[path[:-8]] = np.average(snr)
    bar_plot_transmited_dict[path[:-8]] = received_packets[-1] / 500.0 * 100
    bar_plot_received_dict[path[:-8]] = received_packets[-1] / 500.0 * 100

plt.title("850m")
plt.ylabel("Signal to noise")
plt.xlabel("RSSI")
plt.grid(True)
plt.legend([x[:-8] for x in data_paths])
# plt.show()

plt.figure(1)
plt.title("Average rssi - 850m")
plt.bar(bar_plot_rssi_dict.keys(), bar_plot_rssi_dict.values())
plt.grid(True)
# plt.show()

plt.figure(2)
plt.title("Average snr - 850m")
plt.bar(bar_plot_snr_dict.keys(), bar_plot_snr_dict.values())
plt.grid(True)

plt.figure(3)
plt.title("Transmited - 850m")
plt.bar(bar_plot_transmited_dict.keys(), bar_plot_transmited_dict.values())
plt.grid(True)

plt.figure(4)
plt.title("Received - 850m")
plt.bar(bar_plot_received_dict.keys(), bar_plot_received_dict.values())
plt.grid(True)
plt.show()